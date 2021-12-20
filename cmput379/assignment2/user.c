#include "server.h"
#include "observer.h"
#include "user.h"

#define USER_BUFFER_SIZE 4096 * sizeof(Message)

int user_sock = 0;

// defined signal handler
void user_handler (int sig){
	printf("\nUser Client Terminates.\n");
    close(user_sock);
    exit(0);	
}

// defined function for processing user client action
int user_client(char *saddr, int sport){

	struct sockaddr_in address;
	char *hello = "user client";
	Message *inotify_info[4096];
	memset(inotify_info, 0, sizeof(inotify_info));

    // initialize signal handler
	struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = user_handler;
    sigaction(SIGINT, &sa, NULL);

    // initialize the socket
	if ((user_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		return -1;
	}

    // initialize sockaddr_in struct
	address.sin_family = AF_INET;
	address.sin_port = htons(sport);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, saddr, &address.sin_addr) <= 0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

    // connect the socket
	if (connect(user_sock, (struct sockaddr *) &address, sizeof(address)) < 0) {
		perror("connect failed");
		return -1;
	}

    // send ack message to the server
	send(user_sock, hello, strlen(hello), 0);

    while (1){
    	int ack = 1; // ack message send back every time receive any packets from server

    	// read the count of observers
	    int N;
		read(user_sock, &N, sizeof(int));
		send(user_sock, &ack, sizeof(int), 0); // send ack
		
		// read inotify information from all observers
		char user_buffer[USER_BUFFER_SIZE];

		int index = 0; // current index of inotify_info
		while (index < N){
			// read the count of bytes that will be sent
			int byte_count;
			int byteread = read(user_sock, &byte_count, sizeof(int));
			send(user_sock, &ack, sizeof(int), 0); // send ack
			if (byte_count == 0){
				index++;
				continue;
			}

            // read 'byte_count' bytes from server
			byteread = read(user_sock, user_buffer, byte_count);
			send(user_sock, &ack, sizeof(int), 0); // send ack
			inotify_info[index] = deserialize(user_buffer, byteread);

            // clean up buffer
			memset(user_buffer, 0, sizeof(user_buffer));
			index++;
		}

        index = 0;
		printf("\nTime\t\t\tHost\t\tMONITORED\tEVENT\n");
		while (index < N){
			if (!inotify_info[index]){
				index++;
				continue;
			}
			// print information to stdout
			printf("%lu.%d\t%s\t%s\t\t%s %s %s\n\n",
				   inotify_info[index] -> time_sec, inotify_info[index] -> time_usec,
				   inotify_info[index] -> hostaddr,
				   inotify_info[index] -> fileordir_monitored,
				   inotify_info[index] -> fileordir_changed,
				   inotify_info[index] -> action,
				   inotify_info[index] -> ifdir);

			// free memory
			free(inotify_info[index] -> hostaddr);
			free(inotify_info[index] -> fileordir_monitored);
			free(inotify_info[index] -> fileordir_changed);
			free(inotify_info[index] -> action);
			free(inotify_info[index] -> ifdir);
			free(inotify_info[index]);
			inotify_info[index] = NULL;

			index++;
		}
    }

	return 0;
}