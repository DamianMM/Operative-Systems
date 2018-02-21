#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ITERATIONS 1000000

int main(){
	int mode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
	mkfifo("sesame", mode);
	mkfifo("emases", mode);

	int flag = O_WRONLY;
	int flag2 = O_RDONLY;
	int pipe = open("sesame",flag);
	int pipe2 = open("emases",flag2);
	
	for (int i = 0; i < ITERATIONS; i++){
		char buffer[5];
		write(pipe,"ping",4);
		printf("Server sent: ping\n");
		read(pipe2, &buffer,4);
		buffer[4] = 0;
		printf("Server received: %s\n", buffer);
	}
}