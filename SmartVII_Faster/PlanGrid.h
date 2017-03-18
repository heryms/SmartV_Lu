/************************************************************************/
/* ����ʵʱ������
  �ṩ����LCMЭ���ͳһͨѶ��ʽ�������ո�ʽ����ͳһ����������
  ��Ը����������ṩ���������ݽ���ʱ���ǺͿռ�λ�ü���
  ���ṩ�Դ������ݵ�ʵʱ��������������ͨѶ
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
    
//���ݹ���
	BOOL ReadConfigFile();
	BOOL CreateSensorGrid();
	BOOL UpdateSensorGrid();

};

