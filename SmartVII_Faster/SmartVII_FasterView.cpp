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

// SmartVII_FasterView.cpp : CSmartVII_FasterView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "SmartVII_Faster.h"
#endif

#include "MainControlDlg.h"
//#include "DataFusionCenter.h"
//#include "DataShow.h"

#include "SmartVII_FasterDoc.h"
#include "MainFrm.h"
#include "SmartVII_FasterView.h"
//#include "OfflineLidar.h"
#include "ckCoordinate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


extern ckLcmType::velodynegrid_t m_velodynegrid;

void DrawTextRGB(float x, float y, float z, float r, float g, float b, char * outputstring)
{
	glPushMatrix();
	glColor3f(r,g,b);	
	wglUseFontBitmaps(wglGetCurrentDC(), 0, 255, 200);

	glListBase(200);
	glRasterPos3f(x, y, z);
	glCallLists( sizeof(outputstring), GL_UNSIGNED_BYTE,_T(outputstring)); 

	glPopMatrix();
}

void VelodyneLayer(const vector<PointShow> ptcloud)
{
	glPointSize(10);
	glBegin(GL_POINTS);
	//glVertex3f(ptcloud[0].x,ptcloud[0].y,ptcloud[0].z);
	for (int i=0;i<ptcloud.size();i++)
	{
		glVertex3f(ptcloud[i].x,ptcloud[i].y,ptcloud[i].z);
	}
	glEnd();	
	glFlush();
} 

void CurrentPosLayer(const vector<PointShow> pos)
{
	if (pos.size()<2)
	{
		return;
	}
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(18);
	glColor3f(0,1,1);
	glBegin(GL_POINTS);
	glVertex3f(pos[0].x,pos[0].y,0);
	glEnd();
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(20);
	glColor3f(1,0,1);
	glBegin(GL_POINTS);
	glVertex3f(pos[1].x,pos[1].y,0);
	glEnd();
	//int n=500;
	//glEnable(GL_POLYGON_SMOOTH);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glColor3f(1,0,1);
	//glBegin(GL_POLYGON);
	//for(int i=0;i<n;i++)
	//	glVertex3f(pos[0].x+10*cos(2*M_PI/n*i), pos[0].y+10*sin(2*M_PI/n*i),1.0);
	//glEnd();
	//glColor3f(0,0,1);
	//glBegin(GL_POLYGON);
	//for(int i=0;i<n;i++)
	//	glVertex3f(pos[1].x+10*cos(2*M_PI/n*i), pos[1].y+10*sin(2*M_PI/n*i),1.0);
	//glEnd();
	//glFlush();
}
void RoadLaneLayer0(const vector<PointShow> pos)
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(2);
	glColor4f(0,0,1,1);
	glBegin(GL_POINTS);
	for(int i=0;i<pos.size();i++)
	{
		glVertex3f(pos[i].x,pos[i].y,0);
	}
	
	glEnd();
}
void drawString0(const char* str) {
	static int isFirstCall = 1;
	static GLuint lists;

	if( isFirstCall ) { // 如果是第一次调用，执行初始化
		// 为每一个ASCII字符产生一个显示列表
		isFirstCall = 0;

		// 申请MAX_CHAR个连续的显示列表编号
		lists = glGenLists(128);

		// 把每个字符的绘制命令都装到对应的显示列表中
		wglUseFontBitmaps(wglGetCurrentDC(), 0, 128, lists);
	}
	// 调用每个字符对应的显示列表，绘制每个字符
	for(; *str!='\0'; ++str)
		glCallList(lists + *str);
}
void DrawTextLineRGB0(float x, float y, float z, float r, float g, float b, char * outputstring)
{
	glPushMatrix();
	glColor3f(r,g,b);	
	wglUseFontBitmaps(wglGetCurrentDC(), 0, 255, 200);

	//glListBase(200);
	glRasterPos3f(x, y, z);
	drawString0(outputstring);
	//glCallLists( sizeof(outputstring), GL_UNSIGNED_BYTE,_T(outputstring)); 

	glPopMatrix();
}
void DrawMarkPoint0(vector<ckMissionPoint> mission)
{
	if (mission.empty())
	{
		return;
	}
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(10);
	glBegin(GL_POINTS);
	for (int i=0;i<mission.size();i++)
	{
		if (mission.at(i).property0==0)
		{
			glColor4f(0,1,0,0.6);
		}
		else if (mission.at(i).property0==7)
		{
			glColor4f(1,0,0,0.6);
		}
		else
			glColor4f(1,1,0,0.6);

		glVertex3f(mission[i].x,mission[i].y,1.0);
		//	PointCircle(mission.at(i).x,mission.at(i).y,1.0/eyehig);
	}
	glEnd();

	//glEnable(GL_DEPTH_TEST);
	//for (int i=0;i<mission.size();i++)
	//{
	//	char num[20];
	//	memset(num,'\0',20);
	//	sprintf(num,"%d",i);
	//	DrawTextLineRGB0(mission[i].x+2,mission[i].y+2,mission[i].hgt,1.0,1.0,1.0,num);
	//}
}
// CSmartVII_FasterView

IMPLEMENT_DYNCREATE(CSmartVII_FasterView, CFormView)

