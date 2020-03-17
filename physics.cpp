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
extern Point normal;
extern Circle flipperCircles[];
extern Line flipperLines[];
extern Point flipperLinePoints[];
extern int8_t flipperAngle[2];
extern float flipperAngularVelocity[2];
extern float gravity;
extern int collisionX, collisionY, collisionIndex;
extern int screenY1, screenY2;
extern int numSprites;
extern uint8_t scanline[220];
extern uint8_t scanlinesDirty[22];
extern uint8_t collisionData[1024];//16x16 (8bits=angle + 5bits=trigger + 3bits=bounce=v/4)
extern bool colliding;
extern int collisionValue;
extern int collisionValue2;
extern int collisionValue3;

void directCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;
	
	Pokitto::Display::directPixel(x0, y0, color);
	
	Pokitto::Display::directPixel(x0, y0 + r, color);
	Pokitto::Display::directPixel(x0, y0 - r, color);
	Pokitto::Display::directPixel(x0 + r, y0, color);
	Pokitto::Display::directPixel(x0 - r, y0, color);
	
	while (x < y) {
		if (f >= 0) {
			
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		
		Pokitto::Display::directPixel(x0 + x, y0 + y, color);
		Pokitto::Display::directPixel(x0 - x, y0 + y, color);
		Pokitto::Display::directPixel(x0 + x, y0 - y, color);
		Pokitto::Display::directPixel(x0 - x, y0 - y, color);
		Pokitto::Display::directPixel(x0 + y, y0 + x, color);
		Pokitto::Display::directPixel(x0 - y, y0 + x, color);
		Pokitto::Display::directPixel(x0 + y, y0 - x, color);
		Pokitto::Display::directPixel(x0 - y, y0 - x, color);
		
	}
}

uint8_t clipLine(int16_t *x0, int16_t *y0, int16_t *x1, int16_t *y1){
	// Check X bounds
	if (*x1 < *x0) {
		//std::swap (*x1,*x0); // swap so that we dont have to check x1 also
		swapWT(int16_t*, x1, x0);
		//std::swap (*y1,*y0); // y needs to be swaaped also
		swapWT(int16_t*, y1, y0);
	}
	
	if (*x0 >= 220) return 0; // whole line is out of bounds
	
	// Clip against X0 = 0
	if (*x0 < 0) {
		if (*x1 < 0) return 0; // nothing visible
		int32_t dx = (*x1 - *x0);
		int32_t dy = ((*y1 - *y0) << 16); // 16.16 fixed point calculation trick
		int32_t m = dy/dx;
		*y0 = *y0 + ((m*-*x0) >> 16); // get y0 at boundary
		*x0 = 0;
	}
	
	// Clip against x1 >= 220
	if (*x1 >= 220) {
		int32_t dx = (*x1 - *x0);
		int32_t dy = ((*y1 - *y0) << 16); // 16.16 fixed point calculation trick
		int32_t m = dy / dx;
		*y1 = *y1 + ((m * ((220 - 1) - *x1)) >> 16); // get y0 at boundary
		*x1 = 220-1;
	}
	
	// Check Y bounds
	if (*y1 < *y0) {
		//std::swap (*x1,*x0); // swap so that we dont have to check x1 also
		swapWT(int16_t*, x1, x0);
		//std::swap (*y1,*y0); // y needs to be swaaped also
		swapWT(int16_t*, y1, y0);
	}
	
	if (*y0 >= 176) return 0; // whole line is out of bounds
	
	if (*y0 < 0) {
		if (*y1 < 0) return 0; // nothing visible
		int32_t dx = (*x1 - *x0) << 16;
		int32_t dy = (*y1 - *y0); // 16.16 fixed point calculation trick
		int32_t m = dx / dy;
		*x0 = *x0 + ((m * -(*y0)) >> 16); // get x0 at boundary
		*y0 = 0;
	}
	
	// Clip against y1 >= 176
	if (*y1 >= 176) {
		int32_t dx = (*x1 - *x0) << 16;
		int32_t dy = (*y1 - *y0); // 16.16 fixed point calculation trick
		int32_t m = dx / dy;
		*x1 = *x1 + ((m * ((176 - 1) - *y1)) >> 16); // get y0 at boundary
		*y1 = 176-1;
	}
	return 1; // clipped succesfully
}

void directLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
	if ((uint16_t)x0 >= 220 || (uint16_t)y0 >= 176 || (uint16_t)x1 >= 220 || (uint16_t)y1 >= 176 ) {
		if (clipLine (&x0,&y0,&x1,&y1)==0) return; // line out of bounds
	}
	if (x0 == x1)
	{
		for (int16_t y = std::min(y0, y1); y <= std::max(y0, y1); ++y)
			Pokitto::Display::directPixel(x0, y, color);
	}
	else if (y0 == y1)
	{
		for (int16_t x = std::min(x0, x1); x <= std::max(x0, x1); ++x)
			Pokitto::Display::directPixel(x, y0, color);
	}
	else {
		int e;
		signed int dx,dy,j, temp;
		signed char s1,s2, xchange;
		signed int x,y;
		
		x = x0;
		y = y0;
		
		//take absolute value
		if (x1 < x0) {
			dx = x0 - x1;
			s1 = -1;
		}
		else if (x1 == x0) {
			dx = 0;
			s1 = 0;
		}
		else {
			dx = x1 - x0;
			s1 = 1;
		}
		
		if (y1 < y0) {
			dy = y0 - y1;
			s2 = -1;
		}
		else if (y1 == y0) {
			dy = 0;
			s2 = 0;
		}
		else {
			dy = y1 - y0;
			s2 = 1;
		}
		
		xchange = 0;
		
		if (dy>dx) {
			temp = dx;
			dx = dy;
			dy = temp;
			xchange = 1;
		}
		
		e = ((int)dy<<1) - dx;
		
		for (j=0; j<=dx; j++) {
			Pokitto::Display::directPixel(x,y, color);
			
			if (e>=0) {
				if (xchange==1) x = x + s1;
				else y = y + s2;
				e = e - ((int)dx<<1);
			}
			if (xchange==1)
				y = y + s2;
			else
				x = x + s1;
			e = e + ((int)dy<<1);
		}
	}
}

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
	return p1.x * p2.x + p1.y * p2.y;
}

bool linePoint(const Line &l, const Point &p)
{
	float d1 = dist(p, l.p1);
	float d2 = dist(p, l.p2);
	float lineLen = dist(l.p1, l.p2);
	float buffer = 0.1;
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
	{
		closest.x = l.p1.x;
		closest.y = l.p1.y;
		return true;
	}
	else if (pointCircle(l.p2, c))
	{
		closest.x = l.p2.x;
		closest.y = l.p2.y;
		return true;
	}
	float len = dist(l.p1, l.p2);
	float dot = ( ((c.center.x-l.p1.x)*(l.p2.x-l.p1.x)) + ((c.center.y-l.p1.y)*(l.p2.y-l.p1.y)) ) / pow(len,2);
	
	// find the closest point on the line
	closest.x = l.p1.x + (dot * (l.p2.x-l.p1.x));
	closest.y = l.p1.y + (dot * (l.p2.y-l.p1.y));
	
	// is this point actually on the line segment?
	// if so keep going, but if not, return false
	//if (!linePoint(l, closest)) // Closest point already lies along line's plane, just need to test if it falls within it's bounds.
	//	return false;
	if (closest.x < std::min(l.p1.x, l.p2.x))
		return false;
	if (closest.x > std::max(l.p1.x, l.p2.x))
		return false;
	if (closest.y < std::min(l.p1.y, l.p2.y))
		return false;
	if (closest.y > std::max(l.p1.y, l.p2.y))
		return false;
	
	if (dist(c.center, closest) <= c.radius + 0.1)
		return true;
	return false;
}

bool circleCircle(const Circle &c1, const Circle &c2, Point &closest)
{
	float distance = dist(c1.center, c2.center);
	if (distance <= c1.radius + c2.radius)
	{
		closest.x = c1.center.x + ((c1.center.x - c2.center.x) / distance) * c1.radius;
		closest.y = c1.center.y + ((c1.center.y - c2.center.y) / distance) * c1.radius;
		return true;
	}
	return false;
}

void updateCollisionBuffer()
{
	if ((int)ball.center.x / 32 != collisionX || (int)ball.center.y / 32 != collisionY)
	{
		collisionX = (int)ball.center.x / 32;
		collisionY = (int)ball.center.y / 32;
		file->seek(TABLE_HEIGHT * TABLE_ROW_BYTES + (collisionY * (TABLE_WIDTH / 32) + collisionX) * 1024);
		file->read(collisionData, 512);
		file->read(collisionData + 512, 512);
		//memcpy(collisionData, tableMask2 + TABLE_HEIGHT * TABLE_ROW_BYTES + (collisionY * (TABLE_WIDTH / 32) + collisionX) * 1024, 1024);
	}
}

void rotatePoint(Point &p, const Point &o, const uint8_t angle)
{
	//x2=cosβx1−sinβy1
	//y2=sinβx1+cosβy1
	p.x -= o.x;
	p.y -= o.y;
	float tmp = p.x * cosTable[angle] - p.y * sinTable[angle];
	p.y = p.x * sinTable[angle] + p.y * cosTable[angle];
	p.x = tmp;
	p.x += o.x;
	p.y += o.y;
}

