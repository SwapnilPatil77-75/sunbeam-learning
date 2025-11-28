#include <stdio.h>
#include <unistd.h>
#include<stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include<pthread.h>



#define SERV_IP "127.0.0.1"
#define SERV_PORT 2809

int serv_fd;

void handle_operation(void*);
void list_files(char *filePath, int cli_fd);

void sigint_handler(int sig){
    shutdown(serv_fd, SHUT_RDWR);
    _exit(0);
}

int count = 0;
int main(){
    int cli_fd, ret, flag = 0;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clisock_len = sizeof(cli_addr);

    //signal(SIGINT, sigint_handler);

    serv_fd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERV_IP);
    serv_addr.sin_port = htons(SERV_PORT);

    ret = bind(serv_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(serv_fd, 5);

    while(1){
        memset(&cli_addr, 0, sizeof(cli_addr));
        cli_fd = accept(serv_fd, (struct sockaddr *)&cli_addr, &clisock_len);

        printf("Server is activated at port 2809\n");

        if(cli_fd >= 0){
            if(flag != 1){
                int retu = mkdir("server_directory", 0744);
                if(retu == -1 && errno != EEXIST){
                    perror("mkdir");
                    _exit(-1);
                }
                flag = 1;
            }
            pthread_t t1;
            int *temp_cli_id = (int*)malloc(sizeof(int));
            *temp_cli_id = cli_fd;

            //handle_operation(cli_fd);
            ret = pthread_create(&t1,NULL,handle_operation,temp_cli_id);
            if(ret == 0){
                count+= 1;
                //printf(" %d Clients are connected to the server\n",count);
            }
            pthread_detach(t1);
        }
    }

    return 0;
}

void handle_operation(void *argm){
    printf("%d clients handled\n",count);
    char msg[1024];
    int n;
    int cli_fd = *(int*)argm;
    while(1){
        n = read(cli_fd, msg, sizeof(msg));
        if(n <= 0){
            close(cli_fd);
            break;
        }

        msg[n] = '\0';
        msg[strcspn(msg, "\r\n")] = '\0';
        if(strcmp(msg, "list") == 0){
            list_files("server_directory", cli_fd);
            write(cli_fd, "empty", strlen("empty") + 1);
            continue;
        }

        if(strncmp(msg,"mkdir",5) == 0){
            char temp1[1024];
            strcpy(temp1, msg);

            char *opt = strtok(temp1, " ");
            char *opt1 = strtok(NULL, " ");

            if(opt1 != NULL){
                char path[280];
                sprintf(path,"server_directory/%s",opt1);
                mkdir(path,0744);
            }
            continue;
        }
        if(strncmp(msg,"upload",6) == 0){
            char temp2[1024];
            strcpy(temp2, msg);

            char *opt = strtok(temp2, " ");
            char *opt1 = strtok(NULL, " ");

            if(opt1 != NULL){
                char path[280];
                sprintf(path,"server_directory/%s",opt1);

                FILE *fp = fopen(path,"w");
                if(!fp) continue;

                char data[1024];

                while(1){
                    int x = read(cli_fd,data,sizeof(data));
                    if(x <= 0) break;

                    data[x] = '\0';

                    if(strcmp(data,"empty") == 0)
                        break;

                    fputs(data,fp);
                }
                fclose(fp);
            }
            continue;
        }

        if(strncmp(msg,"delete",6) == 0){
            char temp3[1024];
            strcpy(temp3, msg);

            char *opt = strtok(temp3, " ");
            char *opt1 = strtok(NULL, " ");

            if(opt1 != NULL){
                char path[280];
                sprintf(path,"server_directory/%s",opt1);
                remove(path);
            }
            continue;
        }

        if(strncmp(msg,"download",8) == 0){
            char temp4[1024];
            strcpy(temp4, msg);

            char *opt = strtok(temp4," ");
            char *opt1 = strtok(NULL," ");

            if(opt1 == NULL){
                write(cli_fd,"empty",strlen("empty")+1);
                continue;
            }

            char path[280];
            sprintf(path,"server_directory/%s",opt1);

            FILE *fp = fopen(path,"r");
            if(fp == NULL){
                write(cli_fd,"empty",strlen("empty")+1);
                continue;
            }

            char data[1024];
            char *eof;

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
            continue;
        }
        if(strcmp(msg, "exit") == 0){
            close(cli_fd);
            count = count - 1;
            break;
        }
    }
}

void list_files(char *filePath, int cli_fd){
    DIR *temp = NULL;
    struct dirent *acc;
    char subdir[257];
    char msg[1024];

    temp = opendir(filePath);
    if(temp == NULL){
        printf("Cannot open the directory\n");
        return;
    }

    while((acc = readdir(temp)) != NULL){
        if(strcmp(acc->d_name, ".") == 0 || strcmp(acc->d_name, "..") == 0)
            continue;

        if(acc->d_type == DT_DIR){
            sprintf(subdir, "%s/%s", filePath, acc->d_name);
            strcpy(msg, acc->d_name);
            write(cli_fd, msg, strlen(msg) + 1);

            list_files(subdir, cli_fd);
        }
        else{
            strcpy(msg, acc->d_name);
            write(cli_fd, msg, strlen(msg) + 1);
        }
    }

    closedir(temp);
}

