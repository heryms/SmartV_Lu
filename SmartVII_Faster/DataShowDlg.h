#pragma once
#include "afxwin.h"


// CDataShowDlg 对话框

class CDataShowDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDataShowDlg)

public:
	CDataShowDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDataShowDlg();
	inline HWND SetWnd(HWND wnd) {return m_hWnd = wnd;}
// 对话框数据
	enum { IDD = IDD_DATASHOWDLG };
private:
	HWND m_hWnd;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	inline BOOL isCancel(){return m_bCancelFlag;}
	inline void SetCancel(BOOL b){m_bCancelFlag = b;}
private:
	CFont m_font;
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnCancel();
private:
	BOOL m_bCancelFlag;
public:
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnBnClickedUp();
	afx_msg void OnBnClickedDown();
	afx_msg void OnBnClickedLeft();
	afx_msg void OnBnClickedRight();
	afx_msg void OnBnClickedSpace();
	afx_msg void OnBnClickedC();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnClose();
	afx_msg void OnBnClickedUnbrake();
};
