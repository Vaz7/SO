#include "utils.h"

int main(int argc, char** argv){
	if(mkfifo("stats", 0777) == -1)
		if(errno != EEXIST){
			printf("Could not create fifo file\n");
		}

	int fd = open("stats", O_RDONLY);
	if(fd == -1){
		perror("Failed to open FIFO\n");
	}

	int duration=0;
	
	ENTRY e, e_end; //Change top array of entries that are then matched after prog ends

	read(fd, &e, sizeof(e));

	read(fd,&e_end, sizeof(e));

	e_end.timestamp -= e.timestamp;

	printf("[%d] Finished Command %s\n",e.pid,e.cmdName);

	return 0;
}
