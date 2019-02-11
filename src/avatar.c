/*
 * avatar.c
 *
 * This file contains the definitions of the avatar class.
 *
 * usage: ./avatar avatarid nAvatars difficulty hostname mazePort logFile.log
 *
 * example: ./avatar 0 3 2 129.170.212.235 10829 Amazing_gevorg_3_2.log
 *
 * This class hold the logic for how an avatar in the maze makes a move and
 * prints that move to stdout if it is that avatars turn
 *
 * Team 9 - March 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <strings.h>
#include <pthread.h>
#include "file.h"
#include "memory.h"
#include "amazing.h"

static int MazeWidth = 0; // immediately initialized by reading a file
static int MazeHeight = 0; // immediately initialized by reading a file
int first = 0; // just to put in backtrace on first move
int lastX = -1; // keeps track of last move
int lastY = -1; // keeps track kof last move
static int lastMove = -1; //saves direction of last move
int count = 2; // incremented to give the backtrace something to follow backwards
int maze[500][500]; // holds all the walls of the maze

static int connectTo (const char *hostname, unsigned long *port);
int makeMove (XYPos * pos, int avatarid, int nAvatars, int comm_sock,
		int backtrace[MazeWidth][MazeHeight], FILE *log);
int getNextMove (XYPos * pos, int TurnId, int nAvatars,
		int backtrace[MazeWidth][MazeHeight]);
XYPos *getCenter (XYPos * pos, int nAvatars);
void printMaze(XYPos *pos, int nAvatars);
void initializeMaze();
void initializeBacktrace(int nAvatars, int backtrace[][MazeWidth][MazeHeight]);
void getDimensions();

/*********************************************************/
/* main(): 1. parse arguments; 2. initialize variables
 *         3. receive response message from server
 *         4. get optimal move and send to server
 *         5. write to logfiles all the moves and walls, and solve maze 
 */ 
	int
