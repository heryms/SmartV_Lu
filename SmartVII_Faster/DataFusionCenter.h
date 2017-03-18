/************************************************************************/
/* 集成所有底层数据采集部分，统一于GPS时间为校准基础，通过此时间对所有底层数据做统一标记，个传感器识别算法以此为基础
 */
/************************************************************************/                                                            
#pragma once
#include "LidarDataPro.h"
#include "LocationPro.h"
#include "CanControl.h"
#include "PathGenerate.h"
#include "PursuitTset.h"
#include "ShowPathPlanDlg.h"
#include "Lcmtype_VeloHandle.h"
#include "LcmType_LocationHandle.h"
#include "LcmType_LaneHandle.h"
#include "RoadLaneData.h"

//#include "lcm/lcm.h"
//#include "udpReceive.h"
//、、#include "Location_t.hpp"
#define  _max_datapro_length_ 51200

//规划部分变量

//static UCHAR obstacle[2048];

typedef struct  
{
	double Gnx;
	double Gny;
	int type;
}MapPoint;
typedef struct
{
	double x;
	double y; 
}ckVector;
class DataFusionCenter
{
public:
	DataFusionCenter(void);
	~DataFusionCenter(void);

	//传感器构造类
private:


	BOOL m_bRoadLane;
public:
	CCanControl  m_dataCan;
	HANDLE  m_arrHandle[6];  //信号量同步控制

public:
	inline BOOL GetRoadLaneStatus(){return m_bRoadLane;}
	inline BOOL SetRoadLaneStatus(BOOL status){m_bRoadLane=status;}
	BOOL StartRoadLaneLcm();
	BOOL StartVelodyneLcm();
	BOOL InitialLocationLcm();
	//void static  velodyne_message( unsigned char *msg);
public:
	//预备缓冲数据区间
	unsigned char *m_pSensorBuffer;
    //指定协议的同步数据文件
	unsigned char m_chDataBuffer[_max_datapro_length_];
	//数据采集部分
private:
	CString  m_strGpsFile;
	FILE * m_pLidarFile;
    BOOL m_bRecordGps;
	
public:
	vector<GpsLine> m_sideWay;
	inline void SetGpsFile(CString str){m_strGpsFile=str;}
	inline CString GetGpsFile(){return m_strGpsFile; }
	inline void EnableRecord(BOOL flag){m_bRecordGps=flag;}
	inline BOOL GetRecordStatus(){return m_bRecordGps;}
	void static CartoGuassian(double &ptX,double &ptY,GpsLine pos);
	//记录GPS数据
	void WriteGpsTrack(GpsLine pt);
	void CreateMapIndex(FILE *fp);
//	inline NLidarNode GetLidarNode(int i){return m_dataLidar.m_portArray[i];}
public:
	inline int GetCurrentSpeed() {return m_dataCan.m_nCarSpeed;}
	inline int GetCurrentAngle() {return m_dataCan.m_nCarAngle;}
	inline int GetBrakeStatus() {return m_dataCan.m_nBrakeStatus;}
	inline double  GetCurrentGauX() {return m_curLocation.gauX;}
	inline double GetCurrentGauY() {return m_curLocation.gauY;}
	inline double GetCurrentAmizth(){return m_curLocation.azimuth/180.0*3.14159;}
	inline void CarSpeedControlled(unsigned char brake, unsigned short speed)
	{m_dataCan.SendCarSpeed(1,brake,speed);}
	inline void CarAngleControlled(unsigned short angle)
	{m_dataCan.SendCarAngle(1,angle);}
	void GetSensorData();
	void ResetSensor();
	static UINT SensorFusionPro(LPVOID lpParam); //线程函数
	void ProFusionData();
	BOOL GetGpsPortStatus();
	void SendLcmPos(GpsLine pos);
	inline void SetWnd(HWND wnd){m_hWnd=wnd;}
	
private:
	HWND m_hWnd;

