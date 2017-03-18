#pragma once
#include "afxcmn.h"


// CMarkPointDlg 对话框

class CMarkPointDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMarkPointDlg)

public:
	CMarkPointDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMarkPointDlg();

// 对话框数据
	enum { IDD = IDD_DIALOGMARKPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ctlMarkList;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedMarksave();
	//afx_msg void OnBnClickedMarkquery();
	afx_msg void OnBnClickedMarkquery();
	afx_msg void OnBnClickedMarkdelete();
	afx_msg void OnBnClickedMarkadd();
};
