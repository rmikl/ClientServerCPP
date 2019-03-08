#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <ctype.h>
#define BUFFER_SIZE 256



void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int createSocket();
int createSockaddr(struct sockaddr_in*,struct hostent*, int);
int bindSocketToPort(int, struct sockaddr_in*);
int establishConnection(int, struct sockaddr_in*);

ssize_t sendFile(int );
FILE* createFile(const char* );



int main(int argc, char *argv[])
{

    int socketFileDescriptor = createSocket();


    int portnumber;
    portnumber = atoi(argv[2]);


    struct hostent *hostname;
    hostname = gethostbyname(argv[1]);
    if(hostname == NULL) error("no such host");


    struct sockaddr_in serverAddress;
    bzero((char*) &serverAddress, sizeof(serverAddress));
    createSockaddr(&serverAddress,hostname,portnumber);


    int _connect;

    _connect = establishConnection(socketFileDescriptor, &serverAddress);
    if(_connect < 0) error("error connecting");


    char buff[BUFFER_SIZE], filebuff[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;
    bzero(buff,BUFFER_SIZE);

    char ch;
    while(1)
    {
        printf("Client: ");
        bzero(buff,BUFFER_SIZE);
        fseek(stdin,0,SEEK_END);
        fgets(buff, sizeof(buff), stdin);
        char tmp[BUFFER_SIZE];
        strcpy(tmp,buff);
        int exit = strncmp("exit", buff, 4);
        int fileTransfer = strncmp("transfer", buff, 8);
        bytes_written = write(socketFileDescriptor, buff, sizeof(buff));
        if(exit == 0)
        {
            break;
        }

        if(fileTransfer == 0)
        {
            bytes_written = sendFile(socketFileDescriptor);
        }
        if(bytes_written == 0)
        {
            printf("WRITE(0) error ---> %s.\n", strerror(errno));
            printf("Nothing was written.\n");
            break;
        }
        memset(buff, 0, sizeof(buff));
        bytes_read = read(socketFileDescriptor, buff, sizeof(buff));
        if(bytes_read < 0)
        {
            printf("Error reading message from \n");
            printf("READ(c) error ---> %s.\n", strerror(errno));
            break;
        }

        //Test to see if the buffer is blank.
        if(bytes_read == 0)
        {
            printf("READ(0) error ---> %s.\n", strerror(errno));
            break;
        }

        fprintf(stdout, "Server: %s", buff);
    }
    close(socketFileDescriptor);

    return 0;

}

int createSocket()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

int createSockaddr(struct sockaddr_in* serverAddress, struct hostent* hostname, int portNumber)
{
    bcopy((char*)hostname->h_addr, (char*)&serverAddress->sin_addr.s_addr, hostname->h_length);
    serverAddress->sin_family = AF_INET;
    serverAddress->sin_port = htons(portNumber);
    return portNumber;
}

int bindSocketToPort(struct sockaddr_in* addr, int sockfd)
{
    int _bind = bind(sockfd, (struct sockaddr*) addr, sizeof(addr));
    return _bind;
}

int establishConnection(int socketFileDescriptor, struct sockaddr_in* serverAddress)
{
    int _connect = connect(socketFileDescriptor, (struct sockaddr*) serverAddress, sizeof(sockaddr_in));
    return _connect;
}

ssize_t sendFile(int socketFileDescriptor)
{
    ssize_t bytes_write;
    char c;
    int words = 0;
    char message[BUFFER_SIZE];
    FILE* f = createFile("file");
    while((c = getc(f)) != EOF)
    {
        fscanf(f,"%s", message);
        if(isspace(c) ||  c == '\t')
        words = words + 1;

    }
    write(socketFileDescriptor,&words, sizeof(int));
    rewind(f);
    while(fscanf(f, "%s", message) == 1)
    {
        ssize_t bytes_write = write(socketFileDescriptor,message,256);
    }
    fclose(f);
    return bytes_write;
}

FILE* createFile(const char* fileName)
{
    char path[BUFFER_SIZE];
    bzero(path,BUFFER_SIZE);
    strcat(path, "/root/");
    strcat(path, fileName);
    return fopen(path,"r");
}
