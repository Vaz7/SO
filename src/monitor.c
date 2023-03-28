#include "utils.h"

int main(int argc, char** argv){
	int fd = open("stats", O_RDONLY);
	if(fd == -1){
		perror("Failed to open FIFO\n");
	}

	int duration=0;
	
	ENTRY e;

	read(fd, &e, sizeof(e));

	duration = e.timestamp * 1000;

	printf("o pid é %d, nome é %s, duracao é %f, o pid que terminou foi %d\n",e.pid,e.cmdName,duration);

	return 0;
}
