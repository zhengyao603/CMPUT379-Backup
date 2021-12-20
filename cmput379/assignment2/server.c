#include "server.h"
#include "observer.h"
#include "user.h"

#define OBSERVER_BUFFER_SIZE 4096 * sizeof(Message)

static Message *server_buffer[4096] = {0}; // shared resources for storing information of inotify event
static int observer_number = 0; // count of observer hosts
static int if_broadcast = -1; // flag check if it is time to broadcast
static FILE *fp = NULL; // file pointer of logfile

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER; // mutex of server_buffer
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER; // mutex of observer_number
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER; // mutex of if_broadcast
pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER; // mutex of logfile file descriptor


// defined signal handler
void server_handler(int sig){
	// close file
	if (fp){
		fclose(fp);
	}
	printf("\nServer Terminates.\n");
	printf("Program Exit.\n");

    pthread_mutex_lock(&mutex2);
	int N = observer_number;
	pthread_mutex_unlock(&mutex2);

    exit(0);
}


// thread for handling observer connection
void *observer_connection(void *socket_fd){
	// read the global count
	pthread_mutex_lock(&mutex2);
	int index = observer_number - 1;
	pthread_mutex_unlock(&mutex2);

	int sock = *(int *)socket_fd;

	// keep reading from observer client
	char observer_buffer[OBSERVER_BUFFER_SIZE];
    while (1){
    	pthread_mutex_lock(&mutex1);
    	if (server_buffer[index]){
    		pthread_mutex_unlock(&mutex1);

    		// read from observer
    		int byteread = read(sock, observer_buffer, OBSERVER_BUFFER_SIZE);
    		if (fp){
    			pthread_mutex_lock(&mutex4);
			    fprintf(fp, "Read %d bytes from observer #%d.\n", byteread, index + 1);
			    pthread_mutex_unlock(&mutex4);
    		}

    		struct timeval tv;
			gettimeofday(&tv, NULL);

    		// clean up the information sent from the disconnected observer
			pthread_mutex_lock(&mutex1);
			free(server_buffer[index] -> fileordir_monitored);
    		free(server_buffer[index] -> fileordir_changed);
    		free(server_buffer[index] -> ifdir);
    		free(server_buffer[index] -> action);
    		free(server_buffer[index] -> hostaddr);
    		free(server_buffer[index]);
    		server_buffer[index] = NULL;
    		pthread_mutex_unlock(&mutex1);

    		// if observer disconnect
    		if (byteread <= 0){
    			if (fp){
    				pthread_mutex_lock(&mutex4);
				    fprintf(fp, "Observer #%d disconnect from server.\n", index + 1);
				    pthread_mutex_unlock(&mutex4);
    			}
				server_buffer[index] = NULL;

    			pthread_exit(NULL);
    		}

    		pthread_mutex_lock(&mutex1);
    		// deserialize and store inotify information
    	    server_buffer[index] = deserialize(observer_buffer, byteread);
    	    #ifdef TIMESTAMP
	    	    server_buffer[index] -> time_sec = tv.tv_sec;
	    	    server_buffer[index] -> time_usec = tv.tv_usec;
	    	#endif
    		pthread_mutex_unlock(&mutex1);

    		continue;
    	}

    	pthread_mutex_unlock(&mutex1);
    	// read inotify information and store it into server_buffer
    	int byteread = read(sock, observer_buffer, OBSERVER_BUFFER_SIZE);

    	struct timeval tv;
		gettimeofday(&tv, NULL);

    	// if observer disconnect
    	if (byteread <= 0){
    		if (fp){
    			pthread_mutex_lock(&mutex4);
			    fprintf(fp, "Observer #%d disconnect from server.\n", index + 1);
			    pthread_mutex_unlock(&mutex4);
    		}
    		pthread_exit(NULL);
    	}

    	pthread_mutex_lock(&mutex1);
		server_buffer[index] = deserialize(observer_buffer, byteread);
		#ifdef TIMESTAMP
    	    server_buffer[index] -> time_sec = tv.tv_sec;
    	    server_buffer[index] -> time_usec = tv.tv_usec;
    	#endif
		pthread_mutex_unlock(&mutex1);

        if (fp){
        	pthread_mutex_lock(&mutex4);
		    fprintf(fp, "Inotify info from observer #%d updated.\n", index + 1);
		    pthread_mutex_unlock(&mutex4);
        }

        // clean up buffer
		memset(observer_buffer, 0, sizeof(observer_buffer)); 
    }
}


