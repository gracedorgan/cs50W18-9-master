# Maze Solver Design Spec
## Team 9

## AMStartup.c

### User Interface
The AMStartup’s only interface with the user is a graphical interace to show the progression of the maze.

### Inputs and Outputs
Input: This program takes nothing besides command-line parameters

`./AMStartup nAvatars Difficulty Hostname`

For example:

`$ ./AMStartup 5 4 flume.cs.dartmouth.edu`

Output:
a display of the maze that shows the positions of each of the avatars and the walls that they find

### Functional decomposition into modules
Contains the following modules or functions:
 * main, parses the arguments, send the initialization message, create the avatars, create the display, create the log file

### Pseudo code for AMStartup
1. parses arguments
2. sends an AM_INIT message to the server at the AM_SERVER_PORT specifying: nAvatars - the desired number of Avatars in the maze, and Difficulty - the difficulty of the maze.
3. receive a server response message. Check message type.
  1. If message is AM_INIT_FAILED, end game.
  2. Proceed only If message is AM_INIT_OK containing a unique MazePort which is the TCP/IP Port number, the MazeWidth, and MazeHeight of the new maze (two more integers).
    1. create a log file
    2. create the given number of client avatars with the Mazeport
    3. start each client program


## Avatar
1. main()
 1. initialize the parameters
 2. initialize the data structures
 3. sends an AM_AVATAR_READY message containing its assigned AvatarId to the server.
 4. Get turn ID and position
 5. Listen
The following process continues unless: an Avatar’s socket connection to the server is broken, the maximum number of moves (a function of AM_MAX_MOVES and Difficulty) is exceeded, the server’s AM_WAIT_TIME timer expires, or the server determines that all of the Avatars are located at the same (x,y) position, meaning the maze has been solved.:
 6. receives an identical AM_AVATAR_TURN message. This message contains each Avatar’s current (x,y) position in the maze and a TurnID indicating which Avatar is to move first.
  1. Store current position in a temporary var
  2. update wall data based on locations
  3. if this avatars turn
   1. sends an AM_AVATAR_MOVE message to the server specifying in which direction it wishes to move. That direction will be determined by the heuristics the team comes up with for guiding the Avatars to find each other in the maze.
   2. Update the graphic, printing a wall if it hasn’t moved, printing its new position if it has 
   3. Write the move and its results to logfile 
  4. else just get move data

This process will continue until one of the following occurs:

When the maze is solved, the server sends an AM_MAZE_SOLVED message to all of the Avatars. The server then frees all the data structures relating to this maze and closes the MazePort. Upon receiving this AM_MAZE_SOLVED message, the Avatars ensure that the AM_MAZE_SOLVED message is written to a log file once.

Whether the maze is solved or one of the other exit conditions occurred, the Avatars should log their success/progress, close any files, free any allocated memory, etc., and then exit.i


## Dataflow through modules
1. AMStartup
3. createAvatars
Avatar
1. main
 1. initialize data structures
 2. Listen
  1. update walls and backtrace
  2. printMaze
  3. makeMove if necessary
  4. findMove
  

## getNextMove Algorithm
1. each of the avatars attempts to find in the middle of the maze
2. try to make a move in a direction towards the center if there is no wall and no backtrace there
3. if failed try second best direction
4. if failed try third best direction
5. if failed try fourth best direction
6. if all directions fail then backtrack until a move can be made with no wall or backtrace and try the one most towards the center


## Major data structures
 * a 2d array of ints that holds the positions of all walls and dead ends in the maze
 * array of coordinates for avatars
 * a 3d array for holding all of the 

## Testing plan
1. Unit test every module 
2. Check edge cases: 
  1. navatar input is 1: should start up a maze and an avatar as usual 
  2. Navatar input is max_navatars - 1
  3. Navatar input is large, maze difficulty is small 
3. Test difficulty 0 - 4
4. Test difficulty 5 - 7
5. Test difficulty greater than 7
