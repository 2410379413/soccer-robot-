#pragma once
#include <math.h>
#define pi 3.1415926
struct Vector2D
{
	double x, y;
	Vector2D (double x1, double y1)
	{
		x = x1;
		y = y1;
	};
	Vector2D ()
	{
		x = 0;
		y = 0;
	};
	double  operator*(Vector2D r)
	{
		return r.x*x + r.y*y;
	}
	double module () const
	{
		return sqrt (x*x + y*y);
	}
	Vector2D regularize ()
	{
		Vector2D ret;
		ret.x = x / module ();
		ret.y = y / module ();
		return ret;
	}
	Vector2D rotateByAngle (double angle)
	{
		angle = angleToRadian (angle);
		//Ë³Ê±ÕëÐý×ª
		//x1 = x0 * cosB + y0 * sinB
		//y1 = -x0 * sinB + y0 * cosB
		Vector2D ret;
		ret.x = x * cos (angle) + y * sin (angle);
		ret.y = -x * sin (angle) + y * cos (angle);
		return ret;
	}
	static Vector2D CreateVecFromAngle (double angle)
	{
		Vector2D ret;
		angle = angleToRadian (angle);
		ret.x = cos (angle);
		ret.y = sin (angle);
		return ret;
	}
	static double angleToRadian (double angle)
	{
		return angle = angle / 180.0* pi;
	}
	static double radianToAngle (double radian)
	{
		return radian = radian / pi*180.0;
	}

	static double GetAngle (Vector2D v1, Vector2D v2)
	{
		double theta = Vector2D::radianToAngle (acos ((v1*v2) / (v1.module ()*v2.module ())));
		theta = fabs (theta);
		if (v1.x*v2.y - v1.y*v2.x > 0)
			theta = 0 - theta;
		return theta;
	}

};
struct Point
{
	double x;
	double y;
	Point (double x1, double y1)
	{
		x = x1;
		y = y1;
	}
	Point ()
	{
		x = 0;
		y = 0;
	}
	double disp (double x1, double y1)
	{
		return sqrt ((x - x1)*(x - x1) + (y - y1)*(y - y1));
	}
};
