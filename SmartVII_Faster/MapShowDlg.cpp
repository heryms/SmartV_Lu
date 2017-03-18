// MapShowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SmartVII_Faster.h"
#include "MapShowDlg.h"
#include "afxdialogex.h"


// CMapShowDlg 对话框

IMPLEMENT_DYNAMIC(CMapShowDlg, CDialogEx)

CMapShowDlg::CMapShowDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMapShowDlg::IDD, pParent)
{

}

CMapShowDlg::~CMapShowDlg()
{
}

void CMapShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMapShowDlg, CDialogEx)
END_MESSAGE_MAP()


// CMapShowDlg 消息处理程序
