#ifndef PHYSICS_H
#define PHYSICS_H

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

struct Box
{
	Point ul;
	Point br;
};

const float sinTable[] =
{
	 0.000000, 0.024637, 0.049260, 0.073853, 0.098400, 0.122888, 0.147302, 0.171626, 0.195845, 0.219946, 0.243914, 0.267733, 0.291390, 0.314870, 0.338158, 0.361242,
	 0.384106, 0.406737, 0.429121, 0.451244, 0.473094, 0.494656, 0.515918, 0.536867, 0.557489, 0.577774, 0.597707, 0.617278, 0.636474, 0.655284, 0.673696, 0.691698,
	 0.709281, 0.726434, 0.743145, 0.759405, 0.775204, 0.790532, 0.805381, 0.819740, 0.833602, 0.846958, 0.859800, 0.872120, 0.883910, 0.895163, 0.905873, 0.916034,
	 0.925638, 0.934680, 0.943154, 0.951057, 0.958381, 0.965124, 0.971281, 0.976848, 0.981823, 0.986201, 0.989980, 0.993159, 0.995734, 0.997705, 0.999070, 0.999829,
	 0.999981, 0.999526, 0.998464, 0.996795, 0.994522, 0.991645, 0.988165, 0.984086, 0.979410, 0.974139, 0.968276, 0.961826, 0.954791, 0.947177, 0.938988, 0.930229,
	 0.920906, 0.911023, 0.900587, 0.889604, 0.878081, 0.866025, 0.853444, 0.840344, 0.826734, 0.812622, 0.798017, 0.782928, 0.767363, 0.751332, 0.734845, 0.717912,
	 0.700543, 0.682749, 0.664540, 0.645928, 0.626924, 0.607539, 0.587785, 0.567675, 0.547220, 0.526432, 0.505325, 0.483911, 0.462204, 0.440216, 0.417960, 0.395451,
	 0.372702, 0.349727, 0.326539, 0.303153, 0.279583, 0.255843, 0.231948, 0.207912, 0.183750, 0.159476, 0.135105, 0.110653, 0.086133, 0.061561, 0.036951, 0.012320,
	-0.012320,-0.036951,-0.061561,-0.086133,-0.110653,-0.135105,-0.159476,-0.183750,-0.207912,-0.231948,-0.255843,-0.279583,-0.303153,-0.326539,-0.349727,-0.372702,
	-0.395451,-0.417960,-0.440216,-0.462204,-0.483911,-0.505325,-0.526432,-0.547220,-0.567675,-0.587785,-0.607539,-0.626924,-0.645928,-0.664540,-0.682749,-0.700543,
	-0.717912,-0.734845,-0.751332,-0.767363,-0.782928,-0.798017,-0.812622,-0.826734,-0.840344,-0.853444,-0.866025,-0.878081,-0.889604,-0.900587,-0.911023,-0.920906,
	-0.930229,-0.938988,-0.947177,-0.954791,-0.961826,-0.968276,-0.974139,-0.979410,-0.984086,-0.988165,-0.991645,-0.994522,-0.996795,-0.998464,-0.999526,-0.999981,
	-0.999829,-0.999070,-0.997705,-0.995734,-0.993159,-0.989980,-0.986201,-0.981823,-0.976848,-0.971281,-0.965124,-0.958381,-0.951057,-0.943154,-0.934680,-0.925638,
	-0.916034,-0.905873,-0.895163,-0.883910,-0.872120,-0.859800,-0.846958,-0.833602,-0.819740,-0.805381,-0.790532,-0.775204,-0.759405,-0.743145,-0.726434,-0.709281,
	-0.691698,-0.673696,-0.655284,-0.636474,-0.617278,-0.597707,-0.577774,-0.557489,-0.536867,-0.515918,-0.494656,-0.473094,-0.451244,-0.429121,-0.406737,-0.384106,
	-0.361242,-0.338158,-0.314870,-0.291390,-0.267733,-0.243914,-0.219946,-0.195845,-0.171626,-0.147302,-0.122888,-0.098400,-0.073853,-0.049260,
};

