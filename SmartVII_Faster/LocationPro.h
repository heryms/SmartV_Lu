/************************************************************************/
/* 集成Novatel IGM系列常用指令和卫星信号情况                                                                     */
/************************************************************************/
#pragma once
#include "serialmanger.h"
#define  _RESPONSE_HEADER_LENGTH_ 28
#define  _INSPVA_MSG_LENGTH   88

////惯导状态信息
//enum INSSTA
//{
//	INS_INACTIVE=0,INS_ALIGNING=1,INS_HIGH_VARIANCE=2,INS_SOLUTION_GOOD=3,
//	INS_SOLUTION_FREE=4,INS_ALIGNMENT_COMPLETE=5,
//	DETERMINING_ORIENTATION=6,WAITING_INITIALPOS=7,
//};
//struct headerMSG
//{
//	char   sync1;
//	char   sync2;
//	char   sync3;
//
//
//	UCHAR  Msg_Len;
//	USHORT Msg_ID;
//	USHORT Week_Num;
//	ULONG  Mill_Sec;
//};
//
//struct INSPVA
//{
//	double Second;
//	double Lat;
//	double Lon;
//	double Hgt;
//	double NVelocity;
//	double EVelocity;
//	double UpVelocity;
//	double Roll;
//	double Pitch;
//	double Azimuth;
//};
//
//struct INSPVAX
//{
//	double Lat;
//	double Lon;
//	double Hgt;
//	float Undulation;
//	double NVelocity;
//	double EVelocity;
//	double UpVelocity;
//	double Roll;
//	double Pitch;
//	double Azimuth;
//	float delta_Lat;
//	float delta_Lon;
//	float delta_hgt;
//	float delta_NVelocity;
//	float delta_EVelocity;
//	float delta_UpVelocaity;
//	float delta_Roll;
//	float delta_Pitch;
//	float delta_Azimuth;
//
//};
//typedef struct tagGnussLocation
//{
//	LONGLONG time;
//    double gnX;
//	double gnY;
//
//};
//typedef struct tagGPSConfig
//{
//	int port;
//	LONGLONG DataLen;
//	bool status;
//
//}GPSConfig;
class CLocationPro :
	public CSerialManger
{
public:
	CLocationPro(void);
	~CLocationPro(void);

	BOOL OpenPort();
	void ClosePort();
	BOOL GetPortStatus();
	BOOL m_bRecFlag;
private:
	GPSConfig m_gpsConfig;
	void ReadConfigFile();
protected:
	void InitPortSetting();

	void  ProcComData();
	inline BOOL IsHead(const unsigned char *const buf, int iPos);
	 inline BOOL IsCheckSumOK(const unsigned char *const buf, int iPos);
	//inline BOOL IsCheckSumOK(unsigned long ulCount, unsigned char *ucBuffer);
  //  void _stdcall DataRcv(int port);
private:
	//FILE* m_pGpsFile;
public:
	//inline void SetGpsRecord(FILE *fp){m_pGpsFile=fp;}
public:
	INSPVA m_insPva;
	headerMSG m_msgHeader;
	INSSTA m_insStatus; //卫星状态
	UINT  m_nWeek;
	void CreateThread();	//创建线程，suspended状态
	void StartThread();		//开始线程，如果线程未创建，则创建；然后设置为运行状态
	void SuspendThread(); //暂停线程
	void ResumeThread(); //恢复线程
	void StopThread(); //停止线程
	void StartDataOutput();
	void StopDataOutput();
	void AlignIMU();
	void ConnectCORS();
	
    static UINT GPSThreadPro(LPVOID lpParam); //线程函数
};

