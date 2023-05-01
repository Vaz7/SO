#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#define NAMESIZE 70

typedef struct Entry {
	pid_t pid;
	char cmdName[NAMESIZE];
	time_t timestamp;
} ENTRY;

void removeEnters(char *string){//remove enters xDD
    int size = strlen(string);
    size--;

    if(string[size]=='\n'){
    string[size]='\0';
    }
}
