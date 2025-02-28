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

void sigint_handler(int signum);
void close_socket(int action);
void print_stats(t_stats stats);

void sigint_handler(int signum) {
	printf("INTERRUPT PING\n");
	// print_stats();
	close_socket(-1);
	exit(signum + 0x80);
}

void close_socket(int action) {
	static int sockfd;

	if (action == -1)
		close(sockfd);
	else
		sockfd = action;
}

void print_stats(t_stats stats) {
	printf("--- %s ping statistics ---\n", stats.target);
	printf("%d packets transmitted, %d received, %d%% packet loss, time %dms\n",
		stats.transmitted, stats.received, stats.loss, stats.time);
	printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
		stats.min, stats.avg, stats.max, stats.mdev);
}



int main() {
	int sockfd;

	if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
		return (perror("socket creation failed\n"), 1);
	close_socket(sockfd);
	
	return 0;
}
