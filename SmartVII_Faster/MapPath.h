#pragma once
#include "Interface.h"
#include <windows.h>

//ԭʼ���ݵ�
class PT{
public:
	double pointX;
	double pointY;
	//�õ��м�㵽��β�����ߵľ���	
	double getVLength(vector<PT*>::iterator fPoint,vector<PT*>::iterator sPoint);
};	
class MapCreater:public IMapInterface{
private:
	vector<PT*>  data;
	vector<PT*>  outData;
	//������˹ ��ֵ
	double D;
	//����ļ���
	const char * shpName;
	//����
	int stepLength;
	//�������뾶	��λ����λ��
	double bufferRadius;
	//������˹
	vector<vector<PT*>::iterator> ptStack1;
	vector<vector<PT*>::iterator> ptStack2;
	vector<PT*>::iterator m_it1;
public:
	//����ȫ�ֵ�ͼ
	virtual void Init(const char *shpName,double D,int stepLength,float bufferRadius);
	virtual void GetLocalPath(carPosition *curP,vector<carPosition *> & dstP);
	virtual void CreateGlobalMap(const char *ptFile,int mode);
	virtual void ShowLocalPath(vector<carPosition*> &pathPoint);
	void ReadPoints(const char* fileName);
	void DPLine(vector<PT*>::iterator first,vector<PT*>::iterator second);
	void CreatShapefile();
	//�����ֲ�·��
	void ReadShapefile();
	int LocatePoint(OGRPoint * poPoint,OGRLineString * poLineString);
	BOOL getNextPoint(OGRPoint * currentPT,OGRLineString * poLineString,double stepLength,OGRPoint * nextPT);
	void CreatShapePoint(vector<carPosition *> & points);
	~MapCreater();
};
