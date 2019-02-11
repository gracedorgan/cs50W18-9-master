# CS50 Maze Solver

## Team 9 - Feburary 2018

### avatar

### Compilation

To compile, simply `make`.

### Usage
Go down to the `src` directory, 
```./amstartup nAvatars difficulty hostname(flume.cs.dartmouth.edu)```

The *avatar* function is defined to decide movements of the maze and track where it has been. It is used by *amstartup* to get next step to send to the server.

```c
/*this method checks the hostname, then connects to the right server*/
static int *connectTo(const char *hostname, const int port);

/* this method first waits for one second for the sake of the graphics                       
 * output, then sends an appropriate avatar_move message to the                              
 * server.  It will exit if there is a failure to send the message                           
 * to the server.                                                                            
 */
void makeMove(XYPos pos);

/* returns the direction that the avatar should move in next in the                          
 * form of an integer.  Will return either M_NORTH, M_SOUTH,                                 
 * M_EAST, or M_WEST                                                                         
 */
uint32_t *getNextMove(XYPos *pos, int *avatarid, int *nAvatars);

/* returns the center of the dimensions given in the form of
 * a set of coordinates.
 */
XYPos *getCenter (XYPos * pos, int nAvatars);

/* this method iterates through the maze and prints a visual
 * representation of what the maze looks like
 */
void printMaze(XYPos *pos, int nAvatars);
```

### Implementation

We implement the *avatar* function by a three dimensional array to keep track of whether each coordinate has a wall. Our movement uses a combination of the right hand rule and keeping track of already explored locations to solve the maze.

The 'connectTo' method checks the host name, then attempts to establish a connection with the server

The 'maveMake' method sends the move given by 'getNextMove' to the server

The 'getNextMove' method returns the direction the avatar should move next using the right hand rule and keeping track of whether a spot has already been visited

The 'getCenter' method returns the center of the maze

The 'printMaze' method iterates through all the pixels of the maze to print a visul representation of the maze
###Exit Status
1 - not right number of arguments
2 - invalid number of avatars given
3 - invalid level of difficulty given
4 - invalid hostname given
5 - hostname given to connect function in invalid
6 - cannot open given socket on server
7 - cannot connect to stream from socket
8 - unable to write to socket
9 -  unable to send message to server
10 - Error reading response from server
11 - server response to any message from an unknown AvatarId
12 - Could not open dimensions file


### Assumptions

No assumptions beyond those that are clear from the spec.
