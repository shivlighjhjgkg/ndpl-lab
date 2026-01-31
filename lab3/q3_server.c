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
#define MAX_STR 100

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    char buffer[MAX_STR], input_str[MAX_STR], numbers[50], chars[50];
    pid_t pid;
    int opt = 1;

    printf("=== ALPHANUMERIC SERVER ===\n");
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
    printf("Listening on port %d...\n", PORT);

    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
    if (newsockfd < 0) {
        perror("Accept failed");
        exit(1);
    }

    printf("Client connected!\n");

    // Receive input string
    int n = recv(newsockfd, input_str, MAX_STR - 1, 0);
    input_str[n] = '\0';
    printf("Received: '%s'\n", input_str);

    // Extract numbers and chars
    int num_idx = 0, char_idx = 0;
    for (int i = 0; input_str[i]; i++) {
        if (input_str[i] >= '0' && input_str[i] <= '9') {
            numbers[num_idx++] = input_str[i];
        } else {
            chars[char_idx++] = input_str[i];
        }
    }
    numbers[num_idx] = '\0';
    chars[char_idx] = '\0';

    printf("Numbers: '%s', Chars: '%s'\n", numbers, chars);

    // FORK: Child = sort numbers ASC, Parent = sort chars DESC
    pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0) {
        // CHILD: Sort numbers ASCENDING
        printf("\n=== CHILD PROCESS (NUMBERS ASC) ===\n");
        printf("Child PID: %d, PPID: %d\n", getpid(), getppid());
        
        // Bubble sort numbers ASC
        int len = strlen(numbers);
        for (int i = 0; i < len - 1; i++) {
            for (int j = 0; j < len - i - 1; j++) {
                if (numbers[j] > numbers[j + 1]) {
                    char temp = numbers[j];
                    numbers[j] = numbers[j + 1];
                    numbers[j + 1] = temp;
                }
            }
        }
        
        // Send result + PID to client
        sprintf(buffer, "Child PID %d: %s", getpid(), numbers);
        send(newsockfd, buffer, strlen(buffer) + 1, 0);
        printf("Child sent: %s\n", buffer);
        
        close(newsockfd);
        exit(0);
    } else {
        // PARENT: Sort characters DESCENDING
        printf("\n=== PARENT PROCESS (CHARS DESC) ===\n");
        printf("Parent PID: %d, PPID: %d\n", getpid(), getppid());
        
        // Bubble sort chars DESC
        int len = strlen(chars);
        for (int i = 0; i < len - 1; i++) {
            for (int j = 0; j < len - i - 1; j++) {
                if (chars[j] < chars[j + 1]) {
                    char temp = chars[j];
                    chars[j] = chars[j + 1];
                    chars[j + 1] = temp;
                }
            }
        }
        
        // Wait for child briefly
        sleep(1);
        
        // Send result + PID to client
        sprintf(buffer, "Parent PID %d: %s", getpid(), chars);
        send(newsockfd, buffer, strlen(buffer) + 1, 0);
        printf("Parent sent: %s\n", buffer);
        
        wait(NULL);  // Wait for child
        close(newsockfd);
        close(sockfd);
    }

    return 0;
}
