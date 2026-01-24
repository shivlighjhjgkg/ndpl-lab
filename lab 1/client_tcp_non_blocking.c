#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#define MAXSIZE 50

int main()
{
    char buff[MAXSIZE];
    int sockfd, retval;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Socket Creation Error");
        exit(1);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    retval = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == -1)
    {
        perror("Connection error");
        close(sockfd);
        exit(1);
    }

    // fork so that client can both send and receive concurrently
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork error");
        close(sockfd);
        exit(1);
    }

    if (pid == 0)
    {
        // Child process: receive from server and print
        while (1)
        {
            memset(buff, '\0', sizeof(buff));
            recedbytes = recv(sockfd, buff, sizeof(buff), 0);
            if (recedbytes <= 0)
            {
                // error or connection closed
                break;
            }

            buff[recedbytes] = '\0';
            // printf("Server: %s\n", buff);
            printf("Other: %s\n", buff);


            if (strncmp(buff, "stop", 4) == 0)
            {
                break;
            }
        }
        close(sockfd);
        exit(0);
    }
    else
    {
        // Parent process: read from stdin and send to server
        while (1)
        {
            memset(buff, '\0', sizeof(buff));
            //printf("Client: ");
            printf("You: ");

            fflush(stdout);

            if (fgets(buff, sizeof(buff), stdin) == NULL)
            {
                break;
            }
            buff[strcspn(buff, "\n")] = '\0';

            int s = strlen(buff);
            if (s == 0)
                continue;

            sentbytes = send(sockfd, buff, s, 0);
            if (sentbytes <= 0)
            {
                break;
            }

            if (strncmp(buff, "stop", 4) == 0)
            {
                break;
            }
        }
        close(sockfd);
        // child will terminate when socket closes
        exit(0);
    }
}
