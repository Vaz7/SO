#include "utils.h"
#include <glib.h>


// COMPILER : gcc `pkg-config --cflags glib-2.0` monitor.c `pkg-config --libs glib-2.0` 



void fHandler(int pipe){
	int fd = open("exec_stats", O_CREAT | O_APPEND | O_WRONLY, 0600);
	if(fd == -1)
		perror("Failed to open file exec stats!\n");
	
	ENTRY e;

	while(1){
		read(pipe, &e, sizeof(e));
		write(fd, &e, sizeof(e));
	}

	close(fd);
	close(pipe);
	_exit(0);
}


int main(int argc, char** argv){

	if(mkfifo("stats", 0777) == -1)
		if(errno != EEXIST){
			printf("Could not create fifo file\n");
		}

	int fd = open("stats", O_RDWR); // Needs to be RDWR or pip won't block
					// which causes seg. fault
	if(fd == -1){
		perror("Failed to open FIFO\n");
	}

	int child_pipe[2];

	if(pipe(child_pipe) == -1){
		perror("Failed to create pipe to file writer!\n");
	}	

	if(fork() == 0){
		close(child_pipe[1]);
		fHandler(child_pipe[0]);
	}

	close(child_pipe[0]);

	int count = 0, pos = 0, free_pos = 0;
	GHashTable *process = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
	ENTRY e_buf[20];	
	ENTRY e; 

	while(1){
		read(fd, &e, sizeof(e));

		if(g_hash_table_contains(process, GINT_TO_POINTER((int)e.pid)) == TRUE){
			ENTRY *v = g_hash_table_lookup(process, GINT_TO_POINTER((int)e.pid));
			ENTRY aux;
			aux.timestamp = e.timestamp - v->timestamp;
			strcpy(aux.cmdName, e.cmdName);
			aux.pid = e.pid;
			printf("[%d] Finished Command %s\n",e.pid,e.cmdName);
			write(child_pipe[1], &aux, sizeof(ENTRY)); // TODO CHECK
			g_hash_table_remove(process, GINT_TO_POINTER((int)e.pid));
				
		} else {
			printf("[%d] Started Executing %s\n", e.pid, e.cmdName);
			g_hash_table_insert(process, GINT_TO_POINTER((int)e.pid), &e);
		}
	}

	close(child_pipe[1]);
	close(fd);

	return 0;
}
