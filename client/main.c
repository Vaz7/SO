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

	//nao tinhas inicializado o o_ptr a NULL, assim tinha UB, como pus corre sempre pelo menos 1x
	while(o_ptr != NULL || index==0){
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
	
	char string[1024]=""; //acho overkill usar uma string dinamica (que raio de comando vai usar 1000 chars?!)

	if(argc == 1){
		printf("Invalid Input");
		return 0;
	}

	//tomas, tinhas te esquecido de verificar o numero de argumentos antes de 
	//ir buscar o indice para comparar e dava segfault se faltassem flags

	
	if(!strcmp(argv[1], "execute") && argc >= 4){
		if(strcmp(argv[2], "-u") == 0){

			for(int i=3;i<argc;i++){ //imprimir para a string o comando a executar e as flags separados por " "
				strcat(string, argv[i]);
    			strcat(string, " ");
			}

			string[strlen(string)-1] = '\0'; //para tirar o ultimo espaço da string
			
			c_exec(string);
		}
			
		//else if(strcmp(argv[2], "-p") == 0)
			//TODO
	} else if (strcmp(argv[1], "status"))
		c_status();

	return 0;
}
