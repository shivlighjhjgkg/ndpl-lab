
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 3388
#define BUFFER_SIZE 1024
#define MAX_MSG 256

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    pid_t pid;
    int opt = 1;

    printf("=== TCP CHAT SERVER ===\n");
    printf("Server PID: %d, PPID: %d\n", getpid(), getppid());

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    listen(sockfd, 1);
    printf("Server listening on port %d...\n", PORT);

    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
    if (newsockfd < 0) {
        perror("Accept failed");
        exit(1);
    }

    printf("Client connected! Forking processes...\n");
    
    // FORK: Parent = Send, Child = Receive
    pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0) {
        // CHILD: Receive from client and print
        printf("\n=== CHILD RECEIVE PROCESS ===\n");
        printf("Child PID: %d, PPID: %d\n", getpid(), getppid());
        
        while (1) {
            int n = recv(newsockfd, buffer, BUFFER_SIZE - 1, 0);
            if (n <= 0) break;
            buffer[n] = '\0';
            
            if (strcmp(buffer, "BYE") == 0) {
                printf("Client: %s\n", buffer);
                  shutdown(newsockfd, SHUT_RD); 
                break;
            }
            printf("Client: %s\n", buffer);
        }
        close(newsockfd);
        exit(0);
    } else {
        // PARENT: Send to client
        printf("\n=== PARENT SEND PROCESS ===\n");
        printf("Parent PID: %d, PPID: %d\n", getpid(), getppid());
        
        while (1) {
            printf("Server: ");
            fgets(buffer, MAX_MSG, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            
            if (strcmp(buffer, "BYE") == 0) {
                send(newsockfd, buffer, strlen(buffer) + 1, 0);
 shutdown(newsockfd, SHUT_WR);  // Shutdown WRITE
    printf("Sent BYE - waiting for child...\n");
                break;
            }
            
            send(newsockfd, buffer, strlen(buffer) + 1, 0);
        }
        close(newsockfd);
        wait(NULL);  // Wait for child
        close(sockfd); 
        printf("server fully terminated\n");
    }

    close(sockfd);
    return 0;
}
