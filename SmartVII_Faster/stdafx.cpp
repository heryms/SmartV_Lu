// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://msdn.microsoft.com/officeui��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// SmartVII_Faster.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"



float VehicleWidth = 1.765;
float VehicleLength = 4.285;

/************************************************************************/
/*�����ȫ�ֱ���                                                        */
/************************************************************************/

///************************************************************************/
///* �����ȫ�ֱ���                                                       */
///************************************************************************/

//vector<NLidarNode> g_LidarParam;
HANDLE g_gpsHandle=NULL;
HANDLE g_lidarHandle1=NULL;
HANDLE g_lidarHandle2=NULL;
HANDLE g_lidarHandle3=NULL;
HANDLE g_lidarHandle4=NULL;
//HANDLE g_trcHandle=NULL;
HANDLE g_laneHandle=NULL;
HANDLE g_VeloRcvHandle=NULL;
HANDLE g_canHandle=NULL;
GpsLine   g_curPostion;
BRAKESTATUS g_eBrakeStatus;
LocationStatus g_curLocation;

unsigned short g_TagWheelAngle=547;
unsigned short g_TagSpeed=0;
unsigned short g_TagBrakeStatus;
unsigned short g_CurWheelAngle=547;
unsigned short g_CurSpeed=0;
CONTROLTYPE g_ctlTypes;
int g_Satellite=0; //��¼����״̬ 0��ʾ���Ƚϲ�,1��ʾ���ȽϺ�

//int g_curSpeed;
////unsigned char obstacledata[2048];
//Laser_sick g_Sick;

