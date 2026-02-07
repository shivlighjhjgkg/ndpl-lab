#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3388
#define MAX 200

int main() {

    int sockfd, option;
    struct sockaddr_in serv_addr;

    char input[MAX], buffer[MAX];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr);

    connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

    printf("1. Registration Number\n");
    printf("2. Student Name\n");
    printf("3. Subject Code\n");

    scanf("%d",&option);

    printf("Enter detail: ");
    scanf("%s",input);

    send(sockfd,&option,sizeof(option),0);
    send(sockfd,input,strlen(input)+1,0);

    recv(sockfd,buffer,MAX,0);
    printf("\nServer Response:\n%s\n",buffer);

    close(sockfd);
    return 0;
}
