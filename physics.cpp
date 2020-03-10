#include "Pokitto.h"
#include <File>
#include "gfx.h"
#include "physics.h"

extern File *file;
extern Graphic graphics[];
extern Sprite sprites[];
extern Sprite *ballSprite;
extern Sprite *flipperLSprite;
extern Sprite *flipperRSprite;
extern Circle ball;
extern Point ballVelocity;
extern float gravity;
extern int collisionX, collisionY, collisionIndex;
extern int screenY1, screenY2;
extern int numSprites;
extern uint8_t scanline[220];
extern uint8_t scanlinesDirty[22];
extern uint8_t collisionData[1024];//16x16 (8bits=angle + 5bits=trigger + 3bits=bounce=v/4)

float dist2(const Point &p1, const Point &p2)
{
	return (p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y);
}

float dist(const Point &p1, const Point &p2)
{
	return sqrt(dist2(p1, p2));
}

float dot(const Point &p1, const Point &p2)
{
	return p1.x * p2.x + p1.y * p1.y;
}

bool linePoint(const Line &l, const Point &p)
{
	float d1 = dist2(p, l.p1);
	float d2 = dist2(p, l.p2);
	float lineLen = dist2(l.p1, l.p2);
	float buffer = 0.01;
	if (d1 + d2 >= lineLen - buffer && d1 + d2 <= lineLen + buffer)
		return true;
	return false;
}

bool pointCircle(const Point &p, const Circle &c)
{
	if (dist2(p, c.center) <= c.radius * c.radius)
		return true;
	return false;
}

bool lineCircle(const Line &l, const Circle &c, Point &closest)
{
	if (pointCircle(l.p1, c))
		return true;
	if (pointCircle(l.p2, c))
		return true;
	float len2 = (l.p2.x - l.p1.x) * (l.p2.x * l.p1.x) + (l.p2.y - l.p1.y) * (l.p2.y - l.p1.y);
	float dot = (((c.center.x - l.p1.x) * (l.p2.x - l.p1.x)) + ((c.center.y - l.p1.y) * (l.p2.y - l.p1.y))) / len2;
	closest.x = l.p1.x + (dot * (l.p2.x - l.p1.x));
	closest.y = l.p1.y + (dot * (l.p2.y - l.p1.y));
	if (!linePoint(l, closest))
		return false;
	if (dist2(closest, c.center) <= c.radius * c.radius)
		return true;
	return false;
}

int step()
{
	uint8_t collisionValue;
	ballVelocity.y += gravity;
	ball.center.x += ballVelocity.x;
	ball.center.y += ballVelocity.y;
	if (ball.center.x - ball.radius <= 0.0)
	{
		ball.center.x = ball.radius * 2 - ball.center.x;
		ballVelocity.x *= -0.9;
	}
	else if (ball.center.x + ball.radius >= TABLE_WIDTH)
	{
		ball.center.x -= (ball.center.x + ball.radius - TABLE_WIDTH) * 2;
		ballVelocity.x *= -0.9;
	}
	if (ball.center.y - ball.radius <= 0.0)
	{
		ball.center.y = ball.radius * 2 - ball.center.y;
		ballVelocity.y *= -0.9;
	}
	else if (ball.center.y + ball.radius >= TABLE_HEIGHT)
	{
		ball.center.y -= (ball.center.y + ball.radius - TABLE_HEIGHT) * 2;
		ballVelocity.y *= -0.9;
	}
	if ((int)ball.center.x / 32 != collisionX || (int)ball.center.y / 32 != collisionY)
	{
		collisionX = (int)ball.center.x / 32;
		collisionY = (int)ball.center.y / 32;
		file->seek(TABLE_HEIGHT * TABLE_ROW_BYTES + (collisionY * (TABLE_WIDTH / 32) + collisionX) * 1024);
		file->read(collisionData, 1024);
		memcpy(collisionData, tableMask2 + (collisionY * (TABLE_WIDTH / 32) + collisionX) * 1024, 1024);
	}
	collisionValue = tableMask[(int)ball.center.y * TABLE_WIDTH + (int)ball.center.x];
	if (collisionValue != 255)
	{
		Point normal = {cosTable[collisionValue], sinTable[collisionValue]};
		Point r;
		float dot2 = dot(ballVelocity, normal) * 2.0;
		r.x = ballVelocity.x - dot2 * normal.x;
		r.y = ballVelocity.y - dot2 * normal.y;
		ballVelocity.x = r.x;
		ballVelocity.y = r.y;
	}
	return collisionValue;
}
