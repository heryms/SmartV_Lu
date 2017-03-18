#pragma once
#include "include/glut.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "include/ippcore.h"
#include "include/ippdefs.h"
#include "include/ippm.h"
#include "fl/Headers.h"
//#include"lcm-cpp.hpp"
/*#include "lcm/LaneInfo_t.hpp"

#include "lcm/velodynegrid_t.hpp"*/
//#include "Location_t.hpp"
//#include "Location_t.hpp"
//#include "Traffic_t.hpp"
#pragma comment(lib,"lib/lcm.lib")
#pragma comment(lib,"lib/getopt.lib")
//opencv path
#pragma comment(lib,"lib/opencv_core231d.lib")
#pragma comment(lib,"lib/opencv_core231.lib")
#pragma comment(lib,"lib/opencv_highgui231d.lib")
#pragma comment(lib,"lib/opencv_highgui231.lib")
#pragma comment(lib,"lib/opencv_imgproc231d.lib")
#pragma comment(lib,"lib/opencv_imgproc231.lib")


#pragma comment(lib,"lib/ippcore_t.lib")//ippcore
#pragma comment(lib,"lib/ippcorel.lib")

#pragma comment(lib,"lib/ippmemerged.lib")//ippm
#pragma comment(lib,"lib/ippmmerged.lib")
#pragma comment(lib,"lib/ippmmerged_t.lib")

#pragma comment(lib,"lib/libguide.lib")
#pragma comment(lib,"lib/fuzzylited.lib")
#define WM_SPEEDUP (WM_USER+100)  //
#define WM_SPEEDDOWN (WM_USER+101)
#define WM_ANGLELEFT (WM_USER+102)
#define WM_ANGLERIGHT (WM_USER+103)
#define WM_BRAKE3 (WM_USER+104)  // 急刹车 刹车开度为0
#define WM_BRAKE4 (WM_USER+105)  //换刹车 刹车开度为20
#define WM_DISABLE_BRAKE (WM_USER+106) //送刹车模式 开度为100


//typedef ckVelodyne::velodynegrid_t VelodyneLcm;
//typedef ckVelodyne::VelodynegridRcv VelodyneLcmRcv;
//typedef ckLocation::Location_t LocationLcm;
//typedef cxLanedetect::LaneInfo_t RoadLaneLcm;
//typedef cxLanedetect::LaneInfo_t RoadLaneLcmRcv;
using namespace std;
double GetFrequency(void)
{
	LARGE_INTEGER iLarge;
	double dbFreq;
	QueryPerformanceFrequency( &iLarge ); 
	dbFreq = (double) iLarge.QuadPart; 
	return dbFreq;
}

double GetTimePoint(void)
{
	LARGE_INTEGER iLarge;
	double point;
	QueryPerformanceCounter( &iLarge ); 
	point = (double) iLarge.QuadPart;
	return point;
}

int GetTime(double b,double e)
{
	double f;
	int nms;
	f = GetFrequency();
	nms =  (int) ((e - b) * 1000.0 / f);
	//CCanControl temp;
	//temp
	return nms;
}

enum BAUD_RATE {BR9600, BR19200, BR38400, BR500K,};

typedef struct tagCOMMDATA
{
	int     Port;
	int     BaudRate,Parity,ByteSize,StopBits;
	int	 ibaudrate,iparity,ibytesize,istopbits;

	BOOL     Hw;		// RTS/CTS hardware flow control 
	BOOL 	Sw;		// XON/XOFF software flow control 
	BOOL    Dtr,Rts;
}COMMDATA,*LPCOMMDATA;

//线程工作模式
enum THREADMODE 
{
	MODENULL, //未创建线程
	MODERUN, //线程在运行
	MODESUSPEND, //线程暂停
	MODESTOP, //线程停止
};

