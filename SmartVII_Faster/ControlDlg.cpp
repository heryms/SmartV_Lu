// ControlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SmartVII_Faster.h"
#include "ControlDlg.h"
#include "afxdialogex.h"
#include "ckCoordinate.h"

// CControlDlg 对话框

IMPLEMENT_DYNAMIC(CControlDlg, CDialogEx)

CControlDlg::CControlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CControlDlg::IDD, pParent)
	, m_fLongtitude(0)
	, m_fLatituede(0)
{
	m_Ctlstatus=false;
	m_nLightType=0;
   m_bStopThread=FALSE;
   m_eCarStatus=RUN;
}

CControlDlg::~CControlDlg()
{
	StopThread();
}

void CControlDlg::DoDataExchange(CDataExchange* pDX)
{

	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_SCELAT, m_fLongtitude);
	DDX_Text(pDX, IDC_SCELON, m_fLatituede);
	DDX_Text(pDX,IDC_EDIT_SECBRAKEDIS,m_fBrakeDis);
	DDX_Text(pDX,IDC_EDIT_SECBRAKESPEED,m_nCarSpeed);
	DDX_Text(pDX,IDC_EDIT_SECDECE,m_fDecelateDis);
}


BEGIN_MESSAGE_MAP(CControlDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTNSECCROSS, &CControlDlg::OnBnClickedBtnseccross)
	ON_BN_CLICKED(IDC_BTNSECRECORD, &CControlDlg::OnBnClickedBtnsecrecord)
	ON_BN_CLICKED(IDC_BTNSECSTART, &CControlDlg::OnBnClickedBtnsecstart)
	ON_BN_CLICKED(IDC_BTNSECLIGHT, &CControlDlg::OnBnClickedBtnseclight)
END_MESSAGE_MAP()


// CControlDlg 消息处理程序

UINT CControlDlg::InitCanInfo(CCanControl can)
{
	if (!can.GetConnectStatus()==1)
	{
		AfxMessageBox("CAN卡未正确连接");
	    return 0;
	}
	m_canCtl=can;
	return 1;
}

