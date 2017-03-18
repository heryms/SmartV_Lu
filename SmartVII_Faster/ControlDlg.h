#pragma once

#include "CanControl.h"

// CControlDlg 对话框

class CControlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CControlDlg)

public:
	CControlDlg(CWnd* pParent = NULL);   // 标准构造函数
//	CControlDlg(CCanControl can,CWnd* pParent = NULL);
	virtual ~CControlDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_CONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CCanControl m_canCtl;//CAN卡通讯模块
	bool m_Ctlstatus;
	GpsLine m_crossPoint;   //路口点位置
	float  m_fCrossDis;     //当前位置到路口距离
	int  m_nLightType;           //1红灯 0green          
	short   m_nCarSpeed;     //汽车启动速度
	float  m_fBrakeDis;         //汽车刹车距离
	float  m_fDecelateDis;      //汽车减速距离
	BOOL m_bStopThread;
	CARSTATUS m_eCarStatus;
private:
	void StopThread();
public:
	UINT InitCanInfo(CCanControl can);
	BOOL LoadCrossScene();
	afx_msg void OnBnClickedBtnseccross();
	afx_msg void OnBnClickedBtnsecrecord();
	afx_msg void OnBnClickedBtnsecstart();
	void ProCrossTest();
//	virtual void PreInitDialog();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnseclight();
	double m_fLongtitude;
	double m_fLatituede;
};
