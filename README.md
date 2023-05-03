# News group system

## How to compile and run

### Compiling the program

1. Navigate to the src/ directory
2. Run the following commands to compile the program
    > `make` compiles and links the object files\
    > `make install`  compiles the and links the object files as well as moves the runnable files to the bin/ folder

### Running the server

0. Compile the program as described above
1. Navigate to the bin/ directory
2. Run the following command in the terminal
    > `./server *port*`\
    Where port is the number of a port of your choosing.

### Running a client

0. Compile the program and run a server
1. Open up a second terminal
2. Navigate to the bin/ directory
3. Run the following commands in the terminal
    > `./client *host* *port*`\
    Where *host* is a host of your choosing (can be localhost for example), and *port* is the same port number as for the server.

## Using the program

The program does not like badly formatted input. Sometimes, when a user enters a letter when an integer is requested, the program will close.