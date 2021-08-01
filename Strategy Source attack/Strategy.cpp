// Strategy.cpp : Defines the entry point for the DLL application.
//
//1.5a 
// yellow 
// attack

#include "stdafx.h"
#include "Strategy.h"

#include <math.h>

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
//const double ballx;const double bally;
int a=0;int b=0;

const double PI = 3.1415923;
void Position2( Robot *robot, double x, double y);
char myMessage[200]; //big enough???
bool ifchao( Robot *robot,double x,double y);//是否朝向某一点
void AngleOfPosition(Robot *robot,double x, double y);//让机器人 ( 原地 ) 旋转始终对着某一点, 不分前后 
void circle( Robot *robot,double Targetx,double Targety,double SameSpeed);
double suanr(double tempx, double tempy);
void posz(Robot *robot,double jiao);
void shoot( Robot *robot, Environment *env );
double leng(double x1,double x2,double y1,double y2);
void Angle1(Robot *robot,int desired_angle);
void Position1( Robot *robot, double x, double y ,Environment *env);
//double  jiaodu(double x1,double y1,double x2,double y2);
//void move(Robot *robot,double jiao1,double r);

void PredictBall ( Environment *env );
void Goalie1 ( Robot *robot, Environment *env );
void NearBound2 ( Robot *robot, double vl, double vr, Environment *env );
void Attack2 ( Robot *robot, Environment *env );
void Defend ( Robot *robot, Environment *env, double low, double high );

// by moon at 9/2/2002
void MoonAttack (Robot *robot, Environment *env );
// just for testing to check whether the &env->opponent works or not
void MoonFollowOpponent (  Robot *robot, OpponentRobot *opponent );


void Velocity ( Robot *robot, double vl, double vr );// int  gai wei  double
void Angle ( Robot *robot, double desired_angle);  // tong shang
void Position( Robot *robot, double x, double y );

extern "C" STRATEGY_API void Create ( Environment *env )
{
	// allocate user data and assign to env->userData
	// eg. env->userData = ( void * ) new MyVariables ();
}

extern "C" STRATEGY_API void Destroy ( Environment *env )
{
	// free any user data created in Create ( Environment * )

	// eg. if ( env->userData != NULL ) delete ( MyVariables * ) env->userData;
}


extern "C" STRATEGY_API void Strategy ( Environment *env )
{

	// the below codes are just for demonstration purpose....don't take this seriously please.

	int testInt = 100;
	int k;

	switch (env->gameState)
	{
		case 0:
			// default

		/*	MoonFollowOpponent ( &env->home [1], &env->opponent [2] );
			MoonFollowOpponent ( &env->home [2], &env->opponent [3] );
			MoonFollowOpponent ( &env->home [3], &env->opponent [4] );
			
			MoonAttack ( &env->home [4], env );
			Goalie1 ( &env->home [0], env ); */
			shoot(&env->home [4], env );
//Position(&env->home [4],0,58);

			break;

		case FREE_BALL:

			// Follow opponent guy
			MoonFollowOpponent ( &env->home [1], &env->opponent [2] );
			MoonFollowOpponent ( &env->home [2], &env->opponent [3] );
			MoonFollowOpponent ( &env->home [3], &env->opponent [4] );

			// attack
			MoonAttack ( &env->home [4], env );

			// Goal keeper
			Goalie1 ( &env->home [0], env );

			// by moon at 24/03/2002
			// below code will not work.... never try....
			//	env->home[0].pos.x = 50;
			//	env->home[0].pos.y = 0;
			//	env->home[0].rotation = 20.0;

			break;

		case PLACE_KICK:
			MoonAttack ( &env->home [2], env );
			break;			
		case PENALTY_KICK: // dian qiu
			switch (env->whosBall)
			{
			case ANYONES_BALL:
				MoonAttack ( &env->home [1], env );
				break;
			case BLUE_BALL:
				MoonAttack ( &env->home [4], env );
				break;
			case YELLOW_BALL:
				MoonAttack ( &env->home [0], env );
				break;
			}
			break;

		case FREE_KICK:

			FILE * debugfile; 
			debugfile = fopen("debugfile.txt","a"); 
			
			for (k=0;k<=4;k++) 
				fprintf(debugfile, "robot: %d x: %f y: %f z: %f \n",
					k, env->opponent[k].pos.x, env->opponent[k].pos.y, 
					env->opponent[k].pos.z); 
			
			fclose(debugfile); 

			MoonAttack ( &env->home [0], env );

			break;

		case GOAL_KICK:
			MoonAttack ( &env->home [0], env );
			break;
  }
}

