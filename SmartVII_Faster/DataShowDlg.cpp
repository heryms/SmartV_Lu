// DataShowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SmartVII_Faster.h"
#include "DataShowDlg.h"
#include "afxdialogex.h"


// CDataShowDlg 对话框

IMPLEMENT_DYNAMIC(CDataShowDlg, CDialogEx)

CDataShowDlg::CDataShowDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDataShowDlg::IDD, pParent)
	, m_bCancelFlag(FALSE)
{

}

CDataShowDlg::~CDataShowDlg()
{
}

void CDataShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDataShowDlg, CDialogEx)
	ON_WM_KEYDOWN()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_KEYUP()
	ON_BN_CLICKED(IDC_UP, &CDataShowDlg::OnBnClickedUp)
	ON_BN_CLICKED(IDC_DOWN, &CDataShowDlg::OnBnClickedDown)
	ON_BN_CLICKED(IDC_LEFT, &CDataShowDlg::OnBnClickedLeft)
	ON_BN_CLICKED(IDC_RIGHT, &CDataShowDlg::OnBnClickedRight)
	ON_BN_CLICKED(IDC_SPACE, &CDataShowDlg::OnBnClickedSpace)
	ON_BN_CLICKED(IDC_C, &CDataShowDlg::OnBnClickedC)
	ON_WM_ACTIVATE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_UNBRAKE, &CDataShowDlg::OnBnClickedUnbrake)
END_MESSAGE_MAP()


// CDataShowDlg 消息处理程序


void CDataShowDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nChar)
	{
	case VK_UP:
		::PostMessage(m_hWnd,WM_SPEEDUP,0,0);
		((CButton*)GetDlgItem(IDC_UP))->SetCheck(TRUE);
		GetDlgItem(IDC_SPEED)->SetFocus();
		break;
	case VK_DOWN:
		::PostMessageA(m_hWnd,WM_SPEEDDOWN,0,0);
		((CButton*)GetDlgItem(IDC_DOWN))->SetCheck(TRUE);
		GetDlgItem(IDC_SPEED)->SetFocus();
		break;
	case VK_LEFT:
		::PostMessage(m_hWnd,WM_ANGLELEFT,0,0);
		((CButton*)GetDlgItem(IDC_LEFT))->SetCheck(TRUE);
		GetDlgItem(IDC_SPEED)->SetFocus();
		break;
	case VK_RIGHT:
		::PostMessage(m_hWnd,WM_ANGLERIGHT,0,0);
		((CButton*)GetDlgItem(IDC_RIGHT))->SetCheck(TRUE);
		GetDlgItem(IDC_SPEED)->SetFocus();
		break;
	case VK_SPACE:
		::PostMessage(m_hWnd,WM_BRAKE4,0,0);
		((CButton*)GetDlgItem(IDC_SPACE))->SetCheck(TRUE);
		break;
	case 67://c
		::PostMessage(m_hWnd,WM_BRAKE3,0,0);
		((CButton*)GetDlgItem(IDC_C))->SetCheck(TRUE);
		break;
	case 68://d
		((CButton*)GetDlgItem(IDC_UNBRAKE))->SetCheck(TRUE);
		break;
	}
	return;
	//CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CDataShowDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN) return TRUE;
		else
		OnKeyDown(pMsg->wParam,LOWORD(pMsg->wParam),HIWORD(pMsg->wParam));
	}
	else if (pMsg->message == WM_KEYUP)
	{
		OnKeyUp(pMsg->wParam,LOWORD(pMsg->wParam),HIWORD(pMsg->wParam));
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDataShowDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	delete this;
	// TODO: 在此处添加消息处理程序代码
}


BOOL CDataShowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//AddFontResource("DS-DIGI.TTF");
	////::SendMessage(HWND_BROADCAST,WM_FONTCHANGE,0,0);
	////LOGFONT lf;
	////memset(&lf,0,sizeof(LOGFONT));
	////lf.lfHeight = 300;
	////lf.lfWeight = 20;
	////strcpy(lf.lfFaceName,"DS-Digital");
	////m_font.CreatePointFontIndirect(&lf);
	GetDlgItem(IDC_SPEED)->SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


