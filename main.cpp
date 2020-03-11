#include "Pokitto.h"
#include <File>
#include "gfx.h"
#include "physics.h"

#define SLOW_MOTION 0

using PC = Pokitto::Core;
using PD = Pokitto::Display;
using PB = Pokitto::Buttons;

static const uint8_t BTN_MASK_UP = 1 << UPBIT;
static const uint8_t BTN_MASK_DOWN = 1 << DOWNBIT;
static const uint8_t BTN_MASK_LEFT = 1 << LEFTBIT;
static const uint8_t BTN_MASK_RIGHT = 1 << RIGHTBIT;
static const uint8_t BTN_MASK_A = 1 << ABIT;
static const uint8_t BTN_MASK_B = 1 << BBIT;
static const uint8_t BTN_MASK_C = 1 << CBIT;
static const uint8_t PHYSICS_STEP = 5;
static const uint8_t FLIPPER_MAX_ANGLE = 10;
static const uint8_t FLIPPER_ROTATE_AMOUNT = 4;

File *file;
Graphic graphics[] =
{
	{gfxBall,8,8},
	{gfxFlipper,28,25}
};
Sprite sprites[] =
{
	{&graphics[1],34,215,Sprite::FLAG_NONE},
	{&graphics[1],85,215,Sprite::FLAG_NONE|Sprite::FLAG_MIRROR},
	{&graphics[0],87,9,Sprite::FLAG_DRAW}
	//{&graphics[0],87,192,Sprite::FLAG_DRAW}
};
Sprite *ballSprite = &sprites[2];
Sprite *flipperLSprite = &sprites[0];
Sprite *flipperRSprite = &sprites[1];
Circle ball = {{87, 9},4};
Point ballVelocity {0, 0};
Point normal = {0,0};
Circle flipperCircles[] =
{
	//{{4,12},5},
	//{{25,22},3},
	{{4+34,12+215},5},
	{{25+34,22+215},3},
	{{23+85,12+215},5},
	{{2+85,22+215},3}
};
Line flipperLines[] =
{
	//{{4,8},{26,20}},
	//{{3,16},{24,24}},
	
	{{4+34,8+215},{26+34,20+215}},
	{{3+34,16+215},{24+34,24+215}},
	
	{{27-4+85,8+215},{27-26+85,20+215}},
	{{27-3+85,16+215},{27-24+85,24+215}},
};
int8_t flipperAngle[2] = {0,0};
float flipperAngularVelocity[2] = {0, 0};
float gravity = 0.005;
int collisionX, collisionY, collisionIndex;
int screenY1, screenY2;
int numSprites = 3;
uint8_t __attribute__((section (".bss"))) __attribute__ ((aligned)) scanline[220];
uint8_t scanlinesDirty[22];
uint8_t __attribute__((section (".bss"))) __attribute__ ((aligned)) collisionData[1024];//16x16 (8bits=angle + 5bits=trigger + 3bits=bounce=v/4)
bool colliding = false;
int collisionValue = 0;
int collisionValue2 = 0;
int collisionValue3 = 0;

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
	uint32_t fpsCurrentTime = 0, fpsPreviousTime = 0;
	uint32_t previousTime, currentTime, fileSkip;
	uint8_t buttonsPreviousState = 0, buttonsJustPressed = 0;
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
			PB::pollButtons();
			buttonsJustPressed = ~buttonsPreviousState & PB::buttons_state;
			buttonsPreviousState = PB::buttons_state;
			updateLines(ballSprite->y - screenY1, ballSprite->y - screenY1 + ballSprite->gfx->h - 1);
			currentTime = PC::getTime();
			if (buttonsJustPressed & BTN_MASK_C)
			{
				ball.center.x = 87;
				ball.center.y = 9;
				ballVelocity.x = 0;
				ballVelocity.y = 0;
			}
			else if (buttonsJustPressed & BTN_MASK_UP)
			{
				ballVelocity.y -= 1.28;
				if (ballVelocity.y < -1.28)
					ballVelocity.y = -1.28;
			}
#if SLOW_MOTION == 2
			if ((buttonsJustPressed & BTN_MASK_A) || (buttonsPreviousState & BTN_MASK_B))
#else
			while (previousTime < currentTime)
