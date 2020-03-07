#include "Pokitto.h"
#include <File>
#include "gfx.h"

struct Point
{
	float x;
	float y;
};

struct Line
{
	Point p1;
	Point p2;
};

struct Circle
{
	Point center;
	float radius;
};

using PC = Pokitto::Core;
using PD = Pokitto::Display;

File *file;
Graphic ballGraphic = {gfxBall,8,8};
Sprite ballSprite = {&ballGraphic,150,230,Sprite::FLAG_DRAW};
Circle ball = {{154, 224},4};
Point ballVelocity {0, 2.56};
int collisionX, collisionY;
int screenY1, screenY2;
uint8_t scanline[220];
uint8_t scanlinesDirty[22];
uint8_t collisionData[512];//16x16 (8bits=angle + 5bits=trigger + 3bits=bounce=v/4)

void updateLines(int start, int end)
{
	if (start < 0)
		start = 0;
	if (end > 175)
		end = 175;
	if (start > end)
		return;
	for (int y = start; y <= end; ++y)
		scanlinesDirty[y/8] |= 128 >> (y % 8);
}

void step()
{
	ball.center.x += ballVelocity.x;
	ball.center.y += ballVelocity.y;
	if (ball.center.x - ball.radius <= 0.0)
	{
		ball.center.x = ball.radius * 2 - ball.center.x;
		ballVelocity.x *= -1.0;
	}
	else if (ball.center.x + ball.radius >= TABLE_WIDTH)
	{
		ball.center.x -= (ball.center.x + ball.radius - TABLE_WIDTH) * 2;
		ballVelocity.x *= -1.0;
	}
	if (ball.center.y - ball.radius <= 0.0)
	{
		ball.center.y = ball.radius * 2 - ball.center.y;
		ballVelocity.y *= -1.0;
	}
	else if (ball.center.y + ball.radius >= TABLE_HEIGHT)
	{
		ball.center.y -= (ball.center.y + ball.radius - TABLE_HEIGHT) * 2;
		ballVelocity.y *= -1.0;
	}
}

int main()
{
	int fps = 0;
	int steps = 0;
	uint32_t fpsCurrentTime = 0, fpsPreviousTime = 0;
	uint32_t previousTime, currentTime, fileSkip;
	PC::begin();
	PD::persistence = true;
	Pokitto::lcdPrepareRefresh();
	file = new File();
	file->openRO("/pokiball/table2.dat");
	memset(scanline, 0, 220);
	memset(scanlinesDirty, 0xFF, 22);
	previousTime = currentTime = PC::getTime();
	while(PC::isRunning())
	{
		if(PC::update(true))
		{
			updateLines(ballSprite.y - screenY1, ballSprite.y - screenY1 + ballSprite.gfx->h - 1);
			currentTime = PC::getTime();
			while (previousTime < currentTime)
			{
				step();
				++steps;
				previousTime += 5;
			}
			ballSprite.x = (int)(ball.center.x - ball.radius);
			ballSprite.y = (int)(ball.center.y - ball.radius);
			updateLines(ballSprite.y - screenY1, ballSprite.y - screenY1 + ballSprite.gfx->h - 1);
			updateLines(0,9);
			screenY2 = ballSprite.y - 84;
			if (screenY2 < 0)
				screenY2 = 0;
			else if (screenY2 + 176 > TABLE_HEIGHT)
				screenY2 = TABLE_HEIGHT - 176;
			if (screenY2 != screenY1)
				memset(scanlinesDirty, 0xFF, 22);
			screenY1 = screenY2;
			fileSkip = 0;
			file->seek(screenY1 * TABLE_ROW_BYTES);
			for (int y = 0; y < 176; ++y)
			{
				if ((scanlinesDirty[y / 8] & (128 >> (y % 8))) != 0)
				{
					Pokitto::setDRAMpoint(0, y);
					if (fileSkip != 0)
					{
						file->seek(file->tell() + fileSkip * TABLE_ROW_BYTES);
						fileSkip = 0;
					}
					file->read(scanline, TABLE_ROW_BYTES);
					uint8_t *pNibbles = scanline + TABLE_ROW_BYTES - 1;
					uint8_t *pBytes = scanline + TABLE_WIDTH - 1;
					for (int x = 0; x < TABLE_ROW_BYTES; ++x)
					{
						*pBytes-- = *pNibbles & 0xF;
						*pBytes-- = *pNibbles-- >> 4;
					}
					if (y < 10)
					{
						pixelCopy(scanline, gfxNumbers_8x10 + ((fps / 100) % 10) * 80 + y * 8, 8);
						pixelCopy(scanline + 10, gfxNumbers_8x10 + ((fps / 10) % 10) * 80 + y * 8, 8);
						pixelCopy(scanline + 20, gfxNumbers_8x10 + (fps % 10) * 80 + y * 8, 8);
						pixelCopy(scanline + 50, gfxNumbers_8x10 + ((steps / 100) % 10) * 80 + y * 8, 8);
						pixelCopy(scanline + 60, gfxNumbers_8x10 + ((steps / 10) % 10) * 80 + y * 8, 8);
						pixelCopy(scanline + 70, gfxNumbers_8x10 + (steps % 10) * 80 + y * 8, 8);
					}
					if (y + screenY1 >= ballSprite.y && y + screenY1 < ballSprite.y + 8)
						pixelCopy(scanline + ballSprite.x, ballSprite.gfx->data + (y + screenY1 - ballSprite.y) * ballSprite.gfx->w, ballSprite.gfx->w);
					flushLine(tablePalette, scanline);
				}
				else
					++fileSkip;
			}
			memset(scanlinesDirty, 0x00, 22);
			fpsCurrentTime = Pokitto::Core::getTime();
			steps = 0;
			if (fpsCurrentTime - fpsPreviousTime == 0)
				fps = 999;
			else
				fps = 1000 / (fpsCurrentTime - fpsPreviousTime);
			fpsPreviousTime = fpsCurrentTime;
		}
	}
	return 0;
}