HBRUSH CDataShowDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() == IDC_SPEED || pWnd->GetDlgCtrlID() == IDC_ANGLE 
		|| pWnd->GetDlgCtrlID() == IDC_LAT || pWnd->GetDlgCtrlID() == IDC_LON)
	{
		pDC->SelectObject(&m_font);
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CDataShowDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	ShowWindow(SW_HIDE);
	m_bCancelFlag = !m_bCancelFlag;
	//CDialogEx::OnCancel();
}


void CDataShowDlg::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nChar)
	{
	case VK_UP:
		((CButton*)GetDlgItem(IDC_UP))->SetCheck(FALSE);
		break;
	case VK_DOWN:
		((CButton*)GetDlgItem(IDC_DOWN))->SetCheck(FALSE);
		break;
	case VK_LEFT:
		((CButton*)GetDlgItem(IDC_LEFT))->SetCheck(FALSE);
		break;
	case VK_RIGHT:
		((CButton*)GetDlgItem(IDC_RIGHT))->SetCheck(FALSE);
		break;
	case VK_SPACE:
		((CButton*)GetDlgItem(IDC_SPACE))->SetCheck(FALSE);
		break;
	case 67:
		((CButton*)GetDlgItem(IDC_C))->SetCheck(FALSE);
		break;
	case 68:
		((CButton*)GetDlgItem(IDC_UNBRAKE))->SetCheck(FALSE);
		break;
	}
	return;
	//CDialogEx::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CDataShowDlg::OnBnClickedUp()
{
	// TODO: 在此添加控件通知处理程序代码
	::PostMessage(m_hWnd,WM_SPEEDUP,0,0);
	GetDlgItem(IDC_SPEED)->SetFocus();
	((CButton*)GetDlgItem(IDC_UP))->SetCheck(FALSE);
}


void CDataShowDlg::OnBnClickedDown()
{
	// TODO: 在此添加控件通知处理程序代码
	::PostMessage(m_hWnd,WM_SPEEDDOWN,0,0);
	GetDlgItem(IDC_SPEED)->SetFocus();
	((CButton*)GetDlgItem(IDC_DOWN))->SetCheck(FALSE);
}


void CDataShowDlg::OnBnClickedLeft()
{
	// TODO: 在此添加控件通知处理程序代码
	::PostMessage(m_hWnd,WM_ANGLELEFT,0,0);
	GetDlgItem(IDC_SPEED)->SetFocus();
	((CButton*)GetDlgItem(IDC_LEFT))->SetCheck(FALSE);
}


void CDataShowDlg::OnBnClickedRight()
{
	// TODO: 在此添加控件通知处理程序代码
	::PostMessage(m_hWnd,WM_ANGLERIGHT,0,0);
 	GetDlgItem(IDC_SPEED)->SetFocus();
	((CButton*)GetDlgItem(IDC_RIGHT))->SetCheck(FALSE);
}


void CDataShowDlg::OnBnClickedSpace()
{
	// TODO: 在此添加控件通知处理程序代码
	::PostMessage(m_hWnd,WM_BRAKE4,0,0);
	GetDlgItem(IDC_SPEED)->SetFocus();
	((CButton*)GetDlgItem(IDC_SPACE))->SetCheck(FALSE);
}


void CDataShowDlg::OnBnClickedC()
{
	// TODO: 在此添加控件通知处理程序代码
	::PostMessage(m_hWnd,WM_BRAKE3,0,0);
	GetDlgItem(IDC_SPEED)->SetFocus();
	((CButton*)GetDlgItem(IDC_C))->SetCheck(FALSE);
}

void CDataShowDlg::OnBnClickedUnbrake()
{
	// TODO: 在此添加控件通知处理程序代码
	::PostMessage(m_hWnd,WM_DISABLE_BRAKE,0,0);
	GetDlgItem(IDC_SPEED)->SetFocus();
	((CButton*)GetDlgItem(IDC_UNBRAKE))->SetCheck(FALSE);
}
void CDataShowDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
	GetDlgItem(IDC_SPEED)->SetFocus();
	// TODO: 在此处添加消息处理程序代码
}

int cnt=0;
void CDataShowDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

//	::SendMessageA(AfxGetMainWnd()->GetSafeHwnd(),)
	//CDialogEx::OnClose();
	if (cnt==0)
	{
	
		MessageBox("再点三下，不然关不掉...");
	}
	else if (cnt==3)
	{
		MessageBox("真乖...，就是关不掉");
		cnt==0;
		return;
	}
	else
	 Sleep(200);
		cnt++;

	
}



