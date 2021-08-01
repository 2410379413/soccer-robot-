#pragma once
#include "Strategy.h"
#include "BezierCurves .h"
#include "struct.h"
#include <iostream>
#define L 3.1
#define MAX_RATE 1.8
class robotMove
{
public:
	double m_last_v = 0;
	double m_last_theta = 0;

public:
	robotMove (Robot*  robot) :m_robot (robot)
	{
		m_last_robot_angle = Vector2D::CreateVecFromAngle (m_robot->rotation);
		m_last_robot_point = Point (m_robot->pos.x, m_robot->pos.y);
	}
	bool Move (Point p, double rate, double err = 0.2, double speedcut1 = 2, double speedcut2 = 5)
	{
		updata ();
		double s = p.disp (m_robot->pos.x, m_robot->pos.y);
		if (s < err)
		{
			pStop ();
			return true;
		}
		else if (s < speedcut1)
		{
			if (m_last_v > 0.2)
			{
				pStop ();
			}
			else
			{
				pMove (Vector2D (p.x - m_robot->pos.x, p.y - m_robot->pos.y), 0.2);
			}
		}
		else if (s < speedcut2 && s>speedcut1)
		{
			if (m_last_v > 1)
			{
				pStop ();
			}
			else
			{
				pMove (Vector2D (p.x - m_robot->pos.x, p.y - m_robot->pos.y), 1);
			}
		}
		else if (s > speedcut2)
		{
			pMove (Vector2D (p.x - m_robot->pos.x, p.y - m_robot->pos.y), rate);
		}
		return false;
	}
	bool Move (Vector2D angle, double rate)
	{
		updata ();
		return  pMove (angle, rate);
	}
	bool MoveByCircle (double r, double rate)
	{
		updata ();

		static double accumulated_angle = 0;
		double x_rate, x_th, v_r, v_l;
		x_th = rate * 2 * L / (2 * r);
		x_rate = rate * 2;
		v_r = (x_rate - x_th) / 2;
		v_l = x_rate - v_r;
		m_robot->velocityRight = v_r / 0.015;
		m_robot->velocityLeft = v_l / 0.015;


		accumulated_angle += m_last_theta;
		if (accumulated_angle > 400)
		{
			accumulated_angle = 0;
			return true;
		}
		else
		{
			return false;
		}
 	}
	void MoveByAngle (double theta, double rate)
	{
		updata ();
		pMoveByAngle (theta, rate);
	}
	void Stop ()
	{
		updata ();
		pStop ();
	}
	void Set (double vr,double vl)
	{
		updata ();
		m_robot->velocityRight = vr;
		m_robot->velocityLeft = vl;
	}
private:
	Robot *m_robot;
	Vector2D m_robot_angle;
	Vector2D m_last_robot_angle;
	Point m_last_robot_point;
	void updata ()
	{
		m_robot_angle = Vector2D::CreateVecFromAngle (m_robot->rotation);

		static Vector2D tmp_last_robot_angle = m_last_robot_angle;
		static Point tmp_last_robot_point = m_last_robot_point;

		m_last_robot_angle = tmp_last_robot_angle;
		m_last_robot_point = tmp_last_robot_point;

		m_last_v = m_last_robot_point.disp (m_robot->pos.x, m_robot->pos.y);
		m_last_theta = Vector2D::GetAngle (m_last_robot_angle, Vector2D::CreateVecFromAngle (m_robot->rotation));

		tmp_last_robot_point = Point (m_robot->pos.x, m_robot->pos.y);
		tmp_last_robot_angle = Vector2D::CreateVecFromAngle (m_robot->rotation);
	}
	bool pMove (Vector2D angle, double rate)
	{
		double theta = Vector2D::GetAngle (m_robot_angle, angle);
		//cout << " ¼Ð½Ç:  " << theta << " ÉÏ´Î¼Ð½Ç:  " << last_theta << endl;
		pMoveByAngle (theta, rate);
		return  (abs (theta) < 0.1) && (abs (m_last_theta) < 0.1);
	}
	void pMoveByAngle (double theta, double rate)
	{
		if (!((m_last_theta<0 && theta < 0 && theta < m_last_theta) || (m_last_theta>0 && theta>0 && theta  > m_last_theta)) && m_last_theta != 0)
		{
			 theta = 0 - (m_last_theta - theta)*1.3;
		}
		double x_th, x_rate, max_valid_rate, v_r, v_l;
		x_th = theta * 2 * pi*L / 360;
		if (x_th < 0 - MAX_RATE * 2)
			x_th = 0 - MAX_RATE * 2;
		if (x_th > MAX_RATE * 2)
			x_th = MAX_RATE * 2;
		// cout << "  Xth   " << x_th;
		max_valid_rate = (MAX_RATE * 2 - abs (x_th)) / 2;
		if (rate > max_valid_rate)
			rate = max_valid_rate;
		x_rate = rate * 2;

		v_r = (x_rate - x_th) / 2;
		v_l = x_rate - v_r;

		m_robot->velocityRight = v_r / 0.015;
		m_robot->velocityLeft = v_l / 0.015;
	//	cout << " r  " << m_robot->velocityRight << "  l   " << m_robot->velocityLeft << endl;
	}
	void pStop ()
	{
		m_robot->velocityLeft = 0;
		m_robot->velocityRight = 0;
	}
};

