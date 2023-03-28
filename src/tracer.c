#include "utils.h"

void c_exec(char* cmd){
	struct timeval curT, endT;
	char* in_ptr = cmd;
	char* o_ptr=NULL;
	char* array[20];//o tamanho deste array é kinda sus
	int index = 0;
	int fd=0;
	
	while(o_ptr != NULL || index==0){
		o_ptr = strsep(&in_ptr, " ");
		array[index++] = o_ptr;
	}

	//Save info for server
	ENTRY e;
	
	strcpy(e.cmdName, array[0]);
	strcat(e.cmdName,"\0");

	gettimeofday(&curT, NULL);
	
	//abre o pipe para leitura
	fd = open("stats", O_WRONLY);
		if(fd == -1)
			perror("Failed to open FIFO\n");

	if((e.pid = fork()) == 0){
		int ret = execvp(array[0], array);
		
		perror("Failed to execute command!\n");
		_exit(-1);// só da exit se falhar, por isso deve dar de -1
	} else{
		int status;
		wait(&status);

		if(WEXITSTATUS(status) != -1){

			gettimeofday(&endT, NULL);
			e.timestamp = endT.tv_sec - curT.tv_sec;

			write(fd, &e, sizeof(e));
		
			int duration = e.timestamp * 1000;
			printf("Exec Time: %d ms\n", duration);
		}

		close(fd);
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

	if(mkfifo("stats", 0777) == -1){
        if(errno != EEXIST){
            printf("Could not create fifo file\n"); //verifica se o fifo foi criado/já estava criado, se der erro nao prossegue
            return 1;
        }
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