BEGIN_MESSAGE_MAP(CSmartVII_FasterView, CFormView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
//	ON_COMMAND(ID_LIDARBIAODING, &CSmartVII_FasterView::OnLidarbiaoding)
    //设备响应函数
    ON_COMMAND(ID_LANEOPEN, &CSmartVII_FasterView::OnLaneopen)
	ON_COMMAND(ID_TRAFFICOPEN, &CSmartVII_FasterView::OnTrafficopen)
	ON_COMMAND(ID_CANCONNECT, &CSmartVII_FasterView::OnCanconnect)
	ON_COMMAND(ID_GPSCONNECT, &CSmartVII_FasterView::OnGpsconnect)
	ON_COMMAND(ID_LIDAROPEN, &CSmartVII_FasterView::OnLidaropen)
	ON_COMMAND(IDC_ALLCONNECT, &CSmartVII_FasterView::OnAllconnect)
	ON_COMMAND(ID_CLOSECAN, &CSmartVII_FasterView::OnClosecan)
	ON_COMMAND(ID_CLOSEGPS, &CSmartVII_FasterView::OnClosegps)
	ON_COMMAND(IDC_LIDARCLOSE, &CSmartVII_FasterView::OnLidarclose)
	ON_COMMAND(ID_DATASHOW, &CSmartVII_FasterView::OnDatashow)

	ON_WM_MOUSEHWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_GPSCONNECT, &CSmartVII_FasterView::OnUpdateGpsconnect)
	ON_WM_TIMER()

	ON_UPDATE_COMMAND_UI(ID_CLOSEGPS, &CSmartVII_FasterView::OnUpdateClosegps)
	ON_UPDATE_COMMAND_UI(ID_CANCONNECT, &CSmartVII_FasterView::OnUpdateCanconnect)
	ON_UPDATE_COMMAND_UI(ID_CLOSECAN, &CSmartVII_FasterView::OnUpdateClosecan)
	ON_UPDATE_COMMAND_UI(IDC_ALLCONNECT, &CSmartVII_FasterView::OnUpdateAllconnect)

	ON_UPDATE_COMMAND_UI(ID_DATASHOW, &CSmartVII_FasterView::OnUpdateDatashow)
	ON_UPDATE_COMMAND_UI(ID_LIDAROPEN, &CSmartVII_FasterView::OnUpdateLidaropen)

	ON_UPDATE_COMMAND_UI(IDC_LIDARCLOSE, &CSmartVII_FasterView::OnUpdateLidarclose)
	ON_MESSAGE(WM_SPEEDUP,OnSpeedUp)
	ON_MESSAGE(WM_SPEEDDOWN,OnSpeedDown)
	ON_MESSAGE(WM_ANGLELEFT,OnAngleLeft)
	ON_MESSAGE(WM_ANGLERIGHT,OnAngleRight)
	ON_MESSAGE(WM_BRAKE3,OnBrake3)
	ON_MESSAGE(WM_BRAKE4,OnBrake4)
	ON_MESSAGE(WM_DISABLE_BRAKE,OnBrakeu)
	ON_COMMAND(ID_BTNVIEWPOS, &CSmartVII_FasterView::OnOrtho)
	//ON_COMMAND(ID_FILEOPEN, &CSmartVII_FasterView::OnFileopen)

	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_BTNCAIJI, &CSmartVII_FasterView::OnBtncaiji)
	ON_COMMAND(ID_BTNOPERA, &CSmartVII_FasterView::OnBtnopera)
	ON_COMMAND(ID_BTNMARTPOINT, &CSmartVII_FasterView::OnBtnmartpoint)
	ON_UPDATE_COMMAND_UI(ID_BTNCAIJI, &CSmartVII_FasterView::OnUpdateBtncaiji)
	ON_COMMAND(ID_BTNADDMAP, &CSmartVII_FasterView::OnBtnaddmap)
	//规划部分
	ON_COMMAND(ID_MAPOPEN,&CSmartVII_FasterView::OnPlanmapopen)
	ON_COMMAND(ID_PLANSTART,&CSmartVII_FasterView::OnPlanstart)
	ON_COMMAND(ID_BTNVELO, &CSmartVII_FasterView::OnBtnvelo)
	ON_COMMAND(ID_BTNSELFDRIVING, &CSmartVII_FasterView::OnBtnautoctl)
	ON_UPDATE_COMMAND_UI(ID_BTNSELFDRIVING, &CSmartVII_FasterView::OnUpdateBtnautoctl)
	//ON_COMMAND(ID_BUTTON2, &CSmartVII_FasterView::OnButton2)
	ON_UPDATE_COMMAND_UI(ID_LANEOPEN, &CSmartVII_FasterView::OnUpdateLaneopen)
	ON_COMMAND(ID_CTLTURNLANE, &CSmartVII_FasterView::OnCtlturnlane)
	ON_COMMAND(ID_CTLUTURN, &CSmartVII_FasterView::OnCtluturn)
	ON_COMMAND(ID_CTLBRAKE, &CSmartVII_FasterView::OnCtlbrake)
	ON_COMMAND(ID_ANALOADMAP, &CSmartVII_FasterView::OnAnaloadmap)
	ON_COMMAND(ID_ANAAUTOTEST, &CSmartVII_FasterView::OnAnaautotest)
//	ON_COMMAND(ID_ANACREATENET, &CSmartVII_FasterView::OnAnacreatenet)
//	ON_COMMAND(ID_ANASHOWMAP, &CSmartVII_FasterView::OnAnashowmap)
	ON_COMMAND(ID_LASTMAP, &CSmartVII_FasterView::OnLastmap)
	ON_COMMAND(ID_CREATECKMAP, &CSmartVII_FasterView::OnCreateckmap)
	ON_COMMAND(ID_BTNADDMISSION, &CSmartVII_FasterView::OnBtnaddmission)
	ON_COMMAND(ID_CHECKLINE, &CSmartVII_FasterView::OnCheckline)
	ON_COMMAND(ID_CHECKLANE, &CSmartVII_FasterView::OnChecklane)
	ON_COMMAND(ID_CHECKTRACK, &CSmartVII_FasterView::OnChecktrack)

	ON_COMMAND(ID_CHECKNUM, &CSmartVII_FasterView::OnChecknum)
	ON_UPDATE_COMMAND_UI(ID_CHECKLINE, &CSmartVII_FasterView::OnUpdateCheckline)
	ON_UPDATE_COMMAND_UI(ID_CHECKLANE, &CSmartVII_FasterView::OnUpdateChecklane)
	ON_UPDATE_COMMAND_UI(ID_CHECKTRACK, &CSmartVII_FasterView::OnUpdateChecktrack)
	ON_UPDATE_COMMAND_UI(ID_CHECKNUM, &CSmartVII_FasterView::OnUpdateChecknum)
	ON_COMMAND(ID_SENSORPOS, &CSmartVII_FasterView::OnSensorpos)
	ON_COMMAND(ID_BTNMARKADD, &CSmartVII_FasterView::OnBtnmarkadd)
END_MESSAGE_MAP()

// CSmartVII_FasterView 构造/析构
	CRITICAL_SECTION cs;
CSmartVII_FasterView::CSmartVII_FasterView()
	: CFormView(CSmartVII_FasterView::IDD)
	, m_nFaster(0)
	, m_bAllOpen(FALSE)
	, m_bLidarOpen(FALSE)
	, m_bCanOpen(FALSE)
	, m_bGpsOpen(FALSE)
	, m_bDataShowFlag(FALSE)
	, m_bDlgCancelFlag(FALSE)
	, m_bOrtho(FALSE)
	, m_bOfflineLidarFlag(FALSE)
	,m_bRoadLaneLcm(FALSE)
	,m_bTrafficLcm(FALSE)
	,m_bVelodyneLcm(FALSE)
{
	m_bmapLoad=FALSE;
	// TODO: 在此处添加构造代码
  ///CWnd*pWnd=  GetDlgItem(IDD_SMARTVII_FASTER_FORM);
	InitializeCriticalSection(&cs);
	m_bmapLoad=FALSE;
	m_bmissionLoad=FALSE;
	for (int i=0;i<3;i++)
	{
		m_bLayers[i]=TRUE;
	}
	m_bLayers[3]=FALSE;
	//SetTimer(2);
}

