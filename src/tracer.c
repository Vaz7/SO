#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#define NAMESIZE 30


void c_exec(char* cmd){
	int pid;
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
	char nome[NAMESIZE];
	strcpy(nome, array[0]);

	gettimeofday(&curT, NULL);

	//abre o pipe para escrita e leitura
	fd = open("stats", O_RDWR);
		if(fd == -1){
			perror("Failed to open FIFO\n");
		}


	if((pid = fork()) == 0){
		pid_t pidfilho = getpid();

		

		write(fd, &pidfilho, sizeof(pid_t));
		nome[strlen(nome-1)]='\0';
		write(fd,nome,sizeof(char)*NAMESIZE);
		write(fd,&curT,sizeof(struct timeval)); // writes para passar para o servidor a info
		
		
		printf("Child process ID: %d\n", getpid()); // Print child process ID
		
		int ret = execvp(array[0], array);
		
		
		perror("Failed to execute command!\n");
		_exit(-1);// só da exit se falhar, por isso deve dar de -1
	} else {
		//TODO Use named pipes to send to server pid, array[0] and
		//curT.tv_sec 
		//handel closing of pipe writing to server
		wait(NULL);
	

		gettimeofday(&endT, NULL);

		write(fd,&endT,sizeof(struct timeval));
		
		close(fd);

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
