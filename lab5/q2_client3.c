#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9090
#define MAX 200

int main() {

    int sock;
    struct sockaddr_in server_addr;

    char str1[MAX], str2[MAX], result[MAX];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    printf("Enter first string: ");
    fgets(str1, MAX, stdin);
    str1[strcspn(str1, "\n")] = 0;

    printf("Enter second string: ");
    fgets(str2, MAX, stdin);
    str2[strcspn(str2, "\n")] = 0;

    // Send strings
    send(sock, str1, sizeof(str1), 0);
    send(sock, str2, sizeof(str2), 0);

    // Receive result
    recv(sock, result, sizeof(result), 0);

    printf("\nServer Response: %s\n", result);

    close(sock);
    return 0;
}
