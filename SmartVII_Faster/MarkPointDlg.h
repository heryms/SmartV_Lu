#pragma once
#include "afxcmn.h"


// CMarkPointDlg �Ի���

class CMarkPointDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMarkPointDlg)

public:
	CMarkPointDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMarkPointDlg();

// �Ի�������
	enum { IDD = IDD_DIALOGMARKPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
