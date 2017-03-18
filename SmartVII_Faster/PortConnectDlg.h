#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CPortConnectDlg �Ի���

class CPortConnectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPortConnectDlg)

public:
	CPortConnectDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPortConnectDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_PORTOPEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