main (int argc, char **argv)
{
	if (argc != 7) {  // check the number of arguments
		fprintf (stderr,
				"usage: ./avatar avatarid nAvatars difficulty hostname mazePort logFile.log\n");
		exit (1);
	}
	getDimensions();
	FILE *log;
	log = fopen(argv[6], "a");

	//initialize variables
	initializeMaze();	
	//parse arguments, read into variables
	int avatarid = 0;
	sscanf (argv[1], "%d", &avatarid);
	int nAvatars = 0;
	sscanf (argv[2], "%i", &nAvatars);
	int difficulty = (int) *argv[3];
	sscanf (argv[3], "%i", &difficulty);
	char *hostname = argv[4];
	unsigned long p;
	sscanf (argv[5], "%lu", &p);
	unsigned long *port = &p;
	int backtrace[nAvatars][MazeWidth][MazeHeight];	//declare backtrace here

	// open connection - exit on error
	int comm_sock = connectTo(hostname, port);

	// read and print the server's response
	AM_Message am_ready;
	memset(&am_ready, 0, sizeof(am_ready));
	am_ready.type = htonl(AM_AVATAR_READY);
	am_ready.avatar_ready.AvatarId = htonl(avatarid);
	if (write(comm_sock, &am_ready, sizeof(AM_Message)) < 0) {
		perror ("could not send Avatar _READY\n");
		exit (9);
	} else {
		fprintf(log, "avatar %d successfully started\n", avatarid);
		printf("avatar ready message sent\n");
	}

	initializeBacktrace(nAvatars, backtrace);

	//execute until maze get solved
	while (true) {
		//read AM_message from server
		AM_Message response;
		if (read(comm_sock, &response, sizeof (AM_Message)) < 0) {
 			//if the server response is not read correctly
			perror ("Error reading response from server");
			exit (10);
		}
		else {
			unsigned long type = ntohl(response.type);
			if (type == AM_NO_SUCH_AVATAR){
	 			//if the rresponse is not the correct one
				perror("message received is NO_SUCH_AVATAR\n");
				exit(11);
			} else if (type == AM_AVATAR_TURN) {
				//if the response is telling us which avatar to move
				unsigned long TurnId = ntohl(response.avatar_turn.TurnId);
				XYPos *coords = response.avatar_turn.Pos;
				for (int i = 0; i <= nAvatars; i++) {
					coords[i].x = ntohl(coords[i].x);
					coords[i].y = ntohl(coords[i].y);
				}
				int lastId;
				if (TurnId == 0) {
					lastId = nAvatars - 1;
				} else {
					lastId = TurnId - 1;
				}
				if (lastX == coords[lastId].x && lastY == coords[lastId].y) {
					fprintf(log, "avatar %d hit a wall and did not move.\n", lastId);
					int x = lastX;
					int y = lastY;
					if (lastMove == M_WEST){
						maze[x][y + 1] += 2;
					} else if (lastMove == M_EAST) {
						maze[x + 1][y + 1] += 2;
					} else if (lastMove == M_SOUTH) {
						maze[x + 1][y + 1] += 1;
					} else if (lastMove == M_NORTH) {
						maze[x + 1][y] += 1;
					}
				} else {
					if (first > 0) {
						backtrace[lastId][lastX][lastY] = count;
						count++;
						first++;
						if (first == nAvatars) {
							first = -1;
						}
					}
					// adds maze if backtracing
					if (backtrace[lastId][coords[lastId].x][coords[lastId].y] == 0) {
						backtrace[lastId][coords[lastId].x][coords[lastId].y] = count;
						count++;
					} else {
						backtrace[lastId][lastX][lastY] = 1;
					}
					fprintf(log, "avatar %d completed their move.\n", avatarid);
				}
				lastX = coords[TurnId].x;
				lastY = coords[TurnId].y;
				if (TurnId == avatarid){
					//if it is this avatar's turn
					system("clear");
					printMaze(coords, nAvatars);
					lastMove = makeMove(coords, avatarid, nAvatars, comm_sock, backtrace[TurnId], log); 
					char *dir;
					if (lastMove == M_EAST) {
						dir = "east";
					}else if (lastMove == M_WEST) {
						dir = "west";
					}else if (lastMove == M_SOUTH) {
						dir = "south";
					}else if (lastMove == M_NORTH) {
						dir = "north";					  
					}
					fprintf(log, "avatar %d has attempted to move %s from its current position at (%d,%d)\n", 
							avatarid, dir, coords[TurnId].x, coords[TurnId].y);

				}else {
					lastMove = getNextMove(coords, TurnId, nAvatars, backtrace[TurnId]);
				}
			}
			//check if response is of any error types
			else if (type == AM_AVATAR_OUT_OF_TURN) {
				perror("avatar out of turn");
			} else if (type == AM_TOO_MANY_MOVES) {
				perror("too many moves");
			} else if (type == AM_SERVER_DISK_QUOTA) {
				perror("disk quota error");
			} else if (type == AM_SERVER_OUT_OF_MEM) {
				perror("out of memory");
			} else if (type == AM_UNKNOWN_MSG_TYPE) {
				perror("unknown message type");
			} else if (type == AM_SERVER_TIMEOUT) {
				perror("server timeout");
			}

			//check if we have solved the maze
			else if (AM_MAZE_SOLVED) {
				//get information from response
				//print mazeSolved message
				int nMoves = ntohl(response.maze_solved.nMoves);
				int Hash = ntohl(response.maze_solved.Hash);
				fprintf(log, "Maze Solved, navatars: %d, difficulty: %d, total moves: %d, hash number: %d !\n"
						,nAvatars, difficulty, nMoves, Hash);
				if (avatarid == 0) {
					printf("Maze Solved\n");
				}
				break;
			} else if (AM_UNEXPECTED_MSG_TYPE) {
				perror("unexpected message type");
			}
		}
	}
	fclose(log);
	exit(0);
}

/***********************************************************/
/* 
 * getDimensions - open the dimensions file and put the info there
 * into MazeWidth and MazeHeight
 * fails if file cannot be opened
 */
void getDimensions(){
	FILE *dimensions = fopen ("dimensions", "r");   //try to open the file
	if (dimensions == NULL){           //if the file can't be opened
		fprintf (stderr, "dimensions file could not be read\n");
		exit (13);
	}
	char *line = readlinep (dimensions);    //get line
	sscanf (line, "%d %d", &MazeWidth, &MazeHeight);  //read into variables
	fclose(dimensions);
	free(line);
}

/************************************************************/
/* 
 * initialize maze - fills maze up with zeros, then forms the 
 * outer borders of the maze
 */
void initializeMaze() {
	//initialize variables
	for (int i = 0; i <= MazeWidth; i++) {
		for (int j = 0; j <= MazeHeight; j++) {
			maze[i][j] = 0;
		}
	}
	for (int i = 1; i <= MazeWidth; i++) {
		maze[i][0] = 1;
		maze[i][MazeHeight] = 1;
	}
	for (int j = 1; j <= MazeHeight; j++) {
		maze[0][j] = 2;
		maze[MazeWidth][j] = 2;
	}
	maze[MazeWidth][MazeHeight] = 3;
}

/***********************************************************/
/*
 * initializeBacktrace - fills the backtrace with zeros
 */
