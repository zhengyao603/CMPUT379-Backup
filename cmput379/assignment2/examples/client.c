#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int user_sock;

// defined signal handler
void user_handler (int sig){
	printf("user client terminate.\n");
    close(user_sock);
    exit(0);	
}

int main(int argc, char const *argv[]) {
	int user_sock = 0;
	struct sockaddr_in serv_addr;
	char *hello = "Hello from client";
	char buffer[BUFFER_SIZE];

	// initialize signal handler
	struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = user_handler;
    sigaction(SIGINT, &sa, NULL);

	if ((user_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(user_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect");
		return -1;
	}

	while (1){
		read(user_sock, buffer, BUFFER_SIZE);
		printf("\nHello message received\n");
	}

	return 0;
}
