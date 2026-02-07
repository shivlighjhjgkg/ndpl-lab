#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 9090
#define MAX 200

// Function to check anagram
int isAnagram(char *s1, char *s2) {

    int count[256] = {0};

    if(strlen(s1) != strlen(s2))
        return 0;

    for(int i = 0; s1[i]; i++) {
        count[s1[i]]++;
        count[s2[i]]--;
    }

    for(int i = 0; i < 256; i++) {
        if(count[i] != 0)
            return 0;
    }

    return 1;
}

int main() {

    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    char str1[MAX], str2[MAX], result[MAX];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    listen(server_fd, 5);

    printf("Iterative Server Running...\n");

    while(1) {

        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);

        // Display client address
        printf("\nConnected to Client: %s : %d\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));

        // Display date and time
        time_t now = time(NULL);
        printf("Date & Time: %s", ctime(&now));

        // Receive strings
        recv(client_fd, str1, sizeof(str1), 0);
        recv(client_fd, str2, sizeof(str2), 0);

        // Check anagram
        if(isAnagram(str1, str2))
            strcpy(result, "Strings are Anagrams");
        else
            strcpy(result, "Strings are NOT Anagrams");

        // Send result to client
        send(client_fd, result, sizeof(result), 0);

        close(client_fd); // Move to next client
    }

    close(server_fd);
    return 0;
}
