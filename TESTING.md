## CS50 Maze Challenge
## Team 9, Feb 2018

### Testing 'amstartup' and 'avatar'

To test our *amstartup* and *avatar* we wrote a test program, [test.sh](test.sh), 
to check the edge cases of the command line parameters and to check that it
solved varying maze difficulty levels with varying numbers of avatars.

This test program does not take input, but provides it to *amstartup*.
All output from *amstartup* and *avatar* is redirected to /dev/null, and the 
output that goes to stdout is just a series of print statements saying 
whether or not it passes the mentioned test case.

test.sh can be called using the *make test* command, which will redirect all

the results of the test are in the results directory and printed to stdout

### Limitations
If the program is unable to solve a certain maze, the user will not know
why, as all output is directed to /dev/null, which includes the printout
of the maze.  This is somewhat addressed by the fact that the log files
will still be made and will be available to be read.