void MoonAttack ( Robot *robot, Environment *env )
{
	//Velocity (robot, 127, 127);
	//Angle (robot, 45);
	PredictBall ( env );
	Position(robot, env->predictedBall.pos.x, env->predictedBall.pos.y);
	// Position(robot, 0.0, 0.0);
}

void MoonFollowOpponent ( Robot *robot, OpponentRobot *opponent )
{
	Position(robot, opponent->pos.x, opponent->pos.y);
}

void Velocity ( Robot *robot, double vl, double vr )
{
	robot->velocityLeft = vl;
	robot->velocityRight = vr;
//	a+=1;
}
void posz(Robot *robot,double jiao)
{robot->pos.z=jiao*(180/PI);

}

// robot soccer system p329
void Angle ( Robot *robot, double desired_angle)
{
	double theta_e, vl, vr;
	theta_e = desired_angle - (double)robot->rotation;
	
	while (theta_e > 180) theta_e -= 360;
	while (theta_e < -180) theta_e += 360;

	if (theta_e < -90) theta_e += 180;
	
	else if (theta_e > 90) theta_e -= 180;

	if (fabs(theta_e) > 50) 
	{
		vl = (double)(-9./90.0 * (double) theta_e);
		vr = (double)(9./90.0 * (double)theta_e);
	}
	else if (fabs(theta_e) > 20)
	{
		vl = (double)(-11.0/90.0 * (double)theta_e);
		vr = (double)(11.0/90.0 * (double)theta_e);
	}
	Velocity (robot, vl, vr);
}

