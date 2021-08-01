#include "BPNeuralNetwork.h"
#include "struct.h"
#include <iostream>
using std::cout;
class brain
{
public:
	double a = 0.5, b = 0.5;
	brain () :bp (0.5, 0.5)
	{
		bp.CreateInputsLayer (5, 5);// 20 10
		bp.CreateHiddenLayer (20);
		bp.CreateHiddenLayer (10);
 		bp.CreateOutputsLayer (1);
	};
	BPNeuralNetwork bp;
	Vector2D GetAction (Point point, Vector2D vec, double v, Point point_e)
	{
		v = v / 125;

		vector<double> in, out;
		static double last_s = 0, last_v = 0, last_alpha = 0, last_vl = 0, last_vr = 0, last_r = 0;
 		double max_vl, max_vr, max_r = 0;
		Vector2D vec_e;

		vec_e.x = point_e.x - point.x;
		vec_e.y = point_e.y - point.y;
		double s = sqrt ((point.x - point_e.x)*(point.x - point_e.x) + (point.y - point_e.y)*(point.y - point_e.y));
		double alpha = abs (Vector2D::radianToAngle (acos ((vec_e*vec) / (vec_e.module ()*vec.module ()))));
		if ((vec.x*vec_e.y - vec.y*vec_e.x) < 0)
			alpha = 0 - alpha;
		auto SearchSpeed = [&]() {
			in.resize (7);
			in[0] = s;
			in[1] = v;
			in[2] = alpha;
 			out.resize (1);
			max_r = 0;
			for (int r = -125; r <= 125; r++)
			{
				for (int l = -125; l <= 125; l++)
				{
					in[3] = r / 125;
					in[4] = l / 125;
					bp.Input (&in, &out);
					if (out[0] > max_r)
					{
						max_vl = l;
						max_vr = r;
						max_r = out[0];
					}
				}
			}
  		};

		if (s<=1)
		{
			getchar ();
		}

		if (last_s != 0)
		{
			SearchSpeed ();
			auto R = [&]()->double {return  100 * (last_s - s) * v; };//v - v / s	,v/(s)
			// Q(S,A) ¡û (1-¦Á)*Q(S,A) + ¦Á*[R + ¦Ã*maxQ(S',a)]
			auto Q = [&]()->double {return (1 - a)*last_r + a*(R () + b*max_r); };
			cout << "Î»ÒÆ:" << last_s - s << "	R:" << R () << "	dQ:" << Q () - last_r << "	¾àÀë:" << s << endl;

			vector<double> tin, tout, te;
			tin.push_back (last_s);
			tin.push_back (last_v);
			tin.push_back (last_alpha);
			tin.push_back (last_vr);
			tin.push_back (last_vl);
			te.push_back (Q ());

			bp.Train (&tin, &tout, &te);
		}
		SearchSpeed ();

		last_alpha = alpha;
		last_r = max_r;
		last_s = s;
		last_v = v;
		last_vl = max_vl;
		last_vr = max_vr;
 
		Vector2D ret;
		ret.x = max_vl * 125;
		ret.y = max_vr * 125;
		return ret;
	}
};

