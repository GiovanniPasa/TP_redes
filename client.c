#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void usage(int argc, char **argv) {
	printf("usage: %s <server IP> <server port>\n", argv[0]);
	printf("example: %s 127.0.0.1 51511\n", argv[0]);
	exit(EXIT_FAILURE);
}

#define BUFSZ 1024

int main(int argc, char **argv) {
	if (argc < 3) {
		usage(argc, argv);
	}
	struct sockaddr_storage storage;
	if (0 != addrparse(argv[1], argv[2], &storage)) {
		usage(argc, argv);
	}

	int s;
	
	char buf[BUFSZ];
	char input[BUFSZ];
	unsigned total = 0;
	
	do{
		s = socket(storage.ss_family, SOCK_STREAM, 0);
		if (s == -1) {
			logexit("socket");
		}
		struct sockaddr *addr = (struct sockaddr *)(&storage);
		if (0 != connect(s, addr, sizeof(storage))) {
			logexit("connect");
		}

		char addrstr[BUFSZ];
		addrtostr(addr, addrstr, BUFSZ);

	
		
		memset(buf, 0, BUFSZ);
		printf("> ");
		fgets(buf, BUFSZ-1, stdin);
		strcpy(input, buf);
		size_t count = send(s, buf, strlen(buf)+1, 0);
		
		if (count != strlen(buf)+1) {
			logexit("send");
		}

		memset(buf, 0, BUFSZ);
		total = 0;
		
		while(1) {
			count = recv(s, buf + total, BUFSZ - total, 0);
			if (count == 0) {
				// Connection terminated
				break;
			}
			
			total += count;
			
		}
		//se receber disconnect do server ele morre, caso contrario continua no loop e buf é printado
		if(strcmp(buf, "disconnect")){
			puts(buf);
		}
		close(s);
	}while((strcmp(buf, "disconnect")));	

	exit(EXIT_SUCCESS);
}