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
//����״̬�ṹ��
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
	//��ʼ�� ���shp�ļ�����DP�������ֲ�·���������������뾶
	virtual void Init(const char *,double ,int ,float)=0;
	//����ȫ�ֵ�ͼ
	virtual void CreateGlobalMap(const char *,int)=0;
	//�õ��ֲ�·��
	virtual void GetLocalPath(carPosition *,vector<carPosition *> &)=0;
	//��ʾ�ֲ�·��
	virtual void ShowLocalPath(vector<carPosition*> &)=0;
};
IMapInterface * CreateInstance();