void CControlDlg::OnBnClickedBtnseccross()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: 在此添加控件通知处理程序代码
	if (!m_Ctlstatus)
	{
		CString strLon,strLat;
		strLon.Format("%lf",g_curPostion.Gnx);
		strLat.Format("%lf",g_curPostion.Gny);
		GetDlgItem(IDC_SCELON)->SetWindowText(strLon);
		GetDlgItem(IDC_SCELAT)->SetWindowText(strLat);
		m_crossPoint=g_curPostion;
		CString str;
	    GetDlgItemText(IDC_EDIT_SECBRAKEDIS,str);
		m_fBrakeDis=atof(str.GetBuffer());	
		GetDlgItemText(IDC_EDIT_SECBRAKESPEED,str);
		m_nCarSpeed=atoi(str.GetBuffer());		
		GetDlgItemText(IDC_EDIT_SECDECE,str);
		m_fDecelateDis=atoi(str.GetBuffer());
		GetDlgItem(IDC_EDIT_SECBRAKESPEED)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_SECBRAKEDIS)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_SECDECE)->EnableWindow(FALSE);
		m_Ctlstatus=true;
		GetDlgItem(IDC_BTNSECCROSS)->SetWindowText("完成");
	}
	else
	{
		GetDlgItem(IDC_EDIT_SECBRAKESPEED)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_SECBRAKEDIS)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_SECDECE)->EnableWindow(TRUE);
		m_Ctlstatus=false;
		GetDlgItem(IDC_BTNSECCROSS)->SetWindowText("路口采集");
	}
}
CString GetFilePath(CString FileName)
{
	CString tmpStr;
	char drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
	char path_buffer[_MAX_PATH];
	strcpy((char*)path_buffer,FileName);
	_splitpath(path_buffer,drive,dir,fname,ext);

	tmpStr = drive;
	tmpStr += dir;
	return tmpStr;
}
//记录路口配置文件
void CControlDlg::OnBnClickedBtnsecrecord()
{
	CString strLon,strLat,strBrakeDis,strDeceDis,strSpeed;
	GetDlgItemText(IDC_SCELON,strLon);
	GetDlgItemText(IDC_SCELAT,strLat);
	GetDlgItemText(IDC_EDIT_SECBRAKEDIS,strBrakeDis);
	GetDlgItemText(IDC_EDIT_SECDECE,strDeceDis);
	GetDlgItemText(IDC_EDIT_SECBRAKESPEED,strSpeed);
	char buffer[_MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), buffer, _MAX_PATH);
	CString str = GetFilePath(buffer);
	
   str.Append("Scene\\");
   if (!PathFileExists(str))
   {
	   CreateDirectory(str,NULL);
   }
   CString filter;  
   filter="任务文件(*.ini)|*.ini||";  
   CFileDialog fdlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,filter); 
   fdlg.m_ofn.lpstrInitialDir=str;
   if (fdlg.DoModal()==IDOK)
   {
    str=fdlg.GetPathName()+".ini";
	// WritePrivateProfileSection("Cross","CROSS0",str);
	WritePrivateProfileString("CROSS0","Lon",strLon,str);
	WritePrivateProfileString("CROSS0","Lat",strLat,str);
	WritePrivateProfileString("CROSS0","BRAKEDIS",strBrakeDis,str);
	WritePrivateProfileString("CROSS0","DECE",strDeceDis,str);
	WritePrivateProfileString("CROSS0","SPEED",strSpeed,str);
   }
   


}
void CrossControlFunc(void*param)
{
	CControlDlg *pDlg=(CControlDlg*)param;
	pDlg->ProCrossTest();
	_endthread();
}
void CControlDlg::StopThread()
{
	m_bStopThread=TRUE;
	
}
BOOL CControlDlg::LoadCrossScene()
{
	CString filter;  
	filter="任务文件(*.ini)|*.ini||";  
	CFileDialog fdlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,filter); 
    if (fdlg.DoModal()==IDOK)
    {
		char res[50];
		GetPrivateProfileString("CROSS0","Lon","0.0",res,sizeof(res),fdlg.GetPathName());
		m_fLongtitude=atof(res);
		GetPrivateProfileString("CROSS0","Lat","0.0",res,sizeof(res),fdlg.GetPathName());
		m_fLatituede=atof(res);
		GetPrivateProfileString("CROSS0","BRAKEDIS","10.0",res,sizeof(res),fdlg.GetPathName());
		m_fBrakeDis=atof(res);
		GetPrivateProfileString("CROSS0","DECE","20.0",res,sizeof(res),fdlg.GetPathName());
		m_fDecelateDis=atof(res);
		GetPrivateProfileString("CROSS0","SPEED","3000",res,sizeof(res),fdlg.GetPathName());
		m_nCarSpeed=atoi(res);
		UpdateData(FALSE);
		return TRUE;
    }
	return FALSE;
}
//开始场景测试
void CControlDlg::OnBnClickedBtnsecstart()
{
	// TODO: 在此添加控件通知处理程序代码
	if (LoadCrossScene())
	{
		m_eCarStatus=RUN;
		m_canCtl.SendCarSpeed(true,0,m_nCarSpeed);
		_beginthread(CrossControlFunc,0,this);
	}
	

}
void CControlDlg::ProCrossTest()
{
	while(!m_bStopThread)
	{
		
		GpsLine pos0,pos1;
		ckCoordinate::LongLat2XY(m_fLongtitude,m_fLatituede,pos0.Gnx,pos0.Gny);
		ckCoordinate::LongLat2XY(g_curPostion.Gnx,g_curPostion.Gny,pos1.Gnx,pos1.Gny);
		double delx=pos0.Gnx-pos1.Gnx;
		double dely=pos0.Gny-pos1.Gny;
		double dis=sqrt((pow(delx,2)+pow(dely,2)));
		CString str;
		str.Format("%f",dis);
	    GetDlgItem(IDC_SECCROSSDIS)->SetWindowText(str);
		if ((m_eCarStatus==RUN)&&dis<m_fDecelateDis&&dis>m_fBrakeDis)
		{
			m_canCtl.SendCarSpeed(true,0,1000);
			m_eCarStatus=DECE;
			TRACE("000\n");
		}
		else if (dis<m_fBrakeDis)
		{
			if ((m_eCarStatus==DECE)&&(m_nLightType==1))
			{
				m_canCtl.SendCarSpeed(true,3,0);
				m_eCarStatus=STOP;
					TRACE("001\n");
			}
  			else if ((m_eCarStatus==STOP)&&(g_eBrakeStatus==SOFTUP)&&(m_nLightType==0))
			{
				m_canCtl.SendCarSpeed(true,4,0);
					TRACE("002\n");
			//	m_eCarStatus=RUN;
			}
			else if ((m_eCarStatus==STOP)&&(g_eBrakeStatus==UNSOFTUP)&&(m_nLightType==0))
			{
				m_canCtl.SendCarSpeed(true,0,700);
			    m_eCarStatus=RUN;
					TRACE("003\n");
			}
			
		}
	   
		Sleep(50);
	}
}
BOOL CControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	GetDlgItem(IDC_SCELON)->SetWindowText("0.0");
	GetDlgItem(IDC_SCELAT)->SetWindowText("0.0");
	GetDlgItem(IDC_EDIT_SECBRAKEDIS)->SetWindowText("10.0");
	GetDlgItem(IDC_EDIT_SECBRAKESPEED)->SetWindowText("3000");
	GetDlgItem(IDC_EDIT_SECDECE)->SetWindowText("20.0");
	CStatic * pStatic  = (CStatic *)GetDlgItem(IDC_SECLIGHTSHOW);
	HBITMAP hBitmap;
	hBitmap =  (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAPGREEN),
		IMAGE_BITMAP,0,0, LR_CREATEDIBSECTION); // IDB_BMP是位图在RC里的ID

	//SS_BITMAP 装载的是位图，SS_CENTERIMAGE 位图居中显示
	pStatic->ModifyStyle(0,SS_BITMAP|SS_CENTERIMAGE); // 修改  CStatic的样式，必须的

	pStatic->SetBitmap(hBitmap); 
	GetDlgItem(IDC_BTNSECLIGHT)->SetWindowText("红灯");

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CControlDlg::OnBnClickedBtnseclight()
{
	// TODO: 在此添加控件通知处理程序代码
	if (0==m_nLightType)//当前为绿灯
	{
		CStatic * pStatic  = (CStatic *)GetDlgItem(IDC_SECLIGHTSHOW);
		HBITMAP hBitmap;
		hBitmap =  (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAPRED),
			IMAGE_BITMAP,0,0, LR_CREATEDIBSECTION); // IDB_BMP是位图在RC里的ID

		//SS_BITMAP 装载的是位图，SS_CENTERIMAGE 位图居中显示
		pStatic->ModifyStyle(0,SS_BITMAP|SS_CENTERIMAGE); // 修改  CStatic的样式，必须的

		pStatic->SetBitmap(hBitmap); 
		GetDlgItem(IDC_BTNSECLIGHT)->SetWindowText("绿灯");
		m_nLightType=1;
	}
	else if (1==m_nLightType)//当前为红灯
	{
		CStatic * pStatic  = (CStatic *)GetDlgItem(IDC_SECLIGHTSHOW);
		HBITMAP hBitmap;
		hBitmap =  (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAPGREEN),
			IMAGE_BITMAP,0,0, LR_CREATEDIBSECTION); // IDB_BMP是位图在RC里的ID

		//SS_BITMAP 装载的是位图，SS_CENTERIMAGE 位图居中显示
		pStatic->ModifyStyle(0,SS_BITMAP|SS_CENTERIMAGE); // 修改  CStatic的样式，必须的

		pStatic->SetBitmap(hBitmap); 
		m_nLightType=0;
		GetDlgItem(IDC_BTNSECLIGHT)->SetWindowText("红灯");
	}
	

}
