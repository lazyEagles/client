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

int main(int argc, char *argv[])
{
	struct sockaddr_in sin;
	char ipaddr[MAX_LINE];
	char *ipptr;
	int sd;
	int buf[MAX_LINE];
	FILE *fd;
	int n;
	char *cptr;
	char eofchar = (char) EOF;
	const char *eofptr = &eofchar;

	bzero((char *) &sin, sizeof(sin));
	sin.sin_family = AF_INET;
	ipptr = "140.114.78.222"; /* ip address */
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
	send(sd, buf, MAX_LINE, 0);
	const char *clinfo = "\"CLOSE:\"";
	fd = fopen("syllabus.pdf", "w");
	while (recv(sd, buf, sizeof(buf), 0) > 0) {
		if (strncmp((char *)buf, clinfo, 8) == 0) {
			fprintf(stdout, "%s\n", (char *)buf);
			break;
		}
		int i;
		for (i = 0; i < MAX_LINE ; i++) {
			fputc(buf[i], fd);
			printf("%d:%x ", i, buf[i]);
			if (buf[i] == EOF)
				break;
		}
		printf("\n");
	}
	fclose(fd);
	fprintf(stdout, "close socket\n");
//	printf("close socket\n");	
	close(sd);
	return 0;
}
