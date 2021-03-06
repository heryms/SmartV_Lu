// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://msdn.microsoft.com/officeui。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "SmartVII_Faster.h"
//#include "SmartVII_FasterView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_SETTINGCHANGE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_BTNOFFLINEOPEN, &CMainFrame::OnBtnofflineopen)
//	ON_COMMAND(ID_BTNOFFLINEPLAY, &CMainFrame::OnBtnofflineplay)
	ON_COMMAND(ID_BTNOFFLINELEFT, &CMainFrame::OnBtnofflineleft)
	ON_COMMAND(ID_BTNRIGHTOFFLINE, &CMainFrame::OnBtnrightoffline)
	ON_COMMAND(ID_BTNPLAY, &CMainFrame::OnBtnplay)
	ON_COMMAND(ID_BTNCONTROL, &CMainFrame::OnBtncontrol)
	ON_COMMAND(ID_ANASHOWMAP, &CMainFrame::OnAnashowmap)
END_MESSAGE_MAP()

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLUE);
	g_gpsHandle=CreateEvent(NULL,TRUE,FALSE,NULL);
	g_VeloRcvHandle=CreateEvent(NULL,TRUE,FALSE,NULL);
	g_laneHandle=CreateEvent(NULL,TRUE,FALSE,NULL);
	g_lidarHandle1=CreateEvent(NULL,TRUE,FALSE,NULL);
	g_lidarHandle2=CreateEvent(NULL,TRUE,FALSE,NULL);
	g_lidarHandle3=CreateEvent(NULL,TRUE,FALSE,NULL);
	g_lidarHandle4=CreateEvent(NULL,TRUE,FALSE,NULL);
	g_canHandle=CreateEvent(NULL,TRUE,FALSE,NULL);
	m_pStatusDlg=new CDataShowDlg();
	m_pMapShowDlg=NULL;
	m_bShowStatus=FALSE;
}
CMainFrame::~CMainFrame()
{
	if (m_pMapShowDlg!=NULL)
	{
		delete m_pMapShowDlg;
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}
	m_pStatusDlg->Create(IDD_DATASHOWDLG,this);
	//if(m_pMapShowDlg==NULL)
	//{
	//	//m_showPath->Create(IDD_SHOW_PATHPLAN,m_hWnd);
	//	m_pMapShowDlg=new CShowPathPlanDlg();
	//	if(m_pMapShowDlg->Create(IDD_SHOW_PATHPLAN)==FALSE)
	//	{
	//		AfxMessageBox("CREAT PATHDLG ERROR");
	//		m_pMapShowDlg=NULL;
	//		return 0;
	//	}
	//	m_pMapShowDlg->IsWindowCreate=1;
	//	m_pMapShowDlg->SetWindowTextA(TEXT("测试地图"));
	//	CRect clientRect,dlgRect;
	//	GetClientRect(&clientRect);
	//	GetWindowRect(&dlgRect);

	//	m_pMapShowDlg->MoveWindow(dlgRect.left,dlgRect.top,dlgRect.Width()/2,dlgRect.Height());
	//	//return 0;
	//}
	//m_pStatusDlg->SetWnd(this->GetActiveView()->GetSafeHwnd());
	//m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	//DockPane(&m_wndOutput);


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	cs.style&= ~(LONG) FWS_ADDTOTITLE;

	return CFrameWnd::PreCreateWindow(cs);
	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	// 创建输出窗口
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	//if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	//{
	//	TRACE0("未能创建输出窗口\n");
	//	return FALSE; // 未能创建
	//}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	//m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	//m_wndOutput.UpdateFonts();
}


void CMainFrame::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CFrameWndEx::OnClose();
}


void CMainFrame::OnBtnofflineopen()
{
	// TODO: 在此添加命令处理程序代码
}


//void CMainFrame::OnBtnofflineplay()
//{
//	// TODO: 在此添加命令处理程序代码
//}


void CMainFrame::OnBtnofflineleft()
{
	// TODO: 在此添加命令处理程序代码
}


void CMainFrame::OnBtnrightoffline()
{
	// TODO: 在此添加命令处理程序代码
}


void CMainFrame::OnBtnplay()
{
	// TODO: 在此添加命令处理程序代码
}


void CMainFrame::OnBtncontrol()
{
	// TODO: 在此添加命令处理程序代码
	if (!m_bShowStatus)
	{
		CRect rt,rt0;
		//CSmartVII_FasterView *pView= (CSmartVII_FasterView*)this->GetActiveView();
	  //  ClientToScreen()
		GetClientRect(&rt);
		m_pStatusDlg->GetClientRect(&rt0);
		
		ClientToScreen(&rt);
		int x=rt.right-rt0.right;
		int y=rt.bottom-rt0.bottom;
		
		m_pStatusDlg->SetWindowPos(this,x-GetSystemMetrics(SM_CXFRAME),y-GetSystemMetrics(SM_CYFRAME)-30,rt0.right,rt0.bottom,SWP_NOSIZE);	
		m_pStatusDlg->ShowWindow(SW_SHOW);
		m_bShowStatus=!m_bShowStatus;
	}
	else
	{
		m_pStatusDlg->ShowWindow(SW_HIDE);
		m_bShowStatus=!m_bShowStatus;
	}

}


void CMainFrame::OnAnashowmap()
{
	// TODO: 在此添加命令处理程序代码
	if(m_pMapShowDlg==NULL)
	{
		//m_showPath->Create(IDD_SHOW_PATHPLAN,m_hWnd);
		m_pMapShowDlg=new CShowPathPlanDlg();
		if(m_pMapShowDlg->Create(IDD_SHOW_PATHPLAN)==FALSE)
		{
			AfxMessageBox("CREAT PATHDLG ERROR");
			m_pMapShowDlg=NULL;
			return;
		}
		m_pMapShowDlg->IsWindowCreate=1;
		m_pMapShowDlg->SetWindowTextA(TEXT("测试地图"));
		CRect clientRect,dlgRect;
		GetClientRect(&clientRect);
		GetWindowRect(&dlgRect);

		m_pMapShowDlg->MoveWindow(dlgRect.left,dlgRect.top,dlgRect.Width()/2,dlgRect.Height());
	}
	m_pMapShowDlg->ShowWindow(SW_SHOW);
}