CSmartVII_FasterView::~CSmartVII_FasterView()
{
	DeleteCriticalSection(&cs);
}

void CSmartVII_FasterView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}


void CSmartVII_FasterView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
    //m_pShowDlg->Create(IDD_SMARTVII_FASTER_FORM,);
	//m_DataShowPro.OnCreate(m_hWnd);

	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

}

void CSmartVII_FasterView::DrawLidar()
{
	/*SYSTEMTIME st;
	SYSTEMTIME st1;
	GetSystemTime(&st);
	int b,e;
	b=st.wMilliseconds;*/
	if (m_bDataShowFlag)
	{
		m_DataCenter.DrawObjectGrid();
		m_DataCenter.DrawLocalPath();
	}
	//
	//GetSystemTime(&st);
	//e=st.wMilliseconds;
	//TRACE("draw time %d\n",e-b);
}
void CSmartVII_FasterView::OnRButtonUp(UINT  nFlags , CPoint point)
{
	m_DataShowPro.OnRButtonUp(nFlags,point);
	ReleaseCapture();
	PostMessage(WM_PAINT,0,0);
		m_DataShowPro.m_nButtonFlag=other;
}

void CSmartVII_FasterView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CSmartVII_FasterView 诊断

#ifdef _DEBUG
void CSmartVII_FasterView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSmartVII_FasterView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CSmartVII_FasterDoc* CSmartVII_FasterView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSmartVII_FasterDoc)));
	return (CSmartVII_FasterDoc*)m_pDocument;
}
#endif //_DEBUG


// CSmartVII_FasterView 消息处理程序


int CSmartVII_FasterView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);
	m_DataShowPro.OnCreate(hWnd);
	// TODO:  在此添加您专用的创建代码
	m_DataCenter.SetWnd(this->GetSafeHwnd());
	m_DataShowPro.CreatePointLayer("VeloLayer",VelodyneLayer);
	m_DataShowPro.CreatePointLayer("RoadLane0",RoadLaneLayer0);
	m_DataShowPro.SetLayerStatus("RoadLane0",true);
	m_DataShowPro.CreatePointLayer("CKCURPOS",CurrentPosLayer);
    m_DataShowPro.CreateMissionLayer("CKMARKPOINTS",DrawMarkPoint0);
	m_DataShowPro.SetLayerStatus("CKMARKPOINTS",true);
	//m_DataShowPro.CreatePointLayer("LocalPath",LocalPathLayer);//创建路径层
	return 0;
}


void CSmartVII_FasterView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CFormView::OnPaint()
	//CPaintDC dc(this); // device context for painting
	HWND hWnd = this->GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);
	wglMakeCurrent(NULL, NULL);
	wglMakeCurrent(hDC,	m_DataShowPro.m_hGLContext);
	//绘图区域，图层管理
	m_DataShowPro.InitialScene(dc,hDC);

	//m_DataShowPro.Draw_grid(5); // 网格
	//m_DataShowPro.Draw_Road_Center_Point();
	//m_DataShowPro.Draw_circle(5,5);
///	m_DataShowPro.Draw_coordinate();
	

//	m_DataCenter.DrawObjectGrid();

//	m_DataShowPro.Draw_coordinate();
//	DrawLidar();
	
	glPopMatrix();
	
	SwapBuffers(dc.m_ps.hdc);
	::ReleaseDC(hWnd,hDC);
}


void CSmartVII_FasterView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	m_DataShowPro.OnSize(nType,cx,cy);
	//TRACE("%d,%d\n",cx,cy);
	// TODO: 在此处添加消息处理程序代码
	
}


void CSmartVII_FasterView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//TRACE("%d,%d\n",point.x,point.y);
	if (m_DataShowPro.mouseleftdown)
	{
		m_DataShowPro.OnMouseMove(nFlags,point);
		PostMessage(WM_PAINT,0,0);	
	}
	 
	// Sleep(100);

	// m_DataShowPro.m_nButtonFlag=other;
	 CFormView::OnMouseMove(nFlags, point);
}
void PortOpenThread(void* param)
{
	CSmartVII_FasterView* pView=(CSmartVII_FasterView*)param;
	pView->m_DataCenter.ActivateDataPro();
	pView->m_DataCenter.CreateThread();
	pView->m_DataCenter.StartFusionThread();
	pView->m_DataCenter.StartThread();
}


//void CSmartVII_FasterView::OnLidarbiaoding()
//{
//	// TODO: 在此添加命令处理程序代码
//	
//	CLidarBiaoding *pBdDlg=new CLidarBiaoding();
//    pBdDlg->Create(IDD_LIDARBIAODING,this);
//	if (pBdDlg!=NULL)
//	{
//		pBdDlg->ShowWindow(SW_SHOW);
//	}
//	
//	
//	
//}

//void  showlidar(void *param)
//{
//	CSmartVII_FasterView *pview=(CSmartVII_FasterView*)param;
//	while(TRUE)
//	{
//		Sleep(50);
//		PostMessage(pview->GetSafeHwnd(),WM_PAINT,0,0);
//	}
//}
//车道线接受
void CSmartVII_FasterView::OnLaneopen()
{
	// TODO: 在此添加命令处理程序代码
	if (!m_bRoadLaneLcm)
	{
		// TODO: 在此添加命令处理程序代码
		if (!m_bRoadLaneLcm)
		{
			m_DataCenter.m_laneCentor.SetBufferSize(5);
			m_DataCenter.m_laneCentor.SetptNum(100);
			BOOL res=m_DataCenter.m_laneCentor.InitialLaneLcm();
			if (res)
			{
				m_bRoadLaneLcm=TRUE;
				SetTimer(3,100,0);
			}
			else		
			{
				m_bRoadLaneLcm=FALSE;
				KillTimer(3);
			}
		}
		else
		{
			MessageBox("道线已打开");
		}


	}



}

//接收交通标志数据
void CSmartVII_FasterView::OnTrafficopen()
{
	// TODO: 在此添加命令处理程序代码
   
}


void CSmartVII_FasterView::OnCanconnect()
{
	// TODO: 在此添加命令处理程序代码
	m_DataCenter.ActiveCanPro();
	SetTimer(1,100,0);
	//SetEvent(g_gpsHandle);
	m_bCanOpen = !m_bCanOpen;
}

