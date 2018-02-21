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
	int flag = O_RDONLY;
	int pipe = open("sesame",flag);
	int flag2 = O_WRONLY;
	int pipe2 = open("emases",flag2);

	for (int i = 0; i < (ITERATIONS); i++){
		char buffer[5];
		read(pipe, &buffer, 4);
		buffer[4] = 0;
		printf("Client received: %s\n",buffer);
		write(pipe2,"pong", 4);
		printf("Client sent: pong\n");
	}
	return 0;
}
