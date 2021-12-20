#include "server.h"
#include "observer.h"
#include "user.h"

#define INT_OFFSET sizeof(long)
#define STRING_OFFSET INT_OFFSET + sizeof(int)

#define INOTIFY_BUFFER_SIZE sizeof(struct inotify_event) + NAME_MAX + 1

int observer_sock = 0;

// defined signal handler
void observer_handler (int sig){
	printf("\nObserver Client Terminates.\n");
    close(observer_sock);
    exit(0);	
}

// defined function for serializing struct
uint8_t *serialize(Message *message, int *length){
	// calculate length of each strings
	int len1 = strlen(message -> fileordir_monitored);
	int len2 = strlen(message -> fileordir_changed);
	int len3 = strlen(message -> ifdir);
	int len4 = strlen(message -> action);
	int len5 = strlen(message -> hostaddr);

    // calculate the length of the buffer
	*length = sizeof(long) + sizeof(int) + len1 + len2 + len3 + len4 + len5 + 5;

    // allocate memory for buffer
	uint8_t *buffer = calloc(sizeof(uint8_t), *length);
    if (buffer == NULL) {
    	printf("memory allocation fail");
        return NULL;
    }

    // set parameters
    *(long *) (buffer) = message -> time_sec;
    *(int *) (buffer + INT_OFFSET) = message -> time_usec;
    //copy strings
    memcpy(buffer + STRING_OFFSET, message -> fileordir_monitored, len1);
    memcpy(buffer + STRING_OFFSET + len1 + 1, message -> fileordir_changed, len2);
    memcpy(buffer + STRING_OFFSET + len1 + 1 + len2 + 1, message -> ifdir, len3);
    memcpy(buffer + STRING_OFFSET + len1 + 1 + len2 + 1 + len3 + 1, message -> action, len4);
    memcpy(buffer + STRING_OFFSET + len1 + 1 + len2 + 1 + len3 + 1 + len4 + 1, message -> hostaddr, len5);

    return buffer;
}


// defined function for deserializing struct
Message *deserialize(uint8_t *buffer, int length){
	// allocate memory for struct
	Message *message = malloc(sizeof(Message));
    // set parameters
	message -> time_sec = *(long *) (buffer);
	message -> time_usec = *(int *) (buffer + INT_OFFSET);


	// calculate len of fileordir_monitored
	int len1 = strnlen(buffer + STRING_OFFSET, length - STRING_OFFSET - 1);
	if (len1 >= length - STRING_OFFSET - 1) {
		printf("error in len1");
        free(message);
        return NULL;
    }
    // allocate memory for fileordir_monitored
    message -> fileordir_monitored = (char *) malloc(len1 + 1);
    if (message -> fileordir_monitored == NULL) {
    	printf("error in fileordir_monitored memory allocation");
        free(message);
        return NULL;
    }
    // copy fileordir_monitored into message
    strncpy(message -> fileordir_monitored, buffer + STRING_OFFSET, len1);
    message -> fileordir_monitored[len1] = '\0';


    // calculate len of fileordir_changed
    int len2 = strnlen(buffer + STRING_OFFSET + len1 + 1, length - STRING_OFFSET - len1 - 2);
    if (len2 >= length - STRING_OFFSET - len1 - 2) {
    	printf("error in len2");
        free(message -> fileordir_monitored);
        free(message);
        return NULL;
    }
    // allocate memory for fileordir_changed
    message -> fileordir_changed = (char *) malloc(len2 + 1);
    if (message -> fileordir_changed == NULL) {
    	printf("error in fileordir_changed memory allocation");
    	free(message -> fileordir_monitored);
        free(message);
        return NULL;
    }
    // copy fileordir_changed into message
    strncpy(message -> fileordir_changed, buffer + STRING_OFFSET + len1 + 1, len2);
    message -> fileordir_changed[len2] = '\0';


    // calculate len of ifdir
    int len3 = strnlen(buffer + STRING_OFFSET + len1 + len2 + 2, length - STRING_OFFSET - len1 - len2 - 3);
    if (len3 >= length - STRING_OFFSET - len1 - len2 - 3) {
    	printf("error in len3");
        free(message -> fileordir_monitored);
        free(message -> fileordir_changed);
        free(message);
        return NULL;
    }
    // allocate memory for ifdir
    message -> ifdir = (char *) malloc(len3 + 1);
    if (message -> ifdir == NULL) {
    	printf("error in ifdir memory allocation");
    	free(message -> fileordir_monitored);
    	free(message -> fileordir_changed);
        free(message);
        return NULL;
    }
    // copy ifdir into message
    strncpy(message -> ifdir, buffer + STRING_OFFSET + len1 + len2 + 2, len3);
    message -> ifdir[len3] = '\0';


    // calculate len of action
    int len4 = strnlen(buffer + STRING_OFFSET + len1 + len2 + len3 + 3, length - STRING_OFFSET - len1 - len2 - len3 - 4);
    if (len4 >= length - STRING_OFFSET - len1 - len2 - len3 - 4) {
    	printf("error in len4");
        free(message -> fileordir_monitored);
        free(message -> fileordir_changed);
        free(message -> ifdir);
        free(message);
        return NULL;
    }
    // allocate memory for action
    message -> action = (char *) malloc(len4 + 1);
    if (message -> action == NULL) {
    	printf("error in ifdir memory allocation");
    	free(message -> fileordir_monitored);
    	free(message -> fileordir_changed);
    	free(message -> ifdir);
        free(message);
        return NULL;
    }
    // copy action into message
    strncpy(message -> action, buffer + STRING_OFFSET + len1 + len2 + len3 + 3, len4);
    message -> action[len4] = '\0';


    // calculate len of hostaddr
    int len5 = length - (STRING_OFFSET + len1 + len2 + len3 + len4 + 4);
    // allocate memory for hostaddr
    message -> hostaddr = (char *) malloc(len5 + 1);
    if (message -> hostaddr == NULL){
    	printf("error in hostaddr memory allocation");
    	free(message -> fileordir_monitored);
    	free(message -> fileordir_changed);
    	free(message -> ifdir);
    	free(message -> action);
        free(message);
        return NULL;
    }
    // copy hostaddr into message
    strncpy(message -> hostaddr, buffer + STRING_OFFSET + len1 + len2 + len3 + len4 + 4, len5);
    message -> hostaddr[len5] = '\0';

    return message;
}


