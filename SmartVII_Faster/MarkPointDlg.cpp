// MarkPointDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SmartVII_Faster.h"
#include "MarkPointDlg.h"
#include "afxdialogex.h"
#include <fstream>

// CMarkPointDlg �Ի���

IMPLEMENT_DYNAMIC(CMarkPointDlg, CDialogEx)

CMarkPointDlg::CMarkPointDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMarkPointDlg::IDD, pParent)
{

}

CMarkPointDlg::~CMarkPointDlg()
{
}

void CMarkPointDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MARKPT, m_ctlMarkList);
}


BEGIN_MESSAGE_MAP(CMarkPointDlg, CDialogEx)
	ON_BN_CLICKED(ID_MARKSAVE, &CMarkPointDlg::OnBnClickedMarksave)
//	ON_BN_CLICKED(ID_MARKQUERY, &CMarkPointDlg::OnBnClickedMarkquery)
ON_BN_CLICKED(ID_MARKQUERY, &CMarkPointDlg::OnBnClickedMarkquery)
ON_BN_CLICKED(ID_MARKDELETE, &CMarkPointDlg::OnBnClickedMarkdelete)
ON_BN_CLICKED(ID_MARKADD, &CMarkPointDlg::OnBnClickedMarkadd)
END_MESSAGE_MAP()


// CMarkPointDlg ��Ϣ�������


BOOL CMarkPointDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_ctlMarkList.DeleteAllItems();//���  
	m_ctlMarkList.InsertColumn(0,_T("���"));//�����  
	m_ctlMarkList.InsertColumn(1,_T("����"));  
	m_ctlMarkList.InsertColumn(2,_T("γ��"));  
	m_ctlMarkList.InsertColumn(3,_T("����"));  
	m_ctlMarkList.InsertColumn(4,_T("����1"));
	m_ctlMarkList.InsertColumn(5,_T("����2"));  
	CRect rt;
	m_ctlMarkList.GetClientRect(&rt);
	m_ctlMarkList.SetColumnWidth(0, rt.Width()/12);//�����п�  
	m_ctlMarkList.SetColumnWidth(1, rt.Width()*3/12);  
	m_ctlMarkList.SetColumnWidth(2, rt.Width()*3/12);
	m_ctlMarkList.SetColumnWidth(3, rt.Width()*3/12);
	m_ctlMarkList.SetColumnWidth(4, rt.Width()/9); 
		m_ctlMarkList.SetColumnWidth(5, rt.Width()/9); 
	CString str="0���,1��������,2����ڳ���,3��ͨ·��,4����ͣ����,5ʻ��ͣ����,6ͣ��λλ��,7�յ�,8�źŵ�,9���е�,10������,11ǰ��ʩ��,12��·���,13������,14����,15��ֹͨ��,16��ֹ��ת,17��ֹ��ת,18��ֱֹ��,19��ֹ��ͷ,20�߼�,21����";
	CString orzString=str;
	vector<CString> splitvector;
	while(TRUE)
	{
		CString temp=orzString.SpanExcluding(",");
		orzString=orzString.Right(orzString.GetLength()-temp.GetLength()-1);
		splitvector.push_back(temp);
		if (orzString.IsEmpty())
		{
			break;
		}
	}
	((CComboBox*)GetDlgItem(IDC_COMBOMARKTYPE))->ResetContent();
	((CComboBox*)GetDlgItem(IDC_COMBOMARKTYPE2))->ResetContent();
	CComboBox* pComb=(CComboBox*)GetDlgItem(IDC_COMBOMARKTYPE);
	for(int i=0;i<splitvector.size();i++)
	{

		pComb->AddString(splitvector[i]);
		((CComboBox*)GetDlgItem(IDC_COMBOMARKTYPE2))->AddString(splitvector[i]);

	}
	((CComboBox*)GetDlgItem(IDC_COMBOMARKTYPE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBOMARKTYPE2))->SetCurSel(0);
	//m_ctlMarkList.SetRedraw(FALSE);//��ֹ�ػ�  
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

//�����ļ�
void CMarkPointDlg::OnBnClickedMarksave()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
   CFileDialog dlg(FALSE,NULL,NULL,OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,"Text File(*.txt)|*.txt||"); 
	
	if (dlg.DoModal()==IDOK)
	{
		CString path=dlg.GetPathName();
		//	FILE *fp=fopen(path,"a+");

	    int pos=path.ReverseFind('.');
		if (pos>0)
		{
			path=path.Left(pos)+".txt";
		}
		else
			path=path+".txt";
		int row=m_ctlMarkList.GetItemCount();
       fstream ff;
	    ff.open(path,ios::out);
		
		for (int i=0;i<row;i++)
		{    CString str;
			for (int j=0;j<6;j++)
			{
				str=m_ctlMarkList.GetItemText(i,j);
			   ff<<str<<"\t";
			}
			ff<<"\n";
		
		}
		ff.close();
		
		
	}
}




//��ѯ��ǰ������λ��
void CMarkPointDlg::OnBnClickedMarkquery()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strLon,strLat,strHgt;
	strLon.Format("%lf",g_curPostion.Gnx);
	strLat.Format("%lf",g_curPostion.Gny);
	strHgt.Format("%lf",g_curPostion.azimuth);
	GetDlgItem(IDC_MARK_LON)->SetWindowText(strLon);
	GetDlgItem(IDC_MARK_LAT)->SetWindowText(strLat);
		GetDlgItem(IDC_MARK_HGT)->SetWindowText(strHgt);
}


void CMarkPointDlg::OnBnClickedMarkdelete()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int row=m_ctlMarkList.GetItemCount();
	m_ctlMarkList.DeleteItem(row-1);
}


void CMarkPointDlg::OnBnClickedMarkadd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int row=m_ctlMarkList.GetItemCount();
	CString strRow;
	strRow.Format("%d",row);
	CString str1,str2,str3,str4,str5;
	GetDlgItemText(IDC_MARK_LON,str1.GetBuffer(50),50);
	GetDlgItemText(IDC_MARK_LAT,str2.GetBuffer(50),50);
	GetDlgItemText(IDC_MARK_HGT,str5.GetBuffer(50),50);
	int type0,type1;
	 type0=((CComboBox*)GetDlgItem(IDC_COMBOMARKTYPE))->GetCurSel();
	 str3.Format("%d",type0);
	 type1=((CComboBox*)GetDlgItem(IDC_COMBOMARKTYPE2))->GetCurSel();
	 str4.Format("%d",type1);
	//GetDlgItemText(IDC_COMBOMARKTYPE,str3.GetBuffer(50),50);
	//GetDlgItemText(IDC_COMBOMARKTYPE2,str4.GetBuffer(50),50);
	m_ctlMarkList.InsertItem(row,strRow);       //�����һ�����ݣ�����0�����ݡ��Ȳ��룬Ȼ�����޸���������Ϣ��
	m_ctlMarkList.SetItemText(row,1,str1);     //�޸ĵ�0�����ݵ�������Ϣ��
	m_ctlMarkList.SetItemText(row,2,str2);
	m_ctlMarkList.SetItemText(row,3,str5);
	m_ctlMarkList.SetItemText(row,4,str3);
		m_ctlMarkList.SetItemText(row,5,str4);
}
