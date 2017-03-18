#pragma once

#include "CanControl.h"

// CControlDlg �Ի���

class CControlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CControlDlg)

public:
	CControlDlg(CWnd* pParent = NULL);   // ��׼���캯��
//	CControlDlg(CCanControl can,CWnd* pParent = NULL);
	virtual ~CControlDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_CONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CCanControl m_canCtl;//CAN��ͨѶģ��
	bool m_Ctlstatus;
	GpsLine m_crossPoint;   //·�ڵ�λ��
	float  m_fCrossDis;     //��ǰλ�õ�·�ھ���
	int  m_nLightType;           //1��� 0green          
	short   m_nCarSpeed;     //���������ٶ�
	float  m_fBrakeDis;         //����ɲ������
	float  m_fDecelateDis;      //�������پ���
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
