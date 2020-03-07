#include "Pokitto.h"
#include "SDFileSystem"

using PC = Pokitto::Core;
using PD = Pokitto::Display;

SDFileSystem *sdFS;
FileHandle *file;
int collisionX, collisionY;
int ballX, ballY;
int screenX, screenY;
uint8_t scanline[220];
uint8_t scanlinesDirty[22];
uint8_t collisionData[512];//16x16 (8bits=angle + 5bits=trigger + 3bits=bounce=v/4)

void step()
{
}

int main()
{
	uint32_t previousTime, currentTime, fileSkip;
	PC::begin();
	PD::persistence = true;
	Pokitto::lcdPrepareRefresh();
	sdFs =new SDFileSystem( P0_9, P0_8, P0_6, P0_7, "sd", NC, SDFileSystem::SWITCH_NONE, 25000000 );
	sdFs->crc(false);
	sdFs->write_validation(false);
	file=sdFs->open("/pokiball/table2.dat", O_RDONLY);
	memset(scanline, 0, 220);
	memset(scanlinesDirty, 0xFF, 22);
	previousTime = currentTime = PC::getTime();
	while(PC::isRunning())
	{
		currentTime = PC::getTime();
		if ((currentTime - previousTime) / 200 > 0)
		{
			for (int t = (currentTime - previousTime) / 200; t > 0; --t)
				step();
			previousTime += ((currentTime - previousTime) / 200) * 200;
		}
		if(PC::update(true))
		{
			fileSkip = 0;
			
			for (int y = 0; y < 176; ++y)
			{
				if ((scanlinesDirty[y / 8] & (128 >> (y % 8))) != 0)
				{
				}
			}
		}
	}
	return 0;
}
