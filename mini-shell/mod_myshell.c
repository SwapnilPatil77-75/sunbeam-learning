#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include <fcntl.h>
void parse_string(char *input,char **args);

int int_ext(char **args);
void exe_internal(char **args);
void exe_exeternal(char **args);
void exe_pipe(char **args,char *input);
void exe_io_redirection(char **args);

int main(){
	char input[100];
	char *args[10];
	while(1){	
		printf("\nMyshell$ ");
		fgets(input,sizeof(input),stdin);
		input[strcspn(input,"\n")] = 0;

		char temp[100];
		strcpy(temp,input);
		parse_string(input,args);


		int ret = int_ext(args);
		if(ret == 1){
			exe_exeternal(args);
		}else if(ret == 2){
			exe_pipe(args,temp);
		}else if(ret == 3){
			exe_io_redirection(args);
		}
		else{
			exe_internal(args);

		}

	}
	return 0;

}
void parse_string(char *input,char **args){
	char *token;
	int i = 0;
	token = strtok(input," ");
	while(token != NULL){
		args[i++] = token;
		token = strtok(NULL," ");
	}
	args[i] = NULL;

}

int int_ext(char **args){

	for(int i = 0; args[i] != NULL; i++){
		if(strcmp(args[i],"|") == 0){
			return 2;
		}
	}

	for(int i = 0; args[i] != NULL; i++){
		if(strcmp(args[i],"<")== 0 || strcmp(args[i],">") == 0 || strcmp(args[i],">>") == 0){
			return 3;
		}
	}
	if(strcmp(args[0],"Exit") == 0 | strcmp(args[0],"exit") == 0){
		printf("Exiting\n");
		_exit(0);
	}else if((strcmp(args[0],"cd")) == 0 || strcmp(args[0],"mv")== 0 || strcmp(args[0],"help") == 0){
		return 0;
	}
	else{
		return 1;
	}

}

void exe_internal(char **args){

	if(strcmp(args[0],"cd")== 0){
		chdir(args[1]);
	}else if(strcmp(args[0],"mv")== 0){
		link(args[1],args[2]);
		unlink(args[1]);
	}else if(strcmp(args[0],"help") == 0){
		printf("The availble commnds are\n mv,pwd,cd,ls");
	}
}


void exe_exeternal(char **args){

	if(fork() == 0){
		execvp(args[0],args);
	}else{
		int s;
		wait(&s);
	}

}

void exe_pipe(char **args,char *input){
	char *str1;
	char *str2;
	char *args1[10];
	char *args2[10];
	char *token;
	int arr[2];
	int i = 0;
	pipe(arr);

	token = strtok(input,"|");
	str1 = token;
	token = strtok(NULL,"|");
	str2 = token;

	if (!str1 || !str2) {
		printf("Invalid pipe command\n");
		return;
	}	

	while(*str1 == ' ')str1++;
	while(*str2 == ' ')str2++;
	for (int j = strlen(str1) - 1; j >= 0 && str1[j] == ' '; j--) 
		str1[j] = '\0';
	for (int j = strlen(str2) - 1; j >= 0 && str2[j] == ' '; j--) 
		str2[j] = '\0';
	token = strtok(str1," ");
	while(token != NULL){
		args1[i++] = token;
		token = strtok(NULL," ");
	}
	args1[i] = NULL;

	i = 0;
	token = strtok(str2," ");
	while(token != NULL){
		args2[i++] = token;
		token = strtok(NULL," ");		
	}
	args2[i] = NULL;


	int pid1 = fork();
	if(pid1 == 0){
		close(arr[0]);
		dup2(arr[1],1);
		close(arr[1]);

		int fail = execvp(args1[0],args1);
		if(fail == -1){
			perror("exec failed");
			exit(-1);
		}
	}
	int pid2 = fork();
	if(pid2 == 0){
		close(arr[1]);
		dup2(arr[0],0);
		close(arr[0]);

		execvp(args2[0],args2);
		perror("exec failied");
		exit(-1);

	}

	close(arr[0]);
	close(arr[1]);
	int s1,s2;
	waitpid(pid1,&s1,0);
	waitpid(pid2,&s2,0);

}

void exe_io_redirection(char **args){
	int fd,s;

	// overwrite 
	for(int i = 0; args[i] != NULL; i++){
		if(strcmp(args[i],">") == 0){
			//open file
			fd = open(args[i+1],O_WRONLY | O_TRUNC | O_CREAT , 0644);
			if(fd == -1){
				perror("not opened");
				exit(-1);
			}
			
			args[i] = NULL; // cutoff other arguments
			if(fork() == 0){
				dup2(fd,1);
				close(fd);

				execvp(args[0],args);
			}
			close(fd);
			wait(&s);
			return;

		}
		
		// append to file
		if(strcmp(args[i],">>") == 0){

			fd = open(args[i+1],O_WRONLY | O_CREAT | O_APPEND,0644);
			if(fd == -1){
				perror("not opend");
				exit(-1);
			}

			args[i] = NULL;
			if(fork() == 0){
				dup2(fd,1);
				close(fd);

				execvp(args[0],args);

			}
			close(fd);
			wait(&s);
			return;
		}
		
		// input redirection 
		if(strcmp(args[i],"<") == 0){
			
			open(args[i+1],O_RDONLY);
			args[i] = NULL;
			if(fork() == 0){
				dup2(fd,0);
				close(fd);

				execvp(args[0],args);
			}
			close(fd);
			wait(&s);
			return;
		}

	}


}



