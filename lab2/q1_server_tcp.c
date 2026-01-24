#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 3388
#define BUFFER_SIZE 1024
#define MAX_STR 256

int count_string(const char *content, const char *str) {
    int count = 0, len = strlen(str);
    char *pos = strstr(content, str);
    while (pos) {
        count++;
        pos = strstr(pos + 1, str);
    }
    return count;
}

char* replace_string(char *content, const char *str1, const char *str2) {
    static char result[BUFFER_SIZE];
    int len1 = strlen(str1), len2 = strlen(str2);
    
    strcpy(result, content);
    char *pos = strstr(result, str1);
    if (!pos) return NULL;
    
    // Move rest of string
    memmove(pos + len2, pos + len1, strlen(pos + len1) + 1);
    memcpy(pos, str2, len2);
    
    return result;
}


void sort_ascii(char *content) {
    int len = strlen(content);
    for (int i = 0; i < len - 1; i++) {
        for (int j = 0; j < len - i - 1; j++) {
            if (content[j] > content[j + 1]) {
                char temp = content[j];
                content[j] = content[j + 1];
                content[j + 1] = temp;
            }
        }
    }
}

int main() {
    int sockfd, newsockfd, n;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    char buffer[BUFFER_SIZE], filename[MAX_STR], file_content[BUFFER_SIZE];
    int fd, choice;
    char *result_str;
    int opt = 1;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
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
    printf("TCP Server listening on port %d...\n", PORT);

    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
    if (newsockfd < 0) {
        perror("Accept failed");
        exit(1);
    }

    // Receive filename
    n = recv(newsockfd, filename, MAX_STR - 1, 0);
    filename[n] = '\0';
    printf("Client requested file: %s\n", filename);

    // Check if file exists
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        send(newsockfd, "File not present", 16, 0);
        close(newsockfd);
        close(sockfd);
        return 0;
    }

    // Read entire file
    n = read(fd, file_content, BUFFER_SIZE - 1);
    close(fd);
    file_content[n] = '\0';
    printf("File content loaded (%d bytes)\n", n);

    send(newsockfd, "File OK", 7, 0);  // Signal file exists

    while (1) {
        // Receive choice
        recv(newsockfd, &choice, sizeof(int), 0);
        printf("Client choice: %d\n", choice);

        if (choice == 4) {  // Exit
            break;
        }
        else if (choice == 1) {  // Search
            // recv(newsockfd, buffer, MAX_STR - 1, 0);
            // buffer[strlen(buffer)] = '\0';
            int len = recv(newsockfd, buffer, MAX_STR - 1, 0);
buffer[len] = '\0';

            int count = count_string(file_content, buffer);
            if (count > 0) {
                sprintf(buffer, "%d", count);
                send(newsockfd, buffer, strlen(buffer) + 1, 0);
            } else {
                send(newsockfd, "String not found", 15, 0);
            }
        }
        else if (choice == 2) {  // Replace
            char str1[MAX_STR], str2[MAX_STR];
            // recv(newsockfd, str1, MAX_STR - 1, 0);
            // recv(newsockfd, str2, MAX_STR - 1, 0);
            // str1[strlen(str1)] = '\0';
            // str2[strlen(str2)] = '\0';
            int len1 = recv(newsockfd, str1, MAX_STR - 1, 0);
str1[len1] = '\0';  // Proper null-termination

int len2 = recv(newsockfd, str2, MAX_STR - 1, 0);
str2[len2] = '\0';

            result_str = replace_string(file_content, str1, str2);
            if (result_str) {
                strcpy(file_content, result_str);

fd = open(filename, O_WRONLY | O_TRUNC);  // Overwrite original
    write(fd, file_content, strlen(file_content));
    close(fd);

                send(newsockfd, "String replaced", 14, 0);
            } else {
                send(newsockfd, "String not found", 15, 0);
            }
        }
        else if (choice == 3) {  // Reorder ASCII
            sort_ascii(file_content);
            fd = open(filename, O_WRONLY | O_TRUNC);
write(fd, file_content, strlen(file_content));
close(fd);
            send(newsockfd, "Reordered", 9, 0);
        }
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}
