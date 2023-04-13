#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#define NAMESIZE 30

typedef struct Entry {
	pid_t pid;
	char cmdName[NAMESIZE];
	time_t timestamp;
	int flag; // 0 -> c_exec | 1 -> status
} ENTRY;
