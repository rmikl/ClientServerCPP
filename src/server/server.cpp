//includes here
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}




int main(int argc, char *argv[])
{
    //create a socket

    int socketFileDescriptor;

    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

        //check whether socket was created?
    if(socketFileDescriptor < 0) error("error on opening socket\n");

    int portNumber;

    portNumber = atoi(argv[1]); //assign on what port server is lestening, you give it as a paramether

////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct sockaddr_in serverAddress;

    serverAddress.sin_addr.s_addr = INADDR_ANY; //assing current machine ip
    serverAddress.sin_family = AF_INET; //assing TCP connection
    serverAddress.sin_port = htons(portNumber); //assign on what port server will listen


 ///////////////////////////////////////////////////////////////////////////////////////////////////////


     //bind socket to port



    int _bind;

    _bind = bind(socketFileDescriptor, (struct sockaddr*) &serverAddress, sizeof(serverAddress));

    if(_bind < 0) error("\nerror on bind\n");


    //listen

    listen(socketFileDescriptor,5);
    socklen_t clientSocketLength;
    struct sockaddr_in clientAddress;
    clientSocketLength = sizeof(clientAddress);


    //accept connection

    int newSocketFileDescriptor;
    newSocketFileDescriptor = accept(socketFileDescriptor, (struct sockaddr*) &clientAddress, &clientSocketLength);

    if(newSocketFileDescriptor < 0) error("error on accepting");

    printf("server: got connection from %s port %d\n",
            inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

    send(newSocketFileDescriptor, "Hello, world!\n", 13, 0);


    int n;
    char buff[256];
    bzero(buff,256);

    n = read(newSocketFileDescriptor,buff, 255);

    if(n <0) error("error readng from socket");

    printf("Here is the message: %s\n",buff);

    close(newSocketFileDescriptor);

    close(socketFileDescriptor);

    return 0;





    //do smth with this



    return 0;
}
