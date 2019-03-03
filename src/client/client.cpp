#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>



void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int socketFileDescriptor;



    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    int portnumber;

    portnumber = atoi(argv[2]);

    struct hostent *hostname;

    hostname = gethostbyname(argv[1]);

    if(hostname == NULL) error("no such host");

    struct sockaddr_in serverAddress;
    bzero((char*) &serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;

    bcopy((char*)hostname->h_addr, (char*)&serverAddress.sin_addr.s_addr, hostname->h_length);

    serverAddress.sin_port = htons(portnumber);

    int _connect;

    _connect = connect(socketFileDescriptor, (struct sockaddr*) &serverAddress, sizeof(serverAddress));

    if(_connect < 0) error("error connecting");

    printf("enter the message:\n");

    char buff[256];
    bzero(buff,256);

    fgets(buff,256,stdin);

    int n;

    n = write(socketFileDescriptor,buff, strlen(buff));

    if(n<0) error("error writing to socket");

    bzero(buff,256);

    n = read(socketFileDescriptor,buff,255);

    if(n<0) error("error reading from socket");

    printf("%s\n", buff);

    close(socketFileDescriptor);

    return 0;




}
