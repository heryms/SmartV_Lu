// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://msdn.microsoft.com/officeui��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// MainFrm.cpp : CMainFrame ���ʵ��
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

// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
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
	// ���ڳ־�ֵ�����Ӿ�����������ʽ
	OnApplicationLook(theApp.m_nAppLook);

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// ���� Visual Studio 2005 ��ʽͣ��������Ϊ
	CDockingManager::SetDockingMode(DT_SMART);
	// ���� Visual Studio 2005 ��ʽͣ�������Զ�������Ϊ
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// ����ͣ������
	if (!CreateDockingWindows())
	{
		TRACE0("δ�ܴ���ͣ������\n");
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
	//	m_pMapShowDlg->SetWindowTextA(TEXT("���Ե�ͼ"));
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
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	cs.style&= ~(LONG) FWS_ADDTOTITLE;

	return CFrameWnd::PreCreateWindow(cs);
	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	// �����������
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	//if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	//{
	//	TRACE0("δ�ܴ����������\n");
	//	return FALSE; // δ�ܴ���
	//}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	//m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

}

// CMainFrame ���

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


// CMainFrame ��Ϣ�������

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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CFrameWndEx::OnClose();
}


void CMainFrame::OnBtnofflineopen()
{
	// TODO: �ڴ���������������
}


//void CMainFrame::OnBtnofflineplay()
//{
//	// TODO: �ڴ���������������
//}


void CMainFrame::OnBtnofflineleft()
{
	// TODO: �ڴ���������������
}


void CMainFrame::OnBtnrightoffline()
{
	// TODO: �ڴ���������������
}


void CMainFrame::OnBtnplay()
{
	// TODO: �ڴ���������������
}


void CMainFrame::OnBtncontrol()
{
	// TODO: �ڴ���������������
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
	// TODO: �ڴ���������������
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
		m_pMapShowDlg->SetWindowTextA(TEXT("���Ե�ͼ"));
		CRect clientRect,dlgRect;
		GetClientRect(&clientRect);
		GetWindowRect(&dlgRect);

		m_pMapShowDlg->MoveWindow(dlgRect.left,dlgRect.top,dlgRect.Width()/2,dlgRect.Height());
	}
	m_pMapShowDlg->ShowWindow(SW_SHOW);
}
