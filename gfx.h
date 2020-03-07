#ifndef GFX_H
#define GFX_H

#define TABLE_WIDTH 160
#define TABLE_HEIGHT 256
#define TABLE_ROW_BYTES 80

struct Graphic
{
	const uint8_t *data;
	uint8_t w, h;
};

struct Sprite
{
	static const uint8_t FLAG_DRAW = 1;
	static const uint8_t FLAG_FLIP = 2;
	static const uint8_t FLAG_MIRROR = 4;
	Graphic *gfx;
	int16_t x, y;
	uint8_t flags;
};

const uint16_t tablePalette[] =
{
	0x0000,0x3193,0xC806,0xCE79,0xF800,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x632C,0x738E,0x840F,0x8C71,0x9CD3,0xA534,0xAD75,0xB595,0xBDD6,0xC638,0xD69A,0xDF1B,0xEF5D,0xF7BE,0xFFFF,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};

const uint8_t gfxBall[] =
{
	0x00,0x00,0x16,0x15,0x13,0x14,0x00,0x00,
	0x00,0x17,0x1E,0x1E,0x1C,0x14,0x11,0x00,
	0x17,0x1E,0x1F,0x1F,0x1F,0x1A,0x11,0x15,
	0x17,0x1F,0x1F,0x1F,0x1E,0x18,0x11,0x13,
	0x14,0x1C,0x1D,0x1C,0x19,0x11,0x13,0x14,
	0x14,0x12,0x12,0x12,0x11,0x14,0x19,0x16,
	0x00,0x14,0x16,0x17,0x19,0x1B,0x17,0x00,
	0x00,0x00,0x15,0x17,0x17,0x15,0x00,0x00
};

const uint8_t gfxFlipper[] =
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x02,0x02,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x02,0x02,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x02,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x02,0x03,0x02,0x02,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x02,0x03,0x02,0x02,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x02,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x02,0x02,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x02,0x02,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

const uint8_t gfxNumbers_8x10[] =
{
	0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,
	0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,
	0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,
	0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,
	0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,
	0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,
	0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,
	0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,
	0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,
	0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x04,
	0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,
	0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,
	0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,
	0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,
	0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
	0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,
	0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,
	0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,
	0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,
	0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,
	0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,
	0x04,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x00,0x00,
	0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
	0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,
	0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,
	0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
	0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,
	0x04,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,
	0x04,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x00,0x00,
	0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,
	0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,
	0x04,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x00,0x00,
	0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,
	0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,
	0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
	0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,
	0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,
	0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,
	0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,
	0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,
	0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,
	0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,
	0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,
	0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,
	0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,
	0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,
	0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x04,
	0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,
	0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00
};

#endif //GFX_H