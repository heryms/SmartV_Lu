#pragma once

#include "DataShow.h"
#include "resource.h"
#include "PathGenerate.h"
// ShowPathPlanDlg �Ի���
class CShowPathPlanDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CShowPathPlanDlg)

public:
	CShowPathPlanDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CShowPathPlanDlg();

// �Ի�������
	enum { IDD = IDD_SHOW_PATHPLAN };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()
	//��Ϣ
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:

public:

	CDataShow m_DataShowPro;
private:
	void Draw_NetPoint(float radious ,RoadNet netpoint);

	double Draw_x;
	double Draw_y;
public:
	RoadPoint initialPoint; //��ʼλ��
	RoadPoint currentLocation; //��ǰλ��
	RoadPoint m_ultiPath[120];//�滮·��
	DWORD m_ultiLength;//��������·���ĳ��ȣ������ֽ�������·���ϵ�ĸ������ֽ���Ϊsizeof(RoadPoint)*m_ultiLength)//
	RoadPoint *m_RoadDataBuffer;
	RoadNet *m_RoadNetBuffer;
	BOOL IsWindowOpen; //�жϴ����Ƿ��
	int targetPointNum;
	BOOL IsWindowCreate;//�жϴ����Ƿ񴴽�
	BOOL bNetFlag;
	int m_nNetLength;
	int flag1;

	int m_roadDataLength;
	RoadPoint targetPoint;//·���滮Ŀ���

	void GussiantoCar(double Gx,double Gy,double *cx,double *cy);

	//////////////////////////////////////////////////////////////////////////////////

	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};
