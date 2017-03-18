/************************************************************************/
/* 构造实时传感类
  提供参照LCM协议的统一通讯格式，并依照格式进行统一的网格生成
  针对各传感数据提供的坐标数据进行时间标记和空间位置计算
  仅提供对传入数据的实时处理，不参与整体通讯
*/
/************************************************************************/
#pragma once

#include "LcmType_LaneHandle.h"
#include "Lcmtype_VeloHandle.h"
typedef struct  
{
	double x;
	double y;
}ckGridPoint;
typedef struct
{
	int width;
	int height;
	int wid_step;
	int height_step;
	ckGridPoint leftdown;
	ckGridPoint righttop;
	vector<unsigned char> grid_data;
}ckSensorGrid;
class CPlanGrid
{
private:
	ckSensorGrid m_localGrid;
	velodynegrid_t m_veloData;
	LaneInfo_t  m_roadLane;
public:
	CPlanGrid(void);
	~CPlanGrid(void);
    
//数据构成
	BOOL ReadConfigFile();
	BOOL CreateSensorGrid();
	BOOL UpdateSensorGrid();

};

