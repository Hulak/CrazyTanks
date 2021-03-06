
#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <thread>
#include <vector>
#define MS_PER_FRAME 15
#define BulletsMaxCount 1000
#define EnemyTanksNum 6

using namespace std;
void initField();
void processInput();
void update();
void render();

int health = 3;
const int HEIGHT = 20, WIDTH = 40;
char field[HEIGHT + 5][WIDTH + 5];
int bulletCount = 0, reloadTime = 10;
bool gameStatus = true;
bool shootPlayer = false, shootEnemy = false;
bool startMove = false;
double deltaTime = MS_PER_FRAME, generalTime = 0;

int currentTanksNum = 6;
int PosPlayerX = HEIGHT - 2, PosPlayerY = WIDTH / 2;
int posBulletX = PosPlayerX, posBulletY = PosPlayerY, wayBullet = 1,
posEnemyBulletX, posEnemyBulletY;

enum class Direction
{
	UP,
	DOWN,
	LEFT,
	RIGHT
} playerDir, enemyBulletDir;

struct Bullet
{

	int posBulletX;
	int posBulletY;
	Direction bulletDir;
	bool side;
	bool active = false;
};
std::vector<Bullet> bulletsPlayer;
std::vector<Bullet> bulletsEnemy;

struct EnemyTank
{
	double lastShot;
	int posEnemyTankX;
	int posEnemyTankY;
	Direction enemyTankDirection;
	bool active = false;
} enemyTanks[EnemyTanksNum];


int main(int argc, char *argv[])
{
	initField();
	bulletsPlayer.resize(BulletsMaxCount);
	bulletsEnemy.resize(BulletsMaxCount);
	render();
	while (gameStatus) {
		clock_t start = clock();
		processInput();
		update();
		render();

		deltaTime = static_cast<double>(clock() - start) / CLK_TCK;
		double timeToWait = MS_PER_FRAME - deltaTime * 1000;
		if (timeToWait > 0)
		{
			Sleep(timeToWait);
		}

		generalTime = static_cast<double>(clock()) / CLK_TCK;
		if (currentTanksNum == 0) gameStatus = false;
	}
	system("cls");
	HANDLE hCon;
	COORD cPos;

	hCon = GetStdHandle(STD_OUTPUT_HANDLE);

	cPos.Y = 15;
	cPos.X = 50;
	SetConsoleCursorPosition(hCon, cPos);

	if (health > 0) cout << "VICTORY!";
	else cout << "GAME OVER";
	system("pause");
}

void processInput() {
	if (GetKeyState(VK_UP) & 0x8000)
	{
		startMove = true;
		playerDir = Direction::UP;
	}
	else if (GetKeyState(VK_DOWN) & 0x8000)
	{
		startMove = true;
		playerDir = Direction::DOWN;
	}
	else if (GetKeyState(VK_LEFT) & 0x8000)
	{
		startMove = true;
		playerDir = Direction::LEFT;
	}
	else if (GetKeyState(VK_RIGHT) & 0x8000)
	{
		startMove = true;
		playerDir = Direction::RIGHT;
	}
	else if (GetKeyState(VK_SPACE) & 0x8000)
	{
		shootPlayer = true;
	}
}

bool FindPlayerBulletPlace()
{
	for (auto& bullet : bulletsPlayer)
	{
		if (!bullet.active) //we've found place for bullet
		{
			bullet.bulletDir = playerDir;
			bullet.posBulletX = PosPlayerX;
			bullet.posBulletY = PosPlayerY;
			bullet.side = true;
			bullet.active = true;
			return true;
		}
	}
	return false;
}

bool FindEnemyBulletPlace()
{
	for (auto& bullet : bulletsEnemy)
	{
		if (!bullet.active) //we've found place for bullet
		{
			bullet.bulletDir = enemyBulletDir;
			bullet.posBulletX = posEnemyBulletX;
			bullet.posBulletY = posEnemyBulletY;
			bullet.side = false;
			bullet.active = true;
			return true;
		}
	}
	return false;
}

