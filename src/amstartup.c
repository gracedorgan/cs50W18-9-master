/*
 * amstartup.c
 *
 * This file contains the definitions of the amstartup class.
 *
 * usage: ./amstartup nAvatars difficulty hostname
 *
 * This class reads the document files produced by the crawler and
 * builds an index
 *
 * Penny Liu, Andrew Shedd - March 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <unistd.h>	      // read, write, close
#include <strings.h>	      // bcopy, bzero
#include <netdb.h>	      // socket-related structures
#include <time.h>
#include <sys/wait.h>	      // wait() and waitpid()
#include "file.h"
#include "memory.h"
#include "amazing.h"

/* main(): call parsArgs; send AM_INIT message; receive AM_INIT_OK message
 * create log file; create given number of cline avatars with mazeport
 */ 
int main(int argc, char **argv)
{
	// initialize parameters
	int nAvatars = 0;
	int difficulty = 0;
	char *hostname = count_malloc(strlen(argv[3]) + 1);

	// parse and check all the parameters
	if (argc != 4) {  //           check the number of arguments
		fprintf(stderr, "usage: ./amstartup nAvatars difficulty hostname\n");
		exit(1);
	}
	if (sscanf(argv[1], "%d", &nAvatars) != 1) {
		fprintf(stderr, "error: invalid number of avatars\n");
		exit(2);
	}
	if (nAvatars < 1 || nAvatars > AM_MAX_AVATAR) {
		fprintf(stderr, "error: invalid number of avatars\n");
		exit(2);
	}
	if (sscanf(argv[2], "%d", &difficulty) != 1) {
		fprintf(stderr, "error: invalid difficulty\n");
		exit(3);
	}
	if(sscanf(argv[3], "%s", hostname) != 1) {
		fprintf(stderr, "error: invalid hostname\n");
		exit(4);
	}

	// Look up the hostname specified on command line
	struct hostent *hostp = gethostbyname(hostname);
	if (hostp == NULL) {  //if hostname is unknown
		fprintf(stderr, "Unknown host: '%s'\n", hostname);
		exit(5);
	}

	// Initialize fields of the server address
	struct sockaddr_in server;  // address of the server
	server.sin_family = AF_INET;
	bcopy(hostp->h_addr_list[0], &server.sin_addr, hostp->h_length);
	server.sin_port = htons(atoi(AM_SERVER_PORT));

	// Create socket
	int comm_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (comm_sock < 0) {
		perror("opening socket");
		exit(6);
	}
	// And connect that socket to that server   
	if (connect(comm_sock, (struct sockaddr *) &server, 
				sizeof(server)) < 0) {
		perror("connecting stream socket");
		exit(7);
	}
	printf("Connected!\n");

	//create an AM_INIT message
	AM_Message message;
	memset(&message,'0',sizeof(AM_Message));
	message.type = htonl(AM_INIT);
	message.init.nAvatars = htonl(nAvatars);
	message.init.Difficulty = htonl(difficulty);

	//write AM_INIT message to server;
	if (write(comm_sock, &message, sizeof(AM_Message)) < 0) {
		perror("writing on stream socket");
		exit(8);
	}

	unsigned long MazePort;
	unsigned long MazeWidth;
	unsigned long MazeHeight;

	//read AM_message from server
	AM_Message init_response;
	int nbytes;
	if ((nbytes = read(comm_sock, &init_response, sizeof(AM_Message))) < 0) {
		perror("Reading response from server");
		exit(9);
	} else {
		printf("Read from server nbytes: %i\n", nbytes);
		unsigned long type = ntohl(init_response.type);
		printf("response type is %li\n", type);

		if (type == AM_INIT_FAILED){
			//check ErrNum and print error type        
			if (ntohl(init_response.init_failed.ErrNum) 
					== AM_INIT_ERROR_MASK) {
				perror("ERROR_MASK");
			}
			if (ntohl(init_response.init_failed.ErrNum) 
					== AM_INIT_TOO_MANY_AVATARS) {
				perror("TOO_MANY_AVATARS");
			} 
			if (ntohl(init_response.init_failed.ErrNum) 
					== AM_INIT_BAD_DIFFICULTY) {
				perror("BAD_DIFFICULTY");
			}
			exit(10);

		} else if (type == AM_INIT_OK) {
			fprintf(stdout, "AM_INIT_OK received\n");
			MazePort = ntohl(init_response.init_ok.MazePort);
			MazeWidth = ntohl(init_response.init_ok.MazeWidth);
			MazeHeight = ntohl(init_response.init_ok.MazeHeight); 
		}
		//Create a log file
		FILE* logFile;
		char* userId = getenv("USER");
		assertp(userId, "userId");

		FILE *dimensions = fopen("dimensions", "w");
		fprintf(dimensions, "%d %d", (int) MazeWidth, (int) MazeHeight);
		fclose(dimensions);
		//assume nAvatars and difficulty are at most 20 digits
		char* logFileName = count_malloc(15 + strlen(userId) + 
				20  + 20  + strlen(userId)); 
		assertp(logFileName, "logFileName");

		//build logFileName and open logFile for writing
		sprintf(logFileName,"../results/Amazing_%s_%i_%i.log", 
				userId, nAvatars, difficulty); 
		fprintf(stdout, "%s\n", logFileName);
		logFile = fopen(logFileName, "w");
		assertp(logFile, "logFile");

		//document time    
		time_t now;
		time(&now);
		fprintf(stdout, "%s\n", ctime(&now));
		//start processes for each avatar
		for (int i = 0; i < nAvatars; i++) {
			char startAvatarCmd[500] = {0}; 
			//assume each command is at most 500 char      
			sprintf(startAvatarCmd, "./avatar %i %i %i %s %li %s", 
					i, nAvatars, difficulty, hostname, MazePort, logFileName);
			fprintf(logFile, "Starting avatar %i: ", i);
			fprintf(logFile, "%s, %li, %s\n", userId, MazePort, ctime(&now));	
			fprintf(stdout, "starting avatar: %s\n", startAvatarCmd);
			if (fork()) {
				system(startAvatarCmd);
				break;
			}
		}
		//reap any zombies
		while (waitpid(0, NULL, WNOHANG) > 0) {
			;
		}
		fclose(logFile);
		count_free(hostname);
		count_free(logFileName);
		exit(0);
	}
}