void showcar(void *param)
{
	
	CSmartVII_FasterView*pView=(CSmartVII_FasterView*)param;
	EnterCriticalSection(&cs);
	char str[100];
	int angle=pView->m_DataCenter.GetCurrentAngle();
	int speed=pView->m_DataCenter.GetCurrentSpeed();
	LeaveCriticalSection(&cs);
	sprintf(str,"当前车辆状态:  速度 %d,转向%d",speed,angle);
	
	pView->SetStatusInfo("123");
} 
void CSmartVII_FasterView::SetStatusInfo(char *str)
{
	CMainFrame *pMain=(CMainFrame*)AfxGetMainWnd();
	pMain->m_wndStatusBar.SetInformation(str);
}

//向GPS串口发送指令，开始接收数据
void CSmartVII_FasterView::OnGpsconnect()
{
	// TODO: 在此添加命令处理程序代码

	if (m_DataCenter.GetGpsPortStatus())
	{
	   m_DataCenter.ActivateGPSPro();
	}
	m_bGpsOpen = !m_bGpsOpen;
}


void CSmartVII_FasterView::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// 此功能要求 Windows Vista 或更高版本。
	// _WIN32_WINNT 符号必须 >= 0x0600。
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_DataShowPro.OnMouseWheel(nFlags,zDelta,pt);

	CFormView::OnMouseHWheel(nFlags, zDelta, pt);
}


void CSmartVII_FasterView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_DataShowPro.OnLButtonDown(nFlags,point);
	//UpdateWindow();
	SetCapture();
//	PostMessage(WM_PAINT,0,0);
	CFormView::OnLButtonDown(nFlags, point);
}


BOOL CSmartVII_FasterView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_DataShowPro.OnMouseWheel(nFlags,zDelta,pt);
	PostMessage(WM_PAINT,0,0);
	return TRUE;
	//return CFormView::OnMouseWheel(nFlags, zDelta, pt);
}


void CSmartVII_FasterView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	

	m_DataShowPro.OnLButtonUp(nFlags,point);
	ReleaseCapture();
	PostMessage(WM_PAINT,0,0);
	CFormView::OnLButtonUp(nFlags, point);
}


void CSmartVII_FasterView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_DataShowPro.OnRButtonDown(nFlags,point);
	//UpdateWindow();
//	m_DataShowPro.m_nButtonFlag=rightDown;
	SetCapture();
	CFormView::OnRButtonDown(nFlags, point);
}



void CSmartVII_FasterView::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CFormView::OnClose();
}


BOOL CSmartVII_FasterView::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	return CFormView::DestroyWindow();
}


void CSmartVII_FasterView::OnUpdateGpsconnect(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (!m_bAllOpen) pCmdUI->Enable(FALSE);
	else
	pCmdUI->Enable(!m_bGpsOpen);
}

