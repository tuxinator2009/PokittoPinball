#include "Pokitto.h"
#include <File>
#include "gfx.h"
#include "physics.h"

extern File *file;
extern Graphic graphics[];
extern Sprite sprites[];
extern Light lights[];
extern Sprite *ballSprite;
extern Sprite *flipperLSprite;
extern Sprite *flipperRSprite;
extern Circle ball;
extern Point ballVelocity;
extern Point normal;
extern Point collisionPoint;
extern Circle flipperCircles[];
extern Line flipperLines[];
extern Point flipperLinePoints[];
extern BumperCircle bumperCircles[];
extern BumperLine bumperLines[];
extern int8_t flipperAngle[2];
extern float flipperAngularVelocity[2];
extern float gravity;
extern int collisionX, collisionY, collisionIndex;
extern int screenY1, screenY2;
extern int numSprites;
extern int numBumperCircles;
extern int numBumperLines;
extern int numLights;
extern uint32_t steps;
extern uint8_t scanline[220];
extern uint8_t scanlinesDirty[22];
extern uint8_t collisionData[1024];//16x16 (8bits=angle + 5bits=trigger + 3bits=bounce=v/4)
extern uint8_t collision;
extern uint8_t bumperSteps[2];

void updateLines(int start, int end)
{
	if (end < 0)
		return;
	if (start > 175)
		return;
	if (start < 0)
		start = 0;
	if (end > 175)
		end = 175;
	if (start > end)
		return;
	for (int y = start; y <= end; ++y)
		scanlinesDirty[y/8] |= 128 >> (y % 8);
}

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
	//use length squared instead of length because the dot product is divided by the squared length anyway
	//this saves having to do a costly square root calculation
	float len2 = dist2(l.p1, l.p2);
	float dot = (((c.center.x - l.p1.x) * (l.p2.x - l.p1.x)) + ((c.center.y - l.p1.y) * (l.p2.y - l.p1.y))) / len2;
	//find the closest point along the line's infinite plane to the circle's center
	closest.x = l.p1.x + (dot * (l.p2.x-l.p1.x));
	closest.y = l.p1.y + (dot * (l.p2.y-l.p1.y));
	//since closest point is already on the line's infinite plane we don't need proper line-point collision checking
	//instead we just need to check if the closest point is within the bounds of the line
	//we check both x and y in case of a completely horizontal or vertical line one would always return true
	if (closest.x < std::min(l.p1.x, l.p2.x))
		return false;
	if (closest.x > std::max(l.p1.x, l.p2.x))
		return false;
	if (closest.y < std::min(l.p1.y, l.p2.y))
		return false;
	if (closest.y > std::max(l.p1.y, l.p2.y))
		return false;
	//finally check if the distance between the closest point and the circle's center is less than or equal to the circle's radius
	//We use the distance squared and compare it to the radius squared to eliminate a costly square root calculation
	if (dist2(c.center, closest) <= c.radius * c.radius + 0.1)
		return true;
	return false;
}

bool circleCircle(const Circle &c1, const Circle &c2, Point &closest)
{
	//avoid square root calculation for checking collision
	float distance = dist2(c1.center, c2.center);
	if (distance <= (c1.radius + c2.radius) * (c1.radius + c2.radius))
	{
		//only calculate the square root if the two circles are in fact colliding
		//this is needed so we can determine the point along the first circle where the collision took place.
		distance = sqrt(distance);
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
		file->seek(TABLE_COLLISION_START + (collisionY * TABLE_COLLISION_WIDTH + collisionX) * 1024);
		file->read(collisionData, 512);
		file->read(collisionData + 512, 512);
	}
}

//Rotate point (p) by angle around origin (o)
//Angle is degrees / 360 * 255
void rotatePoint(Point &p, const Point &o, const float &cosine, const float &sine)
{
	//x2=cosβx1−sinβy1
	//y2=sinβx1+cosβy1
	p.x -= o.x;
	p.y -= o.y;
	float tmp = p.x * cosine - p.y * sine;
	p.y = p.x * sine + p.y * cosine;
	p.x = tmp;
	p.x += o.x;
	p.y += o.y;
}

