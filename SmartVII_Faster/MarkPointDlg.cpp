// MarkPointDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SmartVII_Faster.h"
#include "MarkPointDlg.h"
#include "afxdialogex.h"
#include <fstream>

// CMarkPointDlg 对话框

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


// CMarkPointDlg 消息处理程序


BOOL CMarkPointDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ctlMarkList.DeleteAllItems();//清空  
	m_ctlMarkList.InsertColumn(0,_T("编号"));//添加列  
	m_ctlMarkList.InsertColumn(1,_T("经度"));  
	m_ctlMarkList.InsertColumn(2,_T("纬度"));  
	m_ctlMarkList.InsertColumn(3,_T("海拔"));  
	m_ctlMarkList.InsertColumn(4,_T("属性1"));
	m_ctlMarkList.InsertColumn(5,_T("属性2"));  
	CRect rt;
	m_ctlMarkList.GetClientRect(&rt);
	m_ctlMarkList.SetColumnWidth(0, rt.Width()/12);//设置列宽  
	m_ctlMarkList.SetColumnWidth(1, rt.Width()*3/12);  
	m_ctlMarkList.SetColumnWidth(2, rt.Width()*3/12);
	m_ctlMarkList.SetColumnWidth(3, rt.Width()*3/12);
	m_ctlMarkList.SetColumnWidth(4, rt.Width()/9); 
		m_ctlMarkList.SetColumnWidth(5, rt.Width()/9); 
	CString str="0起点,1交叉口入点,2交叉口出点,3普通路点,4进入停车区,5驶出停车区,6停车位位置,7终点,8信号灯,9人行道,10限速牌,11前方施工,12道路封闭,13上下坡,14绕行,15禁止通行,16禁止左转,17禁止右转,18禁止直行,19禁止掉头,20高架,21过桥";
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
	//m_ctlMarkList.SetRedraw(FALSE);//防止重绘  
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//保存文件
void CMarkPointDlg::OnBnClickedMarksave()
{
	// TODO: 在此添加控件通知处理程序代码
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




//查询当前车辆的位置
void CMarkPointDlg::OnBnClickedMarkquery()
{
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
	int row=m_ctlMarkList.GetItemCount();
	m_ctlMarkList.DeleteItem(row-1);
}


void CMarkPointDlg::OnBnClickedMarkadd()
{
	// TODO: 在此添加控件通知处理程序代码
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
	m_ctlMarkList.InsertItem(row,strRow);       //插入第一个数据，即第0条数据。先插入，然后在修改其他的信息。
	m_ctlMarkList.SetItemText(row,1,str1);     //修改第0条数据的其他信息。
	m_ctlMarkList.SetItemText(row,2,str2);
	m_ctlMarkList.SetItemText(row,3,str5);
	m_ctlMarkList.SetItemText(row,4,str3);
		m_ctlMarkList.SetItemText(row,5,str4);
}
