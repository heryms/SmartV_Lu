#pragma once

#include "DataShow.h"
#include "resource.h"
#include "PathGenerate.h"
// ShowPathPlanDlg 对话框
class CShowPathPlanDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CShowPathPlanDlg)

public:
	CShowPathPlanDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CShowPathPlanDlg();

// 对话框数据
	enum { IDD = IDD_SHOW_PATHPLAN };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
	//消息
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
	RoadPoint initialPoint; //初始位置
	RoadPoint currentLocation; //当前位置
	RoadPoint m_ultiPath[120];//规划路径
	DWORD m_ultiLength;//最终生成路径的长度（不是字节数，是路径上点的个数，字节数为sizeof(RoadPoint)*m_ultiLength)//
	RoadPoint *m_RoadDataBuffer;
	RoadNet *m_RoadNetBuffer;
	BOOL IsWindowOpen; //判断窗口是否打开
	int targetPointNum;
	BOOL IsWindowCreate;//判断窗口是否创建
	BOOL bNetFlag;
	int m_nNetLength;
	int flag1;

	int m_roadDataLength;
	RoadPoint targetPoint;//路径规划目标点

	void GussiantoCar(double Gx,double Gy,double *cx,double *cy);

	//////////////////////////////////////////////////////////////////////////////////

	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};
