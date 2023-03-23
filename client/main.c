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
	char* o_ptr;
	char* array[20];
	int index = 0;

	while(o_ptr != NULL){
		o_ptr = strsep(&in_ptr, " ");
		array[index++] = o_ptr;
	}

	gettimeofday(&curT, NULL);

	if((pid = fork()) == 0){
		int ret = execvp(array[0], array);

		perror("Failed to execute command!\n");
		_exit(ret);
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

void c_status(){
	int pipe[2];

	//TODO
}

int main(int argc, char **argv){
	if(argc == 1){
		printf("Invalid Input");
		return 0;
	}

	if(strcmp(argv[1], "execute") == 0){
		if(strcmp(argv[2], "-u") == 0)
			c_exec(argv[3]);
		//else if(strcmp(argv[2], "-p") == 0)
			//TODO
	} else if (strcmp(argv[1], "status"))
		c_status();

	return 0;
}
