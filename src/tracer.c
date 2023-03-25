#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>

void c_exec(char* cmd){
	int pid;
	struct timeval curT;

	char* in_ptr = cmd;
	char* o_ptr=NULL;
	char* array[20];//o tamanho deste array é kinda sus
	int index = 0;

	
	while(o_ptr != NULL || index==0){
		o_ptr = strsep(&in_ptr, " ");
		array[index++] = o_ptr;
	}

	gettimeofday(&curT, NULL);

	if((pid = fork()) == 0){
		printf("Child process ID: %d\n", getpid()); // Print child process ID
		
		int ret = execvp(array[0], array);

		perror("Failed to execute command!\n");
		_exit(pid);
	} else {
		int pipe[2];
		//TODO Use named pipes to send to server pid, array[0] and
		//curT.tv_sec 
		//handel closing of pipe writing to server
		wait(NULL);

		gettimeofday(&curT, NULL);

		//Open named pipe to write to server
		//write pid and curT
		//close pipe
	}
}

void p_exec(char *cmd){
	char* in_ptr = cmd;
	char* o_ptr=NULL;
	char* array[20];//o tamanho deste array é kinda sus
	int index = 0;

	
	while(o_ptr != NULL || index==0){
		o_ptr = strsep(&in_ptr, " ");
		array[index++] = o_ptr;
	}


	for(int i=0;i<index-1;i++){
		printf("%s\n",array[i]);
	}
	//fazer cenas 
}

void c_status(){
	int pipe[2];

	//TODO
}

int main(int argc, char **argv){
	

	if(argc == 1){
		printf("Invalid Input");
		return 0;
	}
	
	if(!strcmp(argv[1], "execute") && argc == 4){
		if(strcmp(argv[2], "-u") == 0){
			
			c_exec(argv[3]);
		}
			
		else if(strcmp(argv[2], "-p") == 0){

			p_exec(argv[3]);
		}

		else printf("Invalid option\n");

	} else if (strcmp(argv[1], "status"))
		c_status();
	
	else printf("Invalid command name or count.\n");

	return 0;
}
