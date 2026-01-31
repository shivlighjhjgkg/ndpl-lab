#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 3388
#define BUFFER_SIZE 1024
#define MAX_MSG 256

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    pid_t pid;

    printf("=== TCP CHAT CLIENT ===\n");
    printf("Client PID: %d, PPID: %d\n", getpid(), getppid());

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    printf("Connected to server! Forking processes...\n");

    // FORK: Parent = Send, Child = Receive
    pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0) {
        // CHILD: Receive from server
        printf("\n=== CHILD RECEIVE PROCESS ===\n");
        printf("Child PID: %d, PPID: %d\n", getpid(), getppid());
        
        while (1) {
            int n = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
            if (n <= 0) break;
            buffer[n] = '\0';
            
            if (strcmp(buffer, "BYE") == 0) {
                printf("Server: %s\n", buffer);
                shutdown(sockfd, SHUT_RD);
                break;
            }
            printf("Server: %s\n", buffer);
        }
        close(sockfd);
        exit(0);
    } else {
        // PARENT: Send to server
        printf("\n=== PARENT SEND PROCESS ===\n");
        printf("Parent PID: %d, PPID: %d\n", getpid(), getppid());
        
        while (1) {
            printf("Client: ");
            fgets(buffer, MAX_MSG, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            
            if (strcmp(buffer, "BYE") == 0) {
                send(sockfd, buffer, strlen(buffer) + 1, 0);
                    shutdown(sockfd, SHUT_WR);
    printf("Sent BYE - waiting for child...\n");
                break;
            }
            
            send(sockfd, buffer, strlen(buffer) + 1, 0);
        }
        close(sockfd);
        wait(NULL);  // Wait for child
    }

    return 0;
}
