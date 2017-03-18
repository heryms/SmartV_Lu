#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CPortConnectDlg 对话框

class CPortConnectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPortConnectDlg)

public:
	CPortConnectDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPortConnectDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_PORTOPEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_ctlProgress;
	CStatic m_ctlTxt;
	BOOL m_flagClose;
	int* m_nPos;
	UINT static ProgressThread(LPVOID param);
	void ProgressFunc();
	virtual INT_PTR DoModal();
	afx_msg void OnClose();
};
