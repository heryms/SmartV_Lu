// MapControl.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SmartVII_Faster.h"
#include "MapControl.h"
#include "afxdialogex.h"


// CMapControl �Ի���

IMPLEMENT_DYNAMIC(CMapControl, CDialogEx)

CMapControl::CMapControl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMapControl::IDD, pParent)
{

}

CMapControl::~CMapControl()
{
}

void CMapControl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMapControl, CDialogEx)
END_MESSAGE_MAP()


// CMapControl ��Ϣ�������
