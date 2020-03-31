#include "Pokitto.h"
#include <File>
#include "gfx.h"
#include "physics.h"

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
static const uint8_t FLIPPER_MAX_ANGLE = 9;
static const uint8_t FLIPPER_ROTATE_AMOUNT = 4;

File *file;
Graphic graphics[] =
{
	{gfxBall,8,8},
	{gfxFlipper,38,40},
	//{gfxFlipper,28,25},
	{gfxBarrel,23,26},
	//{gfxBarrelNormal,23,26},
	//{gfxBarrelSquished,23,26},
	{gfxLight_Eye,11,16},
	{gfxLight_Water0,7,8},
	{gfxLight_Water1,9,16},
	{gfxLight_Water2,9,16},
	{gfxLight_Water3,9,16},
	{gfxLight_Water4,10,16},
	{gfxLight_Water5,11,16},
	{gfxLight_Water6,11,16},
	{gfxLight_Water7,10,16},
	{gfxLight_Water8,11,16},
	{gfxLight_Water9,12,16},
	{gfxLight_Water10,11,16}
};
Sprite sprites[] =
{
	{&graphics[1],60,290,Sprite::FLAG_DRAW,0},
	{&graphics[1],110,290,Sprite::FLAG_DRAW|Sprite::FLAG_MIRROR,0},
	{&graphics[2],101,50,Sprite::FLAG_DRAW|Sprite::FLAG_LAYER1,0},
	{&graphics[2],130,73,Sprite::FLAG_DRAW|Sprite::FLAG_LAYER1,0},
	{&graphics[0],212,283,Sprite::FLAG_DRAW,0}
};
Light lights[] =
{
	{&graphics[3],38,258,0,0x23,0x22,Light::STATE_OFF,0},
	{&graphics[3],159,258,0,0x23,0x22,Light::STATE_OFF|Light::STATE_MIRROR,0},
	{&graphics[4],22,171,25,0x24,0x25,Light::STATE_ON|Light::STATE_FLASHING,75},
	{&graphics[5],17,164,50,0x24,0x25,Light::STATE_ON|Light::STATE_FLASHING,75},
	{&graphics[6],14,156,75,0x24,0x25,Light::STATE_ON|Light::STATE_FLASHING,75},
	{&graphics[7],10,148,25,0x24,0x25,Light::STATE_OFF|Light::STATE_FLASHING,75},
	{&graphics[8],6,139,50,0x24,0x25,Light::STATE_OFF|Light::STATE_FLASHING,75},
	{&graphics[9],3,128,75,0x24,0x25,Light::STATE_OFF|Light::STATE_FLASHING,75},
	{&graphics[10],1,117,25,0x24,0x25,Light::STATE_ON|Light::STATE_FLASHING,75},
	{&graphics[11],1,107,50,0x24,0x25,Light::STATE_ON|Light::STATE_FLASHING,75},
	{&graphics[12],1,96,75,0x24,0x25,Light::STATE_ON|Light::STATE_FLASHING,75},
	{&graphics[13],2,87,25,0x24,0x25,Light::STATE_OFF|Light::STATE_FLASHING,75},
	{&graphics[14],3,80,50,0x24,0x25,Light::STATE_OFF|Light::STATE_FLASHING,75}
};
Sprite *ballSprite = &sprites[4];
Sprite *flipperSprites[] = {&sprites[0], &sprites[1]};
Circle ball = {{212+4, 283+4},4};
Point ballVelocity {0, 0};
Point normal = {0,0};
Point collisionPoint = {-1,-1};
Circle flipperCircles[] =
{
	{{5+60,17+290},6},
	{{34+60,31+290},4},
	{{32+110,17+290},6},
	{{3+110,31+290},4}
};
Line flipperLines[] =
{
	{{7+60,12+290},{37+60,29+290}},
	{{2+60,22+290},{32+60,34+290}},
	{{0+110,29+290},{30+110,12+290}},
	{{5+110,34+290},{35+110,22+290}}
};
BumperCircle bumperCircles[] =
{
	{{{109,62},9},0.4,0.75},
	{{{138,85},9},0.4,0.75},
	{{{103,114},13},0.4,1.0}
};
BumperLine bumperLines[] =
{
	{{{36,244},{54,276}},0.6,0.5},
	{{{153,276},{171,244}},0.6,0.5}
};