int draw_zj=0;
void CSmartVII_FasterView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
   CMainFrame *pMain=(CMainFrame*)AfxGetMainWnd();
	if (nIDEvent==1)
	{
		char str[100];
		int angle=m_DataCenter.GetCurrentAngle();
		int speed=m_DataCenter.GetCurrentSpeed();
		double lat = m_DataCenter.GetCurrentGauX();
		double lon = m_DataCenter.GetCurrentGauY();
		//sprintf(str,"当前车辆状态:  速度 %d,转向%d,经度%d,纬度%d",speed,angle,lat,lon);
//		SetStatusInfo(str);
		CString str1;
		str1.Format("%d",angle);
		//::SendMessage(WM_SETTEXT)
		pMain->m_pStatusDlg->GetDlgItem(IDC_ANGLE)->SetWindowText(str1);
		CString str2;
		str2.Format("%d",speed);
		pMain->m_pStatusDlg->GetDlgItem(IDC_SPEED)->SetWindowText(str2);
		CString str3;
		str3.Format("%lf",lat);
		pMain->m_pStatusDlg->GetDlgItem(IDC_LAT)->SetWindowText(str3);
		CString str4;
		str4.Format("%lf",lon);
		pMain->m_pStatusDlg->GetDlgItem(IDC_LON)->SetWindowText(str4);
	}
	else if (nIDEvent==2)
	{		
		//draw_zj++;
		if (m_bmapLoad&&m_bmissionLoad)
		{	
			PointShow ptshow,ptmark;
			ptshow.x=m_DataCenter.GetCurrentGauX()-m_showCentor.x;
			ptshow.y=m_DataCenter.GetCurrentGauY()-m_showCentor.y;
			ptmark.x=m_missionPoints.at(m_ctlStrate.m_ncurMisIdx).x-m_showCentor.x;
			ptmark.y=m_missionPoints.at(m_ctlStrate.m_ncurMisIdx).y-m_showCentor.y;
			vector<PointShow> curCar;
			curCar.push_back(ptshow);
			curCar.push_back(ptmark);
			m_DataShowPro.UpdatePointLayer("CKCURPOS",curCar);
			PostMessage(WM_PAINT,0,0);
		}
	
	}
	else if(nIDEvent==3)
	{
		CMainFrame *pMain=(CMainFrame*)AfxGetMainWnd();
		CShowPathPlanDlg *pDlg=pMain->m_pMapShowDlg;
		if (pDlg!=NULL)
		{	
			//m_DataCenter.UpdateRoadLane();
			vector<PointShow> showline;
			//=m_laneCentor.m_showBuffer;*/
			if (m_DataCenter.m_laneCentor.m_gpsBuffer!=NULL)
			{
				for (int i=0;i<m_DataCenter.m_laneCentor.m_nptNum;i++)
				{
					PointShow pt;
					pt.x=m_DataCenter.m_laneCentor.m_gpsBuffer[i].Gnx;
					pt.y=m_DataCenter.m_laneCentor.m_gpsBuffer[i].Gny;
					showline.push_back(pt);
				}
			}
		
			pDlg->m_DataShowPro.UpdatePointLayer("RoadLane",showline);
			::PostMessage(pDlg->GetSafeHwnd(),WM_PAINT,0,0);
		}
	}	
	else if (nIDEvent==5)
	{
		PointShow ptshow;
			vector<PointShow> curtrack;
		for (int i=0;i<m_trackPoints.size();i++)
		{
			ptshow.x=m_trackPoints[i].x-m_trackPoints[2000].x;
			ptshow.y=m_trackPoints[i].y-m_trackPoints[2000].y;
			curtrack.push_back(ptshow);
		}
		m_DataShowPro.UpdatePointLayer("RoadLane0",curtrack);
		PostMessage(WM_PAINT,0,0);
	}
	else if (nIDEvent==4)
	{
		//CMainFrame *pMain=(CMainFrame*)AfxGetMainWnd();
		CShowPathPlanDlg *pDlg=pMain->m_pMapShowDlg;
		if (pDlg!=NULL)
		{
#pragma region 局部坐标显示

			//高斯转局部的方式
			OriginPt oriPt;
			oriPt.X=m_velodynegrid.position[0];
			oriPt.Y=m_velodynegrid.position[1];
			oriPt.Angle=m_velodynegrid.orientation[0]*M_PI/180;
			double OutX,OutY;


			//上一次的规划路径
			vector<PointShow> lastPathPoints;
			int lastLength=m_DataCenter.m_dataPath.GetlastpathLength();
			RoadPoint* lastPath=m_DataCenter.m_dataPath.GetLastGauPath();
			for (int i=0;i<lastLength;i++)
			{
				PointShow pathPoint;
				ckCoordinate::WorldtoMap(oriPt,lastPath[i].x,lastPath[i].y,OutX,OutY);
				pathPoint.x=OutX;
				pathPoint.y=OutY;
				pathPoint.z=0;

				lastPathPoints.push_back(pathPoint);
			}
			pDlg->m_DataShowPro.UpdatePointLayer("LastPath",lastPathPoints);


			////获得局部规划的路径
			vector<PointShow> localPath;
			int pathLength=m_DataCenter.m_dataPath.GetFinalpathLength();
			RoadPoint* finalpath=m_DataCenter.m_dataPath.GetGauPath();
			//RoadPoint* finalpath=m_DataCenter.m_dataPath.GetFinalPath();
			for (int i=0;i<pathLength;i++)
			{
				PointShow pathPoint;
				ckCoordinate::WorldtoMap(oriPt,finalpath[i].x,finalpath[i].y,OutX,OutY);
				pathPoint.x=OutX;
				pathPoint.y=OutY;
				pathPoint.z=0;

				localPath.push_back(pathPoint);
			}

			pDlg->m_DataShowPro.UpdatePointLayer("LocalPath",localPath);


			////更新激光网格显示
			vector<PointShow> velodynePoint;
			
			int* obsGrid = m_DataCenter.m_dataPath.GetObsGrid();
			for (int i=0;i<20000;i++)
			{
				PointShow veloPt;
				int I,J;
				if (obsGrid[i]==1)
				{
					I=(i)%100;
					J=(i)/100;
					veloPt.x=0.2*(I-100/2);
					veloPt.y=0.2*J;
					veloPt.z=0;
					velodynePoint.push_back(veloPt);
				}
			}
			
			pDlg->m_DataShowPro.UpdatePointLayer("VeloLayer",velodynePoint);


			//显示GPS轨迹点
			RoadPoint* gpsTrackPoints=m_DataCenter.m_dataPath.GetGPSRoadDataBuffer();
			int trackLength=m_DataCenter.m_dataPath.GetGPSTrackLength();
			int beginGPSPos=m_DataCenter.m_dataPath.GetBeginGPSPos();

			if (gpsTrackPoints!=NULL && trackLength>0)
			{
				vector<PointShow> roadGPSPoints;

				int beginI,endI;
				beginI=(beginGPSPos-100)>0?(beginGPSPos-100):0;
				endI=(beginGPSPos+100)<trackLength?(beginGPSPos+100):trackLength;

				for (int i=beginI;i<endI;i++)
				//for (int i=0;i<trackLength;i++)
				{
					ckCoordinate::WorldtoMap(oriPt,gpsTrackPoints[i].x,gpsTrackPoints[i].y,OutX,OutY);
					PointShow roadPoint;
					roadPoint.x=OutX;
					roadPoint.y=OutY;
					roadPoint.z=0;

					roadGPSPoints.push_back(roadPoint);

				}
				pDlg->m_DataShowPro.UpdatePointLayer("RoadNet",roadGPSPoints);
			}


			////显示路网点
			//RoadNet* roadNet = m_DataCenter.m_dataPath.GetRoadNetBuffer();
			//if (roadNet!=NULL)
			//{
			//	vector<PointShow> roadNetPoints;
			//	int count=0;
			//	double outX,outY;
			//	while (roadNet[count].roadprop!=1)
			//	{
			//		ckCoordinate::WorldtoMap(oriPt,roadNet[count].x,roadNet[count].y,outX,outY);
			//		PointShow roadPoint;
			//		roadPoint.x=outX;
			//		roadPoint.y=outY;
			//		roadPoint.z=0;
			//
			//		roadNetPoints.push_back(roadPoint);
			//
			//		count++;
			//
			//	}
			//	ckCoordinate::WorldtoMap(oriPt,roadNet[m_DataCenter.m_dataPath.GetNetNum()+1].x,roadNet[m_DataCenter.m_dataPath.GetNetNum()+1].y,outX,outY);
			//	PointShow roadPoint;
			//	roadPoint.x=outX;
			//	roadPoint.y=outY;
			//	roadPoint.z=0;
			//	roadNetPoints.push_back(roadPoint);
			//
			//	pDlg->m_DataShowPro.UpdatePointLayer("RoadNet",roadNetPoints);
			//}

#pragma endregion 局部坐标显示

#pragma region 高斯坐标显示
			//OriginPt oriPt;
			//oriPt.X=m_velodynegrid.position[0];
			//oriPt.Y=m_velodynegrid.position[1];
			//oriPt.Angle=m_velodynegrid.orientation[0]*M_PI/180;
			//double OutX,OutY;





			//////上一次的规划路径
			////vector<PointShow> lastPathPoints;
			////int lastLength=m_DataCenter.m_dataPath.GetlastpathLength();
			////RoadPoint* lastPath=m_DataCenter.m_dataPath.GetLastGauPath();
			////for (int i=0;i<lastLength;i++)
			////{
			////	PointShow pathPoint;
			////	pathPoint.x=lastPath[i].x-oriPt.X;
			////	pathPoint.y=lastPath[i].y-oriPt.Y;
			////	pathPoint.z=0;

			////	lastPathPoints.push_back(pathPoint);
			////}
			////pDlg->m_DataShowPro.UpdatePointLayer("LastPath",lastPathPoints);

			//////获得局部规划的路径
			//vector<PointShow> localPath;
			//int pathLength=m_DataCenter.m_dataPath.GetFinalpathLength();
			//RoadPoint* finalpath=m_DataCenter.m_dataPath.GetGauPath();
			////RoadPoint* finalpath=m_DataCenter.m_dataPath.GetFinalPath();
			//for (int i=0;i<pathLength;i++)
			//{
			//	PointShow pathPoint;
			//	pathPoint.x=finalpath[i].x-oriPt.X;
			//	pathPoint.y=finalpath[i].y-oriPt.Y;
			//	pathPoint.z=0;

			//	localPath.push_back(pathPoint);
			//}

			//pDlg->m_DataShowPro.UpdatePointLayer("LocalPath",localPath);


			////更新激光网格显示
			//vector<PointShow> velodynePoint;

			////unsigned char* obsGrid = m_DataCenter.m_dataPath.GetObsGrid();
			////for (int i=0;i<20000;i++)
			////{
			////	PointShow veloPt;
			////	int I,J;
			////	if (obsGrid[i]==1)
			////	{
			////		I=(i)%100;
			////		J=(i)/100;
			////		veloPt.x=0.2*(I-100/2);
			////		veloPt.y=0.2*J;
			////		veloPt.z=0;
			////		velodynePoint.push_back(veloPt);
			////	}
			////}

			//for (int i=0;i<m_velodynegrid.grid_nums;i++)
			//{
			//	PointShow veloPt1,veloPt2;
			//	int I,J;//网格列号I和行号J
			//	if ((m_velodynegrid.velo_grid[i]>>4)&1)
			//	{
			//		I=(2*i)%m_velodynegrid.width;
			//		J=(2*i)/m_velodynegrid.width;
			//		veloPt1.x=m_velodynegrid.wid_step*(I-m_velodynegrid.width/2);
			//		veloPt1.y=m_velodynegrid.height_step*J;

			//		ckCoordinate::MaptoWorld(oriPt,veloPt1.x,veloPt1.y,OutX,OutY);
			//		veloPt1.x=OutX-oriPt.X;
			//		veloPt1.y=OutY-oriPt.Y;

			//		veloPt1.z=0;
			//		velodynePoint.push_back(veloPt1);
			//	}

			//	if (m_velodynegrid.velo_grid[i]&1)
			//	{
			//		I=(2*i+1)%m_velodynegrid.width;
			//		J=(2*i+1)/m_velodynegrid.width;
			//		veloPt2.x=m_velodynegrid.wid_step*(I-m_velodynegrid.width/2);
			//		veloPt2.y=m_velodynegrid.height_step*J;

			//		ckCoordinate::MaptoWorld(oriPt,veloPt2.x,veloPt2.y,OutX,OutY);
			//		veloPt2.x=OutX-oriPt.X;
			//		veloPt2.y=OutY-oriPt.Y;

			//		veloPt2.z=0;
			//		velodynePoint.push_back(veloPt2);
			//	}
			//}
			//pDlg->m_DataShowPro.UpdatePointLayer("VeloLayer",velodynePoint);



			////显示GPS轨迹点
			//RoadPoint* gpsTrackPoints=m_DataCenter.m_dataPath.GetGPSRoadDataBuffer();
			//int trackLength=m_DataCenter.m_dataPath.GetGPSTrackLength();
			//int beginGPSPos=m_DataCenter.m_dataPath.GetBeginGPSPos();

			//if (gpsTrackPoints!=NULL)
			//{
			//	vector<PointShow> roadGPSPoints;
			//	//OriginPt oriPt;
			//	//oriPt.X=m_velodynegrid.position[0];
			//	//oriPt.Y=m_velodynegrid.position[1];
			//	//oriPt.Angle=m_velodynegrid.orientation[0]*M_PI/180;

			//	//oriPt.X=m_DataCenter.m_dataPath.GetCurrentPoint().x;
			//	//oriPt.Y=m_DataCenter.m_dataPath.GetCurrentPoint().y;
			//	//oriPt.Angle=m_DataCenter.m_dataPath.GetCurrentPoint().angle;
			//	//double outX,outY;

			//	//int beginI,endI;
			//	//beginI=(beginGPSPos-20)>0?(beginGPSPos-20):0;
			//	//endI=(beginGPSPos+20)<trackLength?(beginGPSPos+20):trackLength;

			//	//for (int i=beginI;i<endI;i++)
			//	for (int i=0;i<trackLength;i++)
			//	{
			//		//ckCoordinate::WorldtoMap(oriPt,gpsTrackPoints[i].x,gpsTrackPoints[i].y,outX,outY);
			//		PointShow roadPoint;
			//		roadPoint.x=gpsTrackPoints[i].x-oriPt.X;
			//		roadPoint.y=gpsTrackPoints[i].y-oriPt.Y;
			//		roadPoint.z=0;

			//		roadGPSPoints.push_back(roadPoint);

			//	}
			//	pDlg->m_DataShowPro.UpdatePointLayer("RoadNet",roadGPSPoints);
			//}


			//////显示路网点
			////RoadNet* roadNet = m_DataCenter.m_dataPath.GetRoadNetBuffer();
			////if (roadNet!=NULL)
			////{
			////	vector<PointShow> roadNetPoints;
			////	int count=0;
			////	//OriginPt oriPt;
			////	//oriPt.X=m_velodynegrid.position[0];
			////	//oriPt.Y=m_velodynegrid.position[1];
			////	//oriPt.Angle=m_velodynegrid.orientation[0]*M_PI/180;
			////	//double outX,outY;
			////	while (roadNet[count].roadprop!=1)
			////	{
			////		//ckCoordinate::WorldtoMap(oriPt,roadNet[count].x,roadNet[count].y,outX,outY);
			////		PointShow roadPoint;
			////		roadPoint.x=roadNet[count].x-oriPt.X;
			////		roadPoint.y=roadNet[count].y-oriPt.Y;
			////		roadPoint.z=0;

			////		roadNetPoints.push_back(roadPoint);

			////		count++;

			////	}
			////	//ckCoordinate::WorldtoMap(oriPt,roadNet[m_DataCenter.m_dataPath.GetNetNum()+1].x,roadNet[m_DataCenter.m_dataPath.GetNetNum()+1].y,outX,outY);
			////	PointShow roadPoint;
			////	roadPoint.x=roadNet[m_DataCenter.m_dataPath.GetNetNum()+1].x-oriPt.X;
			////	roadPoint.y=roadNet[m_DataCenter.m_dataPath.GetNetNum()+1].y-oriPt.Y;
			////	roadPoint.z=0;
			////	roadNetPoints.push_back(roadPoint);

			////	pDlg->m_DataShowPro.UpdatePointLayer("RoadNet",roadNetPoints);
			////}

#pragma endregion 高斯坐标显示

			::PostMessage(pDlg->GetSafeHwnd(),WM_PAINT,0,0);
		}


	}
	CFormView::OnTimer(nIDEvent);
}
void CSmartVII_FasterView::InitialSideStop()
{
	// TODO: 在此添加控件通知处理程序代码
	GpsLine ru_position,ting_position,chu_position;
	ru_position.Gnx=120.773133		;ru_position.Gny=31.592423					;ru_position.azimuth=236.395577				;
	ting_position.Gnx=120.772991		;ting_position.Gny=31.592415		;ting_position.azimuth=247.093857		;
	chu_position.Gnx=120.772900		;chu_position.Gny=31.592325		;chu_position.azimuth=232.195764	;
	ckCoordinate::LongLat2XY(ru_position.Gnx,ru_position.Gny,ru_position.Gnx,ru_position.Gny);
	ckCoordinate::LongLat2XY(ting_position.Gnx,ting_position.Gny,ting_position.Gnx,ting_position.Gny);
	ckCoordinate::LongLat2XY(chu_position.Gnx,chu_position.Gny,chu_position.Gnx,chu_position.Gny);

	ru_position.azimuth=ru_position.azimuth*PI/180;
	ting_position.azimuth=ting_position.azimuth*PI/180;
	chu_position.azimuth=chu_position.azimuth*PI/180;
	RoadPoint my_xy1[110],my_xy2[110];
	double k=0,dk=0,L=0;
        m_DataCenter.m_dataPath.buildClothoid(k,dk,L,ru_position.Gny,ru_position.Gnx,ru_position.azimuth,ting_position.Gny,ting_position.Gnx,ting_position.azimuth,0.05);
		m_DataCenter.m_dataPath.pointsOnClothoid(my_xy1,ru_position.Gny,ru_position.Gnx,ru_position.azimuth,k,dk,L,CLOTHOIDPOINTNUM);
		m_DataCenter.m_dataPath.buildClothoid(k,dk,L,ting_position.Gny,ting_position.Gnx,ting_position.azimuth,chu_position.Gny,chu_position.Gnx,chu_position.azimuth,0.05);
		m_DataCenter.m_dataPath.pointsOnClothoid(my_xy2,ting_position.Gny,ting_position.Gnx,ting_position.azimuth,k,dk,L,CLOTHOIDPOINTNUM);
		for (int i=0;i<CLOTHOIDPOINTNUM;i++)      
		{
			GpsLine pt;
			pt.Gnx=my_xy1[i].y;
			pt.Gny=my_xy1[i].x;
			pt.azimuth=my_xy1[i].angle;
			m_DataCenter.m_sideWay.push_back(pt);              
		}
		for (int i=0;i<CLOTHOIDPOINTNUM;i++)      
		{
			GpsLine pt;
			pt.Gnx=my_xy2[i].y;
			pt.Gny=my_xy2[i].x;
			pt.azimuth=my_xy2[i].angle;
			m_DataCenter.m_sideWay.push_back(pt);
		}
	
}
//打开所有设备串口

