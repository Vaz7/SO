#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char** argv){
	int fd = open("stats", O_RDONLY);
	if(fd == -1){
		perror("Failed to open FIFO\n");
	}

	pid_t pid;
	char nome[20];
	struct timeval startT, endT;
	double duration;

	read(fd, &pid, sizeof(pid_t));
	read(fd, &nome, sizeof(nome));
	read(fd, &startT, sizeof(struct timeval));
	read(fd,&endT, sizeof(struct timeval));

	duration = (endT.tv_sec - startT.tv_sec) + (endT.tv_usec - startT.tv_usec) / 1000000.0;

	printf("o pid é %d, nome é %s, duracao é %f\n",pid,nome,duration);


	return 0;
}