	BOOL		m_bStopThread;
	CWinThread	*m_pThreadHandle;
	THREADMODE  m_threadMode;

public:
	bool  m_bGpsOn;
	bool m_bLidarOn;
	bool m_bCanOn;
public:
	int m_nActivateStep;
	LocationStatus m_curLocation;
    // float m_arrLidar[4][4096];
	//BOOL LoadSensorData();
    BOOL ActiveCanPro();
	BOOL ActivateDataPro();
	BOOL StartFusionThread();
	BOOL ActivateGPSPro();
	void CreateThread();	//创建线程，suspended状态
	void StartThread();		//开始线程，如果线程未创建，则创建；然后设置为运行状态
	void SuspendThread(); //暂停线程
	void ResumeThread(); //恢复线程
	void StopThread(); //停止线程
	void StopGpsPro();
	//自动控制部分
private:
	CONTROLTYPE m_ectlType;//标记控制模式 包括手动、自动、测试三种模式
	int m_brakeStatus;
	BOOL m_bpstMapStatus;
public:
	inline void SetControlModel(CONTROLTYPE e) { m_ectlType=e;}
	inline CONTROLTYPE GetControlType(){return m_ectlType;}
	//Pursuit 精度控制测试
   RoadPoint *m_RoadBuffer;
   GpsLine *m_RoadLaneBuffer;
  vector<GpsLine> m_calLine;
   int m_nRoadLength;
   int m_nRoadLaneLength;
   GpsLine m_targetPoint;
   void LoadMap(CString path);
   void LoadTestTrack(CString path);
   void CalTargetPoint(float radious);
private:  
   GpsLine Newton_divide(GpsLine p0,GpsLine p1,float radious); 
   double calulate_radios_err(double x,double y ,double radious);
   bool ywFindPursuitPoint(vector<GpsLine> line,GpsLine curPoint,GpsLine &TargetPoint,float Speed);
   void ywNew_ClothoidTrack(GpsLine car_curpoint,GpsLine targetpoint);
   GpsLine GautoMycar_oid(GpsLine car_curpoint,GpsLine daizhuan_point);
   //规划部分
public:
    CShowPathPlanDlg *m_showPath;
//	CShowPathPlanDlg *m_pMapWindow; //测试地图用
	CPathGenerate m_dataPath;	
	CPursuitTset m_dataPursuitPath;
	udpReceiveMsg recvmsg;  

private: //lcm通信程序
	CLocationPro m_dataLocation;
	LcmLaneHandler m_lancLcmHandle;
	LcmVeloHandler m_veloLcmHandle;
	LcmLocationHandler m_gpsLcmHandle;
private:
    RoadPoint m_xy[CLOTHOIDPOINTNUM];
	GpsLine fir_targetpoint;

	int m_nIdexPos;
public:
	vector<GpsLine> m_trackLayer;
	vector<GpsLine> m_laneBuffer;
	vector<PointShow> m_drawlaneBuffer;
public:
     void ShowCtlSceneDlg();
public:
	void ReadIpConfig();
	//改进版纯追随
	bool FindPursuitPoint(vector<GpsLine> line,GpsLine curPoint,GpsLine& TargetPoint,float Speed);
	void StartPathPlan();
	void ShowPlanPath();
//	BOOL ShowMapWindow();
	static void PlanOutDataFinalpath(RoadPoint *pData,int len);
	static void PlanOutDataGPSpath(RoadPoint *pData,int len);
	static void PlanOutDataPoints(RoadPoint *pData,int len);
	static void  OutData_obstacle(vector<unsigned char> grid);
//画激光数据网格
	void DrawObjectGrid();
	//计算车道线
	UINT UpdateRoadLane();
public:
	CRoadLaneData m_laneCentor;
//画规划路径
	void DrawLocalPath();
	//计算预瞄点
	void CalTargetPoint(double &X,double& Y,double pursuit);
	void StartVelodyneListen();
	void ActiveLidarPro();
	void DestoryLidarPro();
	void DestoryGpsPro();
	void DestoryCanPro();
	BOOL ConnectAllSensor();

};

