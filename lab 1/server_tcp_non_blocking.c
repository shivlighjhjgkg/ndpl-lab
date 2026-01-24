#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#define MAXSIZE 90

int main()
{
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
//also gonna use socketoptions here 



    if (sockfd == -1)
    {
        perror("Socket creation error");
        exit(1);
    }
int opt = 1;
if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
    perror("setsockopt reuseaddr");
    exit(1);
}

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    retval = bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == -1)
    {
        perror("Binding error");
        close(sockfd);
        exit(1);
    }

    retval = listen(sockfd, 1);
    if (retval == -1)
    {
        perror("Listen error");
        close(sockfd);
        exit(1);
    }

    actuallen = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &actuallen);
    if (newsockfd == -1)
    {
        perror("Accept error");
        close(sockfd);
        exit(1);
    }

    // At this point we have one connected client.
    // Use fork(): one process receives, the other sends.

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork error");
        close(newsockfd);
        close(sockfd);
        exit(1);
    }

    if (pid == 0)
    {
        // Child process: receive messages from client and print
        while (1)
        {
            memset(buff, '\0', sizeof(buff));
            recedbytes = recv(newsockfd, buff, sizeof(buff), 0);
            if (recedbytes <= 0)
            {
                // error or connection closed
                break;
            }

            buff[recedbytes] = '\0';
            //printf("Client: %s\n", buff);
            printf("Other: %s\n", buff);


            if (strncmp(buff, "stop", 4) == 0)
            {
                break;
            }
        }
        close(newsockfd);
        close(sockfd);
        exit(0);
    }
    else
    {
        // Parent process: read from stdin and send to client
        while (1)
        {
            memset(buff, '\0', sizeof(buff));
            //printf("Server: ");
            printf("You: ");

            fflush(stdout);

            // use fgets to allow spaces; trim newline
            if (fgets(buff, sizeof(buff), stdin) == NULL)
            {
                break;
            }
            buff[strcspn(buff, "\n")] = '\0';

            int s = strlen(buff);
            if (s == 0)
                continue;

            sentbytes = send(newsockfd, buff, s, 0);
            if (sentbytes <= 0)
            {
                break;
            }

            if (strncmp(buff, "stop", 4) == 0)
            {
                break;
            }
        }
        close(newsockfd);
        close(sockfd);
        // let child get SIGPIPE/EOF and exit
        exit(0);
    }
}