void Position( Robot *robot, double x, double y )
{
	int desired_angle = 0, theta_e = 0, d_angle = 0, vl, vr, vc = 110;

	double dx, dy, d_e, Ka = 10.0/90.0;
	dx = x - robot->pos.x;
	dy = y - robot->pos.y;

	d_e = sqrt(dx * dx + dy * dy);
	if (dx == 0 && dy == 0)
		desired_angle = 90;
	else
		desired_angle = (int)(180. / PI * atan2((double)(dy), (double)(dx)));// hudu hua 30 du( such as)
	theta_e = desired_angle - (int)robot->rotation;// zhuan de jiao du
	
	while (theta_e > 180) theta_e -= 360;
	while (theta_e < -180) theta_e += 360;

	if (d_e > 100.) 
		Ka = 17. / 90.;
	else if (d_e > 50)
		Ka = 19. / 90.;
	else if (d_e > 30)
		Ka = 21. / 90.;
	else if (d_e > 20)
		Ka = 23. / 90.;
	else 
		Ka = 25. / 90.;
	
	if (theta_e > 95 || theta_e < -95)
	{
		theta_e += 180;
		
		if (theta_e > 180) 
			theta_e -= 360;
		if (theta_e > 80)
			theta_e = 80;
		if (theta_e < -80)
			theta_e = -80;
		if (d_e < 5.0 && abs(theta_e) < 40)
			Ka = 0.1;
		vr = (int)(-vc * (1.0 / (1.0 + exp(-3.0 * d_e)) - 0.3) + Ka * theta_e);
		vl = (int)(-vc * (1.0 / (1.0 + exp(-3.0 * d_e)) - 0.3) - Ka * theta_e);
	}
	
	else if (theta_e < 85 && theta_e > -85)
	{
		if (d_e < 5.0 && abs(theta_e) < 40)
			Ka = 0.1;
		vr = (int)( vc * (1.0 / (1.0 + exp(-3.0 * d_e)) - 0.3) + Ka * theta_e);
		vl = (int)( vc * (1.0 / (1.0 + exp(-3.0 * d_e)) - 0.3) - Ka * theta_e);
	}

	else
	{
		vr = (int)(+.17 * theta_e);
		vl = (int)(-.17 * theta_e);
	}
//robot->velocityLeft = vl;
//	robot->velocityRight = vr;
	Velocity(robot, vl, vr);
}
void Position1( Robot *robot, double x, double y)//stop
{
	int desired_angle = 0, theta_e = 0, d_angle = 0, vl, vr, vc = 100;

	double dx, dy, d_e, Ka = 10.0/90.0;
	dx = x - robot->pos.x;
	dy = y - robot->pos.y;

	d_e = sqrt(dx * dx + dy * dy);
	if (dx == 0 && dy == 0)
		desired_angle = 90;
	else
		desired_angle = (int)(180. / PI * atan2((double)(dy), (double)(dx)));// hudu hua 30 du( such as)
	theta_e = desired_angle - (int)robot->rotation;// zhuan de jiao du
	
	while (theta_e > 180) theta_e -= 360;
	while (theta_e < -180) theta_e += 360;

	if (d_e > 100.) 
		Ka = 17. / 90.;
	else if (d_e > 50)
		Ka = 19. / 90.;
	else if (d_e > 30)
		Ka = 21. / 90.;
	else if (d_e > 20)
		Ka = 23. / 90.;
	else 
		Ka = 25. / 90.;
	
	if (theta_e > 95 || theta_e < -95)
	{
		theta_e += 180;
		
		if (theta_e > 180) 
			theta_e -= 360;
		if (theta_e > 80)
			theta_e = 80;
		if (theta_e < -80)
			theta_e = -80;
		if (d_e < 5.0 && abs(theta_e) < 40)
			Ka = 0.1;
		vr = (int)(-vc * (1.0 / (1.0 + exp(-3.0 * d_e)) - 0.3) + Ka * theta_e);
		vl = (int)(-vc * (1.0 / (1.0 + exp(-3.0 * d_e)) - 0.3) - Ka * theta_e);
	}
	
	else if (theta_e < 85 && theta_e > -85)
	{
		if (d_e < 5.0 && abs(theta_e) < 40)
			Ka = 0.1;
		vr = (int)( vc * (1.0 / (1.0 + exp(-3.0 * d_e)) - 0.3) + Ka * theta_e);
		vl = (int)( vc * (1.0 / (1.0 + exp(-3.0 * d_e)) - 0.3) - Ka * theta_e);
	}

	else
	{
		vr = (int)(+.17 * theta_e);
		vl = (int)(-.17 * theta_e);
	}

	Velocity(robot, vl, vr);
	if(fabs(robot->pos.x-x)<0.7 && fabs(robot->pos.y-y)<0.7)
//	if(robot->pos .x ==x && robot->pos .y ==y)
Velocity(robot, 0, 0);
}
void Position2( Robot *robot, double x, double y)//70
{
	int desired_angle = 0, theta_e = 0, d_angle = 0, vl, vr, vc = 70;

	double dx, dy, d_e, Ka = 10.0/90.0;
	dx = x - robot->pos.x;
	dy = y - robot->pos.y;

	d_e = sqrt(dx * dx + dy * dy);
	if (dx == 0 && dy == 0)
		desired_angle = 90;
	else
		desired_angle = (int)(180. / PI * atan2((double)(dy), (double)(dx)));// hudu hua 30 du( such as)
	theta_e = desired_angle - (int)robot->rotation;// zhuan de jiao du
	
	while (theta_e > 180) theta_e -= 360;
	while (theta_e < -180) theta_e += 360;

	if (d_e > 100.) 
		Ka = 17. / 90.;
	else if (d_e > 50)
		Ka = 19. / 90.;
	else if (d_e > 30)
		Ka = 21. / 90.;
	else if (d_e > 20)
		Ka = 23. / 90.;
	else 
		Ka = 25. / 90.;
	
	if (theta_e > 95 || theta_e < -95)
	{
		theta_e += 180;
		
		if (theta_e > 180) 
			theta_e -= 360;
		if (theta_e > 80)
			theta_e = 80;
		if (theta_e < -80)
			theta_e = -80;
		if (d_e < 5.0 && abs(theta_e) < 40)
			Ka = 0.1;
		vr = (int)(-vc * (1.0 / (1.0 + exp(-3.0 * d_e)) - 0.3) + Ka * theta_e);
		vl = (int)(-vc * (1.0 / (1.0 + exp(-3.0 * d_e)) - 0.3) - Ka * theta_e);
	}
	
	else if (theta_e < 85 && theta_e > -85)
	{
		if (d_e < 5.0 && abs(theta_e) < 40)
			Ka = 0.1;
		vr = (int)( vc * (1.0 / (1.0 + exp(-3.0 * d_e)) - 0.3) + Ka * theta_e);
		vl = (int)( vc * (1.0 / (1.0 + exp(-3.0 * d_e)) - 0.3) - Ka * theta_e);
	}

	else
	{
		vr = (int)(+.17 * theta_e);
		vl = (int)(-.17 * theta_e);
	}

	Velocity(robot, vl, vr);
	if(fabs(robot->pos.x-x)<1 && fabs(robot->pos.y-y)<1)
//	if(robot->pos .x ==x && robot->pos .y ==y)
Velocity(robot, 0, 0);
}


