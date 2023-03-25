#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#define NAMESIZE 30

int main(int argc, char** argv){
	int fd = open("stats", O_RDONLY);
	if(fd == -1){
		perror("Failed to open FIFO\n");
	}

	pid_t pid = 0, pid_terminou;
	char nome[NAMESIZE]="";
	struct timeval startT, endT;
	double duration=0;

	read(fd, &pid, sizeof(pid_t));
	read(fd, &nome,sizeof(char)*sizeof(nome));
	read(fd, &startT, sizeof(struct timeval));
	read(fd, &endT, sizeof(struct timeval));
	read(fd, &pid_terminou, sizeof(pid_t));


	duration = (endT.tv_sec - startT.tv_sec) + (endT.tv_usec - startT.tv_usec) / 1000000.0;

	printf("o pid é %d, nome é %s, duracao é %f, o pid que terminou foi %d\n",pid,nome,duration, pid_terminou);


	return 0;
}
