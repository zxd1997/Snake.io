#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>  
#include<fcntl.h>  
#include<sys/mman.h>  
#include<stdlib.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<time.h>
#include<pthread.h>
#define PORT 8888
#define length 30
#define width 100
int map[length][width];
int seedx=0, seedy=0;
const int forwardx[4] = { 0,1,0,-1 };
const int forwardy[4] = { 1,0,-1,0 };
pthread_mutex_t mutex;
void init()
{
	memset(map, 0, sizeof(map));
	int i;
	for (i = 0; i < length; i++)
	{
		map[i][1] = 1;
		map[i][width-1] = 1;
	}
	for (i = 0; i < width; i++)
	{
		map[length-1][i] = 1;
		map[1][i] = 1;
	}
	pthread_mutex_init(&mutex, NULL);
	system("clear");
	printf("\033[2J");
	printf("\033[?25l");
}
void generate(int *x,int *y)
{
	srand((int)time(0));
	*x = 4 + rand() % (length - 10);
	*y = 4 + rand() % (width - 10);
	while (map[*x][*y] != 0)
	{
		*x = 4 + rand() % (length - 10);
		*y = 4 + rand() % (width - 10);
	}
}
int make_server_socket(int port)
{
	int sock_id;
	struct sockaddr_in server;
	sock_id = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_id == -1)
	{
		perror("creating socket failed");
		return -1;
	}
	int opt = SO_REUSEADDR;
	setsockopt(sock_id, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock_id, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
	{
		perror("bind error");
		return -1;
	}
	if (listen(sock_id, 5) == -1)
	{
		perror("listen error");
		return -1;
	}
	return sock_id;
}
void game(int fd)
{
	int headx = 0, heady = 0, h = 0, t = 0, dir, i, live = 1,j;
	int bodyx[100], bodyy[100];
	int mapp[length][width];
	srand((int)time(0));
	dir = rand() % 4;
	generate(&headx, &heady);
	pthread_mutex_lock(&mutex);
	for (i = 0; i < 5; i++)
	{
		t++;
		headx += forwardx[dir];
		heady += forwardy[dir];
		bodyx[i] = headx;
		bodyy[i] = heady;
		map[headx][heady] = 1;
		printf("\033[%d;%dH*", headx, heady);
		fflush(stdout);
	}
	pthread_mutex_unlock(&mutex);
	FILE *f;
	FILE *ff;
	ff = fdopen(fd, "w");
	f = fdopen(fd, "r");
	char c;
	while (1)
	{
		int i,j;
		c = fgetc(f);
		if (c != 'a' && c != 'd' && c != 'w') continue;
		switch (c)
		{
		case 'd':
		{
			dir++;
			if (dir == 4)dir = 0;
			break;
		}
		case 'a':
		{
			dir--;
			if (dir == -1)dir = 3;
			break;
		}
		default:
			break;
		}
		headx += forwardx[dir]; heady += forwardy[dir];
		pthread_mutex_lock(&mutex);
		if (map[headx][heady] == 1)
		{
			int hh = h;
			while (hh != t)
			{
				mapp[bodyx[hh]][bodyy[hh]] = 0; 
				printf("\033[%d;%dH ", bodyx[hh], bodyy[hh]); fflush(stdout);
				hh++; if (hh == 100)hh = 0;
			}
			live = 0;
			fprintf(ff, "%d", live); fflush(ff);
			pthread_mutex_unlock(&mutex);
			close(fd);
			return;
		}
		bodyx[t] = headx; bodyy[t] = heady;
		printf("\033[%d;%dH*", headx, heady);
		map[bodyx[t]][bodyy[t]] = 1;
		fflush(stdout);
		t++; if (t == 100) t = 0;
		if (headx == seedx && heady == seedy)
		{
			map[seedx][seedy] = 1;
			generate(&seedx, &seedy);
			map[seedx][seedy] = 3;
			printf("\033[%d;%dH*", seedx, seedy); fflush(stdout);
		}
		else {
			printf("\033[%d;%dH ", bodyx[h], bodyy[h]);
			fflush(stdout);
			map[bodyx[h]][bodyy[h]] = 0;
			h++; if (h == 100) h = 0;
		}
		pthread_mutex_unlock(&mutex);
		fprintf(ff, "%d", live); fflush(ff);
		for (i = 1; i < length; i++)
			for (j = 1; j < width; j++)
				mapp[i][j] = map[i][j];
		int hh = h;
		while (hh != t)
		{
			mapp[bodyx[hh]][bodyy[hh]] = 2;
			hh++; if (hh == 100)hh = 0;
		}
		for (i = 1; i < length; i++)
			for (j = 1; j < width; j++)
				fprintf(ff,"%d", mapp[i][j]);
		fflush(ff);
	}
}
int main() 
{
	int socket, fd;
	init();
	int i, j;
	for (i = 1; i < length; i++)
	{
		for (j = 1; j < width; j++)
		{
			if (map[i][j] != 0)printf("*");
			else printf(" ");
		}
		printf("\n");
	}
	fflush(stdout);
	generate(&seedx, &seedy);
	map[seedx][seedy] = 3;
	printf("\033[%d;%dH*", seedx, seedy); fflush(stdout);
	if ((socket = make_server_socket(PORT))==-1)
	{
		printf("cannot open socket");
		exit(0);
	}
	while (1)
	{
		fd = accept(socket, NULL, NULL);
		pthread_create((pthread_t *)malloc(sizeof(pthread_t)), NULL, game, fd);
	}
	printf("\033[41;0H");printf("\033[?25h");
	return 0;
}