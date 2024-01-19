# Computer System and Programming Project - Exam January 2024

**Author**: Chiara Menghini
**Matricola**: 1968050

## Description

This project consist in the implementation of a log server that connectsan unlimited amount of clients, create for each one a stateful connection, and record the connection and the messages in a log file.

## Requirements

**OS:** Ubuntu 22.04.1
**Compiler:** gcc version 11.4.0

## Notes

1. The folder in which the file will be store _must_ exist, otherwise the file will not be saved properly. The program will not create a new folder.
2. If there are no inputs inserted in the command line, the program will run on default settings,creating a file on your current folder.

## Guide to the compilation and test

1. Open a terminal window.
2. Navigate to the project directory using the `cd` command:
   Example: cd path/to/CsapProject.
3. On the terminal write 'make'.
4. If you want default values, write on the terminal ./server and press enter. It will use port 5000 and create a log file in the folder you're currently in. To customize it write ./server <port> and <folderpath>
5. Now its time to connect the clients. Open a new terminal. Similarly to point 4, if you want default values, write on the terminal ./client and press enter. To customize it write ./client <IP> <port>
6. Send a series of messages from the clients to the server that will be saved in the log file.
7. To close the client connection write on the client terminal "exit" or Ctrl+C.
8. To shut down the server press Ctrl+C.

