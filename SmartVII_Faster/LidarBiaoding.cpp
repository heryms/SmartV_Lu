// LidarBiaoding.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SmartVII_Faster.h"
#include "LidarBiaoding.h"
#include "afxdialogex.h"


// CLidarBiaoding �Ի���

IMPLEMENT_DYNAMIC(CLidarBiaoding, CDialogEx)

CLidarBiaoding::CLidarBiaoding(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLidarBiaoding::IDD, pParent)
{

}

CLidarBiaoding::~CLidarBiaoding()
{
}

void CLidarBiaoding::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_LISTPARAM, m_ctlParamList);
}


BEGIN_MESSAGE_MAP(CLidarBiaoding, CDialogEx)
	//ON_NOTIFY(NM_DBLCLK, IDC_LISTPARAM, &CLidarBiaoding::OnNMDblclkListparam)
	//ON_NOTIFY(HDN_ITEMCLICK, 0, &CLidarBiaoding::OnHdnItemclickListparam)
	ON_BN_CLICKED(IDC_BTNBIAOTST, &CLidarBiaoding::OnBnClickedBtnbiaotst)
	//ON_//BN_CLICKED(IDC_BTNBIAOSAVE, &CLidarBiaoding::OnBnClickedBtnbiaosave)
END_MESSAGE_MAP()


// CLidarBiaoding ��Ϣ�������


void CLidarBiaoding::PreInitDialog()
{
	// TODO: �ڴ����ר�ô����/����û���

	CDialogEx::PreInitDialog();
}


BOOL CLidarBiaoding::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//m_ctlParamList.InsertColumn( 0, "������", LVCFMT_LEFT, 80 );// ������ 
	//m_ctlParamList.InsertColumn( 1, "X", LVCFMT_LEFT, 80 );
	//m_ctlParamList.InsertColumn( 2, "Y", LVCFMT_LEFT, 80 );
	//m_ctlParamList.InsertColumn( 3, "Z", LVCFMT_LEFT, 80 );
	//m_ctlParamList.InsertColumn( 4, "ROLL", LVCFMT_LEFT, 80 );
	//m_ctlParamList.InsertColumn( 5, "PITCH", LVCFMT_LEFT, 80 );
	//m_ctlParamList.InsertColumn( 6, "HEAD", LVCFMT_LEFT, 80 );
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}





//void CLidarBiaoding::OnHdnItemclickListparam(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	*pResult = 0;
//}


void CLidarBiaoding::OnBnClickedBtnbiaotst()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

}


void CLidarBiaoding::OnBnClickedBtnbiaosave()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
