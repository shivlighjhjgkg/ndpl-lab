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
#define MAX_STR 256

int main() {
    int sockfd, choice;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE], filename[MAX_STR], search_str[MAX_STR], str1[MAX_STR], str2[MAX_STR];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    printf("Enter filename: ");
    fgets(filename, MAX_STR, stdin);
    filename[strcspn(filename, "\n")] = '\0';

    sendto(sockfd, filename, strlen(filename) + 1, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    // Check file status
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, NULL, NULL);
    buffer[n] = '\0';
    if (strcmp(buffer, "File not present") == 0) {
        printf("Server: File not present\n");
        close(sockfd);
        return 0;
    }

    printf("File exists! Menu:\n1. Search\n2. Replace\n3. Reorder ASCII\n4. Exit\n");

    while (1) {
        printf("\nChoice (1-4): ");
        fgets(buffer, 10, stdin);
        sscanf(buffer, "%d", &choice);

        sendto(sockfd, &choice, sizeof(int), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

        if (choice == 4) break;

        if (choice == 1) {  // Search
            printf("Enter string to search: ");
            fgets(search_str, MAX_STR, stdin);
            search_str[strcspn(search_str, "\n")] = '\0';
            sendto(sockfd, search_str, strlen(search_str) + 1, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
            
            n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, NULL, NULL);
            buffer[n] = '\0';
            printf("Server: %s\n", buffer);
        }
        else if (choice == 2) {  // Replace
            printf("Enter string to replace: ");
            fgets(str1, MAX_STR, stdin);
            str1[strcspn(str1, "\n")] = '\0';
            printf("Enter replacement string: ");
            fgets(str2, MAX_STR, stdin);
            str2[strcspn(str2, "\n")] = '\0';
            sendto(sockfd, str1, strlen(str1) + 1, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
            sendto(sockfd, str2, strlen(str2) + 1, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
            
            n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, NULL, NULL);
            buffer[n] = '\0';
            printf("Server: %s\n", buffer);
        }
        else if (choice == 3) {  // Reorder
            n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, NULL, NULL);
            buffer[n] = '\0';
            printf("Server: %s\n", buffer);
        }
    }

    close(sockfd);
    return 0;
}
