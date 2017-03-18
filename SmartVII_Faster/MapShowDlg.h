#pragma once


// CMapShowDlg 对话框

class CMapShowDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMapShowDlg)

public:
	CMapShowDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMapShowDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_MAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