void CSmartVII_FasterView::OnAllconnect()
{
	// TODO: 在此添加命令处理程序代码
	m_DataCenter.InitialLocationLcm();
	if (!m_DataCenter.ConnectAllSensor())
	{
	//	AfxMessageBox("设备启动不成功");
		return;
	}
	InitialSideStop();
	//OneKeyRead();
	CMainFrame*pMain=(CMainFrame*)AfxGetMainWnd();
    pMain->m_pStatusDlg->SetWnd(this->GetSafeHwnd());
	m_bAllOpen = TRUE;
}



void CSmartVII_FasterView::OnClosecan()
{
	// TODO: 在此添加命令处理程序代码
   m_DataCenter.DestoryCanPro();
   KillTimer(1);
	m_bCanOpen = !m_bCanOpen;
}



void CSmartVII_FasterView::OnClosegps()
{
	// TODO: 在此添加命令处理程序代码
	BOOL flag=m_DataCenter.GetGpsPortStatus();
	if (flag)
	{
		m_DataCenter.StopGpsPro();
	}
	m_bGpsOpen = !m_bGpsOpen;
}


void CSmartVII_FasterView::OnDatashow()
{
	// TODO: 在此添加命令处理程序代码

	SetTimer(2,100,0);
	m_bmapLoad=TRUE;
	//if (!m_bDataShowFlag)
	//{
	//	SetTimer(2,200,0);
	//}
	//else
	//{
	//	KillTimer(2);
	//}
	//m_bDataShowFlag = !m_bDataShowFlag;

}

