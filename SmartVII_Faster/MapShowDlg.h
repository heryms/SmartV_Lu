#pragma once


// CMapShowDlg �Ի���

class CMapShowDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMapShowDlg)

public:
	CMapShowDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMapShowDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_MAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
