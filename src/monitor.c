#include "utils.h"
#include <glib.h>

ENTRY* entry_clone(ENTRY *e){
	ENTRY *novo = (ENTRY*) malloc(sizeof(struct Entry));
	novo->pid = e->pid;
	strcpy(novo->cmdName, e->cmdName);
	novo->timestamp = e->timestamp;
	novo->flag = e->flag;
	return novo;
}

void add_element_to_new_table(gpointer key, gpointer value, gpointer new_table) {
	ENTRY *clone = entry_clone((ENTRY*)value);
    g_hash_table_insert((GHashTable *)new_table, key, clone);
}

void sendStatus(GHashTable *process){
	int fd = open("fifo2", O_WRONLY);
	GHashTable *new_table = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, free);
    g_hash_table_foreach(process, add_element_to_new_table, new_table);

	struct timeval a;
	gettimeofday(&a, NULL);
	GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, new_table);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        ENTRY *v = (ENTRY*)value;
		ENTRY e;
		e.pid = v->pid;
		strcpy(e.cmdName, v->cmdName);
		e.timestamp = a.tv_usec - v->timestamp;
		e.flag = 0;
		write(fd, &e, sizeof(ENTRY));
    }

	ENTRY e;
	e.flag = 1;
	write(fd, &e, sizeof(ENTRY));
	g_hash_table_destroy(new_table);

	close(fd);
}


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
			perror("Could not create fifo file");
		}

	int fd = open("stats", O_RDONLY);
	if(fd == -1){
		perror("Failed to open FIFO\n");
	}
	int fd2 = open("stats", O_WRONLY);
	if(fd2 == -1){
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


	GHashTable *process = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, free);	 

	while(1){
		ENTRY *e = (ENTRY*) malloc(sizeof(struct Entry));
		read(fd, e, sizeof(ENTRY));

		if(e->flag == 1){
			sendStatus(process);
		}

		else if(g_hash_table_contains(process, GINT_TO_POINTER((int)e->pid)) == TRUE){
			ENTRY *v = g_hash_table_lookup(process, GINT_TO_POINTER((int)e->pid));
			ENTRY aux;
			aux.timestamp = e->timestamp - v->timestamp;
			strcpy(aux.cmdName, e->cmdName);
			aux.pid = e->pid;
			printf("[%d] Finished Command %s\n",e->pid,e->cmdName);
			write(child_pipe[1], &aux, sizeof(ENTRY));
			g_hash_table_remove(process, GINT_TO_POINTER((int)e->pid));
				
		} else {
			printf("[%d] Started Executing %s\n", e->pid, e->cmdName);
			g_hash_table_insert(process, GINT_TO_POINTER((int)e->pid), e);
		}
	}

	close(child_pipe[1]);
	close(fd);

	g_hash_table_destroy(process);

	return 0;
}
