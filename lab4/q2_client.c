#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "dns_socket"
#define MAX 100

int main() {
    int sock_fd;
    struct sockaddr_un addr;
    char domain[MAX], response[MAX];

    // Create socket
    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    // Connect to server
    if (connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    // Take input from user
    printf("Enter domain name: ");
    scanf("%s", domain);

    // Send domain to server
    write(sock_fd, domain, sizeof(domain));

    // Receive IP from server
    read(sock_fd, response, sizeof(response));

    printf("Server Response: %s\n", response);

    close(sock_fd);
    return 0;
}
