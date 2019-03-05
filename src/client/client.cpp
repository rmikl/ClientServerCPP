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
    char buff[256], filebuff[256];
    ssize_t bytes_read, bytes_written;
    bzero(buff,255);

//file handling variables
    FILE *f;
    int words=0;
    char c,ch;

    for( ; ; )
    {
        printf("Client: ");
        bzero(buff,256);
        fseek(stdin,0,SEEK_END);
        fgets(buff, sizeof(buff), stdin);
        char tmp[256];
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
            char tmp[256];
            strcpy(tmp,buff);
            f = fopen("/root/file","r");
            printf("file opening...\n");
            while((c = getc(f)) != EOF)
            {
                fscanf(f,"%s", filebuff);
                printf("filebuf : %s\n",filebuff);
                if(isspace(c) ||  c == '\t')
                    words = words + 1;

            }
            printf("word counting...\n");

            write(socketFileDescriptor,&words, sizeof(int));
            printf("wirte to buffer...\n");
            rewind(f);

            while(fscanf(f, "%s", filebuff) == 1)
            {
             //   fscanf(f, "%s", filebuff);
                printf("what is inside a file: %s\n", filebuff);
                //printf("ch : %c\n",ch);
                write(socketFileDescriptor,filebuff,256);
             //   ch = fgetc(f);
             //   while(isspace(c) || ch == '\t' || ch == '\n' || ) ch = fgetc(f);
            }










            printf("sending file...\n");
            fclose(f);
        }
        strcpy(buff,tmp);


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
