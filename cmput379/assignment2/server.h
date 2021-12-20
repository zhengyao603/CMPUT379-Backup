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

void *observer_connection(void *socket_fd);

void *user_connection(void *socket_fd);

int server(float interval, int sport, char *logfile);