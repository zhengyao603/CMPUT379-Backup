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

void user_handler(int sig);

int user_client(char *saddr, int sport);