/************************************************************************/
/* 激光雷达数据处理类，包含对四个激光器的软同步处理                                                                     */
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
	//定义回调函数的形式:
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
	void CreateThread();	//创建线程，suspended状态
	void StartThread();		//开始线程，如果线程未创建，则创建；然后设置为运行状态
	void SuspendThread(); //暂停线程
	void ResumeThread(); //恢复线程
	void StopThread(); //停止线程
 //   static UINT LidarThreadPro(LPVOID lpParam); //线程函数
public:
	void CtnsDataOutputStart();	 //开始数据连续输出
	void CtnsDataOutputStop(NLidarNode node);  //停止数据连续输出

};

