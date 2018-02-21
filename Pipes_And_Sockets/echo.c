#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>

#define PORT 8080
#define MAX 5
#define TURNS 1000000
#define TEST "pong"

int main(void){
	int sock;

	assert((sock = socket(AF_INET, SOCK_DGRAM, 0)) != -1);

	struct sockaddr_in name;
	name.sin_family = AF_INET;
	name.sin_port = htons(PORT);
	name.sin_addr.s_addr = htonl(INADDR_ANY);

	assert(bind(sock,(struct sockaddr *)&name, sizeof(name)) != -1);
	struct sockaddr_in client;
	int size = sizeof(struct sockaddr_in);

	for(int i = 0; i < TURNS; i++){
		int n;
		char buffer[MAX];
		n = recvfrom(sock,buffer,MAX-1,0,(struct sockaddr * )&client, &size);
		if(n == 1){
			perror("server");
		}
		buffer[n] = 0;	

		printf("Server: received: %s\n",buffer);
		//printf("Server: from destination: %s %d\n", inet_ntoa(client.sin_addr),ntohs(client.sin_port));

		assert(sendto(sock,TEST,n,0,(struct sockaddr *)&client, size) != -1);
		printf("Server sent: %s\n", TEST);
	}
}