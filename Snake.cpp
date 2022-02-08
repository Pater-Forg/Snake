#include <iostream>
#include <limits>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include "Console.h"

const int width = 20, height = 11, pgHeight = 10;
console con(width, height);

struct Snake
{
	wchar_t texture = '@';
	vector<int> coords;
	int curDirection = 0;
	int size = 5;

	void move()
	{		
		for (int i = 0; i < coords.size()-1; i++)
		{
			coords[i] = coords[i + 1];
		}
		switch (curDirection)
		{
		case 0:
			if ((coords[coords.size() - 1] + 1) % width  == 0)
				coords[coords.size() - 1] -= width - 1;
			else
				coords[coords.size() - 1] += 1;
			break;
		case 1:
			if ((coords[coords.size() - 1] + width) / width > pgHeight)
				coords[coords.size() - 1] -= pgHeight * width;
			else
				coords[coords.size() - 1] += width;
			break;
		case 2:
			if ((coords[coords.size() - 1]) % width == 0)
				coords[coords.size() - 1] += width - 1;
			else
				coords[coords.size() - 1] -= 1;
			break;
		case 3:
			if ((coords[coords.size() - 1] - width) < 0)
				coords[coords.size() - 1] += pgHeight * width;
			else
				coords[coords.size() - 1] -= width;
			break;
		default:
			break;
		}
	}

	void grow()
	{
		if (coords[1] == coords[0] + 1)
			if (curDirection == 0)
				coords.insert(coords.begin(), coords[0] - 1);
			else if (curDirection == 2)
				coords.insert(coords.begin(), coords[0] + 1);

		if (coords[1] == coords[0] + width)
			if (curDirection == 1)
				coords.insert(coords.begin(), coords[0] - width);
			else if (curDirection == 3)
				coords.insert(coords.begin(), coords[0] + width);

		size += 1;
	}

	Snake(int initPoint, int initSize) {
		for (int i = 0; i < initSize; i++)
			coords.push_back(initPoint + i);
	}
};

struct Food
{
	wchar_t texture = '$';
	vector<int> coords;

	Food(int initPoint) { coords = { initPoint }; }
};

struct Screen
{
	wchar_t* screen = NULL;
	int scWidth = 0, scHeight = 0;
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL); // Буфер экрана
	DWORD dwBytesWritten = 0; // Для дебага
	wchar_t texture = '.';

	void update()
	{
		WriteConsoleOutputCharacter(hConsole, screen, scWidth * scHeight, { 0, 0 }, &dwBytesWritten); // Запись в буфер
	}

	Screen(int width, int height)
	{
		ShowConsoleCursor(false);
		scWidth = width;
		scHeight = height;
		screen = new wchar_t[width * height];
		SetConsoleActiveScreenBuffer(hConsole); // Настройка консоли
		screen[width * height - 1] = '\0';  // Последний символ - окончание строки

		for (int i = 0; i < width * height; i++)
		{
			screen[i] = texture;
		}
	}
};

int intersection(vector<int> snakeCoords, vector<int> objectCoords)
{
	for (int i = 0; i < objectCoords.size(); i++)
	{
		if (snakeCoords.back() == objectCoords[i])
			return i;
	}
	return -1;
}

int delay = 200;

int main()
{
	srand(time(0));
	Screen screen(width, pgHeight);
	Snake snake(0, 5);
	Food food(rand() % width * pgHeight);

	for (int step = 0; ;step++)
	{
		for (int crd : snake.coords)
		{
			screen.screen[crd] = snake.texture;
		}
		for (int crd : food.coords)
		{
			screen.screen[crd] = food.texture;
		}
		screen.update();
		for (int crd : snake.coords)
		{
			screen.screen[crd] = screen.texture;
		}

		if (GetAsyncKeyState((unsigned short)'D') & 0x01 && snake.curDirection != 2)
			snake.curDirection = 0;
		if (GetAsyncKeyState((unsigned short)'S') & 0x01 && snake.curDirection != 3)
			snake.curDirection = 1;
		if (GetAsyncKeyState((unsigned short)'A') & 0x01 && snake.curDirection != 0)
			snake.curDirection = 2;
		if (GetAsyncKeyState((unsigned short)'W') & 0x01 && snake.curDirection != 1)
			snake.curDirection = 3;
		snake.move();

		int index = intersection(snake.coords, food.coords);
		if (index != -1)
		{
			snake.grow();
			food.coords.erase(food.coords.begin() + index);
			food.coords.insert(food.coords.begin(), rand() % (width*pgHeight));
		}
		vector<int> snakeBody(snake.coords.begin(), snake.coords.end() - 1);
		if (intersection(snake.coords, snakeBody) != -1)
			break;
		Sleep(delay);
	}
	
	printf("YOU LOSE :)");
	getchar();
	return 0;
}