#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX 200

int client_count = 0;

int main() {

    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[MAX];

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    listen(server_fd, 5);

    printf("Server running...\n");

    // Create/overwrite file with Manipal keyword
    FILE *fp = fopen("output.txt", "w");
    fprintf(fp, "Manipal ");
    fclose(fp);

    while (1) {

        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);

        client_count++;

        // If clients exceed 2
        if (client_count > 2) {
            strcpy(buffer, "terminate session");
            send(client_fd, buffer, sizeof(buffer), 0);
            close(client_fd);
            printf("More than 2 clients. Terminating.\n");
            exit(0);
        }

        if (fork() == 0) {

            recv(client_fd, buffer, sizeof(buffer), 0);

            // Append to file
            FILE *fp = fopen("output.txt", "a");
            fprintf(fp, "%s ", buffer);
            fclose(fp);

            // Display result after 2 clients
            if (client_count == 2) {

                char line[MAX];
                FILE *fp = fopen("output.txt", "r");
                fgets(line, sizeof(line), fp);
                fclose(fp);

                printf("\nFinal Output: %s\n", line);
                printf("Client Address: %s : %d\n",
                       inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port));
            }

            close(client_fd);
            exit(0);
        }

        close(client_fd);
    }

    return 0;
}
