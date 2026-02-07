#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define PORT 3388
#define MAX 300

void trim(char *str){
    str[strcspn(str,"\n")] = 0;
}

int main() {

    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t len;

    char buffer[MAX];
    int option;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

    printf("UDP Server Running...\n");

    len = sizeof(cli_addr);

    // Receive option
    recvfrom(sockfd,&option,sizeof(option),0,
             (struct sockaddr*)&cli_addr,&len);

    // Receive input
    recvfrom(sockfd,buffer,MAX,0,
             (struct sockaddr*)&cli_addr,&len);

    trim(buffer);

    pid_t pid = fork();

    if(pid == 0){

        FILE *fp = fopen("students.txt","r");
        if(!fp){
            perror("File error");
            exit(1);
        }

        char line[MAX];
        char reply[MAX];

        while(fgets(line,MAX,fp)){

            char reg[50], name[50], dept[50], sem[10], sec[10];
            char os[20], db[20], net[20], addr[50];

            sscanf(line,"%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]",
                   reg,name,dept,sem,sec,os,db,net,addr);

            trim(reg);
            trim(name);

            // OPTION 1
            if(option==1 && strcmp(reg,buffer)==0){

                sprintf(reply,
                "Child PID %d\nName: %s\nAddress: %s\n",
                getpid(),name,addr);

                sendto(sockfd,reply,strlen(reply)+1,0,
                       (struct sockaddr*)&cli_addr,len);

                fclose(fp);
                exit(0);
            }

            // OPTION 2
            if(option==2 && strcmp(name,buffer)==0){

                sprintf(reply,
                "Child PID %d\nDept: %s\nSemester: %s\nSection: %s\nCourses: OS, DB, NET\n",
                getpid(),dept,sem,sec);

                sendto(sockfd,reply,strlen(reply)+1,0,
                       (struct sockaddr*)&cli_addr,len);

                fclose(fp);
                exit(0);
            }

            // OPTION 3
            if(option==3){

                if(strcmp(buffer,"OS")==0)
                    sprintf(reply,"Child PID %d\nMarks: %s\n",getpid(),os);

                else if(strcmp(buffer,"DB")==0)
                    sprintf(reply,"Child PID %d\nMarks: %s\n",getpid(),db);

                else if(strcmp(buffer,"NET")==0)
                    sprintf(reply,"Child PID %d\nMarks: %s\n",getpid(),net);

                else
                    strcpy(reply,"Subject Not Found");

                sendto(sockfd,reply,strlen(reply)+1,0,
                       (struct sockaddr*)&cli_addr,len);

                fclose(fp);
                exit(0);
            }
        }

        sendto(sockfd,"Student Not Found",18,0,
               (struct sockaddr*)&cli_addr,len);

        fclose(fp);
        exit(0);
    }

    wait(NULL);
    close(sockfd);

    return 0;
}
