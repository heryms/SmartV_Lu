/************************************************************************/
/* 提供集中需要的坐标转换函数                                                                     */
/************************************************************************/
#pragma once

typedef struct
{
	double  X;
	double  Y;
	double  Angle;
}OriginPt;
class ckCoordinate
{
public:
	ckCoordinate(void);
	~ckCoordinate(void);
    static int LongLat2XY(double L,double B,double &X,double &Y);
    static int XY2LongLat(double X,double Y,double& longitude,double& latitude);
	static int WorldtoMap(OriginPt org,double xIn,double yIn,double &xOut,double &yOut);
	static int MaptoWorld(OriginPt org,double xIn,double yIn,double &xOut,double &yOut);
	//static int MaptoLocal(OriginPt org,OriginPt dst,double&xOut,double &yOut );
};

