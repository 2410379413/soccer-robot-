#pragma once
#include "struct.h"
#include <vector>
using namespace std;
class BezierCurves
{
	Point m_originPoint[4];
	Point m_controlPoint[4];
public:
	const int originCount = 4;
	BezierCurves (Point *origin)
	{
		CreateCurve (origin);
	}
	void CreateCurve (Point *originPoint)
	{
		//控制点收缩系数 ，经调试0.6较好， 
		double scale = 0.6;
		Point midpoints[4];
 		//生成中点       
		for (int i = 0; i < originCount; i++)
		{
			int nexti = (i + 1) % originCount;
			midpoints[i].x = (originPoint[i].x + originPoint[nexti].x) / 2.0;
			midpoints[i].y = (originPoint[i].y + originPoint[nexti].y) / 2.0;
		}

		//平移中点  
		Point extrapoints[2*4];
 
		for (int i = 0; i < originCount; i++)
		{
			int nexti = (i + 1) % originCount;
			int backi = (i + originCount - 1) % originCount;
			Point midinmid;
			midinmid.x = (midpoints[i].x + midpoints[backi].x) / 2.0;
			midinmid.y = (midpoints[i].y + midpoints[backi].y) / 2.0;
			double offsetx = originPoint[i].x - midinmid.x;
			double offsety = originPoint[i].y - midinmid.y;
			int extraindex = 2 * i;
			extrapoints[extraindex].x = midpoints[backi].x + offsetx;
			extrapoints[extraindex].y = midpoints[backi].y + offsety;
			//朝 originPoint[i]方向收缩   
			double addx = (extrapoints[extraindex].x - originPoint[i].x) * scale;
			double addy = (extrapoints[extraindex].y - originPoint[i].y) * scale;
			extrapoints[extraindex].x = originPoint[i].x + addx;
			extrapoints[extraindex].y = originPoint[i].y + addy;

			int extranexti = (extraindex + 1) % (2 * originCount);
			extrapoints[extranexti].x = midpoints[i].x + offsetx;
			extrapoints[extranexti].y = midpoints[i].y + offsety;
			//朝 originPoint[i]方向收缩   
			addx = (extrapoints[extranexti].x - originPoint[i].x) * scale;
			addy = (extrapoints[extranexti].y - originPoint[i].y) * scale;
			extrapoints[extranexti].x = originPoint[i].x + addx;
			extrapoints[extranexti].y = originPoint[i].y + addy;

		}
		//生成4控制点，产生贝塞尔曲线  
		for (int i = 0; i < originCount; i++)
		{
			m_controlPoint[0] = originPoint[i];
			int extraindex = 2 * i;
			m_controlPoint[1] = extrapoints[extraindex + 1];
			int extranexti = (extraindex + 2) % (2 * originCount);
			m_controlPoint[2] = extrapoints[extranexti];
			int nexti = (i + 1) % originCount;
			m_controlPoint[3] = originPoint[nexti];
		}
	}
	double bezier3funcX (double uu)
	{
		double part0 = m_controlPoint[0].x * uu * uu * uu;
		double part1 = 3 * m_controlPoint[1].x * uu * uu * (1 - uu);
		double part2 = 3 * m_controlPoint[2].x * uu * (1 - uu) * (1 - uu);
		double part3 = m_controlPoint[3].x * (1 - uu) * (1 - uu) * (1 - uu);
		return part0 + part1 + part2 + part3;
	}
	double bezier3funcY (double uu)
	{
		double part0 = m_controlPoint[0].y * uu * uu * uu;
		double part1 = 3 * m_controlPoint[1].y * uu * uu * (1 - uu);
		double part2 = 3 * m_controlPoint[2].y * uu * (1 - uu) * (1 - uu);
		double part3 = m_controlPoint[3].y * (1 - uu) * (1 - uu) * (1 - uu);
		return part0 + part1 + part2 + part3;
	}
	Vector2D GetSlope (double t)
	{
		Vector2D ret;
		ret.x = bezier3funcX (t + 0.01) - bezier3funcX (t);
		ret.y = bezier3funcY (t + 0.01) - bezier3funcY (t);
		ret=ret.regularize ();
		return ret;
	}
};
 //三次贝塞尔曲线  
