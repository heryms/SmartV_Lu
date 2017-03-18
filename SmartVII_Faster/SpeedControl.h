#pragma once
#include "CanControl.h"

// CSpeedControl 

class CSpeedControl 
{
	DECLARE_DYNAMIC(CSpeedControl)

public:
	CSpeedControl();   // 标准构造函数
   ~CSpeedControl();


public:
	
	double wantedspeed;
	void shan_speedcontrol(double WantedSpeed,double CurrentSpeed,double CurrentSpeedError,double CurrentAcceleration);
	void send_speedcontrol();
	CCanControl m_canCtl;//CAN卡通讯模块
	bool tui_flag;
	double pre_speed;
	double acc_v;
	double dif_v;
	int BreakTime;
	int ThrottleControl;
	int con_flag;
	double speederror;
	void ywSpeedControl(int tag_speed);
	bool ting;
};
