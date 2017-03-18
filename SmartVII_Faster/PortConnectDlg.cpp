// PortConnectDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SmartVII_Faster.h"
#include "PortConnectDlg.h"
#include "afxdialogex.h"


// CPortConnectDlg 对话框

IMPLEMENT_DYNAMIC(CPortConnectDlg, CDialogEx)

CPortConnectDlg::CPortConnectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPortConnectDlg::IDD, pParent)
{
  m_nPos=0;
  m_flagClose=FALSE;
}

CPortConnectDlg::~CPortConnectDlg()
{
}

void CPortConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_PROGRESSPORT, m_ctlProgress);
	DDX_Control(pDX, IDC_STATICPORT, m_ctlTxt);
}


BEGIN_MESSAGE_MAP(CPortConnectDlg, CDialogEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CPortConnectDlg 消息处理程序
UINT CPortConnectDlg::ProgressThread(LPVOID param)
{
	CPortConnectDlg *dlg=(CPortConnectDlg*)param;
	dlg->ProgressFunc();
	return 1;
}
void CPortConnectDlg::ProgressFunc()
{
	while(!m_flagClose)
	{
		int pos=*m_nPos;
		m_ctlProgress.SetPos(pos);
		Sleep(50);
	}
	
}


INT_PTR CPortConnectDlg::DoModal()
{
	// TODO: 在此添加专用代码和/或调用基类
	ProgressFunc();
	return CDialogEx::DoModal();
}


void CPortConnectDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnClose();
}