#endif
			{
				flipperAngularVelocity[0] = 0;
				flipperAngularVelocity[1] = 0;
				if (buttonsPreviousState & BTN_MASK_LEFT)
				{
					if (flipperAngle[0] < FLIPPER_MAX_ANGLE)
					{
						++flipperAngle[0];
						flipperAngularVelocity[0] = -FLIPPER_ROTATE_AMOUNT / 255.0;
						rotatePoint(flipperCircles[1].center, flipperCircles[0].center, 255 - FLIPPER_ROTATE_AMOUNT);
						rotatePoint(flipperLines[0].p1, flipperCircles[0].center, 255 - FLIPPER_ROTATE_AMOUNT);
						rotatePoint(flipperLines[0].p2, flipperCircles[0].center, 255 - FLIPPER_ROTATE_AMOUNT);
						rotatePoint(flipperLines[1].p1, flipperCircles[0].center, 255 - FLIPPER_ROTATE_AMOUNT);
						rotatePoint(flipperLines[1].p2, flipperCircles[0].center, 255 - FLIPPER_ROTATE_AMOUNT);
					}
				}
				else if (flipperAngle[0] > 0)
				{
					--flipperAngle[0];
					flipperAngularVelocity[0] = FLIPPER_ROTATE_AMOUNT / 255.0;
					rotatePoint(flipperCircles[1].center, flipperCircles[0].center, FLIPPER_ROTATE_AMOUNT);
					rotatePoint(flipperLines[0].p1, flipperCircles[0].center, FLIPPER_ROTATE_AMOUNT);
					rotatePoint(flipperLines[0].p2, flipperCircles[0].center, FLIPPER_ROTATE_AMOUNT);
					rotatePoint(flipperLines[1].p1, flipperCircles[0].center, FLIPPER_ROTATE_AMOUNT);
					rotatePoint(flipperLines[1].p2, flipperCircles[0].center, FLIPPER_ROTATE_AMOUNT);
				}
				if (buttonsPreviousState & BTN_MASK_RIGHT)
				{
					if (flipperAngle[1] < FLIPPER_MAX_ANGLE)
					{
						++flipperAngle[1];
						flipperAngularVelocity[1] = FLIPPER_ROTATE_AMOUNT / 255.0;
						rotatePoint(flipperCircles[3].center, flipperCircles[2].center, FLIPPER_ROTATE_AMOUNT);
						rotatePoint(flipperLines[2].p1, flipperCircles[2].center, FLIPPER_ROTATE_AMOUNT);
						rotatePoint(flipperLines[2].p2, flipperCircles[2].center, FLIPPER_ROTATE_AMOUNT);
						rotatePoint(flipperLines[3].p1, flipperCircles[2].center, FLIPPER_ROTATE_AMOUNT);
						rotatePoint(flipperLines[3].p2, flipperCircles[2].center, FLIPPER_ROTATE_AMOUNT);
					}
				}
				else if (flipperAngle[1] > 0)
				{
					--flipperAngle[1];
					flipperAngularVelocity[1] = -FLIPPER_ROTATE_AMOUNT / 255.0;
					rotatePoint(flipperCircles[3].center, flipperCircles[2].center, 255 - FLIPPER_ROTATE_AMOUNT);
					rotatePoint(flipperLines[2].p1, flipperCircles[2].center, 255 - FLIPPER_ROTATE_AMOUNT);
					rotatePoint(flipperLines[2].p2, flipperCircles[2].center, 255 - FLIPPER_ROTATE_AMOUNT);
					rotatePoint(flipperLines[3].p1, flipperCircles[2].center, 255 - FLIPPER_ROTATE_AMOUNT);
					rotatePoint(flipperLines[3].p2, flipperCircles[2].center, 255 - FLIPPER_ROTATE_AMOUNT);
				}
				step();
				//collisionValue2 = collisionData[((int)ball.center.y % 32) * 32 + ((int)ball.center.x % 32)];
#if SLOW_MOTION == 0
				previousTime += PHYSICS_STEP;
#else
				previousTime += PHYSICS_STEP * 10;
#endif
			}
			ballSprite->x = (int)(ball.center.x - ball.radius);
			ballSprite->y = (int)(ball.center.y - ball.radius);
			updateLines(ballSprite->y - screenY1, ballSprite->y - screenY1 + ballSprite->gfx->h - 1);
			updateLines(0,9);
			screenY2 = ballSprite->y - 56;
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
						pixelCopy(scanline + 130, gfxNumbers_8x10 + ((collisionValue3 / 100) % 10) * 80 + y * 8, 8);
						pixelCopy(scanline + 140, gfxNumbers_8x10 + ((collisionValue3 / 10) % 10) * 80 + y * 8, 8);
						pixelCopy(scanline + 150, gfxNumbers_8x10 + (collisionValue3 % 10) * 80 + y * 8, 8);
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
			directLine(ball.center.x, ball.center.y - screenY1, ball.center.x + normal.x * 16, ball.center.y - screenY1 + normal.y * 16, 0x001F);
			directLine(ball.center.x, ball.center.y - screenY1, ball.center.x + ballVelocity.x * 50, ball.center.y - screenY1 + ballVelocity.y * 50, 0x07E0);
			for (int i = 0; i < 4; ++i)
				directCircle(flipperCircles[i].center.x, flipperCircles[i].center.y - screenY1, flipperCircles[i].radius - 1, 0xF81F);
			for (int i = 0; i < 4; ++i)
				directLine(flipperLines[i].p1.x, flipperLines[i].p1.y - screenY1, flipperLines[i].p2.x, flipperLines[i].p2.y - screenY1, 0xF800);
			//memset(scanlinesDirty, 0x00, 22);
			memset(scanlinesDirty, 0xFF, 22);
			fpsCurrentTime = PC::getTime();
			if (fpsCurrentTime - fpsPreviousTime == 0)
				fps = 999;
			else
				fps = 1000 / (fpsCurrentTime - fpsPreviousTime);
			fpsPreviousTime = fpsCurrentTime;
		}
	}
	return 0;
}
