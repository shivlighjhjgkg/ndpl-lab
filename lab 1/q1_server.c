#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 3388
#define MAX_ARRAY 50
#define BUFFER_SIZE 256

void sort_asc(int arr[], int n) {
    for (int i = 0; i < n-1; i++)
        for (int j = 0; j < n-i-1; j++)
            if (arr[j] > arr[j+1])
                // swap
                arr[j] ^= arr[j+1] ^= arr[j] ^= arr[j+1];
}

void sort_desc(int arr[], int n) {
    for (int i = 0; i < n-1; i++)
        for (int j = 0; j < n-i-1; j++)
            if (arr[j] < arr[j+1])
                // swap
                arr[j] ^= arr[j+1] ^= arr[j] ^= arr[j+1];
}

void split_odd_even(int arr[], int n, int odd[], int even[], int *odd_cnt, int *even_cnt) {
    *odd_cnt = 0;
    *even_cnt = 0;
    for (int i = 0; i < n; i++) {
        if (arr[i] % 2 == 0)
            even[(*even_cnt)++] = arr[i];
        else
            odd[(*odd_cnt)++] = arr[i];
    }
}

int main() {
    int sockfd, newsockfd, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[BUFFER_SIZE];
    int arr[MAX_ARRAY], n, choice;
    int odd[MAX_ARRAY], even[MAX_ARRAY], odd_cnt, even_cnt;
    int opt = 1;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Reuse address
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    listen(sockfd, 5);
    printf("Server listening on port %d...\n", PORT);

    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
    if (newsockfd < 0) {
        perror("Accept failed");
        exit(1);
    }

    printf("Client connected\n");

    while (1) {
        // Receive choice first
        int bytes = recv(newsockfd, buffer, sizeof(int), 0);
        if (bytes <= 0) break;
        memcpy(&choice, buffer, sizeof(int));

        if (choice == 0) {
            printf("Client chose EXIT\n");
            break;
        }

        // Receive array size
        bytes = recv(newsockfd, buffer, sizeof(int), 0);
        if (bytes <= 0) break;
        memcpy(&n, buffer, sizeof(int));

        // Receive array
        bytes = recv(newsockfd, arr, n * sizeof(int), 0);
        if (bytes <= 0) break;

        printf("Received %d numbers: ", n);
        for (int i = 0; i < n; i++) printf("%d ", arr[i]);
        printf("\nChoice: %d\n", choice);

        char result[BUFFER_SIZE];
        if (choice == 1) {  // Sort ascending
            sort_asc(arr, n);
            sprintf(result, "Sorted ASC: ");
            for (int i = 0; i < n; i++) sprintf(result + strlen(result), "%d ", arr[i]);
        }
        else if (choice == 2) {  // Sort descending
            sort_desc(arr, n);
            sprintf(result, "Sorted DESC: ");
            for (int i = 0; i < n; i++) sprintf(result + strlen(result), "%d ", arr[i]);
        }
        else if (choice == 3) {  // Split odd/even
            split_odd_even(arr, n, odd, even, &odd_cnt, &even_cnt);
            sprintf(result, "Odd(%d): ", odd_cnt);
            for (int i = 0; i < odd_cnt; i++) sprintf(result + strlen(result), "%d ", odd[i]);
            sprintf(result + strlen(result), "Even(%d): ", even_cnt);
            for (int i = 0; i < even_cnt; i++) sprintf(result + strlen(result), "%d ", even[i]);
        }
        else {
            sprintf(result, "Invalid choice!");
        }

        printf("Sending: %s\n", result);
        send(newsockfd, result, strlen(result) + 1, 0);
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}
