#pragma once
#include "afxcmn.h"


// CLidarBiaoding �Ի���

class CLidarBiaoding : public CDialogEx
{
	DECLARE_DYNAMIC(CLidarBiaoding)

public:
	CLidarBiaoding(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLidarBiaoding();

// �Ի�������
	enum { IDD = IDD_LIDARBIAODING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ctlParamList;
	virtual void PreInitDialog();
	virtual BOOL OnInitDialog();
	//afx_msg void OnNMDblclkListparam(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnHdnItemclickListparam(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnbiaotst();
	afx_msg void OnBnClickedBtnbiaosave();
};