void PredictBall ( Environment *env )
{
	double dx = env->currentBall.pos.x - env->lastBall.pos.x;
	double dy = env->currentBall.pos.y - env->lastBall.pos.y;
	env->predictedBall.pos.x = env->currentBall.pos.x + dx;
	env->predictedBall.pos.y = env->currentBall.pos.y + dy;

}

void Goalie1 ( Robot *robot, Environment *env )
{
	double velocityLeft = 0, velocityRight = 0;
	
	double Tx = env->goalBounds.right - env->currentBall.pos.x;
	double Ty = env->fieldBounds.top - env->currentBall.pos.y;

	double Ax = env->goalBounds.right - robot->pos.x;
	double Ay = env->fieldBounds.top - robot->pos.y;

	if ( Ay > Ty + 0.9 && Ay > 27 )
	{
		velocityLeft = -100;
		velocityRight = -100;
	}

	if ( Ay > Ty - 0.9 && Ay < 43 )
	{
		velocityLeft = 100;
		velocityRight = 100;
	}

	if ( Ay < 27 )
	{
		velocityLeft = 100;
		velocityRight = 100;
	}

	if ( Ay > 43 )
	{
		velocityLeft = -100;
		velocityRight = -100;
	}

	double Tr = robot->rotation;
	if ( Tr < 0.001 )
		Tr = Tr + 360;
	if ( Tr > 360.001 )
		Tr = Tr - 360;
	if ( Tr > 270.5 )
		velocityRight = velocityRight + fabs ( Tr - 270 );
	else if ( Tr < 269.5 )
		velocityLeft = velocityLeft + fabs ( Tr - 270 );

	robot->velocityLeft = velocityLeft;
	robot->velocityRight = velocityRight;
}



void Attack2 ( Robot *robot, Environment *env )
{
	Vector3D t = env->currentBall.pos;
	double r = robot->rotation;
	if ( r < 0 ) r += 360;
	if ( r > 360 ) r -= 360;
	double vl = 0, vr = 0;

	if ( t.y > env->fieldBounds.top - 2.5 ) t.y = env->fieldBounds.top - 2.5;
	if ( t.y < env->fieldBounds.bottom + 2.5 ) t.y = env->fieldBounds.bottom + 2.5;
	if ( t.x > env->fieldBounds.right - 3 ) t.x = env->fieldBounds.right - 3;
	if ( t.x < env->fieldBounds.left + 3 ) t.x = env->fieldBounds.left + 3;

	double dx = robot->pos.x - t.x;
	double dy = robot->pos.y - t.y;

	double dxAdjusted = dx;
	double angleToPoint = 0;

	if ( fabs ( robot->pos.y - t.y ) > 7 || t.x > robot->pos.x )
		dxAdjusted -= 5;

	if ( dxAdjusted == 0 )
	{
		if ( dy > 0 )
			angleToPoint = 270;
		else
			angleToPoint = 90;
	}
	else if ( dy == 0 )
	{
		if ( dxAdjusted > 0 )
			angleToPoint = 360;
		else
			angleToPoint = 180;
		
	}
	else
		angleToPoint = atan ( fabs ( dy / dx ) ) * 180.0 / PI;

	if ( dxAdjusted > 0 )
	{
		if ( dy > 0 )
			angleToPoint -= 180;
		else if ( dy < 0 )
			angleToPoint = 180 - angleToPoint;
	}
	if ( dxAdjusted < 0 )
	{
		if ( dy > 0 )
			angleToPoint = - angleToPoint;
		else if ( dy < 0 )
			angleToPoint = 90 - angleToPoint;
	}

	if ( angleToPoint < 0 ) angleToPoint = angleToPoint + 360;
	if ( angleToPoint > 360 ) angleToPoint = angleToPoint - 360;
	if ( angleToPoint > 360 ) angleToPoint = angleToPoint - 360;

	double c = r;

	double angleDiff = fabs ( r - angleToPoint );

	if ( angleDiff < 40 )
	{
		vl = 100;
		vr = 100;
		if ( c > angleToPoint )
			vl -= 10;
		if ( c < angleToPoint )
			vr -= 10;
	}
	else
	{
		if ( r > angleToPoint )
		{
			if ( angleDiff > 180 )
				vl += 360 - angleDiff;
			else
				vr += angleDiff;
		}
		if ( r < angleToPoint )
		{
			if ( angleDiff > 180 )
				vr += 360 - angleDiff;
			else
				vl += angleDiff;
		}
	}

	NearBound2 ( robot, vl, vr, env );
}