// thread for recording time
void *timer_heartbeat(void *flag){
	float time = *(float *)flag;

	while (1){
		pthread_mutex_lock(&mutex3);
		if_broadcast = -1;
		pthread_mutex_unlock(&mutex3);

		sleep(time);

		pthread_mutex_lock(&mutex3);
		if_broadcast = 1;
		pthread_mutex_unlock(&mutex3);
	}
}


// thread for handling user connection
void *user_connection(void *socket_fd){
	int sock = *(int *)socket_fd;

	// keep broadcasting to user client
	while (1){
		// if it is not the time to broadcast
		pthread_mutex_lock(&mutex3);
		if (if_broadcast == -1){
			pthread_mutex_unlock(&mutex3);
			continue;
		}
		pthread_mutex_unlock(&mutex3);

		if (fp){
			pthread_mutex_lock(&mutex4);
		    fprintf(fp, "Start broadcasting to users.\n");
		    pthread_mutex_unlock(&mutex4);
		}

        // read count of observers
		pthread_mutex_lock(&mutex2);
		int N = observer_number;
		pthread_mutex_unlock(&mutex2);

		int ack; // ack receive each time from user
        // send the count of observers
		send(sock, &N, sizeof(int), 0);
		int byteread = read(sock, &ack, sizeof(int)); // read ack
		if (byteread <= 0){
			if (fp){
				pthread_mutex_lock(&mutex4);
			    fprintf(fp, "User disconnect from server.\n");
			    pthread_mutex_unlock(&mutex4);
			}
			pthread_exit(NULL);
		}

        int index = 0; // current index of server_buffer
        // send inotify information from each observer
        while (index < N){
        	int temp_len;

        	pthread_mutex_lock(&mutex1);
        	// if observer i does not send any info to server
        	if (!server_buffer[index]){
        		pthread_mutex_unlock(&mutex1);
        		int empty_count = 0;
        		// send a zero count and empty packet to user
        		send(sock, &empty_count, sizeof(int), 0);
        		byteread = read(sock, &ack, sizeof(int)); // read ack
        		// if user disconnect
        		if (byteread <= 0){
        			if (fp){
						pthread_mutex_lock(&mutex4);
					    fprintf(fp, "User disconnect from server.\n");
					    pthread_mutex_unlock(&mutex4);
					}
        			pthread_exit(NULL);
        		}
        		index++;
        		continue;
        	}

    		// serialize inotify information
        	uint8_t *user_buffer = serialize(server_buffer[index], &temp_len);
        	pthread_mutex_unlock(&mutex1);

            // send the count and serialized information to user
            send(sock, &temp_len, sizeof(int), 0);
        	byteread = read(sock, &ack, sizeof(int)); // read ack
        	// if server disconnect
        	if (byteread <= 0){
        		if (fp){
					pthread_mutex_lock(&mutex4);
				    fprintf(fp, "User disconnect from server.\n");
				    pthread_mutex_unlock(&mutex4);
				}
        		pthread_exit(NULL);
        	}

        	send(sock, user_buffer, temp_len, 0);
        	byteread = read(sock, &ack, sizeof(int)); // read ack
        	// if server disconnect
        	if (byteread <= 0){
        		if (fp){
					pthread_mutex_lock(&mutex4);
				    fprintf(fp, "User disconnect from server.\n");
				    pthread_mutex_unlock(&mutex4);
				}
        		pthread_exit(NULL);
        	}
        	free(user_buffer);

        	if (fp){
				pthread_mutex_lock(&mutex4);
			    fprintf(fp, "Finish broadcasting to users.\n");
			    pthread_mutex_unlock(&mutex4);
			}

        	index++;
        }
	}
}


