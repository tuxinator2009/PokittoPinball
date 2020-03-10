#include "Pokitto.h"
#include <File>
#include "gfx.h"
#include "physics.h"

#define PHYSICS_STEP 5
#define SLOW_MOTION 1

using PC = Pokitto::Core;
using PD = Pokitto::Display;

File *file;
Graphic graphics[] =
{
	{gfxBall,8,8},
	{gfxFlipper,28,25}
};
Sprite sprites[] =
{
	{&graphics[1],34,215,Sprite::FLAG_DRAW},
	{&graphics[1],85,215,Sprite::FLAG_DRAW|Sprite::FLAG_MIRROR},
	{&graphics[0],87,3,Sprite::FLAG_DRAW}
};
Sprite *ballSprite = &sprites[2];
Sprite *flipperLSprite = &sprites[0];
Sprite *flipperRSprite = &sprites[1];
Circle ball = {{87, 3},4};
Point ballVelocity {0, 0};
float gravity = 0.005;
int collisionX, collisionY, collisionIndex;
int screenY1, screenY2;
int numSprites = 3;
uint8_t __attribute__((section (".bss"))) __attribute__ ((aligned)) scanline[220];
uint8_t scanlinesDirty[22];
uint8_t __attribute__((section (".bss"))) __attribute__ ((aligned)) collisionData[1024];//16x16 (8bits=angle + 5bits=trigger + 3bits=bounce=v/4)

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

int main()
{
	int fps = 0;
	int collisionValue = 0;
	int collisionValue2 = 0;
	uint32_t fpsCurrentTime = 0, fpsPreviousTime = 0;
	uint32_t previousTime, currentTime, fileSkip;
	PC::begin();
	PD::persistence = true;
	Pokitto::lcdPrepareRefresh();
	file = new File();
	file->openRO("/pinball/test.dat");
	memset(scanline, 0, 220);
	memset(scanlinesDirty, 0xFF, 22);
	previousTime = currentTime = PC::getTime();
	collisionX = -1;
	collisionY = -1;
	while(PC::isRunning())
	{
		if(PC::update(true))
		{
			updateLines(ballSprite->y - screenY1, ballSprite->y - screenY1 + ballSprite->gfx->h - 1);
			currentTime = PC::getTime();
			while (previousTime < currentTime)
			{
				collisionValue = step();
				collisionValue2 = collisionData[((int)ball.center.y % 32) * 32 + ((int)ball.center.x % 32)];
#if SLOW_MOTION == 0
				previousTime += PHYSICS_STEP;
#else
				previousTime = currentTime;
#endif
			}
			ballSprite->x = (int)(ball.center.x - ball.radius);
			ballSprite->y = (int)(ball.center.y - ball.radius);
			updateLines(ballSprite->y - screenY1, ballSprite->y - screenY1 + ballSprite->gfx->h - 1);
			updateLines(0,9);
			screenY2 = ballSprite->y - 84;
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
						pixelCopy(scanline + 50, gfxNumbers_8x10 + ((collisionValue / 100) % 10) * 80 + y * 8, 8);
						pixelCopy(scanline + 60, gfxNumbers_8x10 + ((collisionValue / 10) % 10) * 80 + y * 8, 8);
						pixelCopy(scanline + 70, gfxNumbers_8x10 + (collisionValue % 10) * 80 + y * 8, 8);
						pixelCopy(scanline + 90, gfxNumbers_8x10 + ((collisionValue2 / 100) % 10) * 80 + y * 8, 8);
						pixelCopy(scanline + 100, gfxNumbers_8x10 + ((collisionValue2 / 10) % 10) * 80 + y * 8, 8);
						pixelCopy(scanline + 110, gfxNumbers_8x10 + (collisionValue2 % 10) * 80 + y * 8, 8);
					}
					for (int i = 0; i < numSprites; ++i)
					{
						if ((sprites[i].flags & Sprite::FLAG_DRAW) != 0 && y + screenY1 >= sprites[i].y && y + screenY1 < sprites[i].y + sprites[i].gfx->h)
						{
							const uint8_t *gfx;
							if ((sprites[i].flags & Sprite::FLAG_FLIP) != 0)
								gfx = sprites[i].gfx->data + (sprites[i].gfx->h - (y + screenY1 - sprites[i].y) - 1) * sprites[i].gfx->w;
							else
								gfx = sprites[i].gfx->data + (y + screenY1 - sprites[i].y) * sprites[i].gfx->w;
							if ((sprites[i].flags & Sprite::FLAG_MIRROR) != 0)
								pixelCopyMirror(scanline + sprites[i].x, gfx, sprites[i].gfx->w);
							else
								pixelCopy(scanline + sprites[i].x, gfx, sprites[i].gfx->w);
						}
					}
					flushLine(tablePalette, scanline);
				}
				else
					++fileSkip;
			}
			memset(scanlinesDirty, 0x00, 22);
			fpsCurrentTime = Pokitto::Core::getTime();
			if (fpsCurrentTime - fpsPreviousTime == 0)
				fps = 999;
			else
				fps = 1000 / (fpsCurrentTime - fpsPreviousTime);
			fpsPreviousTime = fpsCurrentTime;
		}
	}
	return 0;
}
