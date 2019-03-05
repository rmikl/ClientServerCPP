//includes here
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#define BUFFER_SIZE 256

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int createSocket();
int createSockaddr(struct sockaddr_in*, int);
int bindSocketToPort(int, struct sockaddr_in*);

int createConnection(int, struct sockaddr_in*, socklen_t);
int listenForClient(int);
int acceptConnection(int ,struct sockaddr_in* , socklen_t );


ssize_t recieveTextMessage(int, char*);
ssize_t recieveFile(int);

FILE* createFile(const char*);
int getWordCount(int, int*);


void sendMessage();
void waitForReply();
void closeConnection();



int main(int argc, char *argv[])
{

    int socketFileDescriptor = createSocket();
    if(socketFileDescriptor < 0) error("error on opening socket\n");
    int portNumber;
    portNumber = atoi(argv[1]);

    struct sockaddr_in serverAddress;
    createSockaddr(&serverAddress, portNumber);


    int _bind = bindSocketToPort(socketFileDescriptor, &serverAddress);
    if(_bind < 0) error("\nerror on bind\n");


    socklen_t clientSocketLength;
    struct sockaddr_in clientAddress;
    clientSocketLength = sizeof(clientAddress);
    int newSocketFileDescriptor = createConnection(socketFileDescriptor,&clientAddress,clientSocketLength);
    if(newSocketFileDescriptor < 0) error("error on accepting");
    printf("server: got connection from %s port %d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
    char buff[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;


    while(1)
    {
        bytes_read = recieveTextMessage(newSocketFileDescriptor,buff);
        int exit=1;
        if(strncmp("exit", buff,4) == 0 || strncmp("nara", buff,4) == 0) exit=0;
        int fileTransfer = strncmp("transfer", buff, 8);
        if(exit == 0)
        {
            break;
        }

        if(fileTransfer == 0)
        {
            bytes_read = recieveFile(newSocketFileDescriptor);
        }
        if(bytes_read < 0)
        {
            printf("READ(-1) error ---> %s.\n", strerror(errno));
            break;
        }

        if(bytes_read == 0)
        {
            printf("READ(0) error ---> %s.\n", strerror(errno));
            break;
        }

        fprintf(stdout, "Client: %s", buff);
        printf("Server: ");
        bzero(buff,BUFFER_SIZE);
        fgets(buff, sizeof(buff), stdin);
        bytes_written = write(newSocketFileDescriptor, buff, sizeof(buff));
        if(bytes_written < 0)
        {
            printf("WRITE(-1) error ---> %s.\n", strerror(errno));
            break;
        }
    }
    close(newSocketFileDescriptor);
    close(socketFileDescriptor);

    return 0;

}

int createSocket()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

int createSockaddr(struct sockaddr_in* addr, int portNumber)
{
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_family = AF_INET;
    addr->sin_port = htons(portNumber);
    return portNumber;
}

int bindSocketToPort(struct sockaddr_in* addr, int sockfd)
{
    int _bind = bind(sockfd, (struct sockaddr*) addr, sizeof(addr));
    return _bind;
}

int createConnection(int sockfd,struct sockaddr_in* clientAddress, socklen_t clientSocketLength)
{
    printf("listetnig fro client..\n");
    if (listenForClient(sockfd) == 0)
    {
        return acceptConnection(sockfd,clientAddress,clientSocketLength);
    }return -2;
}

int listenForClient(int sockfd)
{
    return listen(sockfd, 5);
}

int acceptConnection(int sockfd,struct sockaddr_in* clientAddress, socklen_t cSLength)
{
    return accept(sockfd, (struct sockaddr*) clientAddress, &cSLength);//
}

int bindSocketToPort(int socketFileDescriptor ,struct sockaddr_in* serverAddress)
{
    return bind(socketFileDescriptor, (struct sockaddr*) serverAddress, sizeof(*serverAddress));
}

ssize_t recieveTextMessage(int newSocketFileDescriptor,char* messageBuffer)
{
    return read(newSocketFileDescriptor, messageBuffer, sizeof(*messageBuffer) * BUFFER_SIZE);
}

ssize_t recieveFile(int newSocketFileDescriptor)
{
    char filebuff[BUFFER_SIZE];
    int wordCount;
    FILE* fileHanlder = createFile("test");
    ssize_t bytes_read = getWordCount(newSocketFileDescriptor,&wordCount);
    for (int wordRead=0; wordRead < wordCount; wordRead++)
    {
        recieveTextMessage(newSocketFileDescriptor, filebuff);
        if(filebuff == NULL) printf("is null\n");
        fprintf(fileHanlder," %s", filebuff);
    }

    bzero(filebuff,BUFFER_SIZE);

    fclose(fileHanlder);
    return bytes_read;
}

FILE* createFile(const char* fileName)
{
    char path[BUFFER_SIZE];
    bzero(path,BUFFER_SIZE);
    strcat(path, "/root/");
    strcat(path, fileName);
    return fopen(path,"a");
}

int getWordCount(int newSocketFileDescriptor,int* wordCount)
{
    return read(newSocketFileDescriptor, wordCount, sizeof(*wordCount));
}