int8_t flipperAngle[2] = {0,0};
float flipperAngularVelocity[2] = {0, 0};
float gravity = 0.005;
int collisionX, collisionY, collisionIndex;
int screenY1, screenY2;
int numSprites = 5;
int numBumperCircles = 3;
int numBumperLines = 2;
int numLights = 13;
uint32_t steps = 0;
uint8_t __attribute__((section (".bss"))) __attribute__ ((aligned)) scanline[220];
uint8_t scanlinesDirty[22];
uint8_t __attribute__((section (".bss"))) __attribute__ ((aligned)) collisionData[1024];//16x16 (8bits=angle + 5bits=trigger + 3bits=bounce=v/4)
uint8_t collision = COLLISION_NONE;
uint8_t bumperSteps[2] = {0,0};

int main()
{
	float len;
	int fps = 0;
	uint32_t fpsCurrentTime = 0, fpsPreviousTime = 0;
	uint32_t previousTime, currentTime, fileSkip;
	uint8_t buttonsPreviousState = 0, buttonsJustPressed = 0;
	const uint8_t *tableLayer;
	uint8_t layerMask;
	PC::begin();
	PD::persistence = true;
	Pokitto::lcdPrepareRefresh();
	file = new File();
	file->openRO("/pinball/drgnrush.dat");
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
			updateLines(flipperSprites[0]->y - screenY1, flipperSprites[0]->y - screenY1 + flipperSprites[0]->gfx->h - 1);
			updateLines(flipperSprites[1]->y - screenY1, flipperSprites[1]->y - screenY1 + flipperSprites[1]->gfx->h - 1);
			currentTime = PC::getTime();
			if (buttonsJustPressed & BTN_MASK_C)
			{
				ball.center.x = 212+4;
				ball.center.y = 283+4;
				ballVelocity.x = 0;
				ballVelocity.y = 0;
			}
			else if (buttonsJustPressed & BTN_MASK_UP)
			{
				ballVelocity.y -= 2.0;
				len = ballVelocity.x * ballVelocity.x + ballVelocity.y * ballVelocity.y;
				if (len > 4.0)
				{
					len = 2.0 / sqrt(len);
					ballVelocity.x *= len;
					ballVelocity.y *= len;
				}
			}
			while (previousTime < currentTime)
			{
				flipperAngularVelocity[0] = 0.0;
				flipperAngularVelocity[1] = 0.0;
				if (buttonsPreviousState & BTN_MASK_LEFT)
				{
					//flipperSprites[0]->flags |= Sprite::FLAG_FLIP;
					if (flipperAngle[0] < FLIPPER_MAX_ANGLE)
					{
						flipperSprites[0]->frame = 1;
						++flipperAngle[0];
						flipperAngularVelocity[0] = FLIPPER_ROTATE_AMOUNT / 255.0;
						rotatePoint(flipperCircles[1].center, flipperCircles[0].center, cosTable[255 - FLIPPER_ROTATE_AMOUNT], sinTable[255 - FLIPPER_ROTATE_AMOUNT]);
						rotatePoint(flipperLines[0].p1, flipperCircles[0].center, cosTable[255 - FLIPPER_ROTATE_AMOUNT], sinTable[255 - FLIPPER_ROTATE_AMOUNT]);
						rotatePoint(flipperLines[0].p2, flipperCircles[0].center, cosTable[255 - FLIPPER_ROTATE_AMOUNT], sinTable[255 - FLIPPER_ROTATE_AMOUNT]);
						rotatePoint(flipperLines[1].p1, flipperCircles[0].center, cosTable[255 - FLIPPER_ROTATE_AMOUNT], sinTable[255 - FLIPPER_ROTATE_AMOUNT]);
						rotatePoint(flipperLines[1].p2, flipperCircles[0].center, cosTable[255 - FLIPPER_ROTATE_AMOUNT], sinTable[255 - FLIPPER_ROTATE_AMOUNT]);
						if (collision == COLLISION_LFLIPPER)
						{
							normal.x = collisionPoint.x;
							normal.y = collisionPoint.y;
							rotatePoint(collisionPoint, flipperCircles[0].center, cosTable[255 - FLIPPER_ROTATE_AMOUNT], sinTable[255 - FLIPPER_ROTATE_AMOUNT]);
							if (pointCircle(collisionPoint, ball))
							{
								float rad = dist(normal, flipperCircles[0].center);
								float vel = flipperAngularVelocity[0] * 1.2 * PI * rad;
								rotatePoint(ball.center, flipperCircles[0].center, cosTable[255 - FLIPPER_ROTATE_AMOUNT], sinTable[255 - FLIPPER_ROTATE_AMOUNT]);
								len = dist(normal, collisionPoint);
								normal.x = (collisionPoint.x - normal.x) / len;
								normal.y = (collisionPoint.y - normal.y) / len;
								ballVelocity.x += vel * normal.x;
								ballVelocity.y += vel * normal.y;
							}
							else
								collision = COLLISION_NONE;
						}
					}
					else
					{
						flipperSprites[0]->frame = 2;
						collision = COLLISION_NONE;
					}
				}
				else
				{
					//flipperSprites[0]->flags = Sprite::FLAG_DRAW;
					if (flipperAngle[0] > 0)
					{
						flipperSprites[0]->frame = 1;
						--flipperAngle[0];
						flipperAngularVelocity[0] = -FLIPPER_ROTATE_AMOUNT / 255.0;
						rotatePoint(flipperCircles[1].center, flipperCircles[0].center, cosTable[FLIPPER_ROTATE_AMOUNT], sinTable[FLIPPER_ROTATE_AMOUNT]);
						rotatePoint(flipperLines[0].p1, flipperCircles[0].center, cosTable[FLIPPER_ROTATE_AMOUNT], sinTable[FLIPPER_ROTATE_AMOUNT]);
						rotatePoint(flipperLines[0].p2, flipperCircles[0].center, cosTable[FLIPPER_ROTATE_AMOUNT], sinTable[FLIPPER_ROTATE_AMOUNT]);
						rotatePoint(flipperLines[1].p1, flipperCircles[0].center, cosTable[FLIPPER_ROTATE_AMOUNT], sinTable[FLIPPER_ROTATE_AMOUNT]);
						rotatePoint(flipperLines[1].p2, flipperCircles[0].center, cosTable[FLIPPER_ROTATE_AMOUNT], sinTable[FLIPPER_ROTATE_AMOUNT]);
						collision = COLLISION_NONE;
					}
					else
					{
						flipperSprites[0]->frame = 0;
						collision = COLLISION_NONE;
					}
				}
				if (buttonsPreviousState & BTN_MASK_RIGHT || buttonsPreviousState & BTN_MASK_A)
				{
					//flipperSprites[1]->flags |= Sprite::FLAG_FLIP;
					if (flipperAngle[1] < FLIPPER_MAX_ANGLE)
					{
						flipperSprites[1]->frame = 1;
						++flipperAngle[1];
						flipperAngularVelocity[1] = FLIPPER_ROTATE_AMOUNT / 255.0;
						rotatePoint(flipperCircles[3].center, flipperCircles[2].center, cosTable[FLIPPER_ROTATE_AMOUNT], sinTable[FLIPPER_ROTATE_AMOUNT]);
						rotatePoint(flipperLines[2].p1, flipperCircles[2].center, cosTable[FLIPPER_ROTATE_AMOUNT], sinTable[FLIPPER_ROTATE_AMOUNT]);
						rotatePoint(flipperLines[2].p2, flipperCircles[2].center, cosTable[FLIPPER_ROTATE_AMOUNT], sinTable[FLIPPER_ROTATE_AMOUNT]);
						rotatePoint(flipperLines[3].p1, flipperCircles[2].center, cosTable[FLIPPER_ROTATE_AMOUNT], sinTable[FLIPPER_ROTATE_AMOUNT]);
						rotatePoint(flipperLines[3].p2, flipperCircles[2].center, cosTable[FLIPPER_ROTATE_AMOUNT], sinTable[FLIPPER_ROTATE_AMOUNT]);
						if (collision == COLLISION_RFLIPPER)
						{
							normal.x = collisionPoint.x;
							normal.y = collisionPoint.y;
							rotatePoint(collisionPoint, flipperCircles[2].center, cosTable[FLIPPER_ROTATE_AMOUNT], sinTable[FLIPPER_ROTATE_AMOUNT]);
							if (pointCircle(collisionPoint, ball))
							{
								float rad = dist(collisionPoint, flipperCircles[2].center);
								float vel = flipperAngularVelocity[1] * 1.2 * PI * rad;
								rotatePoint(ball.center, flipperCircles[2].center, cosTable[FLIPPER_ROTATE_AMOUNT], sinTable[FLIPPER_ROTATE_AMOUNT]);
								len = dist(normal, collisionPoint);
								normal.x = (collisionPoint.x - normal.x) / len;
								normal.y = (collisionPoint.y - normal.y) / len;
								ballVelocity.x += vel * normal.x;
								ballVelocity.y += vel * normal.y;
							}
							else
								collision = COLLISION_NONE;
						}
					}
					else
					{
						flipperSprites[1]->frame = 2;
						collision = COLLISION_NONE;
					}
				}
				else
				{
					//flipperSprites[1]->flags = Sprite::FLAG_DRAW|Sprite::FLAG_MIRROR;
					if (flipperAngle[1] > 0)
					{
						flipperSprites[1]->frame = 1;
						--flipperAngle[1];
						flipperAngularVelocity[1] = -FLIPPER_ROTATE_AMOUNT / 255.0;
						rotatePoint(flipperCircles[3].center, flipperCircles[2].center, cosTable[255 - FLIPPER_ROTATE_AMOUNT], sinTable[255 - FLIPPER_ROTATE_AMOUNT]);
						rotatePoint(flipperLines[2].p1, flipperCircles[2].center, cosTable[255 - FLIPPER_ROTATE_AMOUNT], sinTable[255 - FLIPPER_ROTATE_AMOUNT]);
						rotatePoint(flipperLines[2].p2, flipperCircles[2].center, cosTable[255 - FLIPPER_ROTATE_AMOUNT], sinTable[255 - FLIPPER_ROTATE_AMOUNT]);
						rotatePoint(flipperLines[3].p1, flipperCircles[2].center, cosTable[255 - FLIPPER_ROTATE_AMOUNT], sinTable[255 - FLIPPER_ROTATE_AMOUNT]);
						rotatePoint(flipperLines[3].p2, flipperCircles[2].center, cosTable[255 - FLIPPER_ROTATE_AMOUNT], sinTable[255 - FLIPPER_ROTATE_AMOUNT]);
						collision = COLLISION_NONE;
					}
					else
					{
						flipperSprites[1]->frame = 0;
						collision = COLLISION_NONE;
					}
				}
				step();
				if (bumperSteps[0] > 0)
				{
					--bumperSteps[0];
					if (bumperSteps[0] == 0)
					{
						updateLines(sprites[2].y - screenY1, sprites[2].y - screenY1 + sprites[2].gfx->h - 1);
						sprites[2].frame = 0;
					}
				}
				if (bumperSteps[1] > 0)
				{
					--bumperSteps[1];
					if (bumperSteps[1] == 0)
					{
						updateLines(sprites[3].y - screenY1, sprites[3].y - screenY1 + sprites[3].gfx->h - 1);
						sprites[3].frame = 0;
					}
				}
				for (int i = 0; i < numLights; ++i)
				{
					if (lights[i].state & Light::STATE_FLASHING)
					{
						--lights[i].timer;
						if (lights[i].timer == 0)
						{
							updateLines(lights[i].y - screenY1, lights[i].y - screenY1 + lights[i].gfx->h - 1);
							lights[i].state ^= Light::STATE_ON;
							lights[i].timer = lights[i].timerStart;
						}
					}
					else if (lights[i].state & Light::STATE_ON && lights[i].timer > 0)
					{
						--lights[i].timer;
						if (lights[i].timer == 0)
						{
							updateLines(lights[i].y - screenY1, lights[i].y - screenY1 + lights[i].gfx->h - 1);
							lights[i].state ^= Light::STATE_ON;
						}
					}
				}
				previousTime += PHYSICS_STEP;
			}
			ballSprite->x = (int)(ball.center.x - ball.radius);
			ballSprite->y = (int)(ball.center.y - ball.radius);
			updateLines(ballSprite->y - screenY1, ballSprite->y - screenY1 + ballSprite->gfx->h - 1);
			updateLines(flipperSprites[0]->y - screenY1, flipperSprites[0]->y - screenY1 + flipperSprites[0]->gfx->h - 1);
			updateLines(flipperSprites[1]->y - screenY1, flipperSprites[1]->y - screenY1 + flipperSprites[1]->gfx->h - 1);
			updateLines(0,9);
			screenY2 = ballSprite->y - 42;
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
				layerMask = 1 << ((y + screenY1) % 8);
				tableLayer = tableLayers + ((y + screenY1) / 8) * TABLE_WIDTH;
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
					}
					for (int i = 0; i < numLights; ++i)
					{
						if (y + screenY1 >= lights[i].y && y + screenY1 < lights[i].y + lights[i].gfx->h)
						{
							const uint8_t *gfx = lights[i].gfx->data + ((y + screenY1 - lights[i].y) / 8) * lights[i].gfx->w;
							uint8_t lightMask = 1 << ((y + screenY1 - lights[i].y) % 8);
							uint8_t color = (lights[i].state & Light::STATE_ON) ? lights[i].onColor:lights[i].offColor;
							if ((lights[i].state & Light::STATE_MIRROR) != 0)
							{
								for (int x = 0; x < lights[i].gfx->w; ++x)
								{
									if (gfx[lights[i].gfx->w - x - 1] & lightMask)
										scanline[x + lights[i].x] = color;
								}
							}
							else
							{
								for (int x = 0; x < lights[i].gfx->w; ++x)
								{
									if (gfx[x] & lightMask)
										scanline[x + lights[i].x] = color;
								}
							}
						}
					}
					for (int i = 0; i < numSprites; ++i)
					{
						if ((sprites[i].flags & Sprite::FLAG_DRAW) != 0 && y + screenY1 >= sprites[i].y && y + screenY1 < sprites[i].y + sprites[i].gfx->h)
						{
							const uint8_t *gfx = sprites[i].gfx->data + sprites[i].frame * sprites[i].gfx->w * sprites[i].gfx->h;
							if ((sprites[i].flags & Sprite::FLAG_FLIP) != 0)
								gfx += (sprites[i].gfx->h - (y + screenY1 - sprites[i].y) - 1) * sprites[i].gfx->w;
							else
								gfx += (y + screenY1 - sprites[i].y) * sprites[i].gfx->w;
							if ((sprites[i].flags & Sprite::FLAG_MIRROR) != 0)
							{
								for (int x = 0; x < sprites[i].gfx->w; ++x)
								{
									if (gfx[sprites[i].gfx->w - x - 1] != 0 && ((sprites[i].flags & Sprite::FLAG_LAYER1) != 0 || (tableLayer[x + sprites[i].x] & layerMask) == 0))
										scanline[x + sprites[i].x] = gfx[sprites[i].gfx->w - x - 1];
								}
								//pixelCopyMirror(scanline + sprites[i].x, gfx, sprites[i].gfx->w);
							}
							else
							{
								for (int x = 0; x < sprites[i].gfx->w; ++x)
								{
									if (gfx[x] != 0 && ((sprites[i].flags & Sprite::FLAG_LAYER1) != 0 || (tableLayer[x + sprites[i].x] & layerMask) == 0))
										scanline[x + sprites[i].x] = gfx[x];
								}
								//pixelCopy(scanline + sprites[i].x, gfx, sprites[i].gfx->w);
							}
						}
					}
					flushLine(tablePalette, scanline);
				}
				else
					++fileSkip;
			}
			memset(scanlinesDirty, 0x00, 22);
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
