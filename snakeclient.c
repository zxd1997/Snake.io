#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<time.h>
#include<pthread.h>
#include <termio.h>
#define TTY_PATH            "/dev/tty"
#define STTY_US             "stty -echo -icanon -F"
#define STTY_DEF            "stty echo icanon -F"
#define PORT 8888
#define length 30
#define width 100
int map[length][width];
int seedx = 0, seedy = 0, score = -1, t = 400000, level = 1;
static int get_char()
{
	fd_set rfds;
	struct timeval tv;
	int ch = 0;
	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	tv.tv_sec = 0;
	if (score == level) 
	{
		t /= 2;
		level += 2;
	}
	tv.tv_usec = t;
	if (select(1, &rfds, NULL, NULL, &tv) > 0)
		ch = getchar();
	else ch = 'w';
	return ch;
}
int connect_server(char *addr)
{
	int sock_id; 
	struct sockaddr_in server;
	sock_id = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_id == -1)
	{
		perror("creating socket failed");
		return -1;
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = inet_addr(addr);
	if (connect(sock_id, (struct sockaddr*)&server, sizeof(server)) == -1)
	{
		printf("Connetc Error\n");
		exit(0);
	}
	return sock_id;
}
int main(int ac, char* argv[])
{
	int socket, n, i, j, x = 0, y = 0;
	system(STTY_US TTY_PATH);
	system("clear");
	printf("\033[?25l");
	FILE *f,*ff;
	if (ac == 1)
	{
		fprintf(stderr, "ip error\r\n");
		exit(1);
	}
	socket = connect_server(argv[1]);
	f = fdopen(socket,"r");
	ff = fdopen(socket, "w");
	while (1)
	{
		char c,x,y;
		c= get_char();
		if (c == 'a' || c == 'd' || c == 'w') 
		{
			fprintf(ff, "%c", c); fflush(ff);
		}
		else continue;
		if (score>-1)printf("\033[0;102Hsocre %d",score); fflush(stdout);
		c=fgetc(f);
		if (c == '0') break; 
		for (i = 1; i < length; i++)
		{
			for (j = 1; j < width; j++)
			{
				c = fgetc(f);
				printf("\033[%d;%dH", i, j);
				if (c == '1')
					printf("*");
				else if (c == '2')printf("\033[34;1m*\033[0m");
				else if (c == '3')
				{
					if ((i != x) || (j != y))
					{
						score++;
						x = i; y = j;
					}
					printf("\033[31;1m*\033[0m");
				}
				else printf(" ");
			}
			printf("\n");
		}
		fflush(stdout);
	}
	printf("\033[15;40HGAME OVER!Press Enter Exit"); fflush(stdout);
	getchar();
	system(STTY_DEF TTY_PATH);
	printf("\033[41;0H");printf("\033[?25h");
}