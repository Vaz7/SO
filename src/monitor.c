#include "utils.h"
#include <glib.h>

ENTRY* entry_clone(ENTRY *e){
	ENTRY *novo = (ENTRY*) malloc(sizeof(struct Entry));
	novo->pid = e->pid;
	strcpy(novo->cmdName, e->cmdName);
	novo->timestamp = e->timestamp;
	return novo;
}

void add_element_to_new_table(gpointer key, gpointer value, gpointer new_table) {
	ENTRY *clone = entry_clone((ENTRY*)value);
    	g_hash_table_insert((GHashTable *)new_table, key, clone);
}

void sendStatus(GHashTable *process, pid_t pid){
	char s_pid[10];
	sprintf(s_pid, "%d", pid);

	if(mkfifo(s_pid, 0777) == -1)
		if(errno != EEXIST)
			perror("Could not create status fifo");

	int fd = open(s_pid, O_WRONLY);

	GHashTable *new_table = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, free);
    	g_hash_table_foreach(process, add_element_to_new_table, new_table);

	struct timeval a;
	gettimeofday(&a, NULL);
	GHashTableIter iter;
    	gpointer key, value;
    
	int size = g_hash_table_size(process);

	write(fd, &size, sizeof(int));

	g_hash_table_iter_init(&iter, new_table);
    	
	while (g_hash_table_iter_next(&iter, &key, &value)) {
        	ENTRY *v = (ENTRY*)value;
		ENTRY e;
		e.pid = v->pid;
		strcpy(e.cmdName, v->cmdName);
		e.timestamp = a.tv_usec - v->timestamp;
		printf("Sending: %d; %s;\n", e.pid, e.cmdName);
		write(fd, &e, sizeof(ENTRY));
    	}

	g_hash_table_destroy(new_table);

	close(fd);
}


void fHandler(int pipe){
	int fd = open("exec_stats", O_CREAT | O_APPEND | O_WRONLY, 0600);
	if(fd == -1)
		perror("Failed to open file exec stats!\n");
	
	ENTRY e;
	int res;

	while((res = read(pipe, &e, sizeof(e))) > 0)
		write(fd, &e, res);

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
	ENTRY *e = (ENTRY*) malloc(sizeof(struct Entry));

	int res;

	while((res = read(fd, e, sizeof(ENTRY)) > 0)){

		if(!strcmp(e->cmdName, "status")){
			printf("[%d] Asked for Status\n", e->pid);
			if(fork() == 0)
				sendStatus(process, e->pid);
		}
		else if(g_hash_table_contains(process, GINT_TO_POINTER((int)e->pid)) == TRUE){
			printf("[%d] Finished Command %s\n", e->pid, e->cmdName);
			write(child_pipe[1], &e, res);
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
