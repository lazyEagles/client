#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT 33568
#define CLIENT_PORT 33567
#define MAX_PENDING 5
#define MAX_LINE 256

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("usage <host>\n");
		exit(-1);
	}

	struct sockaddr_in sin;
	char ipaddr[MAX_LINE];
	char *ipptr = argv[1];
	int sd;
	int buf[MAX_LINE];
	FILE *fd;
	int n;
	char *cptr;
	char eofchar = (char) EOF;
	const char *eofptr = &eofchar;
	const char *download = "102062999.jpg";
	unsigned int file_size = 0;

	bzero((char *) &sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(ipptr);
	sin.sin_port = htons(SERVER_PORT);

	/* create a socket */
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("simplex-talk: socket");
		exit(EXIT_FAILURE);
	}

	/* connect to server */
	if (connect(sd, (struct sockaddr*) &sin, sizeof(sin)) < 0) {
		perror("connect");
		exit(EXIT_FAILURE);
	}

	recv(sd, buf, sizeof(buf), 0);
	fprintf(stdout, "%s\n", (char *)buf);
	fgets((char *)buf, MAX_LINE, stdin);
	send(sd, buf, MAX_LINE, 0); // should send GET:xxx.jpg
	const char *clinfo = "\"CLOSE:\"";
	fd = fopen(download, "w");
	int m;
	while ((m = recv(sd, buf, sizeof(buf), 0)) > 0) {
		printf("%d\n", m);
		usleep(1000);
		if (strncmp((char *)buf, clinfo, 8) == 0) {
			fprintf(stdout, "%s\n", (char *)buf);
			break;
		}
		int i;
		for (i = 0; i < MAX_LINE ; i++) {
			fputc(buf[i], fd);
			file_size++;
			//printf("%d:%x ", i, buf[i]);
			if (buf[i] == EOF)
				break; // download complete
		}
		//printf("\n");
	}
	fclose(fd);
	fprintf(stdout, "close socket\n");
//	printf("close socket\n");	
	printf("we get %d bytes\n",file_size);
	close(sd);
	return 0;
}
