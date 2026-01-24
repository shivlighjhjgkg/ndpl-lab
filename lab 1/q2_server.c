#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>  // For UNIX sockets (though we use inet for UDP)

#define PORT 3388
#define BUFFER_SIZE 256
#define MAX_VOWELS 5

int is_palindrome(const char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        if (str[i] != str[len - 1 - i]) return 0;
    }
    return 1;
}

int count_vowel(const char *str, char vowel) {
    int count = 0;
    for (int i = 0; str[i]; i++) {
        if (tolower(str[i]) == vowel) count++;
    }
    return count;
}

int main() {
    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    char result[BUFFER_SIZE];
    int a_count, e_count, i_count, o_count, u_count;
    char vowels[] = {'a', 'e', 'i', 'o', 'u'};

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);  // UDP socket
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

    printf("UDP Server listening on port %d...\n", PORT);

    clilen = sizeof(cli_addr);
    while (1) {
        // Receive string from client
        int bytes = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, 
                           (struct sockaddr*)&cli_addr, &clilen);
        if (bytes < 0) {
            perror("recvfrom failed");
            continue;
        }
        buffer[bytes] = '\0';

        printf("Received: '%s'\n", buffer);

        if (strcmp(buffer, "Halt") == 0) {
            printf("Client sent Halt - terminating\n");
            break;
        }

        // Process string
        int len = strlen(buffer);
        int pal = is_palindrome(buffer);

        // Count vowels
        a_count = count_vowel(buffer, 'a');
        e_count = count_vowel(buffer, 'e');
        i_count = count_vowel(buffer, 'i');
        o_count = count_vowel(buffer, 'o');
        u_count = count_vowel(buffer, 'u');

        // Format result
        sprintf(result, "Palindrome: %s | Length: %d | a:%d e:%d i:%d o:%d u:%d",
                pal ? "YES" : "NO", len, a_count, e_count, i_count, o_count, u_count);

        printf("Sending: %s\n", result);

        // Send result back to client
        sendto(sockfd, result, strlen(result) + 1, 0, 
               (struct sockaddr*)&cli_addr, clilen);
    }

    close(sockfd);
    return 0;
}