void rotateBox(Box &b, const Point &o, const float &cosine, const float &sine)
{
	Point ul = {b.ul.x, b.ul.y};
	Point ur = {b.br.x, b.ul.y};
	Point bl = {b.ul.x, b.br.y};
	Point br = {b.br.x, b.br.y};
	rotatePoint(ul, o, cosine, sine);
	rotatePoint(ur, o, cosine, sine);
	rotatePoint(bl, o, cosine, sine);
	rotatePoint(br, o, cosine, sine);
	b.ul.x = std::min(std::min(std::min(ul.x, ur.x), bl.x), br.x);
	b.ul.y = std::min(std::min(std::min(ul.y, ur.y), bl.y), br.y);
	b.br.x = std::max(std::max(std::max(ul.x, ur.x), bl.x), br.x);
	b.br.y = std::max(std::max(std::max(ul.y, ur.y), bl.y), br.y);
}

void step()
{
	Point closest;
	float len;
	uint8_t collisionValue;
	ballVelocity.y += gravity;
	ball.center.x += ballVelocity.x;
	ball.center.y += ballVelocity.y;
	if (ball.center.x - ball.radius <= 0.0)
	{
		ball.center.x = ball.radius + 0.1;
		ballVelocity.x *= -0.3;
	}
	else if (ball.center.x + ball.radius >= TABLE_WIDTH)
	{
		ball.center.x = TABLE_WIDTH - ball.radius - 0.1;
		ballVelocity.x *= -0.3;
	}
	if (ball.center.y - ball.radius <= 0.0)
	{
		ball.center.y = ball.radius + 0.1;
		ballVelocity.y *= -0.3;
	}
	else if (ball.center.y + ball.radius >= TABLE_HEIGHT)
	{
		ball.center.y = TABLE_HEIGHT - ball.radius - 0.1;
		ballVelocity.y *= -0.6;
	}
	for (int i = 0; i < numBumperCircles && collision == COLLISION_NONE; ++i)
	{
		if (circleCircle(bumperCircles[i].circle, ball, closest))
		{
			len = dist(bumperCircles[i].circle.center, ball.center);
			normal.x = (ball.center.x - bumperCircles[i].circle.center.x) / len;
			normal.y = (ball.center.y - bumperCircles[i].circle.center.y) / len;
			float vel = dot(ballVelocity, normal) * (1.0 + bumperCircles[i].rest) - bumperCircles[i].push;
			ballVelocity.x -= vel * normal.x;
			ballVelocity.y -= vel * normal.y;
			ball.center.x += normal.x * (ball.radius + bumperCircles[i].circle.radius + 0.1 - len);
			ball.center.y += normal.y * (ball.radius + bumperCircles[i].circle.radius + 0.1 - len);
			collisionPoint.x = closest.x;
			collisionPoint.y = closest.y;
			collision = COLLISION_BUMPER;
			if (i == 0)
			{
				updateLines(sprites[2].y - screenY1, sprites[2].y - screenY1 + sprites[2].gfx->h - 1);
				sprites[2].frame = 1;
				//sprites[BUMPER_1_NORMAL].flags = Sprite::FLAG_NONE|Sprite::FLAG_LAYER1;
				//sprites[BUMPER_1_SQUISH].flags = Sprite::FLAG_DRAW|Sprite::FLAG_LAYER1;
				bumperSteps[0] = BUMPER_STEPS;
			}
			else if (i == 1)
			{
				updateLines(sprites[3].y - screenY1, sprites[3].y - screenY1 + sprites[3].gfx->h - 1);
				sprites[3].frame = 1;
				//sprites[BUMPER_2_NORMAL].flags = Sprite::FLAG_NONE|Sprite::FLAG_LAYER1;
				//sprites[BUMPER_2_SQUISH].flags = Sprite::FLAG_DRAW|Sprite::FLAG_LAYER1;
				bumperSteps[1] = BUMPER_STEPS;
			}
		}
	}
	for (int i = 0; i < numBumperLines && collision == COLLISION_NONE; ++i)
	{
		if (lineCircle(bumperLines[i].line, ball, closest))
		{
			len = dist(closest, ball.center);
			normal.x = (ball.center.x - closest.x) / len;
			normal.y = (ball.center.y - closest.y) / len;
			float vel = dot(ballVelocity, normal) * (1.0 + bumperLines[i].rest) - bumperLines[i].push;
			ballVelocity.x -= vel * normal.x;
			ballVelocity.y -= vel * normal.y;
			ball.center.x += normal.x * (ball.radius + 0.1 - len);
			ball.center.y += normal.y * (ball.radius + 0.1 - len);
			collisionPoint.x = closest.x;
			collisionPoint.y = closest.y;
			collision = COLLISION_BUMPER;
			if (i <= 1)
			{
				updateLines(lights[i].y - screenY1, lights[i].y - screenY1 + lights[i].gfx->h - 1);
				lights[i].state |= Light::STATE_ON;
				lights[i].timer = 50;
			}
		}
	}
	if (collision == COLLISION_NONE)
	{
		updateCollisionBuffer();
		collisionValue = collisionData[((int)ball.center.y % 32) * 32 + ((int)ball.center.x % 32)];
		if (collisionValue != 255)
		{
			normal.x = cosTable[collisionValue];
			normal.y = sinTable[collisionValue];
			float dot2 = dot(ballVelocity, normal) * 1.3;
			if (dot2 < 0.0)
			{
				ballVelocity.x -= dot2 * normal.x;
				ballVelocity.y -= dot2 * normal.y;
				ball.center.x += normal.x * (ball.center.x - (int)ball.center.x + 0.1);
				ball.center.y += normal.y * (ball.center.y - (int)ball.center.y + 0.1);
				collision = COLLISION_TABLE;
			}
			//updateCollisionBuffer();
			//collisionValue = collisionData[((int)ball.center.y % 32) * 32 + ((int)ball.center.x % 32)];
		}
	}
	for (int i = 0; i < 4 && collision == COLLISION_NONE; ++i)
	{
		if (lineCircle(flipperLines[i], ball, closest))
		{
			float rad = dist(closest, flipperCircles[i & 2].center);
			float vel = flipperAngularVelocity[i / 2] * 1.2 * PI * rad;
			len = dist(closest, ball.center);
			normal.x = (ball.center.x - closest.x) / len;
			normal.y = (ball.center.y - closest.y) / len;
			vel += std::abs(dot(ballVelocity, normal) * 1.2);
			ballVelocity.x += vel * normal.x;
			ballVelocity.y += vel * normal.y;
			ball.center.x += normal.x * (ball.radius + 0.1 - len);
			ball.center.y += normal.y * (ball.radius + 0.1 - len);
			collisionPoint.x = closest.x;
			collisionPoint.y = closest.y;
			if (i / 2 == 0)
				collision = COLLISION_LFLIPPER;
			else
				collision = COLLISION_RFLIPPER;
		}
	}
	for (int i = 0; i < 4 && collision == COLLISION_NONE; ++i)
	{
		if (circleCircle(flipperCircles[i], ball, closest))
		{
			float rad = dist(closest, flipperCircles[i & 2].center);
			float vel = flipperAngularVelocity[i / 2] * 1.2 * PI * rad;
			len = dist(flipperCircles[i].center, ball.center);
			normal.x = (ball.center.x - flipperCircles[i].center.x) / len;
			normal.y = (ball.center.y - flipperCircles[i].center.y) / len;
			vel += std::abs(dot(ballVelocity, normal) * 1.2);
			ballVelocity.x += vel * normal.x;
			ballVelocity.y += vel * normal.y;
			ball.center.x += normal.x * (ball.radius + flipperCircles[i].radius + 0.1 - len);
			ball.center.y += normal.y * (ball.radius + flipperCircles[i].radius + 0.1 - len);
			collisionPoint.x = closest.x;
			collisionPoint.y = closest.y;
			if (i / 2 == 0)
				collision = COLLISION_LFLIPPER;
			else
				collision = COLLISION_RFLIPPER;
		}
	}
	len = ballVelocity.x * ballVelocity.x + ballVelocity.y * ballVelocity.y;
	if (len > 4.0)
	{
		len = 2.0 / sqrt(len);
		ballVelocity.x *= len;
		ballVelocity.y *= len;
	}
	else if (len <= 0.000001)
	{
		ballVelocity.x = 0.0;
		ballVelocity.y = 0.0;
	}
	++steps;
}
