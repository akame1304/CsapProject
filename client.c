#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

void CtrlC(int signal); // to print on the terminal the shut down of the server

void error(char *msg); // function if any error occurred

int main(int argc, char *argv[])
{
    int Socket, port, n;
    struct sockaddr_in serverAddress;
    struct hostent *server;
    char tempServer[256];

    char buffer[256];
    if (argc < 3) // to use the default settings
    {
        fprintf(stderr, "To customize: insert %s <hostname> <port>\n The client will start with default settings.\n", argv[0]);
        FILE *configclient = fopen("clientconf.txt", "r");
        if (configclient == NULL)
        {
            error(" fopen Error\n");
            exit(1);
        }
        fscanf(configclient, "%s %d", tempServer, &port);
        server = gethostbyname(tempServer);
        fclose(configclient);
    }
    else // to use costumized settings
    {
        server = gethostbyname(argv[1]);
        port = atoi(argv[2]);
    }

    Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (Socket < 0)
        error("ERROR opening socket");

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *)&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serverAddress.sin_addr.s_addr, server->h_length);
    serverAddress.sin_port = htons(port);

    if (connect(Socket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
        error("ERROR connecting");

    signal(SIGINT, CtrlC);
    while (1)
    {
        printf("Please enter the message (type 'exit' to close connection): ");
        fgets(buffer, 255, stdin);

        n = write(Socket, buffer, strlen(buffer));

        if (strcmp(buffer, "exit\n") == 0)
        {
            printf("Connection closed by client.\n");
            break;
        }

        if (n < 0)
            error("ERROR writing to socket");

        bzero(buffer, 256);

        n = read(Socket, buffer, 255);
        if (n < 0)
            error("ERROR reading from socket");
    }
    close(Socket);
    return 0;
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void CtrlC(int signal)
{
    printf("\nCtrl+C pressed. Connection closed by client.\n");
    exit(0);
}
