#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "dns_socket"
#define MAX 100

// Function to search database.txt
void search_database(char *domain, char *result) {
    FILE *fp = fopen("database.txt", "r");
    if (!fp) {
        strcpy(result, "Database not found");
        return;
    }

    char file_domain[100], ip[100];
    int found = 0;

    while (fscanf(fp, "%s %s", file_domain, ip) != EOF) {
        if (strcmp(domain, file_domain) == 0) {
            strcpy(result, ip);
            found = 1;
            break;
        }
    }

    if (!found)
        strcpy(result, "Domain not found");

    fclose(fp);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buffer[MAX], response[MAX];

    // Create UNIX socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Remove old socket file if exists
    unlink(SOCKET_PATH);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    // Bind
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Listen
    listen(server_fd, 5);
    printf("DNS Server is running...\n");

    while (1) {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        // Receive domain
        read(client_fd, buffer, sizeof(buffer));
        printf("Client requested domain: %s\n", buffer);

        // Search database
        search_database(buffer, response);

        // Send result
        write(client_fd, response, sizeof(response));

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
