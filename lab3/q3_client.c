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
#define MAX_STR 100

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    char input_str[MAX_STR], buffer[BUFFER_SIZE];

    printf("=== ALPHANUMERIC CLIENT ===\n");
    printf("Enter alphanumeric string: ");
    fgets(input_str, MAX_STR, stdin);
    input_str[strcspn(input_str, "\n")] = '\0';

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

    printf("Input string: %s\n", input_str);
    printf("Waiting for server response...\n");

    // Send string to server
    send(sockfd, input_str, strlen(input_str) + 1, 0);

    // Receive both responses
    int n1 = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
    buffer[n1] = '\0';
    printf("\n%s\n", buffer);

    int n2 = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
    buffer[n2] = '\0';
    printf("%s\n", buffer);

    close(sockfd);
    return 0;
}