void initializeBacktrace(int nAvatars, int backtrace[][MazeWidth][MazeHeight]) {
	//intialize backtrace[][][] to be all 0
	for (int i = 0; i < MazeWidth; i++) {
		for (int j = 0; j < MazeHeight; j++) {
			for (int k = 0; k < nAvatars; k++) {
				backtrace[k][i][j] = 0;
			}
		}
	}
}


/* ********************************************************* */
/* connectTo(): get avatar to connect to socket
*/ 
static int connectTo (const char *hostname, unsigned long *port)
{
	struct hostent *hostp = gethostbyname (hostname);
	if (hostp == NULL) {	//if hostname is unknown
		fprintf (stderr, "avatar.c: unknown host '%s'\n", hostname);
		exit(5);
	}
	// Initialize fields of the server address
	struct sockaddr_in server;	// address of the server
	server.sin_family = AF_INET;
	bcopy (hostp->h_addr_list[0], &server.sin_addr, hostp->h_length);
	server.sin_port = htons(*port);
	int comm_sock = socket (AF_INET, SOCK_STREAM, 0);
	if (comm_sock < 0) {
		perror ("opening socket");
		exit(6);
	}
	if (connect (comm_sock, (struct sockaddr *) &server, sizeof (server)) < 0) {
		perror ("connecting stream socket");
		exit(7);
	}
	// writ connected message to log file
	return comm_sock;
}

/**********************************************************/
/* 
 * makeMove: get best move from getNextMove, send move message to server
*/ 
int makeMove (XYPos * pos, int avatarid, int nAvatars, int comm_sock,
		int backtrace[MazeWidth][MazeHeight], FILE *log)
{
	AM_Message message;
	memset(&message, 0, sizeof(message));
	message.type = htonl(AM_AVATAR_MOVE);
	message.avatar_move.AvatarId = htonl((uint32_t) avatarid);
	int nextMove = getNextMove(pos, avatarid, nAvatars, backtrace);
	message.avatar_move.Direction = htonl(nextMove);
	if (write(comm_sock, &message, sizeof (AM_Message)) < 0) {
		//send the message
		perror ("could not send AM_AVATAR_MOVE\n");
		exit (9);
	}
	return nextMove;
}

/*************************************************************/
/* 
 * getNextMove(): compute the optimal next move based on 
 * 1. position difference between avatar and center
 * 2. maze walls
 * 3. backtrace values
 * return optimal move or backtrack
 */ 