const float cosTable[] =
{
	 1.000000, 0.999696, 0.998786, 0.997269, 0.995147, 0.992421, 0.989092, 0.985162, 0.980635, 0.975512, 0.969797, 0.963493, 0.956604, 0.949135, 0.941089, 0.932472,
	 0.923289, 0.913545, 0.903247, 0.892401, 0.881012, 0.869089, 0.856638, 0.843667, 0.830184, 0.816197, 0.801714, 0.786745, 0.771298, 0.755383, 0.739009, 0.722186,
	 0.704926, 0.687237, 0.669131, 0.650618, 0.631711, 0.612420, 0.592758, 0.572735, 0.552365, 0.531659, 0.510631, 0.489293, 0.467658, 0.445738, 0.423549, 0.401102,
	 0.378411, 0.355491, 0.332355, 0.309017, 0.285492, 0.261793, 0.237935, 0.213933, 0.189801, 0.165554, 0.141206, 0.116773, 0.092268, 0.067708, 0.043107, 0.018479,
	-0.006160,-0.030795,-0.055411,-0.079994,-0.104528,-0.128999,-0.153392,-0.177691,-0.201882,-0.225951,-0.249883,-0.273663,-0.297277,-0.320710,-0.343949,-0.366979,
	-0.389786,-0.412356,-0.434676,-0.456733,-0.478512,-0.500000,-0.521185,-0.542053,-0.562593,-0.582791,-0.602635,-0.622113,-0.641213,-0.659925,-0.678235,-0.696134,
	-0.713610,-0.730653,-0.747253,-0.763398,-0.779081,-0.794290,-0.809017,-0.823253,-0.836989,-0.850217,-0.862929,-0.875117,-0.886774,-0.897892,-0.908465,-0.918487,
	-0.927951,-0.936852,-0.945184,-0.952942,-0.960122,-0.966718,-0.972728,-0.978148,-0.982973,-0.987202,-0.990831,-0.993859,-0.996284,-0.998103,-0.999317,-0.999924,
	-0.999924,-0.999317,-0.998103,-0.996284,-0.993859,-0.990831,-0.987202,-0.982973,-0.978148,-0.972728,-0.966718,-0.960122,-0.952942,-0.945184,-0.936852,-0.927951,
	-0.918487,-0.908465,-0.897892,-0.886774,-0.875117,-0.862929,-0.850217,-0.836989,-0.823253,-0.809017,-0.794290,-0.779081,-0.763398,-0.747253,-0.730653,-0.713610,
	-0.696134,-0.678235,-0.659925,-0.641213,-0.622113,-0.602635,-0.582791,-0.562593,-0.542053,-0.521185,-0.500000,-0.478512,-0.456733,-0.434676,-0.412356,-0.389786,
	-0.366979,-0.343949,-0.320710,-0.297277,-0.273663,-0.249883,-0.225951,-0.201882,-0.177691,-0.153392,-0.128999,-0.104528,-0.079994,-0.055411,-0.030795,-0.006160,
	 0.018479, 0.043107, 0.067708, 0.092268, 0.116773, 0.141206, 0.165554, 0.189801, 0.213933, 0.237935, 0.261793, 0.285492, 0.309017, 0.332355, 0.355491, 0.378411,
	 0.401102, 0.423549, 0.445738, 0.467658, 0.489293, 0.510631, 0.531659, 0.552365, 0.572735, 0.592758, 0.612420, 0.631711, 0.650618, 0.669131, 0.687237, 0.704926,
	 0.722186, 0.739009, 0.755383, 0.771298, 0.786745, 0.801714, 0.816197, 0.830184, 0.843667, 0.856638, 0.869089, 0.881012, 0.892401, 0.903247, 0.913545, 0.923289,
	 0.932472, 0.941089, 0.949135, 0.956604, 0.963493, 0.969797, 0.975512, 0.980635, 0.985162, 0.989092, 0.992421, 0.995147, 0.997269, 0.998786,
};

const uint8_t tableMask[] =
{
	#include "tableMask.txt"
};

const uint8_t tableMask2[] = 
{
	#include "tableMask2.txt"
};

float dist2(const Point &p1, const Point &p2);
float dist(const Point &p1, const Point &p2);
bool linePoint(const Line &l, const Point &p);
bool pointCircle(const Point &p, const Circle &c);
bool lineCircle(float x1, float y1, float x2, float y2, float cx, float cy, float r, float &closestX, float &closestY);
int step();

#endif //PHYSICS_H