#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 3388
#define BUFFER_SIZE 16348
#define MAX_STR 20

void swap(char *x, char *y) {
    char temp = *x;
    *x = *y;
    *y = temp;
}

void permute(char *str, int l, int r, char *result, int *result_len) {
    if (l == r) {
        strcat(result, str);
        strcat(result, "\n");
        (*result_len) += strlen(str) + 1;
        return;
    }
    
    for (int i = l; i <= r; i++) {
        swap(&str[l], &str[i]);
        permute(str, l + 1, r, result, result_len);
        swap(&str[l], &str[i]);  // backtrack
    }
}

int main() {
    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    char buffer[MAX_STR], input_str[MAX_STR];
    char permutations[BUFFER_SIZE];
    int result_len = 0;

    printf("=== UDP HALF-DUPLEX PERMUTATION SERVER ===\n");
    printf("Server listening on port %d...\n", PORT);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
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

    clilen = sizeof(cli_addr);
    
    while (1) {
        // Receive string from client
        int n = recvfrom(sockfd, input_str, MAX_STR - 1, 0, (struct sockaddr*)&cli_addr, &clilen);
        input_str[n] = '\0';
        
        printf("\nReceived string: '%s'\n", input_str);
        printf("Generating permutations...\n");

        if (strcmp(input_str, "STOP") == 0) {
            printf("Client STOP - terminating\n");
            break;
        }

        // Generate all permutations
        permutations[0] = '\0';
        result_len = 0;
        int len = strlen(input_str);
        permute(input_str, 0, len - 1, permutations, &result_len);
        
        printf("Generated %d permutations (%d bytes)\n", 
               1 << len, result_len);  // 2^len for distinct chars
        
        // Send all permutations back
        sendto(sockfd, permutations, result_len, 0, (struct sockaddr*)&cli_addr, clilen);
        printf("Sent permutations to client\n");
    }

    close(sockfd);
    return 0;
}
