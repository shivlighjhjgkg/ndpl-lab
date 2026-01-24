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
#define MAX_ARRAY 50
#define BUFFER_SIZE 256

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    int arr[MAX_ARRAY], n, choice;

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

    printf("Connected to server. Choices: 1=sort asc, 2=sort desc, 3=split odd/even, 0=exit\n");

    while (1) {
        printf("\nEnter choice (0-3): ");
        scanf("%d", &choice);
        if (choice == 0) {
            send(sockfd, &choice, sizeof(int), 0);
            break;
        }

        printf("Enter array size (max %d): ", MAX_ARRAY);
        scanf("%d", &n);
        if (n > MAX_ARRAY || n <= 0) {
            printf("Invalid size!\n");
            continue;
        }

        printf("Enter %d integers: ", n);
        for (int i = 0; i < n; i++) {
            scanf("%d", &arr[i]);
        }

        // Send choice
        send(sockfd, &choice, sizeof(int), 0);
        // Send size
        send(sockfd, &n, sizeof(int), 0);
        // Send array
        send(sockfd, arr, n * sizeof(int), 0);

        // Receive result
        int bytes = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            printf("Server response: %s\n", buffer);
        }
    }

    close(sockfd);
    return 0;
}
