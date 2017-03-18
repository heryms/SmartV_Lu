// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://msdn.microsoft.com/officeui��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// SmartVII_FasterView.h : CSmartVII_FasterView ��Ľӿ�
//

#pragma once

#include "resource.h"
#include "DataShow.h"
#include "LidarBiaoding.h"
#include "DataFusionCenter.h"
#include "PortConnectDlg.h"
#include "DataShowDlg.h"
#include "ckMapBaseObject.h"
#include "ControlStrategy.h"
//#include "OfflineLidar.h"
class CSmartVII_FasterView : public CFormView
{
protected: // �������л�����
	CSmartVII_FasterView();
	DECLARE_DYNCREATE(CSmartVII_FasterView)

public:
	enum{ IDD = IDD_SMARTVII_FASTER_FORM };

// ����
public:
	CSmartVII_FasterDoc* GetDocument() const;

// ����
public:

// ��д
public:
	//virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CSmartVII_FasterView();
	void SetStatusInfo(char *str);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif



private:
	vector<ckPointS> m_trackShow;
	vector<ckMifPoint>m_trackPoints;
	vector<ckMissionPoint> m_missionPoints;
	CDataShow m_DataShowPro;
	void ReadTrackGps0(CString path);
	void ReadMissionPt(CString path);
	void OneKeyRead();
	void InitialSideStop();
// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	//CDataShowDlg* pDlg;
	int m_nFaster;
	CLidarBiaoding *pBdDlg;
	DataFusionCenter m_DataCenter;
	CControlStrategy m_ctlStrate;
//	COfflineLidar m_OfflineLidar;
	// LCM status
private:
	BOOL m_bmapLoad;
	BOOL m_bmissionLoad;
private:
	BOOL m_bRoadLaneLcm;
	BOOL m_bVelodyneLcm;
	BOOL m_bTrafficLcm;
public:
	afx_msg void OnLaneopen();
	afx_msg void OnTrafficopen();
	afx_msg void OnCanconnect();
	afx_msg void OnGpsconnect();
	afx_msg void OnLidaropen();
	afx_msg void OnDatashow(); //��ʾ����
	afx_msg void OnAllconnect();//һ������������ͬʱ�������豸
	//�ر��豸
public:
	afx_msg void OnClosecan();
	afx_msg void OnClosegps();
	afx_msg void OnLidarclose();
	//�������º���
public:
	//�򿪰�������
	afx_msg void OnUpdateGpsconnect(CCmdUI *pCmdUI);		
	afx_msg void OnUpdateCanconnect(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLidaropen(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAllconnect(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDatashow(CCmdUI *pCmdUI);
	//�رհ�������
	afx_msg void OnUpdateClosecan(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLidarclose(CCmdUI *pCmdUI);

	afx_msg void OnUpdateClosegps(CCmdUI *pCmdUI);

	//��ͼ����
	//�滮����
	afx_msg void OnPlanmapopen();
	afx_msg void OnPlanstart();
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	virtual BOOL DestroyWindow();

public:
	void DrawLidar();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	BOOL GetMapCentor(ckPointS &centor);

private:
	BOOL m_bAllOpen;
	BOOL m_bLidarOpen;
	BOOL m_bCanOpen;
	BOOL m_bGpsOpen;
	BOOL m_bOfflineLidarFlag;
public:

private:
	BOOL m_bDataShowFlag;

	//�Զ�����Ϣ��������������
public:

	afx_msg LRESULT OnSpeedUp(WPARAM w, LPARAM l);
	afx_msg LRESULT OnSpeedDown(WPARAM w, LPARAM l);
	afx_msg LRESULT OnAngleLeft(WPARAM w, LPARAM l);
	afx_msg LRESULT OnAngleRight(WPARAM w, LPARAM l);
	afx_msg LRESULT OnBrake3(WPARAM w, LPARAM l);
	afx_msg LRESULT OnBrake4(WPARAM w, LPARAM l);
	afx_msg LRESULT OnBrakeu(WPARAM w, LPARAM l);
private:
	BOOL m_bDlgCancelFlag;
	BOOL m_bLayers[4];
	
public:
	afx_msg void OnOrtho();
private:
	BOOL m_bOrtho;
	vector<PointShow> m_showTrackPoints;
	vector<ckMissionPoint> m_showMissionPoints;
	ckPointS m_showCentor;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//afx_msg void OnFileopen();
	afx_msg void OnBtncaiji();
	afx_msg void OnBtnopera();
	afx_msg void OnBtnmartpoint();
	afx_msg void OnUpdateBtncaiji(CCmdUI *pCmdUI);
	afx_msg void OnBtnaddmap();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnBtnvelo();
	afx_msg void OnBtnautoctl();
	afx_msg void OnUpdateBtnautoctl(CCmdUI *pCmdUI);
	afx_msg void OnButton2();
	afx_msg void OnUpdateLaneopen(CCmdUI *pCmdUI);
	afx_msg void OnCtlturnlane();
	afx_msg void OnCtluturn();
	afx_msg void OnCtlbrake();
	afx_msg void OnAnaloadmap();
	afx_msg void OnAnaautotest();
	
//	afx_msg void OnAnashowmap();
	//��ϸ����ͼģ��
	ckMapBaseObject m_MapObject;
	afx_msg void OnLastmap();
	afx_msg void OnCreateckmap();
	afx_msg void OnBtnaddmission();
	afx_msg void OnCheckline();
	afx_msg void OnChecklane();
	afx_msg void OnChecktrack();

	afx_msg void OnChecknum();
	afx_msg void OnUpdateCheckline(CCmdUI *pCmdUI);
	afx_msg void OnUpdateChecklane(CCmdUI *pCmdUI);
	afx_msg void OnUpdateChecktrack(CCmdUI *pCmdUI);
	afx_msg void OnUpdateChecknum(CCmdUI *pCmdUI);
	afx_msg void OnSensorpos();
	afx_msg void OnBtnmarkadd();
};

#ifndef _DEBUG  // SmartVII_FasterView.cpp �еĵ��԰汾
inline CSmartVII_FasterDoc* CSmartVII_FasterView::GetDocument() const
   { return reinterpret_cast<CSmartVII_FasterDoc*>(m_pDocument); }
#endif

