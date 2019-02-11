#!/bin/bash
chmod +x test.sh
#Grace Dorgan Jan 2018
#An indexer testing script

#test wrong number of params
./amstartup &> /dev/null
if [ $? -ne 0 ]
then
	echo passed test with no params
else
	echo did not pass test with no params
fi
#test with too many params
./amstartup 1 2 3 flume &> /dev/null
if [ $? -ne 0 ]
then
        echo Passed test with too many params
else
        echo did not pass test with too many params
fi

#test with difficulty over 9

./amstartup 2 11 flume.cs.dartmouth.edu &> /dev/null
if [ $? -ne 0 ]
then
        echo Passed test with difficulty over 9
else
        echo did not pass test with difficulty over 9
fi

#Test with over 10 avatars

./amstartup 35 2 flume.cs.dartmouth.edu &> /dev/null
if [ $? -ne 0 ]
then
        echo Passed test with over 10 avatars
else
        echo did not pass test with over 10 avatars
fi

#Test with bad hostname

./amstartup 2 2 flume.grace.dorgan &> /dev/null
if [ $? -ne 0 ]
then
        echo Passed test with bad hostname
else
        echo did not pass test with bad hostname
fi

#Solved mazes of difficulty up to 3 with 3 Avatars,
#level 1
./amstartup 3 1 flume.cs.dartmouth.edu &> /dev/null
if [ $? -ne 0 ]
then
        echo did not pass test of solving level 1
else
        echo passed test of solving level 1
fi

#level 2
./amstartup 3 2 flume.cs.dartmouth.edu &> /dev/null
if [ $? -ne 0 ]
then
        echo did not pass test of solving level 2
else
        echo passed test of solving level 2
fi

#level 3
./amstartup 3 3 flume.cs.dartmouth.edu &> /dev/null
if [ $? -ne 0 ]
then
        echo did not pass test of solving level 3
else
        echo passed test of solving level 3
fi

#Solved mazes of difficulty up to 5 with 4 or more Avatars
#level 4
./amstartup 5 4 flume.cs.dartmouth.edu &> /dev/null
if [ $? -ne 0 ]
then
        echo did not pass test of solving level 4
else
        echo passed test of solving level 4
fi

#level 5
./amstartup 5 5 flume.cs.dartmouth.edu &> /dev/null
if [ $? -ne 0 ]
then
        echo did not pass test of solving level 5
else
        echo passed test of solving level 5
fi

#Solved mazes of difficulty 7+ with 4 or more Avatars
#level 6
./amstartup 5 6 flume.cs.dartmouth.edu &> /dev/null
if [ $? -ne 0 ]
then
        echo did not pass test of solving level 6
else
        echo passed test of solving level 6
fi

#level 7
./amstartup 5 7 flume.cs.dartmouth.edu &> /dev/null
if [ $? -ne 0 ]
then
        echo did not pass test of solving level 7
else
        echo passed test of solving level 7
fi

#level 8
./amstartup 5 8 flume.cs.dartmouth.edu &> /dev/null
if [ $? -ne 0 ]
then
        echo did not pass test of solving level 8
else
        echo passed test of solving level 8
fi

#level 9
./amstartup 5 9 flume.cs.dartmouth.edu &> /dev/null
if [ $? -ne 0 ]
then
        echo did not pass test of solving level 9
else
        echo passed test of solving level 9
fi

echo done with testing!

