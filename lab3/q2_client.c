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
#define MAX_STR 20

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    char input_str[MAX_STR];
    char permutations[BUFFER_SIZE];

    printf("=== UDP HALF-DUPLEX PERMUTATION CLIENT ===\n");
    printf("Send strings to server (\"STOP\" to exit):\n");

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    while (1) {
        printf("\nEnter string (max %d chars): ", MAX_STR - 1);
        fgets(input_str, MAX_STR, stdin);
        input_str[strcspn(input_str, "\n")] = '\0';

        if (strcmp(input_str, "STOP") == 0) {
            sendto(sockfd, input_str, strlen(input_str) + 1, 0, 
                   (struct sockaddr*)&serv_addr, sizeof(serv_addr));
            printf("Sent STOP - exiting\n");
            break;
        }

        // Send string to server
        sendto(sockfd, input_str, strlen(input_str) + 1, 0, 
               (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        printf("Sent '%s' to server...\n", input_str);

        // Receive permutations
        int n = recvfrom(sockfd, permutations, BUFFER_SIZE - 1, 0, NULL, NULL);
        permutations[n] = '\0';
        
        printf("\n=== ALL PERMUTATIONS ===\n%s", permutations);
        printf("=======================\n");
    }

    close(sockfd);
    return 0;
}
