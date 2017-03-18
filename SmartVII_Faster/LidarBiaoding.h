#pragma once
#include "afxcmn.h"


// CLidarBiaoding 对话框

class CLidarBiaoding : public CDialogEx
{
	DECLARE_DYNAMIC(CLidarBiaoding)

public:
	CLidarBiaoding(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLidarBiaoding();

// 对话框数据
	enum { IDD = IDD_LIDARBIAODING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