void CSmartVII_FasterView::OnUpdateClosegps(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (!m_bAllOpen) 
		pCmdUI->Enable(FALSE);
	else
	pCmdUI->Enable(m_bGpsOpen);
}

 //更新按钮状态
void CSmartVII_FasterView::OnUpdateCanconnect(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (!m_bAllOpen) pCmdUI->Enable(FALSE);
	else
	pCmdUI->Enable(!m_bCanOpen);
}


void CSmartVII_FasterView::OnUpdateClosecan(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (!m_bAllOpen) pCmdUI->Enable(FALSE);
	else
	pCmdUI->Enable(m_bCanOpen);
}


void CSmartVII_FasterView::OnUpdateAllconnect(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(!m_bAllOpen);
}


void CSmartVII_FasterView::OnLidaropen()
{
	// TODO: 在此添加命令处理程序代码
		//m_DataCenter.LoadSensorData();
	//_beginthread(PortOpenThread,0,this);
		/*CPortConnectDlg dlg;
		dlg.m_nPos=&m_DataCenter.m_nActivateStep;
		dlg.m_ctlProgress.SetRange(0,7);
		dlg.DoModal();
		Invalidate(TRUE);*/
		//CMainFrame *pMain=(CMainFrame*)AfxGetMainWnd();
		if (!m_DataCenter.m_bLidarOn)
		{
			m_DataCenter.ActiveLidarPro();
		    //pMain->m_wndRibbonBar.FindByID(ID_LIDAROPEN)->SetText("关闭激光");
		}
		//else
		//{
		//	m_DataCenter.DestoryLidarPro();
		//	pMain->m_wndRibbonBar.FindByID(ID_LIDAROPEN)->SetText("开启激光");
		//}
		m_bLidarOpen = !m_bLidarOpen;
		//SetTimer(2,50,0);
}


void CSmartVII_FasterView::OnUpdateDatashow(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (!m_bDataShowFlag)
	{
		pCmdUI->SetText("显示网格");
	}
	else
	{
		pCmdUI->SetText("关闭显示");
	}
	
}


void CSmartVII_FasterView::OnUpdateLidaropen(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
		if (!m_bAllOpen) pCmdUI->Enable(FALSE);
		else
		pCmdUI->Enable(!m_bLidarOpen);
}


void CSmartVII_FasterView::OnLidarclose()
{
	// TODO: 在此添加命令处理程序代码
		m_bLidarOpen = !m_bLidarOpen;
		KillTimer(2);
}


void CSmartVII_FasterView::OnUpdateLidarclose(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
		if (!m_bAllOpen) pCmdUI->Enable(FALSE);
		else
		pCmdUI->Enable(m_bLidarOpen);
}