void update() {
	if (shootPlayer)
	{
		shootPlayer = false;
		FindPlayerBulletPlace();
	}


	// logic of player bullets
	for (auto& currentBullet : bulletsPlayer)
	{
		if (currentBullet.active)
		{
			if (field[currentBullet.posBulletX][currentBullet.posBulletY] == '$') {
				for (int i = 0; i < EnemyTanksNum; i++) {
					EnemyTank& currentTank = enemyTanks[i];
					if ((currentTank.posEnemyTankX == currentBullet.posBulletX)
						&& (currentTank.posEnemyTankY == currentBullet.posBulletY))
					{
						currentTank.active = false;
						currentBullet.active = false;
						currentTanksNum--;
					}

				}
				continue;
			}
			if (currentBullet.posBulletX != PosPlayerX ||
				currentBullet.posBulletY != PosPlayerY)
			{
				field[currentBullet.posBulletX][currentBullet.posBulletY] = ' ';
			}
			switch (currentBullet.bulletDir)
			{
			case Direction::UP:
				if (field[currentBullet.posBulletX - 1][currentBullet.posBulletY] == '$') {
					(field[currentBullet.posBulletX - 1][currentBullet.posBulletY] = ' ');
					for (int i = 0; i < EnemyTanksNum; i++) {
						EnemyTank& currentTank = enemyTanks[i];
						if ((currentTank.posEnemyTankX == currentBullet.posBulletX - 1)
							&& (currentTank.posEnemyTankY == currentBullet.posBulletY))
						{
							currentTank.active = false;
							currentBullet.active = false;
							currentTanksNum--;
						}

					}
				}
				else
					if (field[currentBullet.posBulletX - 1][currentBullet.posBulletY] != '#'
						&& (field[currentBullet.posBulletX - 1][currentBullet.posBulletY] != '$')) {
						field[currentBullet.posBulletX - 1][currentBullet.posBulletY] = 'o';
						currentBullet.posBulletX--;
					}
					else currentBullet.active = false;
					break;

			case Direction::LEFT:
				if (field[currentBullet.posBulletX][currentBullet.posBulletY - 1] == '$') {
					(field[currentBullet.posBulletX][currentBullet.posBulletY - 1] = ' ');
					for (int i = 0; i < EnemyTanksNum; i++) {
						EnemyTank& currentTank = enemyTanks[i];
						if ((currentTank.posEnemyTankX == currentBullet.posBulletX)
							&& (currentTank.posEnemyTankY == currentBullet.posBulletY - 1)) {
							currentTank.active = false;
							currentBullet.active = false;
							currentTanksNum--;
						}

					}

				}
				else
					if (field[currentBullet.posBulletX][currentBullet.posBulletY - 1] != '#'
						&& (field[currentBullet.posBulletX - 1][currentBullet.posBulletY] != '$')) {
						field[currentBullet.posBulletX][currentBullet.posBulletY - 1] = 'o';
						currentBullet.posBulletY--;
					}
					else currentBullet.active = false;
					break;

			case Direction::RIGHT:
				if (field[currentBullet.posBulletX][currentBullet.posBulletY + 1] == '$') {
					(field[currentBullet.posBulletX][currentBullet.posBulletY + 1] = ' ');
					for (int i = 0; i < EnemyTanksNum; i++) {
						EnemyTank& currentTank = enemyTanks[i];
						if ((currentTank.posEnemyTankX == currentBullet.posBulletX)
							&& (currentTank.posEnemyTankY == currentBullet.posBulletY + 1)) {
							currentTank.active = false;
							currentBullet.active = false;
							currentTanksNum--;
						}

					}

				}
				else
					if (field[currentBullet.posBulletX][currentBullet.posBulletY + 1] != '#'
						&& (field[currentBullet.posBulletX - 1][currentBullet.posBulletY] != '$')) {
						field[currentBullet.posBulletX][currentBullet.posBulletY + 1] = 'o';
						currentBullet.posBulletY++;
					}
					else currentBullet.active = false;
					break;

			case Direction::DOWN:
				if (field[currentBullet.posBulletX + 1][currentBullet.posBulletY] == '$') {
					(field[currentBullet.posBulletX + 1][currentBullet.posBulletY] = ' ');
					for (int i = 0; i < EnemyTanksNum; i++) {
						EnemyTank& currentTank = enemyTanks[i];
						if ((currentTank.posEnemyTankX == currentBullet.posBulletX + 1)
							&& (currentTank.posEnemyTankY == currentBullet.posBulletY)) {
							currentTank.active = false;
							currentBullet.active = false;
							currentTanksNum--;
						}

					}
				}
				else
					if (field[currentBullet.posBulletX + 1][currentBullet.posBulletY] != '#'
						&& (field[currentBullet.posBulletX - 1][currentBullet.posBulletY] != '$')) {
						field[currentBullet.posBulletX + 1][currentBullet.posBulletY] = 'o';
						currentBullet.posBulletX++;
					}
					else currentBullet.active = false;
					break;
			default:
				break;
			}

		}

	}

	// logic of enemy bullets
	for (auto& currentBullet : bulletsEnemy)
	{

		if (currentBullet.active)
		{
			if (field[currentBullet.posBulletX][currentBullet.posBulletY] == '@') {
				if (health <= 1) {
					gameStatus = false;

				}
				currentBullet.active = false;
				health--;
				continue;
			}
			field[currentBullet.posBulletX][currentBullet.posBulletY] = ' ';

			switch (currentBullet.bulletDir)
			{
			case Direction::UP:
				if (field[currentBullet.posBulletX - 1][currentBullet.posBulletY] == '@') {
					if (health <= 1) {
						gameStatus = false;

					}
					currentBullet.active = false;
					health--;
				}
				else
					if (field[currentBullet.posBulletX - 1][currentBullet.posBulletY] != '#'
						&& field[currentBullet.posBulletX - 1][currentBullet.posBulletY] != 'o'
						&& field[currentBullet.posBulletX - 1][currentBullet.posBulletY] != '$') {
						field[currentBullet.posBulletX - 1][currentBullet.posBulletY] = 'o';
						currentBullet.posBulletX--;
					}
					else currentBullet.active = false;
					break;
			case Direction::LEFT:
				if (field[currentBullet.posBulletX][currentBullet.posBulletY - 1] == '@') {
					if (health <= 1) {
						gameStatus = false;

					}
					currentBullet.active = false;
					health--;
				}
				else
					if (field[currentBullet.posBulletX][currentBullet.posBulletY - 1] != '#'
						&& field[currentBullet.posBulletX][currentBullet.posBulletY - 1] != 'o'
						&& field[currentBullet.posBulletX][currentBullet.posBulletY - 1] != '$') {
						field[currentBullet.posBulletX][currentBullet.posBulletY - 1] = 'o';
						currentBullet.posBulletY--;
					}
					else currentBullet.active = false;
					break;
			case Direction::RIGHT:
				if (field[currentBullet.posBulletX][currentBullet.posBulletY + 1] == '@') {
					if (health <= 1) {
						gameStatus = false;

					}
					currentBullet.active = false;
					health--;
				}
				else
					if (field[currentBullet.posBulletX][currentBullet.posBulletY + 1] != '#'
						&& field[currentBullet.posBulletX][currentBullet.posBulletY + 1] != 'o'
						&& field[currentBullet.posBulletX][currentBullet.posBulletY + 1] != '$') {
						field[currentBullet.posBulletX][currentBullet.posBulletY + 1] = 'o';
						currentBullet.posBulletY++;
					}
					else currentBullet.active = false;
					break;
			case Direction::DOWN:
				if (field[currentBullet.posBulletX + 1][currentBullet.posBulletY] == '@') {
					if (health <= 1) {
						gameStatus = false;

					}
					currentBullet.active = false;
					health--;
				}
				else
					if (field[currentBullet.posBulletX + 1][currentBullet.posBulletY] != '#'
						&& field[currentBullet.posBulletX + 1][currentBullet.posBulletY] != 'o'
						&& field[currentBullet.posBulletX + 1][currentBullet.posBulletY] != '$') {
						field[currentBullet.posBulletX + 1][currentBullet.posBulletY] = 'o';
						currentBullet.posBulletX++;
					}
					else currentBullet.active = false;
					break;
			}
		}



	}

	if (startMove)
	{
		startMove = false;
		switch (playerDir)
		{
		case Direction::UP:
			if (field[PosPlayerX - 1][PosPlayerY] != '#')
			{
				field[PosPlayerX][PosPlayerY] = ' ';
				field[PosPlayerX - 1][PosPlayerY] = '@';
				PosPlayerX--;
			}
			break;
		case Direction::LEFT:
			if (field[PosPlayerX][PosPlayerY - 1] != '#')
			{
				field[PosPlayerX][PosPlayerY] = ' ';
				field[PosPlayerX][PosPlayerY - 1] = '@';
				PosPlayerY--;
			}
			break;
		case Direction::RIGHT:
			if (field[PosPlayerX][PosPlayerY + 1] != '#')
			{
				field[PosPlayerX][PosPlayerY] = ' ';
				field[PosPlayerX][PosPlayerY + 1] = '@';
				PosPlayerY++;
			}
			break;
		case Direction::DOWN:
			if (field[PosPlayerX + 1][PosPlayerY] != '#')
			{
				field[PosPlayerX][PosPlayerY] = ' ';
				field[PosPlayerX + 1][PosPlayerY] = '@';
				PosPlayerX++;
			}
			break;
		default:
			break;
		}
	}

	bool flag = false;
	int numOfTurns;
	Direction firstDirection;

	for (int i = 0; i < EnemyTanksNum; i++) {
		EnemyTank& currentTank = enemyTanks[i];
		firstDirection = currentTank.enemyTankDirection;
		numOfTurns = 0;
		if (currentTank.active) {
			flag = false;

			while (!flag && (numOfTurns < 5)) {
				switch (currentTank.enemyTankDirection)
				{
				case Direction::UP:
					if (field[currentTank.posEnemyTankX - 1][currentTank.posEnemyTankY] != '#'
						&& field[currentTank.posEnemyTankX - 1][currentTank.posEnemyTankY] != '$'
						&& field[currentTank.posEnemyTankX - 1][currentTank.posEnemyTankY] != '@')
					{
						field[currentTank.posEnemyTankX][currentTank.posEnemyTankY] = ' ';
						field[currentTank.posEnemyTankX - 1][currentTank.posEnemyTankY] = '$';
						currentTank.posEnemyTankX--;
						flag = true;
					}
					else currentTank.enemyTankDirection = Direction(rand() % 4);
					break;
				case Direction::LEFT:
					if (field[currentTank.posEnemyTankX][currentTank.posEnemyTankY - 1] != '#'
						&& field[currentTank.posEnemyTankX][currentTank.posEnemyTankY - 1] != '$'
						&& field[currentTank.posEnemyTankX][currentTank.posEnemyTankY - 1] != '@')
					{
						field[currentTank.posEnemyTankX][currentTank.posEnemyTankY] = ' ';
						field[currentTank.posEnemyTankX][currentTank.posEnemyTankY - 1] = '$';
						currentTank.posEnemyTankY--;
						flag = true;
					}
					else currentTank.enemyTankDirection = Direction(rand() % 4);
					break;
				case Direction::RIGHT:
					if (field[currentTank.posEnemyTankX][currentTank.posEnemyTankY + 1] != '#'
						&& field[currentTank.posEnemyTankX][currentTank.posEnemyTankY + 1] != '$'
						&& field[currentTank.posEnemyTankX][currentTank.posEnemyTankY + 1] != '@')
					{
						field[currentTank.posEnemyTankX][currentTank.posEnemyTankY] = ' ';
						field[currentTank.posEnemyTankX][currentTank.posEnemyTankY + 1] = '$';
						currentTank.posEnemyTankY++;
						flag = true;
					}
					else currentTank.enemyTankDirection = Direction(rand() % 4);
					break;
				case Direction::DOWN:
					if (field[currentTank.posEnemyTankX + 1][currentTank.posEnemyTankY] != '#'
						&& field[currentTank.posEnemyTankX + 1][currentTank.posEnemyTankY] != '$'
						&& field[currentTank.posEnemyTankX + 1][currentTank.posEnemyTankY] != '@')
					{
						field[currentTank.posEnemyTankX][currentTank.posEnemyTankY] = ' ';
						field[currentTank.posEnemyTankX + 1][currentTank.posEnemyTankY] = '$';
						currentTank.posEnemyTankX++;
						flag = true;
					}
					else currentTank.enemyTankDirection = Direction(rand() % 4);
					break;
				default:
					break;
				}
				numOfTurns++;
			}
			if (!flag) {
				switch (firstDirection)
				{
				case Direction::UP:
					currentTank.enemyTankDirection = Direction::DOWN;
					break;
				case Direction::DOWN:
					currentTank.enemyTankDirection = Direction::UP;
					break;
				case Direction::LEFT:
					currentTank.enemyTankDirection = Direction::RIGHT;
					break;
				case Direction::RIGHT:
					currentTank.enemyTankDirection = Direction::LEFT;
					break;
				default:
					break;
				}

			}

			//shot of enemy bullets 

			if (posEnemyBulletY == currentTank.posEnemyTankY)
				for (int j = currentTank.posEnemyTankX - 1; j > 0; j--) {
					if (field[j][currentTank.posEnemyTankY] == '#') break;
					if (field[j][currentTank.posEnemyTankY] == '@') {
						enemyBulletDir = Direction(0);
						posEnemyBulletX = currentTank.posEnemyTankX - 1;
						posEnemyBulletY = currentTank.posEnemyTankY;
						shootEnemy = true;
						break;
					}
				}
			for (int j = currentTank.posEnemyTankX + 1; j < HEIGHT - 1; j++) {
				if (field[j][currentTank.posEnemyTankY] == '#') break;
				if (field[j][currentTank.posEnemyTankY] == '@') {
					enemyBulletDir = Direction(1);
					posEnemyBulletX = currentTank.posEnemyTankX + 1;
					posEnemyBulletY = currentTank.posEnemyTankY;
					shootEnemy = true;
					break;
				}
			}
			for (int j = currentTank.posEnemyTankY - 1; j > 0; j--) {
				if (field[currentTank.posEnemyTankX][j] == '#') break;
				if (field[currentTank.posEnemyTankX][j] == '@') {
					enemyBulletDir = Direction(2);
					posEnemyBulletX = currentTank.posEnemyTankX;
					posEnemyBulletY = currentTank.posEnemyTankY - 1;
					shootEnemy = true;
					break;
				}
			}
			for (int j = currentTank.posEnemyTankY + 1; j < WIDTH - 1; j++) {
				if (field[currentTank.posEnemyTankX][j] == '#') break;
				if (field[currentTank.posEnemyTankX][j] == '@') {
					enemyBulletDir = Direction(3);
					posEnemyBulletX = currentTank.posEnemyTankX;
					posEnemyBulletY = currentTank.posEnemyTankY + 1;
					shootEnemy = true;
					break;
				}
			}
			if (shootEnemy)
			{
				if (currentTank.lastShot + reloadTime < generalTime)
				{
					// make shot
					shootEnemy = false;
					FindEnemyBulletPlace();
					currentTank.lastShot = generalTime;
				}

			}


		}
	}

}
void initField() {

	int PosPlayerX = HEIGHT - 2, PosPlayerY = WIDTH / 2;
	int posEnemyTankX, posEnemyTankY;

	srand((unsigned)time(NULL));


	for (int i = 0; i < WIDTH + 1; i++)
	{
		field[0][i] = '#';
		field[HEIGHT - 1][i] = '#';
	}
	for (int i = 1; i < HEIGHT - 1; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (j == 0 || j == WIDTH - 1) field[i][j] = '#';
			else field[i][j] = ' ';
		}
	}

	field[PosPlayerX][PosPlayerY] = '@';

	// walls generation

	int wallMaxLength = 5, wallsCount = 20;

	for (int i = 0; i < wallsCount; i++) {

		bool way = rand() % 2;
		int firstPosX = 0, firstPosY = 0;
		int length = rand() % wallMaxLength + 1;

		while (field[firstPosX][firstPosY] == '#')
		{
			firstPosX = rand() % HEIGHT + 1;
			firstPosY = rand() % WIDTH;
		}

		for (int j = 0; j < length; j++) {
			if (way == 0)
			{
				if (firstPosX != HEIGHT - 1 && (firstPosY + j) != WIDTH / 2)
					field[firstPosX][firstPosY + j] = '#';
			}
			else {
				if (firstPosX != HEIGHT - 1 && (firstPosY + j) != WIDTH / 2)
					field[firstPosX + j][firstPosY] = '#';
			}


		}

	}

	//enemy tanks generation
	bool flag;

	for (int i = 0; i < EnemyTanksNum; i++) {
		while (!enemyTanks[i].active) {

			flag = true;
			posEnemyTankX = rand() % (HEIGHT - 1) + 1;
			posEnemyTankY = rand() % (WIDTH - 1) + 1;

			if ((field[posEnemyTankX][posEnemyTankY] != '#')
				&& (field[posEnemyTankX][posEnemyTankY] != '$')
				&& (field[posEnemyTankX][posEnemyTankY] != '@'))
			{
				for (int j = posEnemyTankX - 2; j < posEnemyTankX + 2; j++) {
					for (int k = posEnemyTankY - 2; k < posEnemyTankY + 2; k++)
						if (field[j][k] != ' ' && field[j][k] != '#') {
							flag = false;
							break;
						}
					if (flag == false) break;
				}

				if (flag == true) {
					enemyTanks[i].active = true;
					enemyTanks[i].posEnemyTankX = posEnemyTankX;
					enemyTanks[i].posEnemyTankY = posEnemyTankY;
					enemyTanks[i].enemyTankDirection = Direction(rand() % 4);
					field[posEnemyTankX][posEnemyTankY] = '$';

				}
			}
		}
	}
}


void render() {

	system("cls");
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			cout << field[i][j];
		}
		cout << endl;
	}

	HANDLE hCon;
	COORD cPos;

	hCon = GetStdHandle(STD_OUTPUT_HANDLE);

	cPos.Y = 1;
	cPos.X = 85;
	SetConsoleCursorPosition(hCon, cPos);

	cout << "SCORE:   " << EnemyTanksNum - currentTanksNum;
	cPos.Y = 2;
	cPos.X = 85;
	SetConsoleCursorPosition(hCon, cPos);
	cout << "HEALTH:  " << health << endl;
	cPos.Y = 3;
	cPos.X = 85;
	SetConsoleCursorPosition(hCon, cPos);
	cout << "TIME:    " << generalTime;



}