int getNextMove (XYPos * pos, int TurnId, int nAvatars, 
		int backtrace[MazeWidth][MazeHeight])
{
	XYPos *center = getCenter (pos, nAvatars);
	int xDiff = pos[TurnId].x + 1 - center->x;
	int yDiff = pos[TurnId].y + 1 - center->y;
	int x = pos[TurnId].x + 1;
	int y = pos[TurnId].y + 1;
	//if avatar reaches center, keep it there by sending M_NULL_MOVE
	if (xDiff == 0 && yDiff == 0) {
		free(center);	
		return M_NULL_MOVE;
	}
	// first priority is the direction towards center that's more units away
	if (abs (xDiff) >= abs (yDiff) && xDiff <= 0 
			&& maze[x][y] < 2 && backtrace[x][y - 1] == 0) {
		free(center);
		return M_EAST;
	} else if (abs (xDiff) >= abs (yDiff) && xDiff >= 0       
			&& maze[x - 1][y] < 2 && backtrace[x - 2][y - 1] == 0) {
		free(center);
		return M_WEST;
	} else if (abs (xDiff) <= abs (yDiff) && yDiff <= 0 
			&& (maze[x][y] == 0 || maze[x][y] == 2) && backtrace[x - 1][y] == 0) {
		free(center);
		return M_SOUTH;
	} else if (abs (xDiff) <= abs (yDiff) && yDiff >= 0 && (maze[x][y - 1] == 0
				|| maze[x][y - 1] == 2) && backtrace[x - 1][y - 2] == 0) {
		free(center);
		return M_NORTH;
	}

	// second priority is other direction towards center
	if (abs (xDiff) <= abs (yDiff) && xDiff <= 0 
			&& maze[x][y] < 2 && backtrace[x][y - 1] == 0) {
		free(center);
		return M_EAST;
	} else if (abs (xDiff) <= abs (yDiff) && xDiff >= 0 
			&& maze[x - 1][y] < 2 && backtrace[x - 2][y - 1] == 0) {
		free(center);
		return M_WEST;
	} else if (abs (xDiff) >= abs (yDiff) && yDiff <= 0 
			&& (maze[x][y] == 0 || maze[x][y] == 2) && backtrace[x - 1][y] == 0) {
		free(center);
		return M_SOUTH;
	} else if (abs (xDiff) >= abs (yDiff) && yDiff >= 0 && (maze[x][y - 1] == 0
			       || maze[x][y - 1] == 2) && backtrace[x - 1][y - 2] == 0) {
		free(center);
		return M_NORTH;
	}
	// third priority is opposite second priority
	if (abs (xDiff) <= abs (yDiff) && xDiff <= 0 
			&& maze[x - 1][y] < 2 && backtrace[x - 2][y - 1] == 0) {
		free(center);
		return M_WEST;
	} else if (abs (xDiff) <= abs (yDiff) && xDiff >= 0 
			&& maze[x][y] < 2 && backtrace[x][y - 1] == 0) {
		free(center);
		return M_EAST;
	} else if (abs (xDiff) >= abs (yDiff) && yDiff <= 0 && (maze[x][y - 1] == 0
				|| maze[x][y - 1] == 2) && backtrace[x - 1][y - 2] == 0) {
		free(center);
		return M_NORTH;
	}else if (abs (xDiff) >= abs (yDiff) && yDiff >= 0 && (maze[x][y] == 0
				|| maze[x][y] == 2) && backtrace[x - 1][y] == 0) {
		free(center);
		return M_SOUTH;
	}
	// fourth priority only other direction if no wall or backtrace
	if (abs (xDiff) >= abs (yDiff) && xDiff <= 0 
			&& maze[x - 1][y] < 2 && backtrace[x - 2][y - 1] == 0) {
		free(center);
		return M_WEST;
	}else if (abs (xDiff) >= abs (yDiff) && xDiff >= 0 
			&& maze[x][y] < 2 && backtrace[x][y - 1] == 0) {
		free(center);
		return M_EAST;
	}else if (abs (xDiff) <= abs (yDiff) && yDiff <= 0 && (maze[x][y - 1] == 0
			|| maze[x][y - 1] == 2) && backtrace[x - 1][y - 2] == 0) {
		free(center);
		return M_NORTH;
	}else if (abs (xDiff) <= abs (yDiff) && yDiff >= 0 
			&& (maze[x][y] == 0 || maze[x][y] == 2) && backtrace[x - 1][y] == 0) {
		free(center);
		return M_SOUTH;
	}

	//build moves[] scores based on backtrace
	int moves[4];
	if (maze[x - 1][y] < 2) {
		moves[0] = backtrace[x - 2][y - 1];
	} else {
		moves[0] = 0;
	}
	if (maze[x][y - 1] == 0 || maze[x][y - 1] == 2) {
		moves[1] = backtrace[x - 1][y - 2];
	} else {
		moves[1] = 0;
	}
	if (maze[x][y] == 0 || maze[x][y] == 2) {
		moves[2] = backtrace[x - 1][y];
	} else {
		moves[2] = 0;
	}
	if (maze[x][y] < 2) {
		moves[3] = backtrace[x][y - 1];
	} else {
		moves[3] = 0;
	}
	//find the move with highest score
	int j = 0;
	for (int i = 0; i < 4; i++) {
		if (moves[i] > moves[j]) {
			j = i;
		}
	}
	free(center);
	// if no moves then backtrack
	return j;
}

/**********************************************/
/*
 * getCenter(): helper function to get center of maze
 */
XYPos *getCenter (XYPos * pos, int nAvatars)
{
	XYPos *center = malloc(sizeof(XYPos));
	center->x = MazeWidth/2;
	center->y = MazeHeight/2;
	return center;
}

/*********************************************/
/* 
 * printMaze(): print the maze(avatar and walls) in ASCII UI. 
 */ 
void printMaze(XYPos *pos, int nAvatars)
{
	printf("\n");
	for (int y = 0; y <= MazeHeight; y++) {
		for (int x = 0; x <= MazeWidth; x++ ) {
			//print the avatar if it's there
			int id = -1;
			for (int d = 0; d < nAvatars; d++) {
				if(pos[d].x + 1 == x && pos[d].y + 1 == y) {
					id = d;
				}
			}
			if (id != -1) {
				if (maze[x][y] >= 2) {
					printf("%d|", id);
				} else if (maze[x][y] == 1) {
					printf("%d_", id);
				} else {
					printf("%d ", id);
				}
				continue;
			}
			//right wall
			if (maze[x][y] == 2) {
				printf(" |");
			} else if (maze[x][y] == 1) {//bottom wall
				printf("__");
			} else if (maze[x][y] > 2) {//bottom and right wall
				printf("_|");
			} else {
				printf("  ");
			}
		}
		printf("\n");
	}
}