void NearBound2 ( Robot *robot, double vl, double vr, Environment *env )
{
	//Vector3D t = env->currentBall.pos;

	Vector3D a = robot->pos;
	double r = robot->rotation;

	if ( a.y > env->fieldBounds.top - 15 && r > 45 && r < 130 )
	{
		if ( vl > 0 )
			vl /= 3;
		if ( vr > 0 )
			vr /= 3;
	}

	if ( a.y < env->fieldBounds.bottom + 15 && r < -45 && r > -130 )
	{
		if ( vl > 0 ) vl /= 3;
		if ( vr > 0 ) vr /= 3;
	}

	if ( a.x > env->fieldBounds.right - 10 )
	{
		if ( vl > 0 )
			vl /= 2;
		if ( vr > 0 )
			vr /= 2;
	}

	if ( a.x < env->fieldBounds.left + 10 )
	{
		if ( vl > 0 )
			vl /= 2;
		if ( vr > 0 )
			vr /= 2;
	}

	robot->velocityLeft = vl;
	robot->velocityRight = vr;
}

void Defend ( Robot *robot, Environment *env, double low, double high )
{
	double vl = 0, vr = 0;
	Vector3D z = env->currentBall.pos;

	double Tx = env->goalBounds.right - z.x;
	double Ty = env->fieldBounds.top - z.y;
	Vector3D a = robot->pos;
	a.x = env->goalBounds.right - a.x;
	a.y = env->fieldBounds.top - a.y;

	if ( a.y > Ty + 0.9 && a.y > low )
	{
		vl = -100;
		vr = -100;
	}
	if ( a.y < Ty - 0.9 && a.y < high )
	{
		vl = 100;
		vr = 100;
	}
	if ( a.y < low )
	{
		vl = 100;
		vr = 100;
	}
	if ( a.y > high )
	{
		vl = -100;
		vr = -100;
	}

	double Tr = robot->rotation;

	if ( Tr < 0.001 )
		Tr += 360;
	if ( Tr > 360.001 )
		Tr -= 360;
	if ( Tr > 270.5 )
		vr += fabs ( Tr - 270 );
	else if ( Tr < 269.5 )
		vl += fabs ( Tr - 270 );

	NearBound2 ( robot, vl ,vr, env );
}
// wang shang
void Angle1(Robot *robot,int desired_angle) //  (原地)  旋转
{int theta_e,vl,vr;
double kp=1.2;
theta_e=desired_angle-(int)robot->rotation ;
while(theta_e>180)
theta_e-=360;
while(theta_e<-180)
theta_e+=360;
if(theta_e<-90)
theta_e+=360;
else if(theta_e>90)
theta_e-=360;
vl=(int)(0-kp*theta_e);
vr=(int)(0+kp*theta_e);
Velocity(robot,vl,vr);
}
void AngleOfPosition(Robot *robot,double x, double y)//让机器人 ( 原地 ) 旋转始终对着某一点, 不分前后 
{ 
int vl, vr; 
double dx,dy,d_e,desired_angle,theta_e; 
double Kp=0.8; 
dx=x-robot->pos.x; 
dy=y-robot->pos.y; 
d_e=sqrt(dx*dx+dy*dy); 
if(dx==0 && dy==0)  
{ 
vl=0;
vr=0;
Velocity(robot,vl,vr);
}
else 
desired_angle = (int)(180.0/PI*atan2((double)(dy),(double)(dx))); 
theta_e = desired_angle-(int)robot->rotation; 
while(theta_e >180)  theta_e -= 360; 
while(theta_e <-180) theta_e += 360; 
if (theta_e < -90) theta_e += 180;  
else 
if (theta_e > 90) theta_e -= 180; 
vl = (int)(0 - Kp*theta_e); 
vr = (int)(0 + Kp*theta_e); 
Velocity(robot, vl, vr);  //逆时针旋转 theta_e 度对着某一点

}




  // 自己写的部分
