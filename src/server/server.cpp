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

    printf("server: got connection from %s port %d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));


    char buff[256];
    ssize_t bytes_read, bytes_written;


    //file handling vars
    FILE *fp;
    int ch = 0;
    int words;
    char filebuff[256];


    for( ; ; )
    {
        bytes_read = read(newSocketFileDescriptor, buff, sizeof(buff));
        printf("bufer: %s",buff);
        int exit = strncmp("exit", buff,4);
        int fileTransfer = strncmp("transfer", buff, 8);
        if(exit == 0)
        {
            break;
        }

        if(fileTransfer == 0)
        {
            fp = fopen("/root/recievedFile","a");
            printf("file opening...\n");

            read(newSocketFileDescriptor,&words, sizeof(int));
            printf("word counting...\n");
            printf("words : %d\n",words);

            while (ch != words)
            {
                int _read = read(newSocketFileDescriptor, filebuff, 256);
                fprintf(fp,"%s ", filebuff);
                ch = ch +1;
                printf("inside while loop....................\n");
            }
            bzero(filebuff,256);

            printf("recieving file...\n");
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
        bzero(buff,256);
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