LRESULT CSmartVII_FasterView::OnSpeedUp(WPARAM w, LPARAM l)
{
	unsigned short temp = m_DataCenter.GetCurrentSpeed()+1000;
	m_DataCenter.CarSpeedControlled(0,temp);
	return 1;
}

LRESULT CSmartVII_FasterView::OnSpeedDown(WPARAM w, LPARAM l)
{
	unsigned short temp = m_DataCenter.GetCurrentSpeed()-1000;
	m_DataCenter.CarSpeedControlled(0,temp);
	return 1;
}

LRESULT CSmartVII_FasterView::OnAngleLeft(WPARAM w, LPARAM l)
{
	unsigned short temp = m_DataCenter.GetCurrentAngle()-20;
	m_DataCenter.CarAngleControlled(temp);
	return 1;
}

LRESULT CSmartVII_FasterView::OnAngleRight(WPARAM w, LPARAM l)
{
	unsigned short temp = m_DataCenter.GetCurrentAngle()+20;
	m_DataCenter.CarAngleControlled(temp);
	return 1;
}

LRESULT CSmartVII_FasterView::OnBrake3(WPARAM w, LPARAM l)
{
	unsigned short temp = m_DataCenter.GetCurrentSpeed();
	m_DataCenter.CarSpeedControlled(3,temp);
	return 1;
}

LRESULT CSmartVII_FasterView::OnBrake4(WPARAM w, LPARAM l)
{
	unsigned short temp = m_DataCenter.GetCurrentSpeed();
	m_DataCenter.CarSpeedControlled(4,temp);
	return 1;
}
LRESULT CSmartVII_FasterView::OnBrakeu(WPARAM w, LPARAM l)
{
	//unsigned short temp = m_DataCenter.GetCurrentSpeed();
	m_DataCenter.CarSpeedControlled(4,0);
	return 1;
}
void CSmartVII_FasterView::OnOrtho()
{
	// TODO: 在此添加命令处理程序代码
	m_DataShowPro.SetView(0);
	PostMessage(WM_PAINT,0,0);
	//m_bOrtho = !m_bOrtho;
}


BOOL CSmartVII_FasterView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//return TRUE;
	return CFormView::OnEraseBkgnd(pDC);
}


//void CSmartVII_FasterView::OnFileopen()
//{
//	// TODO: 在此添加命令处理程序代码
//	m_OfflineLidar.ReadFile();
//	m_bOfflineLidarFlag = TRUE;
//}



void CSmartVII_FasterView::OnPlanmapopen()
{
	m_DataCenter.ShowPlanPath();
}
void CSmartVII_FasterView::OnPlanstart()
{
	m_DataCenter.StartPathPlan();

}

BOOL CSmartVII_FasterView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CFormView::PreCreateWindow(cs);
}

void CSmartVII_FasterView::OnBtnvelo()
{
	// TODO: 在此添加命令处理程序代码
	//m_DataCenter.StartVelodyneListen();
	if (!m_bVelodyneLcm)
	{
	  BOOL res=	m_DataCenter.StartVelodyneLcm();
	  if (res)
	  {
		  m_bVelodyneLcm=TRUE;
	  }
	}
	else
	{
	  MessageBox("Velodyne已打开");
	}
   
}



void CSmartVII_FasterView::OnButton2()
{
	// TODO: 在此添加命令处理程序代码
	m_DataCenter.CarSpeedControlled(3,0);
	Sleep(2000);
	m_DataCenter.CarSpeedControlled(5,0);
	Sleep(5);
}


void CSmartVII_FasterView::OnUpdateLaneopen(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
}








void CSmartVII_FasterView::OnLastmap()
{
	// TODO: 在此添加命令处理程序代码
}


void CSmartVII_FasterView::OnCreateckmap()
{
	
}




void CSmartVII_FasterView::OnCheckline()
{
	// TODO: 在此添加命令处理程序代码
	if (!m_bLayers[0])
	{
		m_DataShowPro.SetLayerStatus("CKMAPLINE",true);
		m_bLayers[0]=TRUE;
	}
	else
	{
		m_DataShowPro.SetLayerStatus("CKMAPLINE",false);
		m_bLayers[0]=FALSE;
	}
	PostMessage(WM_PAINT,0,0);
	
}


void CSmartVII_FasterView::OnChecklane()
{
	// TODO: 在此添加命令处理程序代码
	if (!m_bLayers[1])
	{
		m_DataShowPro.SetLayerStatus("CKMAPROADLANE",true);
		m_bLayers[1]=TRUE;
	} 
	else
	{
		m_DataShowPro.SetLayerStatus("CKMAPROADLANE",false);
		m_bLayers[1]=FALSE;
	}
	PostMessage(WM_PAINT,0,0);
}


void CSmartVII_FasterView::OnChecktrack()
{
	// TODO: 在此添加命令处理程序代码	
	if (!m_bLayers[2])
	{
		m_DataShowPro.SetLayerStatus("CKTRACKLANE",true);
		m_bLayers[2]=TRUE;
	} 
	else
	{
		m_DataShowPro.SetLayerStatus("CKTRACKLANE",false);
		m_bLayers[2]=FALSE;
	}
	PostMessage(WM_PAINT,0,0);
}





void CSmartVII_FasterView::OnChecknum()
{
	// TODO: 在此添加命令处理程序代码
	if (!m_bLayers[3])
	{
		m_DataShowPro.SetLayerStatus("CKMAPINDEX",true);
		m_bLayers[3]=TRUE;
	} 
	else
	{
		m_DataShowPro.SetLayerStatus("CKMAPINDEX",false);
		m_bLayers[3]=FALSE;
	}
	PostMessage(WM_PAINT,0,0);
}


void CSmartVII_FasterView::OnUpdateCheckline(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_bmapLoad);
	pCmdUI->SetCheck(m_bLayers[0]);
}


void CSmartVII_FasterView::OnUpdateChecklane(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_bmapLoad);
	pCmdUI->SetCheck(m_bLayers[1]);
}


void CSmartVII_FasterView::OnUpdateChecktrack(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_bmissionLoad);
	pCmdUI->SetCheck(m_bLayers[2]);
}


void CSmartVII_FasterView::OnUpdateChecknum(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_bmapLoad);
	pCmdUI->SetCheck(m_bLayers[3]);
}


void CSmartVII_FasterView::OnSensorpos()
{
	// TODO: 在此添加命令处理程序代码
	m_DataCenter.InitialLocationLcm();
	GpsLine pos;
	pos.Gnx=50000.34;
	pos.Gny=3233234325.22;
	m_DataCenter.SendLcmPos(pos);
}



