#pragma once
#include "BezierCurves .h"
class Path
{
public:
	Path (vector<Point> &vecPoint)
	{
		if (vecPoint.size()<3)
		{

		}
	}
 	vector<BezierCurves> m_vecCurves;
}; 