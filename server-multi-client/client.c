#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_IP "127.0.0.1"
#define SERV_PORT 2809

void server_files(int cli_fd);

int main(int argc, char *argv[])
{
    int cli_fd, ret;
    struct sockaddr_in serv_addr;
    char options[100];

    cli_fd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERV_IP);
    serv_addr.sin_port = htons(SERV_PORT);

    ret = connect(cli_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if(ret == 0){
        printf("client is connected to server\n");
    }
    while(1){
        printf("options available\n");
        printf("<list> <mkdir> <upload> <download> <delete> Exit\n");

        scanf("%[^\n]%*c", options);
        printf("options %s\n",options);
        options[strcspn(options, "\r\n")] = '\0';
	char temp[1024];
	strcpy(temp,options);
        char *opt = strtok(temp," ");
        char *opt1 = strtok(NULL," ");
        printf("opt1 = %s\n",opt1);
        if(strcmp(opt, "Exit") == 0){
            close(cli_fd);
            break;
        }
        else if(strcmp(opt, "list") == 0){
            write(cli_fd,options,strlen(options)+1);
            server_files(cli_fd);
        }
        else if(strcmp(opt,"mkdir") == 0 && opt1 != NULL){
            printf("inside mkdir\n");
            write(cli_fd,options,strlen(options)+1);
        }
        else if(strcmp(opt,"upload") == 0 && opt1 != NULL){
            printf("inside upload");
            char data[1024];
            write(cli_fd,options,strlen(options)+1);
            char *eof;
            FILE *fp = fopen(opt1,"r");
            while(1){
                eof = fgets(data,sizeof(data),fp);
                if(eof == NULL){
                    write(cli_fd,"empty",strlen("empty")+1);
                    break;
                }
                write(cli_fd,data,strlen(data)+1);
                usleep(1000);
            }
            fclose(fp);
        }
        else if(strcmp(opt,"delete")== 0 && opt1 != NULL){
            write(cli_fd,options,strlen(options)+1);
        }
        else if(strcmp(opt,"download") == 0 && opt1 != NULL){
            char data[1024];
            write(cli_fd,options,strlen(options)+1);   // send download command

            FILE *fp;
            fp = fopen(opt1,"w");   // create local file

            while(1){
                int n = read(cli_fd, data, sizeof(data));
                if(n <= 0)
                    break;

                data[n] = '\0';

                if(strcmp(data,"empty") == 0)
                    break;

                fputs(data, fp);
            }

            fclose(fp);
        }
    }
    close(cli_fd);
    return 0;
}

void server_files(int cli_fd){
    char msg[1024];
    int n;

    printf("server files\n");

    while(1){
        n = read(cli_fd, msg, sizeof(msg));
        if(n <= 0) 
            break;

        msg[n] = '\0';

        if(strcmp(msg, "empty") == 0)
            break;

        printf("%s\n", msg);
    }
}

