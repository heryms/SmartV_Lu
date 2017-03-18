#pragma once
#include <ObjBase.h>
#include "ogrsf_frmts.h"
#include "gdal_priv.h"
#include "gdalwarper.h"
#include <Windows.h>
#include <vector>
#include <iterator>
#include <cmath>
using namespace std;
//车辆状态结构体
typedef struct CarPosition
{
	double car_X;
	double car_Y;
	double car_Yaw;
}carPosition;
typedef enum CreateMode{
	FROM_TXT=1,
	FROM_SHP=2,
}CreateMode;
interface IMapInterface
{
	//初始化 输出shp文件名、DP参数、局部路径步长、缓冲区半径
	virtual void Init(const char *,double ,int ,float)=0;
	//创建全局地图
	virtual void CreateGlobalMap(const char *,int)=0;
	//得到局部路径
	virtual void GetLocalPath(carPosition *,vector<carPosition *> &)=0;
	//显示局部路径
	virtual void ShowLocalPath(vector<carPosition*> &)=0;
};
IMapInterface * CreateInstance();