bool ifchao( Robot *robot,double x,double y)//是否朝向某一点 //不完全对，只考虑以下代码所用的，
{double jiao1;
	double jiao2=atan2(fabs(robot->pos .y-y),fabs(robot->pos .x -x));//hudu
if(-90<robot->rotation && robot->rotation<0)//jiaodu    -90<  <0
jiao1=fabs(robot->rotation );
if(robot->rotation <-90)               //  <-90
jiao1=robot->rotation +180;
if(180>robot->rotation && robot->rotation>90)  //     90<  <180
jiao1=180-robot->rotation;  

if(fabs(jiao2*180/PI-jiao1 )<10)//chao xiang
return true;
else
return false;}

double leng(double x1,double x2,double y1,double y2)
{return sqrt((x1-x2)* (x1-x2)  +(y1-y2)*(y1-y2) );
}
double suanr(double tempx, double tempy)//SUAN BAN JIN
{double midx,midy,k,r;// k wei zhong chui xian xie lv
midx=tempx/2;
midy=tempy/2;

k=-1/(tempy/tempx);
r=(-midy)/k+midx;
return r;}



void circle( Robot *robot,double Targetx,double Targety,double SameSpeed)
{
double radius;        double dx,dy;   double tempx; double tempy;
//坐标变换，以小车中心为原点，小车朝向为y轴，求目标点在新坐标下的坐标(temp.x, temp.y)
dx = Targetx - robot->pos.x;
dy = Targety - robot->pos.y;
double hudu=robot->rotation*PI/180;
tempx = dx*sin(hudu) - dy*cos(hudu);
tempy = dx*cos(hudu) + dy*sin(hudu);

radius=suanr(tempx,tempy)*4;
//radius=zhi+4;

//Circle(radius,speed,pLRWheelVelocity);
       	robot->velocityLeft = (radius+4)/radius*SameSpeed;   //radius应为所做的圆运动的圆直径 注意给radius赋值的语句的倍数
		robot->velocityRight=(radius-4)/radius*SameSpeed;
//Position(robot,Targetx,Targety);
}


