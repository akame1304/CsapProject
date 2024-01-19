#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
void error(char *msg);                  // function if any error occurred
void connection(int, int, char *, int); // function that handle the connection
void createNameFile(char **filename);   // function to create the name of the log file
char *time_stamp();                     // function to create the timestamp
void CtrlC(int signal);                 // to print on the terminal the shut down of the server

int main(int argc, char *argv[])
{
    int Socket,
        socketConnection,
        port,
        clientLength,
        n,
        pid,
        fileLog;
    char *filename;
    char path[256];
    char buffer[256];
    struct sockaddr_in serverAddress, clientAddress;

    if (argc < 3) // to use the default settings
    {
        fprintf(stderr, "To customize: insert %s <port> <log_directory>\n The server will start with default settings.\n", argv[0]);
        FILE *configserver = fopen("servconf.txt", "r");
        if (configserver == NULL)
        {
            error(" fopen Error\n");
            exit(1);
        }

        fscanf(configserver, "%d %s", &port, path);
        fclose(configserver);
    }
    else // to use costumized settings
    {
        port = atoi(argv[1]);
        strcpy(path, argv[2]);
    }

    createNameFile(&filename);
    strcat(path, filename);
    fileLog = open(path, O_CREAT | O_WRONLY, 0777);

    Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (Socket < 0)
        error("ERROR opening socket");

    bzero((char *)&serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(Socket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        error("ERROR on binding");
    }

    if (listen(Socket, 5) == -1)
    {
        error("Error listening for connections");
        close(Socket);
        exit(1);
    }
    printf("Server is on and listening...\n");

    clientLength = sizeof(clientAddress);

    signal(SIGINT, CtrlC);
    while (1)
    {
        char myIP[169];
        char *clientname;
        int myPort;
        socketConnection = accept(Socket, (struct sockaddr *)&clientAddress, &clientLength);
        if (socketConnection < 0)
            error("ERROR on accept");

        pid = fork();
        if (pid < 0)
            error("ERROR on fork");
        if (pid == 0)
        {

            close(Socket);

            inet_ntop(AF_INET, &clientAddress.sin_addr, myIP, sizeof(myIP));
            myPort = ntohs(clientAddress.sin_port);
            clientname = inet_ntoa(clientAddress.sin_addr);
            printf("Local ip address: %s\n", myIP);
            printf("Local port : %d\n", myPort);
            connection(socketConnection, fileLog, clientname, myPort);
            exit(0);
        }
        else
        {
            close(socketConnection);
        }
    }
    close(fileLog);
    close(Socket);
    return 0;
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void connection(int sock, int file, char *myIP, int myPort)
{
    int n;
    char buffer[256];
    char header[200];
    int fildes = file;
    ssize_t a;

    sprintf(header, "[%s] Connection from %d\n", myIP, myPort);
    a = write(fildes, header, strlen(header));
    if (a == -1)
    {
        error("error write");
    }

    while (1)
    {
        bzero(buffer, 256);
        n = read(sock, buffer, 255);
        if (n < 0)
        {
            error("Error receiving data from socket");
            break;
        }
        else if (n == 0)
        {
            printf("Client disconnected.\n");
            break;
        }

        sprintf(header, "Address %s,port %d, time %s.\n Messaggio:", myIP, myPort, time_stamp());

        a = write(fildes, header, strlen(header));
        if (a == -1)
        {
            error("error write");
        }
        a = write(fildes, buffer, strlen(buffer));
        if (a == -1)
        {
            error("error write");
        }

        printf("message: %s\n", buffer);

        if (strcmp(buffer, "exit\n") == 0)
        {
            printf("Client address %s,port %d disconnected.\n", myIP, myPort);
            sprintf(header, "Client address %s,port %d disconnected.\n", myIP, myPort);
            a = write(fildes, header, strlen(header));
            if (a == -1)
            {
                error("error write");
            }
            break;
        }

        n = write(sock, "Il server ha ricevuto il messaggio", 34);
        if (n < 0)
        {
            error("Eerror write");
        }
    }
}

void createNameFile(char **filename)
{
    char newFilename[256];
    sprintf(newFilename, "LOGFILE_%s.txt", time_stamp());

    *filename = (char *)malloc(strlen(newFilename) + 1);
    if (*filename == NULL)
    {
        error("Error allocating memory for filename");
    }

    strcpy(*filename, newFilename);
}

char *time_stamp()
{
    char *timestamp = (char *)malloc(sizeof(char) * 16);
    time_t ltime;
    ltime = time(NULL);
    struct tm *tm;
    tm = localtime(&ltime);

    sprintf(timestamp, "%04d-%02d-%02d_%02d-%02d-%02d", tm->tm_year + 1900, tm->tm_mon + 1,
            tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

    return timestamp;
}

void CtrlC(int signal)
{
    printf("\nCtrl+C pressed. Connection closed by server.\n");
    exit(0);
}