//惯导状态信息
enum INSSTA
{
	INS_INACTIVE=0,INS_ALIGNING=1,INS_HIGH_VARIANCE=2,INS_SOLUTION_GOOD=3,
	INS_SOLUTION_FREE=4,INS_ALIGNMENT_COMPLETE=5,
	DETERMINING_ORIENTATION=6,WAITING_INITIALPOS=7,NOSINGAL=110,
};
enum CONTROLTYPE
{
	MANUAL=0,SELFDRIVING=1,TRACKMODEL=2,UNKNOW=3,
};
struct headerMSG
{
	char   sync1;
	char   sync2;
	char   sync3;
	UCHAR  Msg_Len;
	USHORT Msg_ID;
	USHORT Week_Num;
	ULONG  Mill_Sec;
};

struct INSPVA
{
	double Second;
	double Lat;
	double Lon;
	double Hgt;
	double NVelocity;
	double EVelocity;
	double UpVelocity;
	double Roll;
	double Pitch;
	double Azimuth;
};

struct INSPVAX
{
	double Lat;
	double Lon;
	double Hgt;
	float Undulation;
	double NVelocity;
	double EVelocity;
	double UpVelocity;
	double Roll;
	double Pitch;
	double Azimuth;
	float delta_Lat;
	float delta_Lon;
	float delta_hgt;
	float delta_NVelocity;
	float delta_EVelocity;
	float delta_UpVelocaity;
	float delta_Roll;
	float delta_Pitch;
	float delta_Azimuth;

}; struct tagGnussLocation
{
	LONGLONG time;
	double gnX;
	double gnY;

};
struct GpsLine
{
	double Gnx;
	double Gny;
	double azimuth;
};
typedef struct 
{
	float x;
	float y;
	float z;
	int type;

}PointShow;
typedef struct tagGPSConfig
{
	int port;
	LONGLONG DataLen;
	bool status;

}GPSConfig;

typedef enum 
{
	REMOTEDOWN=0,
	REMOTEUP=1,
	UNREMOTEDOWN=2,
	UNREMOTEUP=3,
	SOFTDOWN=4,
	SOFTUP=5,
	UNSOFTDOWN=6,
	UNSOFTUP=7,

	SCEBRAKE=9,
}BRAKESTATUS;
typedef enum
{
	COMMANDRCV=0,
	COMMANDDEED=1,//标记命令已经执行
}COMMANDINFO;

//激光部分结构体
enum ANGLE_MODE {A100100, A100050, A100025, A180050,A180100,};
typedef struct tagLidarNode
{
	int port;
	bool status;
	ANGLE_MODE angle;
	int dataLen;
	bool dataFlag;
	unsigned char okBuf[1024];
	double  mat[10];
	//	float  points[4096];
}NLidarNode;

//激光结构体
struct sPoint
{
	float x;
	float y;
	float z;
	float r;
	float tht;
	float fcr;
	float fc;
	int  objectID;
};
typedef struct tagFusionPoint
{
	float x;
	float y;
	float z;
}FusionPoint;
typedef struct tagLocation
{
	UINT week;
	double second;
	double gauX;
	double gauY;
	double nVelocity;
	double eVelocity;
	double upVelocity;
	double roll;
	double pitch;
	double azimuth;
}LocationStatus;

typedef struct  
{
	float r;
	float g;
	float b;
	float size;
	//float width;
}ckSample;
typedef struct  
{
	double x;
	double y;
}ckPointS;
typedef struct  
{
	vector<ckPointS> ptData;
	short type;
	//ckSample sample;
}ckPloyLineS;

typedef struct 
{
	int num;
	double x;
	double y;
	double hgt;
	int property0;
	int property1;
}ckMissionPoint;
typedef enum
{
	CKSTART=0,
	CKCROSSIN=1,
	CKCROSSOUT=2,
	CKNORMAL=3,
	CKSTOPIN=4,
	CKSOPSECT=5,
	CKSTOPPOS=6,
	CKEND=7,
}EMISSIONTYPE;
typedef enum
{
	RUN=0,
	STOP=1,
	DECE=3,
}CARSTATUS;

