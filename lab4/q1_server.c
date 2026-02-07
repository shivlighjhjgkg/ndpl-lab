#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 3388
#define MAX 300

void trim(char *str){
    str[strcspn(str,"\n")] = 0;
}

int main() {

    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    int option;
    char input[MAX], buffer[MAX];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    listen(sockfd,5);

    printf("Server Running...\n");

    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,(struct sockaddr*)&cli_addr,&clilen);

    recv(newsockfd,&option,sizeof(option),0);
    recv(newsockfd,input,MAX,0);

    trim(input);

    pid_t pid = fork();

    if(pid == 0) {

        FILE *fp = fopen("students.txt","r");
        if(!fp){
            perror("File error");
            exit(1);
        }

        char line[MAX];

        while(fgets(line,MAX,fp)) {

            char reg[50], name[50], dept[50], sem[10], sec[10];
            char os[20], db[20], net[20], addr[50];

            sscanf(line,"%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]",
                   reg,name,dept,sem,sec,os,db,net,addr);

            trim(reg);
            trim(name);

            // OPTION 1
            if(option == 1 && strcmp(reg,input)==0){

                sprintf(buffer,
                "Child PID %d\nName: %s\nAddress: %s\n",
                getpid(),name,addr);

                send(newsockfd,buffer,strlen(buffer)+1,0);
                fclose(fp);
                exit(0);
            }

            // OPTION 2
            if(option == 2 && strcmp(name,input)==0){

                sprintf(buffer,
                "Child PID %d\nDept: %s\nSemester: %s\nSection: %s\nCourses: OS, DB, NET\n",
                getpid(),dept,sem,sec);

                send(newsockfd,buffer,strlen(buffer)+1,0);
                fclose(fp);
                exit(0);
            }

            // OPTION 3
            if(option == 3){

                if(strcmp(input,"OS")==0){
                    sprintf(buffer,"Child PID %d\nMarks: %s\n",getpid(),os);
                }
                else if(strcmp(input,"DB")==0){
                    sprintf(buffer,"Child PID %d\nMarks: %s\n",getpid(),db);
                }
                else if(strcmp(input,"NET")==0){
                    sprintf(buffer,"Child PID %d\nMarks: %s\n",getpid(),net);
                }
                else{
                    strcpy(buffer,"Subject Not Found");
                }

                send(newsockfd,buffer,strlen(buffer)+1,0);
                fclose(fp);
                exit(0);
            }
        }

        send(newsockfd,"Student Not Found",18,0);
        fclose(fp);
        exit(0);
    }

    wait(NULL);

    close(newsockfd);
    close(sockfd);

    return 0;
}
