#include<iostream>
#include<queue>
#include<ctime>
#include<conio.h>
#include<windows.h>
using namespace std;
void SetPos(int x, int y);
class Map
{
public:
	Map();
	~Map();
	void Print();
	void Generate();
	void Set(int x, int y);
	void Relase(int x, int y);
	int seedx, seedy;
	bool Check(int x, int y);
private:
	bool map[40][100];
};
class Snake
{
public:
	Snake(Map &map);
	~Snake();
	bool Forward(Map &map);
	bool Check(Map &map);
	void Left();
	void Right();
	int headx, heady;
	int GetScore();
private:
	queue<int> bodyx;
	queue<int> bodyy;
	const int forwardx[4] = { 0,1,0,-1 };
	const int forwardy[4] = { 1,0,-1,0 };
	int direction = 0;
	int score = 0;
	int length = 5;
};
Snake::Snake(Map &map)
{
	headx = 19; heady = 10;
	map.Set(headx, heady);
	for (int i = 4; i >= 1; i--)
	{
		map.Set(headx, heady - i);
		bodyx.push(headx);
		bodyy.push(heady - i);
		SetPos(headx, heady - i);
		cout << "*";
	}
	bodyx.push(headx);
	bodyy.push(heady);
	SetPos(headx, heady);
	cout << "*";
}
Snake::~Snake()
{
}
bool Snake::Forward(Map &map)
{
	headx += forwardx[direction];
	heady += forwardy[direction];
	if ((headx == map.seedx) && (heady == map.seedy))
	{
		bodyx.push(headx);
		bodyy.push(heady);
		SetPos(headx, heady);
		cout << "*";
		score++;
		length++;
		map.Generate();
		SetPos(0, 108);
		cout << score;
		return true;
	}
	if (!Check(map)) return false;
	map.Relase(bodyx.front(), bodyy.front());
	SetPos(bodyx.front(), bodyy.front());
	cout << " ";
	bodyx.pop();
	bodyy.pop();
	SetPos(headx, heady);
	cout << "*";
	bodyx.push(headx);
	bodyy.push(heady);
	map.Set(headx, heady);
	return true;
}
bool Snake::Check(Map &map)
{
	if (!map.Check(headx, heady))return false;
	return true;
}
void Snake::Left()
{
	direction--;
	if (direction < 0) direction = 3;
}
void Snake::Right()
{
	direction++;
	if (direction > 3) direction = 0;
}
int Snake::GetScore()
{
	return score;
}
Map::Map()
{
	for (int i = 0; i < 40; i++)
		for (int j = 0; j < 100; j++)
			if (i == 0 || j == 0 || i == 39 || j == 99) map[i][j] = false; else map[i][j] = true;
}
Map::~Map()
{
}
void Map::Print()
{
	SetPos(0, 0);
	for (int i = 0; i < 40; i++)
	{
		for (int j = 0; j < 100; j++)
			if (!map[i][j]) cout << "*";
			else cout << " ";
			cout << endl;
	}
}
void Map::Generate()
{
	srand(unsigned(time(NULL)));
	int t = rand();
	int x = (int)rand() % 40;
	int y = (int)rand() % 100;
	while (!map[x][y])
	{
		x = (int)rand() % 40;
		y = (int)rand() % 100;
	}
	seedx = x; seedy = y; map[x][y] = false;
	SetPos(seedx, seedy);
	cout << "*";
}
void Map::Set(int x, int y)
{
	map[x][y] = false;
}
void Map::Relase(int x, int y)
{
	map[x][y] = true;
}
bool Map::Check(int x, int y)
{
	return map[x][y];
}
void SetPos(int y, int x)
{
	COORD pos = { x,y };
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(out, pos);
}
int main()
{
	system("mode con cols=111 lines=42");
	CONSOLE_CURSOR_INFO a;
	a.bVisible = false;
	a.dwSize = sizeof(a);
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &a);
	while (1)
	{
		system("cls");
		Map *map = new Map();
		map->Print();
		SetPos(40, 102);
		cout << "Snake";
		SetPos(41, 100);
		cout << "by zxd1997";
		SetPos(40, 18);
		cout << "Press KeyBoard Left to turn Left, KeyBoard Right to turn Right";
		Snake *snake = new Snake(*map);
		SetPos(0, 102);
		cout << "SCORE:" << 0;
		map->Generate();
		int time = 100;
		int tmp = 0;
		while (1)
		{
			if (_kbhit())
			{
				int ch;
				ch = _getch();
				if (isascii(ch))
					continue;
				ch = _getch();
				switch (ch)
				{
				case 77:
					snake->Right();
					break;
				case 75:
					snake->Left();
					break;
				}
			}
			if (snake->GetScore() - tmp > 5)
			{
				time *= 0.8;
				tmp = snake->GetScore();
			}
			clock_t now = clock();
			while ((clock() - now) < time);
			if (!snake->Forward(*map)) break;
		}
		SetPos(19, 40);
		cout << "game over score:" << snake->GetScore() << endl;
		SetPos(20, 43);
		cout << "YÍË³öNÖØÊÔ" << endl;
		SetPos(21, 48);
		char c;
		cin >> c;
		if (c == 'Y') break;
		if (c == 'N') continue;
	}
}
