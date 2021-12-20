#include "server.h"
#include "observer.h"
#include "user.h"

// defined main function
int main(int argc, char *argv[]){
	// check validity
	if (argc < 2){
		printf("%s\n", "Invalid Input Argument.");
		exit(-1);
	}

	// if the program acts as a server
	if (argv[1][0] == '-' && argv[1][1] == 's'){
		float interval;
		int sport = -1;
		char *logfile = NULL;

		// if two optional arguments are included
		if (argc == 4){
			// check validity of interval argument
			if (argv[2][0] == '-' && argv[2][1] == 't'){
				interval = atof(argv[3]);
			} else {
				printf("%s\n", "Invalid Input Argument.");
				exit(-1);
			}
		}
		// if one optional argument is included
		else if (argc == 6){
			// check validity of arguments
			if (argv[2][0] == '-' && argv[2][1] == 't' && 
				argv[4][0] == '-' && argv[4][1] == 'p'){
				interval = atof(argv[3]);
				sport = atoi(argv[5]);
			} else if (argv[2][0] == '-' && argv[2][1] == 't' &&
				       argv[4][0] == '-' && argv[4][1] == 'l'){
				interval = atof(argv[3]);
				logfile = argv[5];
			} else {
				printf("%s\n", "Invalid Input Argument.");
				exit(-1);
			}
		}
		// if no optional argument included
		else if (argc == 8){
			// check validity of arguments
			if (argv[2][0] == '-' && argv[2][1] == 't' && 
				argv[4][0] == '-' && argv[4][1] == 'p' && 
				argv[6][0] == '-' && argv[6][1] == 'l'){
				interval = atof(argv[3]);
				sport = atoi(argv[5]);
				logfile = argv[7];
			} else {
				printf("%s\n", "Invalid Input Argument.");
				exit(-1);
			}
		}
		else {
			printf("%s\n", "Invalid Input Argument.");
			exit(-1);
		}

		// invoke server function
		server(interval, sport, logfile);
	}

    // if the program acts as an observer client
	else if (argv[1][0] == '-' && argv[1][1] == 'o'){
		char *saddr;
		int sport;
		char *fileordir;

		// check if number of arguments is correct
		if (argc != 5){
			printf("%s\n", "Invalid Input Argument.");
			exit(-1);
		}

		saddr = argv[2];
		sport = atoi(argv[3]);
		fileordir = argv[4];

		// invoke observer_client function
		observer_client(saddr, sport, fileordir);
	}

    // if the program acts as an user client
	else if (argv[1][0] == '-' && argv[1][1] == 'u'){
		char *saddr;
		int sport;

		// check if number of arguments is correct
		if (argc != 4){
			printf("%s\n", "Invalid Input Argument.");
			exit(-1);
		}

        saddr = argv[2];
		sport = atoi(argv[3]);

		// invoke user_client fucntion
		user_client(saddr, sport);
	}

	else {
		printf("%s\n", "Invalid Input Argument.");
	}

	return 0;
}