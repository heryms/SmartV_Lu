// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://msdn.microsoft.com/officeui。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// stdafx.cpp : 只包括标准包含文件的源文件
// SmartVII_Faster.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"



float VehicleWidth = 1.765;
float VehicleLength = 4.285;

/************************************************************************/
/*激光的全局变量                                                        */
/************************************************************************/

///************************************************************************/
///* 激光的全局变量                                                       */
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
int g_Satellite=0; //记录卫星状态 0表示精度较差,1表示精度较好

//int g_curSpeed;
////unsigned char obstacledata[2048];
//Laser_sick g_Sick;