// defined function for processing server action
int server(float interval, int sport, char *logfile){
	// initialize sever_buffer
	memset(server_buffer, 0, sizeof(server_buffer));

	// initialize signal handler
	struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = server_handler;
    sigaction(SIGINT, &sa, NULL);

	// print port number to stdout
	if (sport == -1) sport = 8080;
	printf("Server Port Number:%d\n", sport);

	pthread_t timer_thread;
	pthread_create(&timer_thread, NULL, timer_heartbeat, (void *)&interval);

    // if the logfile is provided
	if (logfile){
		if ((fp = fopen(logfile, "w")) == NULL){
			printf("Fail to create file!\n");
			return -1;
		}
	}

	// initialize socket descriptor
	int sock1, sock2;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	char ack_buffer[20] = {0};

	// initialize socket
	if ((sock1 = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	// debug message
	if (logfile){
		fprintf(fp, "Server Initialize Socket File Descriptor.\n");
	}

    // initialize sockaddr_in struct
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(sport);

    // bind the socket
	if (bind(sock1, (struct sockaddr *) &address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	// debug message
	if (logfile){
		fprintf(fp, "Server Bind Socket File Descriptor And Adress.\n");
	}

    // listen to the port
	if (listen(sock1, 3) < 0) {
		perror("listen failed");
		exit(EXIT_FAILURE);
	}
	// debug message
	if (logfile){
		fprintf(fp, "Server Listen to the Socket File Descriptor.\n");
	}

    // keep receiving ack from possible clients
    while (1){
	    // accept client request
		if ((sock2 = accept(sock1, (struct sockaddr *) &address, (socklen_t*) &addrlen)) < 0) {
			perror("accept failed");
			exit(EXIT_FAILURE);
		}
		// debug message
		if (logfile){
			pthread_mutex_lock(&mutex4);
		    fprintf(fp, "Server Accepet Client Connection Rquest.\n");
		    pthread_mutex_unlock(&mutex4);
		}

    	// read acknowledgement message into assigned buffer
		read(sock2, ack_buffer, 20);
		// debug message
		if (logfile){
			pthread_mutex_lock(&mutex4);
		    fprintf(fp, "Reuqested client is %s\n", ack_buffer);
		    pthread_mutex_unlock(&mutex4);
		}

	    // multithreading based on type of client
		if (strcmp(ack_buffer, "observer client") == 0){
			// update global count of observer hosts
			pthread_mutex_lock(&mutex2);
			observer_number++;
			pthread_mutex_unlock(&mutex2);
	 
	        // create a new thread
			pthread_t observer_thread;
			pthread_create(&observer_thread, NULL, observer_connection, (void *)&sock2);

            if (logfile){
            	pthread_mutex_lock(&mutex4);
			    fprintf(fp, "New Thread Created for an observer client.\n");
			    pthread_mutex_unlock(&mutex4);
            }

		} else if (strcmp(ack_buffer, "user client") == 0){
			// create a new thread
			pthread_t user_thread;
			pthread_create(&user_thread, NULL, user_connection, (void *)&sock2);

            if (logfile){
            	pthread_mutex_lock(&mutex4);
			    fprintf(fp, "New Thread Created for an user client.\n");
			    pthread_mutex_unlock(&mutex4);
            }

		} else {
			printf("invalid client type.\n");
			return -1;
		}

		// clear ack buffer
		memset(ack_buffer, 0, sizeof(ack_buffer)); 
    }

    // close the sockets
    close(sock2);
    close(sock1);

    return 0;
}