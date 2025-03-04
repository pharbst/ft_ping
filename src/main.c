#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

typedef struct s_stats {
	char *target;
	int transmitted;
	int received;
	int loss;
	int time;
	float min;
	float avg;
	float max;
	float mdev;
} t_stats;

// void sigint_handler(int signum);
// void close_socket(int action);
// void print_stats(t_stats stats);

// void sigint_handler(int signum) {
// 	// printf("INTERRUPT PING\n");
// 	// // print_stats();
// 	// close_socket(-1);
// 	// exit(signum + 0x80);
// }

// void print_stats(t_stats stats) {
// 	printf("--- %s ping statistics ---\n", stats.target);
// 	printf("%d packets transmitted, %d received, %d%% packet loss, time %dms\n",
// 		stats.transmitted, stats.received, stats.loss, stats.time);
// 	printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
// 		stats.min, stats.avg, stats.max, stats.mdev);
// }

// int main() {
// 	int sockfd;

// 	if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
// 		return (perror("socket creation failed\n"), 1);
// 	struct icmphdr head;
// 	// bezero(&head, sizeof(head));
// 	head.type = ICMP_ECHO;
// 	head.code = 0;
// 	// head.checksum =
// 	struct sockaddr_in addr;
// 	inet_pton(AF_INET, "127.0.0.1", &addr);
// 	ssize_t bytes =  sendto(sockfd, &head, sizeof(head), 0, (struct sockaddr*)&addr, sizeof(&addr));
// 	printf("%li %s\n",bytes, strerror(errno));

// 	struct iovec urmum;
// 	struct msghdr urdad;
// 	urdad.msg_iov = &urmum;
// 	printf("ur dad is %ld thick\n", recvmsg(sockfd, &urdad, 0));
// 	return 0;
// }

#define NO_ARG 0

void print_help(int status);

int main(int argc, char **argv) {
	int		icmp_socket;

	if (argc == 1)
		return (print_help(NO_ARG), 1);
	icmp_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (icmp_socket < 0)
		return (icmp_socket);
	
	return (0);
}

void print_help(int status) {
	switch (status) {
		case 0:
			printf("ft_ping: usage error: destination address required\n");
			break;
		// case 1:
		// case 2:
		// case 3:
		// case 4:
		// case 5:
		default:
			printf("ft_ping: undefined error %d\n", status);
			break;
	}
}