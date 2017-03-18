/************************************************************************/
/* �������еײ����ݲɼ����֣�ͳһ��GPSʱ��ΪУ׼������ͨ����ʱ������еײ�������ͳһ��ǣ���������ʶ���㷨�Դ�Ϊ����
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
//����#include "Location_t.hpp"
#define  _max_datapro_length_ 51200

//�滮���ֱ���

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

	//������������
private:


	BOOL m_bRoadLane;
public:
	CCanControl  m_dataCan;
	HANDLE  m_arrHandle[6];  //�ź���ͬ������

public:
	inline BOOL GetRoadLaneStatus(){return m_bRoadLane;}
	inline BOOL SetRoadLaneStatus(BOOL status){m_bRoadLane=status;}
	BOOL StartRoadLaneLcm();
	BOOL StartVelodyneLcm();
	BOOL InitialLocationLcm();
	//void static  velodyne_message( unsigned char *msg);
public:
	//Ԥ��������������
	unsigned char *m_pSensorBuffer;
    //ָ��Э���ͬ�������ļ�
	unsigned char m_chDataBuffer[_max_datapro_length_];
	//���ݲɼ�����
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
	//��¼GPS����
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
	static UINT SensorFusionPro(LPVOID lpParam); //�̺߳���
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
	void CreateThread();	//�����̣߳�suspended״̬
	void StartThread();		//��ʼ�̣߳�����߳�δ�������򴴽���Ȼ������Ϊ����״̬
	void SuspendThread(); //��ͣ�߳�
	void ResumeThread(); //�ָ��߳�
	void StopThread(); //ֹͣ�߳�
	void StopGpsPro();
	//�Զ����Ʋ���
private:
	CONTROLTYPE m_ectlType;//��ǿ���ģʽ �����ֶ����Զ�����������ģʽ
	int m_brakeStatus;
	BOOL m_bpstMapStatus;
public:
	inline void SetControlModel(CONTROLTYPE e) { m_ectlType=e;}
	inline CONTROLTYPE GetControlType(){return m_ectlType;}
	//Pursuit ���ȿ��Ʋ���
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
   //�滮����
public:
    CShowPathPlanDlg *m_showPath;
//	CShowPathPlanDlg *m_pMapWindow; //���Ե�ͼ��
	CPathGenerate m_dataPath;	
	CPursuitTset m_dataPursuitPath;
	udpReceiveMsg recvmsg;  

private: //lcmͨ�ų���
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
	//�Ľ��洿׷��
	bool FindPursuitPoint(vector<GpsLine> line,GpsLine curPoint,GpsLine& TargetPoint,float Speed);
	void StartPathPlan();
	void ShowPlanPath();
//	BOOL ShowMapWindow();
	static void PlanOutDataFinalpath(RoadPoint *pData,int len);
	static void PlanOutDataGPSpath(RoadPoint *pData,int len);
	static void PlanOutDataPoints(RoadPoint *pData,int len);
	static void  OutData_obstacle(vector<unsigned char> grid);
//��������������
	void DrawObjectGrid();
	//���㳵����
	UINT UpdateRoadLane();
public:
	CRoadLaneData m_laneCentor;
//���滮·��
	void DrawLocalPath();
	//����Ԥ���
	void CalTargetPoint(double &X,double& Y,double pursuit);
	void StartVelodyneListen();
	void ActiveLidarPro();
	void DestoryLidarPro();
	void DestoryGpsPro();
	void DestoryCanPro();
	BOOL ConnectAllSensor();

};

