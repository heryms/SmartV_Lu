/************************************************************************/
/* �����״����ݴ����࣬�������ĸ�����������ͬ������                                                                     */
/************************************************************************/
#pragma once
#include "serialmanger.h"
#include "LMS291Thread.h"


class CLidarDataPro :
	public	 CSerialManger
{
public:
	CLidarDataPro(void);
	~CLidarDataPro(void);
	BOOL OpenPort();
	void ClosePort();
private:

	void ReadConfigFile();
	//����ص���������ʽ:
	//void OutData(unsigned char *buf, int len) ;
public:
   static void   Laser_DataIn1(unsigned char *buf, int len) ;
   static void   Laser_DataIn2(unsigned char *buf, int len) ;
   static void   Laser_DataIn3(unsigned char *buf, int len) ;
   static void   Laser_DataIn4(unsigned char *buf, int len) ;
public:
	 NLidarNode m_portArray[4];
	CLMS291Thread m_lidarThread[4];
protected:
	void InitPortSetting();
	
//	void ProcommData();
public:
	void CreateThread();	//�����̣߳�suspended״̬
	void StartThread();		//��ʼ�̣߳�����߳�δ�������򴴽���Ȼ������Ϊ����״̬
	void SuspendThread(); //��ͣ�߳�
	void ResumeThread(); //�ָ��߳�
	void StopThread(); //ֹͣ�߳�
 //   static UINT LidarThreadPro(LPVOID lpParam); //�̺߳���
public:
	void CtnsDataOutputStart();	 //��ʼ�����������
	void CtnsDataOutputStop(NLidarNode node);  //ֹͣ�����������

};

