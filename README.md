# 3d-Spatial-Partitioning-and-Collision-Detection-using-Octrees

# Octree Project Execution Guide

## Instructions to Compile and Execute the Octree Project in a Terminal

1. Navigate to the Project Directory
- Open a terminal window.
- Use the `cd` command to navigate to the `Code_file_Group_14` folder where your project files are located. Ensure u have opened the folder containing files.

2. Compile the Code
- Use gcc to compile the code files. In the terminal, run the following command for each file:

  a. gcc -c filename.c

  Perform this for all .c files (octree.c, game.c, study_operations.c). This will make filename.o which is necessary for further run.

  b. gcc -o program1 octree.o study_operations.o

  This will generate an executable named program1.

  c. gcc -o program2 octree.o game.o

  This will generate an executable named program2.

3. Run the Compiled Program
- To run the study_operations.c code, use:
./program1

- To run the game.c code, use:
./program2

4. Contents of folder:
Everytime, the tree is printed in the Octree.txt file. Also the points found in the specified cube in the range query are given in RangeQuery.txt file. 

//The octree.c file contains all the functions used in the project. The octree.h header is used in other two .c files to call the functions.
	##Important: We have defined the size of 3d space with 'MAX_SIZE', Maximum number of points in a node as 'MAX_POINTS', Maximum Depth as 'MAX_DEPTH', Step size for moving point in game.c as 'STEP' and Size of box for collision detection as 'COLLISION_SIZE' in the octree.h file. They are used as global variables. 


//The study_operations.c file is used to study the insert, search, delete, range query, nearest neighbor, collision_detection.
Enter the command (i: insert, d: delete, s: search, r: range query, n: nearest neighbor, f: free, c: collision, q: quit):
Message will be given asking to select operation.

insert:
	Enter the number of points to insert and insert the specified number of points in space separated format.
	The point insertion can be dynamically seen in the Octree.txt file. It can also be viewed on terminal that the point is inserted or not and at what depth.
search:
	Enter the point to search. Result can be viewed in terminal.
delete: 
	Enter the point to delete. Result can be viewed in Octree.txt and terminal.
range query: 
	Enter the minimum co-ordinates and maximum co-ordinates of the cube.
	If min=(x,y,z) and max=(x',y',z') then every point with x co-ordinates greater than or equal to x and less than x', y co-ordinates greater than or equal to y and less than y' 
	& z co-ordinates greater than or equal to z and less than z' are printed. Total number of points found is also printed.
nearest neighbor search:
	Enter the point. It's nearest neighbour will be given.
quit: 
	Quits the code.


//The game.c file is a small simulation of a game where there are points in 3d space representing objects. The points are added initially in 3d space from random1.txt.
The user can select any one point of them and can move it in 3d space using 'w','a','s','d,'e','f'.
The controls are as follows:
'w': Increase the y co-ordinates of the point by the amount of STEP
's': Decrease the y co-ordinates of the point by STEP
'd': Increase the x co-ordinates of the point by STEP
'a': Decrease the x co-ordinates of the point by STEP
'e': Increase the z co-ordinates of the point by STEP
'f': Decrease the z co-ordinates of the point by STEP

You can see the point moving in octree by keeping the Octree.txt file open and running code in terminal simultaneously.
If there is an instance where our point is moved to a place where another point is present inside a specific range (Determined by 'COLLISION_SIZE') then collision is detected and point is reverted back.
Also, if the point moves out of bounds i.e. moves outside the maximum limit of 3d space, then too it is reverted back. 


GIVEN BELOW IS A TEST CASE FOR study_operations.c FUNCTION.
### Testcase for study_operations.c: 

Enter the command (i: insert, d: delete, s: search, r: range query, n: nearest neighbor, f: free, c: collision, q: quit):i
Enter the number of points to insert: 12
(-50 -50 100)
(900 -900 -900)
(900 900 900)
(-100 -100 100)
(-1 2 3)
(0 0 0)
(750 750 750)
(800 800 800)
(500 500 500)
(750 750 800)
(975 975 1000)
(1000 1000 975)
*Don't input round brackets
(each time when you insert a point you can view the octree formed in Octree.txt)
U can observe node division in the tree.

Next u can perform other required operations.


### Additional Notes
- Ensure that `octree.c`, `octree.h`, `main.c` and `main2.c` are in the same directory.
