#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/inotify.h>
#include <sys/time.h>

typedef struct{
	long time_sec; // tv_sec of corresponding host
	int time_usec; // tv_usec of corresponding host
	char *fileordir_monitored; // the file or directory the observer is watching

	char *fileordir_changed; // the file of directory in a directory has benn changed
	char *ifdir; // check IN_ISDIR or not
	char *action; // action happened on certain file or directory
	char *hostaddr; // IP address of observer
} Message;

void observer_handler (int sig);

uint8_t *serialize(Message *message, int *length);
Message *deserialize(uint8_t *buffer, int length);
// Message *deserialize_next(uint8_t *buffer, int len, int *end);

int observer_client(char *saddr, int sport, char *fileordir);