void shoot(Robot *robot,Environment *env)
{
	// yi xia wei way1,直线方法
	double jiao,x,y,ballx,bally;
 if(env->currentBall .pos.y>46.5)
jiao=atan2(	97-env->currentBall .pos.x,env->currentBall .pos.y-34);
 else
jiao=atan2(	95-env->currentBall .pos.x,env->currentBall .pos.y-34);


 ballx=env->currentBall .pos.x;  bally=env->currentBall .pos.y;

x=tan(jiao)*(robot->pos .y-env->currentBall .pos.y);
	if(fabs(robot->pos .x-85)<0.5 && fabs(robot->pos .y-52.5)<0.5)    //39,23 wei qi shi wei zhi
a=0;b=0;


	Position1(robot,env->currentBall .pos.x-x,robot->pos .y); // yi yang
//Velocity (robot,0,0);
	if(fabs(robot->pos .x-(env->currentBall .pos.x-x))<1 && fabs(robot->pos .y-52.5)<1)  //yi yang 
		a=2;
	if(a==2)
    {
		if(env->opponent [0].pos .y >44.6)//   >44.6
		{
		if( ifchao( robot,env->currentBall .pos.x, env->currentBall .pos.y)   )
	{	if(env->currentBall.pos .x != env->lastBall .pos .x 	)
Velocity(robot,0,0);
	else
	Position(robot,ballx,bally);
	}
	else
		AngleOfPosition(robot,env->currentBall .pos.x, env->currentBall .pos.y);
//	Angle1(robot,int(-jiao));
		}// if env->opponent
	
		else //env->opponent [0].pos .y <44.6
		{Position2(robot,robot->pos .x ,env->currentBall .pos .y );
		if(fabs(robot->pos .y -env->currentBall .pos .y)<2.3 )
			b=1;
		if(b==1)
		{
			if(fabs(robot->rotation)<4)// ifchao( robot,env->currentBall .pos.x, env->currentBall .pos.y)   )
	{	if(env->currentBall.pos .x != env->lastBall .pos .x 	)
Velocity(robot,0,0);
	else
	Position(robot,env->currentBall.pos .x,env->currentBall.pos .y);
	}
		else
	AngleOfPosition(robot,env->currentBall .pos.x, env->currentBall .pos.y);


		}//b==1
	}//else
	}//a==2
		
	

/*Position1(robot,80,51);
if(fabs(robot->pos .x-80)<1 && fabs(robot->pos .y-51)<1)
AngleOfPosition(robot,env->currentBall .pos.x, env->currentBall .pos.y);
if(ifchao(robot,env->currentBall .pos.x, env->currentBall .pos.y))
Position(robot,env->currentBall .pos.x, env->currentBall .pos.y);*/



/*	//way2,以下为半圆方法
		if(robot->pos .x -85<1 && robot->pos .y -52.5<1)
a=0;
double jiao=atan2(	95-env->currentBall .pos.x,env->currentBall .pos.y-34);
//double radius=4*leng(env->currentBall .pos.x,robot->pos .x ,env->currentBall .pos.y,robot->pos .y);
 double SameSpeed=-100;
circle(robot,env->currentBall.pos .x,env->currentBall.pos .y-fabs(env->currentBall.pos.y-robot->pos.y) ,-100);
//robot->velocityLeft = (radius+4)/radius*SameSpeed;
	//	robot->velocityRight=(radius-4)/radius*SameSpeed;
		if((jiao-atan2(env->currentBall .pos.x-robot->pos .x ,robot->pos .y -env->currentBall .pos .y))*180/PI <5
			&& env->opponent [0].pos .y >44)//右半圆,keigai,,  三点合一（进攻机器人，球，目标击球点）//守门员在上方 
		a=2;
		if(a==2)
		{
			
		//Velocity(robot,0,0);
if(ifchao(robot,env->currentBall .pos.x,env->currentBall .pos.y))
{
	if(env->currentBall.pos .x != env->lastBall .pos .x 	)// 同上，机器人不跟球
Velocity(robot,0,0);
	else
	Position(robot,env->currentBall .pos.x,env->currentBall .pos.y);//110 dade 
}//if ifchao
else
		AngleOfPosition(robot,env->currentBall .pos.x,env->currentBall .pos.y);
			}// if a==2
			else  //守门员在下方  env->opponent .pos.y<=44.3  继续做圆运动
			{}
		
	
		




		if( fabs(env->currentBall.pos.y-robot->pos.y )<3 ) //守门员在下方
		a=1;
		//	if(fabs(env->currentBall.pos.x-robot->pos.x)-(leng(env->currentBall.pos.x,robot->pos .x ,robot->pos .x ,env->currentBall.pos.y))<2.3)
			//{	//if( fabs( fabs(env->currentBall.pos.x-robot->pos.x)-radius )<2 ) //防止没到该点却碰巧满足上个if条件
			if(a==1)
			{		
if(ifchao(robot,env->currentBall .pos.x,env->currentBall .pos.y))
{
if(env->currentBall.pos .x != env->lastBall .pos .x 	)
Velocity(robot,0,0);
	else
		Position(robot,env->currentBall .pos.x,env->currentBall .pos.y);
}//if ifchao
else
		AngleOfPosition(robot,env->currentBall .pos.x,env->currentBall .pos.y);
			
//	} // if a==1
		}// if 守门员在下方

	*/

}// main


		



		