// defined function for processing observer client action
int observer_client(char *saddr, int sport, char *fileordir){
	// initialize socket descriptor
	struct sockaddr_in address;
	char *hello = "observer client";

	// initialize signal handler
	struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = observer_handler;
    sigaction(SIGINT, &sa, NULL);

    // initialize the socket
	if ((observer_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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
	if (connect(observer_sock, (struct sockaddr *) &address, sizeof(address)) < 0) {
		perror("connect failed");
		return -1;
	}

	// send message to the socket
	send(observer_sock, hello, strlen(hello), 0);

    Message msg;
	msg.fileordir_monitored = fileordir;

    // get the IP address of current observer
	struct sockaddr_in self_address;
	int len = sizeof(self_address);
	getsockname(observer_sock, (struct sockaddr *) &self_address, &len);
	char my_IP[20];
	inet_ntop(AF_INET, &self_address.sin_addr, my_IP, sizeof(my_IP));
	msg.hostaddr = my_IP;

	// initialize inotify file descriptor
	int inotify_fd = inotify_init();
	if (inotify_fd < 0){
		printf("inotify failed\n");
		return -1;
	}

	// initialize watch descriptor
	int watch_des = inotify_add_watch(inotify_fd, fileordir, IN_ALL_EVENTS);
	if (watch_des == -1){
		printf("watch failed\n");
		return -1;
	}
	char inotify_buffer[INOTIFY_BUFFER_SIZE];

    // keep watching
	while (1){
		// read changes from inotify_fd
		int bytes_read = read(inotify_fd, inotify_buffer, INOTIFY_BUFFER_SIZE);
		if (bytes_read < 0){
			printf("read failed\n");
			return -1;
		}

		struct inotify_event *event = (struct inotify_event *) inotify_buffer;
		// if changes occured on directory
		if (event -> mask & IN_ISDIR) msg.ifdir = "IN_ISDIR";
		// if changes occured on file
		else msg.ifdir = "";

        // check wich event occurs
		if (event -> mask & IN_CREATE) msg.action = "IN_CREATE";
		if (event -> mask & IN_DELETE) msg.action = "IN_DELETE";
		if (event -> mask & IN_ACCESS) msg.action = "IN_ACCESS";
		if (event -> mask & IN_MODIFY) msg.action = "IN_MODIFY";
		if (event -> mask & IN_ATTRIB) msg.action = "IN_ATTRIB";
		if (event -> mask & IN_OPEN) msg.action = "IN_OPEN";
		if (event -> mask & IN_CLOSE_WRITE) msg.action = "IN_CLOSE_WRITE";
		if (event -> mask & IN_CLOSE_NOWRITE) msg.action = "IN_CLOSE_NOWRITE";
		if (event -> mask & IN_MOVED_FROM) msg.action = "IN_MOVED_FROM";
		if (event -> mask & IN_MOVED_TO) msg.action = "IN_MOVED_TO";
		if (event -> mask & IN_MOVE_SELF) msg.action = "IN_MOVE_SELF";
		if ((event -> mask & IN_DELETE_SELF)){
			printf("\nObserver Client Terminates.\n");
		    close(observer_sock);
		    exit(0);
		}

		// store the name of changed file/dir (if any)
		msg.fileordir_changed = event -> name;

		// store the current time
		struct timeval tv;
		gettimeofday(&tv, NULL);
		msg.time_sec = tv.tv_sec;
		msg.time_usec = tv.tv_usec;

        // serialize the struct and send it to server
		int buffer_len;
		uint8_t *send_buffer = serialize(&msg, &buffer_len);
		send(observer_sock, send_buffer, buffer_len, 0);
		free(send_buffer);
	}

	inotify_rm_watch(inotify_fd, watch_des);
    close(inotify_fd);

	return 0;
}