int step()
{
	Point closest;
	float len;
	//uint8_t collisionValue;
	ballVelocity.y += gravity;
	ball.center.x += ballVelocity.x;
	ball.center.y += ballVelocity.y;
	colliding = false;
	if (ball.center.x - ball.radius <= 0.0)
	{
		ball.center.x = ball.radius + 0.1;
		ballVelocity.x *= -0.6;
	}
	else if (ball.center.x + ball.radius >= TABLE_WIDTH)
	{
		ball.center.x = TABLE_WIDTH - ball.radius - 0.1;
		ballVelocity.x *= -0.6;
	}
	if (ball.center.y - ball.radius <= 0.0)
	{
		ball.center.y = ball.radius + 0.1;
		ballVelocity.y *= -0.6;
	}
	else if (ball.center.y + ball.radius >= TABLE_HEIGHT)
	{
		ball.center.y = TABLE_HEIGHT - ball.radius - 0.1;
		ballVelocity.y *= -0.6;
	}
	updateCollisionBuffer();
	collisionValue = tableMask[(int)ball.center.y * TABLE_WIDTH + (int)ball.center.x];
	collisionValue2 = tableMask2[TABLE_HEIGHT * TABLE_ROW_BYTES + (collisionY * (TABLE_WIDTH / 32) + collisionX) * 1024 + ((int)ball.center.y % 32) * 32 + ((int)ball.center.x % 32)];
	collisionValue3 = collisionData[((int)ball.center.y % 32) * 32 + ((int)ball.center.x % 32)];
	if (collisionValue != 255)
	{
		normal.x = cosTable[collisionValue];
		normal.y = sinTable[collisionValue];
		float dot2 = dot(ballVelocity, normal) * 1.6;
		ballVelocity.x -= dot2 * normal.x;
		ballVelocity.y -= dot2 * normal.y;
		ball.center.x += normal.x * (ball.center.x - (int)ball.center.x + 0.1);
		ball.center.y += normal.y * (ball.center.y - (int)ball.center.y + 0.1);
		//updateCollisionBuffer();
		//collisionValue = tableMask2[TABLE_HEIGHT * TABLE_ROW_BYTES + (collisionY * (TABLE_WIDTH / 32) + collisionX) * 1024 + ((int)ball.center.y % 32) * 32 + ((int)ball.center.x % 32)];
	}
	for (int i = 0; i < 4; ++i)
	{
		if (lineCircle(flipperLines[i], ball, closest))
		{
			//float rad = dist(closest, flipperCircles[i & 2].center);
			float vel = flipperAngularVelocity[i / 2] / 255.0 * 2.0 * PI;
			//len = sqrt(dot(ballVelocity, ballVelocity));
			//ballVelocity.x += ballVelocity.x / len * vel;
			//ballVelocity.y += ballVelocity.y / len * vel;
			normal.x = ball.center.x - closest.x;
			normal.y = ball.center.y - closest.y;
			len = dist(closest, ball.center);
			normal.x /= len;
			normal.y /= len;
			//circumference = 2 * PI * radius
			//velocity = angleVelocity / 255 * circumfrance
			vel += std::abs(dot(ballVelocity, normal) * 1.6);
			ballVelocity.x += vel * normal.x;
			ballVelocity.y += vel * normal.y;
			ball.center.x += normal.x * (ball.radius + 0.1 - len);
			ball.center.y += normal.y * (ball.radius + 0.1 - len);
			colliding = true;
		}
		else if (circleCircle(flipperCircles[i], ball, closest))
		{
			//float rad = dist(closest, flipperCircles[i & 2].center);
			float vel = flipperAngularVelocity[i / 2] / 255.0 * 2.0 * PI;
			//len = sqrt(dot(ballVelocity, ballVelocity));
			//ballVelocity.x += ballVelocity.x / len * vel;
			//ballVelocity.y += ballVelocity.y / len * vel;
			len = dist(flipperCircles[i].center, ball.center);
			normal.x = (ball.center.x - flipperCircles[i].center.x) / len;
			normal.y = (ball.center.y - flipperCircles[i].center.y) / len;
			vel += std::abs(dot(ballVelocity, normal) * 1.6);
			ballVelocity.x += vel * normal.x;
			ballVelocity.y += vel * normal.y;
			ball.center.x += normal.x * (ball.radius + flipperCircles[i].radius + 0.1 - len);
			ball.center.y += normal.y * (ball.radius + flipperCircles[i].radius + 0.1 - len);
			colliding = true;
		}
	}
	if (ballVelocity.x > 1.28)
		ballVelocity.x = 1.28;
	if (ballVelocity.x < -1.28)
		ballVelocity.x = -1.28;
	if (std::abs(ballVelocity.x) < 0.001)
		ballVelocity.x = 0.0;
	if (ballVelocity.y > 1.28)
		ballVelocity.y = 1.28;
	if (ballVelocity.y < -1.28)
		ballVelocity.y = -1.28;
	if (std::abs(ballVelocity.y) < 0.001)
		ballVelocity.y = 0.0;
	return collisionValue;
}
