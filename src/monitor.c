#include "utils.h"

int find_e(pid_t pid, int size, ENTRY* buf){
	for(int i = 0; i < size; i++)
		if(buf[i].pid == pid)
			return i;

	return -1;
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

	ENTRY e_buf[20];	
	ENTRY e; 

	while(1){
		read(fd, &e, sizeof(e));

		if((pos = find_e(e.pid, count, e_buf)) != -1){
			e_buf[pos].timestamp -= e.timestamp;
			printf("[%d] Finished Command %s\n",e.pid,e.cmdName);

			write(child_pipe[1], &e_buf[pos], sizeof(ENTRY)); // TODO CHECK
			
			free_pos = pos;
		} else {
			if(count >= 20) {
				//TODO GROW ARRAY NEED TO ALSO COMBINE THIS WITH FREE POS?
			}

			printf("[%d] Started Executing %s\n", e.pid, e.cmdName);
			
			if(count < 20)
				e_buf[count++] = e; 
			else
				e_buf[free_pos] = e;
		}
	}

	close(child_pipe[1]);
	close(fd);

	return 0;
}
