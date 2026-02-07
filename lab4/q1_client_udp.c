#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3388
#define MAX 300

int main() {

    int sockfd;
    struct sockaddr_in serv_addr;
    socklen_t len;

    int option;
    char input[MAX], buffer[MAX];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr);

    len = sizeof(serv_addr);

    printf("1. Registration Number\n");
    printf("2. Student Name\n");
    printf("3. Subject Code\n");

    scanf("%d",&option);

    printf("Enter detail: ");
    scanf(" %[^\n]",input);

    // Send option
    sendto(sockfd,&option,sizeof(option),0,
           (struct sockaddr*)&serv_addr,len);

    // Send detail
    sendto(sockfd,input,strlen(input)+1,0,
           (struct sockaddr*)&serv_addr,len);

    // Receive reply
    recvfrom(sockfd,buffer,MAX,0,NULL,NULL);

    printf("\nServer Response:\n%s\n",buffer);

    close(sockfd);
    return 0;
}
