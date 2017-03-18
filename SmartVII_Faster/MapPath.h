#pragma once
#include "Interface.h"
#include <windows.h>

//原始数据点
class PT{
public:
	double pointX;
	double pointY;
	//得到中间点到首尾点连线的距离	
	double getVLength(vector<PT*>::iterator fPoint,vector<PT*>::iterator sPoint);
};	
class MapCreater:public IMapInterface{
private:
	vector<PT*>  data;
	vector<PT*>  outData;
	//道格拉斯 阈值
	double D;
	//输出文件名
	const char * shpName;
	//步长
	int stepLength;
	//缓冲区半径	定位车辆位置
	double bufferRadius;
	//道格拉斯
	vector<vector<PT*>::iterator> ptStack1;
	vector<vector<PT*>::iterator> ptStack2;
	vector<PT*>::iterator m_it1;
public:
	//创建全局地图
	virtual void Init(const char *shpName,double D,int stepLength,float bufferRadius);
	virtual void GetLocalPath(carPosition *curP,vector<carPosition *> & dstP);
	virtual void CreateGlobalMap(const char *ptFile,int mode);
	virtual void ShowLocalPath(vector<carPosition*> &pathPoint);
	void ReadPoints(const char* fileName);
	void DPLine(vector<PT*>::iterator first,vector<PT*>::iterator second);
	void CreatShapefile();
	//创建局部路径
	void ReadShapefile();
	int LocatePoint(OGRPoint * poPoint,OGRLineString * poLineString);
	BOOL getNextPoint(OGRPoint * currentPT,OGRLineString * poLineString,double stepLength,OGRPoint * nextPT);
	void CreatShapePoint(vector<carPosition *> & points);
	~MapCreater();
};
