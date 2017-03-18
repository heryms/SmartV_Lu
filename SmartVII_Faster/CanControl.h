#pragma once
#include "include/ControlCAN.h"
#pragma comment(lib,"lib/ControlCAN.lib")
/*****************************************
USB_CAN�½ṹ��
****************************************/
typedef struct _tagPORT_STRUCT
{
	unsigned char com_NUM;   //�˿ں�	
	unsigned char Model;           //CAN ���� 1,2
	unsigned int CanBaudRate;       //������
	unsigned char SET_ID_TYPE;       //ID��������  ���о�
	unsigned char FILTER_MODE;        //�˲�ģʽ
	unsigned char RXF[4];             //�˲��Ĵ���
	unsigned char RXM[8];        //  ���μĴ���

}PORT_STRUCT;

//  Define can packet struct
typedef struct _tagCAN_PACKET
{
	unsigned char IDbuff[4];    //
	unsigned char Databuff[8];
	unsigned char FreamType;   //����֡����  0
	unsigned char Bytes;      //��Ч�ֽ���
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
	int m_nBrakeStatus; //3��ʾ��ɲ��4��ʾ��ɲ��5��ʾ��
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
	void SendEncodeEx(int status);//��ѯ������״̬
	void SendBrakeEx(int status);//����ɲ������ 0~100

	void SendSpeedEx(int status);//�������ſ��� 0~255
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
	void CreateThread();	//�����̣߳�suspended״̬
	void StartThread();		//��ʼ�̣߳�����߳�δ�������򴴽���Ȼ������Ϊ����״̬
	void SuspendThread(); //��ͣ�߳�
	void ResumeThread(); //�ָ��߳�
	void StopThread(); //ֹͣ�߳�
	void RequireCarStatus();
	static UINT QueryCarStatus(LPVOID lpParam); //�̺߳���
	static UINT RcvCarStatus(LPVOID lpParam); //�̺߳���

};

