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
#define BUFFER_SIZE 256

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    char result[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);  // UDP socket
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    printf("UDP Client connected. Enter strings (\"Halt\" to exit):\n");

    while (1) {
        printf("String: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline

        if (strcmp(buffer, "Halt") == 0) {
            // Send halt and exit
            sendto(sockfd, buffer, strlen(buffer) + 1, 0, 
                   (struct sockaddr*)&serv_addr, sizeof(serv_addr));
            printf("Sent Halt - exiting\n");
            break;
        }

        // Send string to server
        sendto(sockfd, buffer, strlen(buffer) + 1, 0, 
               (struct sockaddr*)&serv_addr, sizeof(serv_addr));

        // Receive result
        int bytes = recvfrom(sockfd, result, BUFFER_SIZE - 1, 0, NULL, NULL);
        if (bytes > 0) {
            result[bytes] = '\0';
            printf("Server: %s\n", result);
        }
    }

    close(sockfd);
    return 0;
}
