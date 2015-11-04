#include <time.h>

//#define SHOW_CONSOLE
#include "include\graphics.h"

using namespace ege;

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512
#define BASE_HEIGHT (WINDOW_HEIGHT - 20)
#define BRICK 1
#define BALL 2
#define BLOCK_COLOR EGERGB(224, 224, 224)


class Guard
{
public:
	Guard(int guard_height = 20, int guard_width = 100, int guard_posX = WINDOW_WIDTH / 2, int guard_posY = 20, color_t guard_color = WHITE)
	{
		height = guard_height;
		width = guard_width;
		posX = guard_posX;
		posY = guard_posY;
		color = guard_color;
	}

	void moveRight(int step = 20)
	{
		posX += step;
		if (posX + width / 2 > WINDOW_WIDTH)
			posX = WINDOW_WIDTH - width / 2;
	}

	void moveLeft(int step = 20)
	{
		posX -= step;
		if (posX - width / 2 < 0)
			posX = width / 2;
	}

	void set_position(int pos)
	{
		posX = pos;
		if (posX + width / 2 > WINDOW_WIDTH)
			posX = WINDOW_WIDTH - width / 2;
		if (posX - width / 2 < 0)
			posX = width / 2;
	}

	bool collided(int pos)
	{
		if (pos >= posX - width / 2 && pos <= posX + width / 2)
			return true;
		return false;
	}

	void draw()
	{
		bar(posX - width / 2,
			WINDOW_HEIGHT - posY,
			posX + width / 2,
			WINDOW_HEIGHT - (posY - height));
	}

private:
	int height;
	int width;
	int posX, posY;
	color_t color;
};


char pixelType[WINDOW_WIDTH][WINDOW_HEIGHT] = { 0 };
char pixelTypeNext[WINDOW_WIDTH][WINDOW_HEIGHT] = { 0 };
int pixelSpeedX[WINDOW_WIDTH][WINDOW_HEIGHT] = { 0 };
int pixelSpeedY[WINDOW_WIDTH][WINDOW_HEIGHT] = { 0 };
color_t pixelColor[WINDOW_WIDTH][WINDOW_HEIGHT] = { 0 };
Guard guard;


void setup()
{
	srand((unsigned)time(NULL));

	for (int i = 0; i < WINDOW_WIDTH; i++)
	for (int j = 0; j < 256; j++)
	{
		pixelType[i][j] = BRICK;
		pixelTypeNext[i][j] = BRICK;
		pixelColor[i][j] = EGERGB(j, j, j);
	}

	int posX = WINDOW_WIDTH / 2;
	int posY = BASE_HEIGHT - 1;
	pixelType[posX][posY] = BALL;
	pixelTypeNext[posX][posY] = BALL;
	pixelColor[posX][posY] = BLOCK_COLOR;
	pixelSpeedX[posX][posY] = 2;
	pixelSpeedY[posX][posY] = -2;
}


void updateStatus()
{
	int nextX, nextY;
	int nextSpeedX, nextSpeedY;
	for (int i = 0; i < WINDOW_WIDTH; i++)
	for (int j = 0; j < WINDOW_HEIGHT; j++)
	if (pixelType[i][j] == BALL)
	{
		nextSpeedX = pixelSpeedX[i][j];
		nextSpeedY = pixelSpeedY[i][j];
		nextX = nextSpeedX + i;
		nextY = nextSpeedY + j;

		if (nextX < 0 || nextX >= WINDOW_WIDTH)
		{
			nextSpeedX = -nextSpeedX;
			pixelSpeedX[i][j] = nextSpeedX;
			continue;
		}

		if ((nextY >= BASE_HEIGHT && nextY <= BASE_HEIGHT + nextSpeedY && guard.collided(nextX)) || nextY < 0)
		//if (nextY > BASE_HEIGHT || nextY < 0)
		{
				nextSpeedY = -nextSpeedY;
				pixelSpeedY[i][j] = nextSpeedY;
				continue;
		}


		if (nextY >= WINDOW_HEIGHT)
		{
			pixelTypeNext[i][j] = 0;
			pixelType[i][j] = 0;
			continue;
		}


		if (pixelType[nextX][nextY] == BRICK)
		{
			bool flag = false;

			if (nextX + 1 < WINDOW_WIDTH && nextX - 1 >= 0 && pixelType[nextX + 1][nextY] == BRICK && pixelType[nextX - 1][nextY] == BRICK)
			{
				nextSpeedY = -nextSpeedY;
				flag = true;
			}
				
			if (nextY - 1 >= 0 && pixelType[nextX][nextY + 1] == BRICK && pixelType[nextX][nextY - 1] == BRICK)
			{
				nextSpeedX = -nextSpeedX;
				flag = !flag;
			}
				
			if (!flag)
			{
				nextSpeedY = -nextSpeedY;
				nextSpeedX = -nextSpeedX;
			}
						
			pixelSpeedX[i][j] = nextSpeedX;
			pixelSpeedY[i][j] = nextSpeedY;

			pixelTypeNext[nextX][nextY] = BALL;
			pixelSpeedX[nextX][nextY] = rand() % 2 ? -(rand() % 5 + 1) : (rand() % 5 + 1);
			pixelSpeedY[nextX][nextY] = rand() % 5 + 1;
		}
		else if (pixelType[nextX][nextY] == 0)
		{
			pixelTypeNext[i][j] = 0;
			pixelTypeNext[nextX][nextY] = BALL;
			pixelColor[nextX][nextY] = pixelColor[i][j];
			pixelSpeedX[nextX][nextY] = nextSpeedX;
			pixelSpeedY[nextX][nextY] = nextSpeedY;
		}
		else
		{
			pixelType[i][j] = 0;
		}
	}

	for (int i = 0; i < WINDOW_WIDTH; i++)
	for (int j = 0; j < WINDOW_HEIGHT; j++)
		pixelType[i][j] = pixelTypeNext[i][j];
}


void updateScene()
{
	for (int i = 0; i < WINDOW_WIDTH; i++)
	for (int j = 0; j < WINDOW_HEIGHT; j++)
	if (pixelType[i][j] != 0)
		putpixel(i, j, pixelColor[i][j]);

	guard.draw();
}


void mainloop()
{
	setup();

	for (; is_run(); delay_fps(60))
	{
		cleardevice();

		while (kbhit())
		{
			int key = getch();
			if (key == 'a')
				guard.moveLeft();
			else if (key == 'd')
				guard.moveRight();
		}

		while (mousemsg())
		{
			mouse_msg mouse = getmouse();
			if (mouse.is_move())
			{
				guard.set_position(mouse.x);
			}
		}

		updateStatus();
		updateScene();
	}
}


int main()
{
	setinitmode(INIT_DEFAULT);
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
	setcaption("Hit Brick Block");
	setrendermode(RENDER_MANUAL);
	setfillcolor(BLOCK_COLOR);

	mainloop();

	closegraph();
	return 0;
}