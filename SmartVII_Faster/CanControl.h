#pragma once
#include "include/ControlCAN.h"
#pragma comment(lib,"lib/ControlCAN.lib")
/*****************************************
USB_CAN新结构体
****************************************/
typedef struct _tagPORT_STRUCT
{
	unsigned char com_NUM;   //端口号	
	unsigned char Model;           //CAN 类型 1,2
	unsigned int CanBaudRate;       //波特率
	unsigned char SET_ID_TYPE;       //ID排序类型  待研究
	unsigned char FILTER_MODE;        //滤波模式
	unsigned char RXF[4];             //滤波寄存器
	unsigned char RXM[8];        //  屏蔽寄存器

}PORT_STRUCT;

//  Define can packet struct
typedef struct _tagCAN_PACKET
{
	unsigned char IDbuff[4];    //
	unsigned char Databuff[8];
	unsigned char FreamType;   //发送帧类型  0
	unsigned char Bytes;      //有效字节数
}CAN_PACKET;
//typedef enum 
//{
//	REMOTEDOWN=0,
//	REMOTEUP=1,
//	UNREMOTEDOWN=2,
//	UREMOTEUP=3,
//	SOFTDOWN=4,
//	SOFTUP=5,
//	UNSOFTDOWN=6,
//	UNSOFTUP=7
//}BRAKESTATUS;

class CCanControl
{

public:
	CCanControl(void);
	~CCanControl(void);
private:
	int m_cannum;
	int m_devtype;
	DWORD m_devind;
	int m_connect;
public:
	int m_nCarAngle;
	int m_nCarSpeed;
	int m_nEncodeNum;
	int m_nBrakeStatus; //3表示急刹，4表示缓刹，5表示回
	BRAKESTATUS  m_eBrakes;
public:
	//inline int Get
	inline void SetCannum(int num){m_cannum=num;}
	inline void SetDevType(int type){m_devtype=type;}
	inline void SetDevIndex(int index){m_devind=index;}
	inline  int GetConnectStatus(){return m_connect;}
	
	BOOL ConnectCan();
	UINT CloseCan();
	//void ControlStrategy(bool ExitControlThread);
	bool InitCanPort();
	void SendCarSpeed(bool control,unsigned char brake, unsigned short speed);
	void SendCarAngle(bool control,unsigned short angle);
	void SendEncodeEx(int status);//查询编码器状态
	void SendBrakeEx(int status);//控制刹车开度 0~100

	void SendSpeedEx(int status);//控制油门开度 0~255
	void SendEncode(bool require);

	PORT_STRUCT m_CanPort;
	CAN_PACKET  m_rcvMsg[5];
	CAN_PACKET  m_sndMsg[5];
	bool m_RcvFlag;
	bool m_CanStates;
	CString m_strRcvData;

private:
	void ProCanRcv();
	void ProCanSend();
	BOOL		m_bStopThread;
	THREADMODE m_threadMode;
	CWinThread* m_sendHandle;
	CWinThread* m_rcvHandle;
	HANDLE m_hRcvHandle;
public:
	void CreateThread();	//创建线程，suspended状态
	void StartThread();		//开始线程，如果线程未创建，则创建；然后设置为运行状态
	void SuspendThread(); //暂停线程
	void ResumeThread(); //恢复线程
	void StopThread(); //停止线程
	void RequireCarStatus();
	static UINT QueryCarStatus(LPVOID lpParam); //线程函数
	static UINT RcvCarStatus(LPVOID lpParam); //线程函数

};

