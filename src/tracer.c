#include "utils.h"

void c_exec(char* cmd){
	struct timeval curT, endT;
	char* in_ptr = cmd;
	char* o_ptr=NULL;
	char* array[NAMESIZE];//o tamanho deste array é kinda sus
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
	e.timestamp = curT;
	
	//abre o pipe para leitura
	fd = open("stats", O_WRONLY);
	if(fd == -1)
		perror("Failed to open FIFO\n");

	if((e.pid = fork()) == 0){
		printf("[%d] Executing command...", getpid());
		int ret = execvp(array[0], array);
		
		perror("Failed to execute command!\n");
		_exit(ret);// só da exit se falhar, por isso deve dar de -1
	} else{
		write(fd, &e, sizeof(e));
		
		int status;
		wait(&status);

		if(WEXITSTATUS(status) != -1){
			gettimeofday(&endT, NULL);


			write(fd, &e, sizeof(e));
			
			long int duration = (endT.tv_sec - curT.tv_sec) * 1000 + (endT.tv_usec - curT.tv_usec) / 1000;
			printf("Exec Time: %ld ms\n", duration);
		}

		close(fd);
	}
}


void removeEspacos(char *str) {
    int i, j;
    for (i = 0, j = 0; str[i]; i++) {
        if (str[i] != ' ') {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}


char** parsePipes(char *cmd){
    char **array = malloc(20 * sizeof(char*));
    int i = 0;
    char *aux;

    while((aux = strsep(&cmd, "|")) != NULL){
        if(i!=0) array[i++] = aux+1;
        else array[i++] = aux;
    }

    array[i] = NULL;
    return array;
}

char*** parseArgs(char** cmd){
    char ***matriz = malloc(20 * sizeof(char**));
    int i = 0;
    
    while (cmd[i] != NULL) {
        char *str = strdup(cmd[i]);
        char *aux;
        int j = 0;
        
        matriz[i] = malloc(20 * sizeof(char*));
        
        while ((aux = strsep(&str, " ")) != NULL) {
            if(strcmp(aux, "") != 0){
				matriz[i][j++] = aux;
				//printf("%s\n", matriz[i][j-1]);
			}
        }

        matriz[i][j] = NULL;
        i++;
    }
    matriz[i] = NULL;
    return matriz;
}

void freeArgs(char ***args){
	/*for (int i = 0; args[i] != NULL; i++) {
		for (int j = 0; args[i][j] != NULL; j++) {
			free(args[i][j]);
		}
	}*/ 
	// está a faltar dar um minor free aqui

	for (int i = 0; args[i] != NULL; i++) {
		free(args[i]);
	}
	free(args);
}

void pipeline(char ***cmd){
	int len = 0;
    while (cmd[len] != NULL) {
        len++;
    }

	int pipes[len-1][2];

	for(int i = 0; i< len; i++){
        if(i == 0){
            pipe(pipes[i]);
            if(fork()==0){
                close(pipes[i][0]);
                dup2(pipes[i][1], 1);
                close(pipes[i][1]);
                execvp(cmd[i][0], cmd[i]);
            }
            else{
                close(pipes[i][1]);
            }
        }
        else if(i == len-1){
            if(fork()==0){
                dup2(pipes[i-1][0],0);
                close(pipes[i-1][0]);
                execvp(cmd[i][0], cmd[i]);
            }
            else{
                close(pipes[i-1][0]);
            }
        }
        else{
            pipe(pipes[i]);
            if(fork()==0){
                close(pipes[i][0]);
                dup2(pipes[i-1][0],0);
                close(pipes[i-1][0]);
                dup2(pipes[i][1],1);
                close(pipes[i][1]);
                execvp(cmd[i][0], cmd[i]);
            }
            else{
                close(pipes[i-1][0]);
                close(pipes[i][1]);
            }
        }
    }

    for(int j = 0; j < len; j++){
        wait(NULL);
    }


	return;
}


void p_exec(char *cmd){
	struct timeval curT, endT;
	ENTRY e;
	strcpy(e.cmdName,cmd);

	char **pipes = parsePipes(cmd);
	char ***commands = parseArgs(pipes);

	int fd = open("stats", O_WRONLY);
	if(fd == -1)
		perror("Failed to open FIFO\n");

	gettimeofday(&curT, NULL);
	e.timestamp = curT;

	if((e.pid = fork()) == 0){
		printf("[%d] Executing command...\n", getpid());
		pipeline(commands);
	} 
	else{
		write(fd, &e, sizeof(e));

		int status;
		wait(&status);

		if(WEXITSTATUS(status) != -1){
			gettimeofday(&endT, NULL);
			e.timestamp = endT;

			write(fd, &e, sizeof(ENTRY));
		
			long int duration = (endT.tv_sec - curT.tv_sec) * 1000 + (endT.tv_usec - curT.tv_usec) / 1000;
			printf("Exec Time: %ld ms\n", duration);
		}

		close(fd);
	}
	

	free(pipes);
	freeArgs(commands);
}

void c_status(){ // falta meter para múltiplos users e verificar o timestamp?
	pid_t pid = getpid();
	char s_pid[10];

	sprintf(s_pid, "%d", pid);

	if(mkfifo(s_pid, 0777) == -1)
		if(errno != EEXIST){
			perror("Could not create fifo file");
		}

	ENTRY e;

	e.pid = pid;
	strcpy(e.cmdName, "status\0");
	e.timestamp.tv_sec = 0;
	e.timestamp.tv_usec = 0;

	int fd = open("stats", O_WRONLY);
	write(fd, &e, sizeof(ENTRY));
	close(fd);

	int fd2 = open(s_pid, O_RDONLY);
	int size;
	
	read(fd2, &size, sizeof(int));

	for(int i = 0; i < size; i++){
		if(read(fd2, &e, sizeof(ENTRY)) > 0){
			long int duration = e.timestamp.tv_sec * 1000 + e.timestamp.tv_usec / 1000;
			printf("Pid: %d; Executing: %s; Current Duration: %ld ms\n", e.pid, e.cmdName, duration);
		} else {
			close(fd2);
			unlink(s_pid);
			perror("Problem in status read!");
		}
	}

	close(fd2);
	unlink(s_pid);
}

void stats_time(int argc, char **argv){
	pid_t aux;
	pid_t pid = getpid();
	char s_pid[10];
	long int duration=-1;
	sprintf(s_pid, "%d", pid);

	if(mkfifo(s_pid, 0777) == -1)
		if(errno != EEXIST){
			perror("Could not create fifo file");
		}

	ENTRY e;

	e.pid = pid;
	strcpy(e.cmdName, "stats-time\0");
	e.timestamp.tv_sec = 0;
	e.timestamp.tv_usec = 0;

	int fd = open("stats", O_WRONLY);//para enviar o comando para o monitor
	write(fd, &e, sizeof(ENTRY));

	int fd2 = open(s_pid,O_WRONLY);
	
	for(int i=2;i<argc;i++){
		aux = atoi(argv[i]);
		write(fd2,&aux,sizeof(int));
	}

	close(fd2);
	close(fd);
	
	fd2 = open(s_pid,O_RDONLY);

	read(fd2,&duration,sizeof(long int));
	
	close(fd2);

	printf("duration = %ld\n",duration);
	
	
	
	
	unlink(s_pid);



}



int main(int argc, char **argv){
	
	removeEnters(argv[argc-1]);

	if(argc == 1){
		printf("Invalid Input");
		return 0;
	}

	if(mkfifo("stats", 0777) == -1)
        	if(errno != EEXIST){
            		printf("Could not create fifo file\n"); //verifica se o fifo foi criado/já estava criado, se der erro nao prossegue
            		return 1;
        	}
	
	if(!strcmp(argv[1], "execute") && argc == 4){
		if(strcmp(argv[2], "-u") == 0)
			c_exec(argv[3]);
			
		else if(strcmp(argv[2], "-p") == 0){
			p_exec(argv[3]);
		}	
		else printf("Invalid option\n");
	}
	
	else if (!strcmp(argv[1], "status") && argc == 2){
		c_status();
	}

	else if(!strcmp(argv[1],"stats-time") && argc >= 3){
		stats_time(argc,argv);
	}

	else if(!strcmp(argv[1],"stats-command") && argc >= 3){
		printf("A fazer status command\n");
	}

	else if(!strcmp(argv[1],"stats-uniq") && argc >= 3){
		printf("A fazer stats uniq\n");
	}


	else printf("Invalid command name or count.\n");

	return 0;
}
