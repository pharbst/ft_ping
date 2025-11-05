#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip_icmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define ICMP_PAYLOAD "ft_ping test payload"
#define EPOLL_TIMEOUT_MS 1000

static uint16_t icmp_checksum(const void *data, size_t len) {
    const uint16_t *words = data;
    uint32_t sum = 0;

    while (len > 1) {
        sum += *words++;
        len -= 2;
    }

    if (len == 1) {
        uint16_t last = 0;
        *(uint8_t *)&last = *(const uint8_t *)words;
        sum += last;
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return (uint16_t)(~sum);
}

static void print_buffer(const char *label, const void *buf, size_t len) {
    const unsigned char *bytes = buf;

    printf("%s (%zu bytes):\n", label, len);
    for (size_t i = 0; i < len; ++i) {
        if (i % 16 == 0) {
            printf("%04zx: ", i);
        }
        printf("%02x ", bytes[i]);
        if ((i + 1) % 8 == 0 && (i + 1) % 16 != 0) {
            printf(" ");
        }
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    if (len % 16 != 0) {
        printf("\n");
    }
    fflush(stdout);
}

int main(void) {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    unsigned char packet[sizeof(struct icmphdr) + sizeof(ICMP_PAYLOAD) - 1];
    memset(packet, 0, sizeof(packet));

    struct icmphdr *icmp = (struct icmphdr *)packet;
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = htons((uint16_t)getpid());
    icmp->un.echo.sequence = htons(1);

    memcpy(packet + sizeof(struct icmphdr), ICMP_PAYLOAD, sizeof(ICMP_PAYLOAD) - 1);
    icmp->checksum = 0;
    icmp->checksum = icmp_checksum(packet, sizeof(packet));

    struct sockaddr_in target;
    memset(&target, 0, sizeof(target));
    target.sin_family = AF_INET;
    target.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    ssize_t sent = sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&target,
                          sizeof(target));
    if (sent < 0) {
        perror("sendto");
        close(sockfd);
        return EXIT_FAILURE;
    }

    print_buffer("ICMP echo request", packet, (size_t)sent);

    int epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create1");
        close(sockfd);
        return EXIT_FAILURE;
    }

    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;
    event.data.fd = sockfd;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
        perror("epoll_ctl");
        close(epfd);
        close(sockfd);
        return EXIT_FAILURE;
    }

    struct epoll_event fired;
    int nfds = epoll_wait(epfd, &fired, 1, EPOLL_TIMEOUT_MS);
    if (nfds == -1) {
        perror("epoll_wait");
        close(epfd);
        close(sockfd);
        return EXIT_FAILURE;
    }

    if (nfds == 0) {
        fprintf(stderr, "timeout waiting for ICMP reply\n");
        close(epfd);
        close(sockfd);
        return EXIT_FAILURE;
    }

    unsigned char response[1024];
    ssize_t received = recv(sockfd, response, sizeof(response), 0);
    if (received < 0) {
        perror("recv");
        close(epfd);
        close(sockfd);
        return EXIT_FAILURE;
    }

    print_buffer("ICMP response", response, (size_t)received);

    close(epfd);
    close(sockfd);

    return EXIT_SUCCESS;
}
