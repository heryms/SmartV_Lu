// MainControlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SmartVII_Faster.h"
#include "MainControlDlg.h"
#include "afxdialogex.h"


// CMainControlDlg 对话框

IMPLEMENT_DYNAMIC(CMainControlDlg, CDialogEx)

CMainControlDlg::CMainControlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainControlDlg::IDD, pParent)
{
	//显示每部分的开关
	m_bDrawPoints=TRUE;

	// 视点选择的位置
	m_eyex=0,  m_eyey=0,  m_eyez=80; 
	m_centerx=0,  m_centery=0, m_centerz=0; 
	m_upx=0,  m_upy=1, m_upz=0;

	///////////Snap Screen //////////
	m_bSnapScreen= FALSE;
	///////////////////////////////

	// OpenGL
	m_hGLContext = NULL;
	m_GLPixelIndex = 0;


	//记录模型平移
	x_move=0;
	y_move=0;		
	//记录模型旋转
	x_rotate=0;
	y_rotate=0;
	z_rotate=0;  
	//缩放比例

	m_zoom=1;				

	//记录鼠标按下时的坐标位置
	x_lbefore=0,y_lbefore=0;          
	x_rbefore=0,y_rbefore=0;
	z_before1=0,z_before2=0;

	m_NumPixel= 5;
}

CMainControlDlg::~CMainControlDlg()
{
}

void CMainControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainControlDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
	
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CMainControlDlg 消息处理程序
BOOL CMainControlDlg::SetWindowPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pixelDesc;
	
	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion = 1;
	
	pixelDesc.dwFlags = \
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER | PFD_STEREO_DONTCARE;
	
	pixelDesc.iPixelType = PFD_TYPE_RGBA;
	pixelDesc.cColorBits = 32;
	pixelDesc.cRedBits = 8;
	pixelDesc.cRedShift = 16;
	pixelDesc.cGreenBits = 8;
	pixelDesc.cGreenShift = 8;
	pixelDesc.cBlueBits = 8;
	pixelDesc.cBlueShift = 0;
	pixelDesc.cAlphaBits = 0;
	pixelDesc.cAlphaShift = 0;
	pixelDesc.cAccumBits = 64;
	pixelDesc.cAccumRedBits = 16;
	pixelDesc.cAccumGreenBits = 16;
	pixelDesc.cAccumBlueBits = 16;
	pixelDesc.cAccumAlphaBits = 0;
	pixelDesc.cDepthBits = 32;
	pixelDesc.cStencilBits = 8;
	pixelDesc.cAuxBuffers = 0;
	pixelDesc.iLayerType = PFD_MAIN_PLANE;
	pixelDesc.bReserved = 0;
	pixelDesc.dwLayerMask = 0;
	pixelDesc.dwVisibleMask = 0;
	pixelDesc.dwDamageMask = 0;
	
	m_GLPixelIndex = ChoosePixelFormat(hDC,&pixelDesc);
	if(m_GLPixelIndex == 0) // Choose default
	{
		m_GLPixelIndex = 1;
		if(DescribePixelFormat(hDC,m_GLPixelIndex,
			sizeof(PIXELFORMATDESCRIPTOR),&pixelDesc)==0)
			return FALSE;
	}
	
	if(!SetPixelFormat(hDC,m_GLPixelIndex,&pixelDesc))
		return FALSE;
	SetTimer(2,10,0);
	return TRUE;
}


//********************************************
// CreateViewGLContext
// Create an OpenGL rendering context
//********************************************
BOOL CMainControlDlg::CreateViewGLContext(HDC hDC)
{
	m_hGLContext = wglCreateContext(hDC);
	
	if(m_hGLContext==NULL)
		return FALSE;
	
	if(wglMakeCurrent(hDC,m_hGLContext)==FALSE)
		return FALSE;
	
	return TRUE;
}
void CMainControlDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	x_rbefore=point.x;
	y_rbefore=point.y;
	x_rotate_save=x_rotate;
	y_rotate_save=y_rotate;
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CMainControlDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CMainControlDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	// 输出坐标位置，为以后观测数据做准备
	CDialogEx::OnMouseMove(nFlags, point);
}


BOOL CMainControlDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	bool zoomin=zDelta>0;
	float zoomratio=zDelta/1200.0;
	//TRACE("OnMouseWheel %f\n",zoomratio);
	if(zoomin>0)
	{
		m_zoom=m_zoom*(1+zoomratio);
	}
	else
	{
		m_zoom=m_zoom/(1-zoomratio);
	}
	this->RenderScene();
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CMainControlDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	RenderScene();
	// Do not call CDialogEx::OnPaint() for painting messages
}
void CMainControlDlg::RenderScene()
{
	CPaintDC dc(this); // device context for painting
	HWND hWnd = this->GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);

	wglMakeCurrent(NULL, NULL);
	wglMakeCurrent(hDC,	m_hGLContext);

	//清除颜色缓冲和深度缓冲
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glViewport(0,0,winX,winY);
	glLoadIdentity();
	/////////////处理各种变换///////////////
	gluLookAt(m_eyex,     m_eyey,      m_eyez,  
		m_centerx,  m_centery,   m_centerz,
		m_upx,      m_upy,       m_upz);

	glRotatef(x_rotate,1,0,0);
	glRotatef(y_rotate,0,1,0);
	glRotatef(z_rotate,0,0,1);

	glTranslatef(x_move,y_move,0);
	glScalef(m_zoom, m_zoom, m_zoom);

	// 开始画图
	Draw_compass(20,-15,0.0); // 罗盘
	Draw_grid(5); // 网格
	//Draw_Road_Center_Point();
	SwapBuffers(dc.m_ps.hdc);
	::ReleaseDC(hWnd,hDC);
	wglMakeCurrent(NULL,NULL);
}

void CMainControlDlg::SetOneViewFocus(int index)
{
	//记录模型平移
	//x_move=m_ViewfocusM[index].x_move;
	//y_move=m_ViewfocusM[index].y_move;
	////缩放比例
	//m_zoom=m_ViewfocusM[index].m_zoom;
	////记录模型旋转
	//x_rotate=m_ViewfocusM[index].x_rotate;
	//y_rotate=m_ViewfocusM[index].y_rotate;
	//z_rotate=m_ViewfocusM[index].z_rotate;
}



void CMainControlDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	x_lbefore=point.x;
	y_lbefore=point.y;
	x_move_save=x_move;
	y_move_save=y_move;
	CDialogEx::OnRButtonDown(nFlags, point);
}


void CMainControlDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnRButtonUp(nFlags, point);
}


void CMainControlDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	//SendMessage(WM_PAINT,0,0);
	//RenderScene();
	GLdouble aspect;
	GLsizei width,height;
	width = cx;
	height = cy;
	winX=cx;
	winY=cy;
	if(cy==0)
		aspect = (GLdouble)width;
	else
		aspect = (GLdouble)width/(GLdouble)height;
	glMatrixMode(GL_PROJECTION);	    		// Select The Projection Matrix
	glLoadIdentity();							// Reset The Projection Matrix
	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f, aspect, 0.1f, 1000.0f);   //试试删掉这句话看看！ 
	//glOrtho(-50,50,-50,50,0.1,1000);
	glMatrixMode(GL_MODELVIEW);					// Select The Modelview Matrix
	glLoadIdentity();
	this->RenderScene();
	//RenderScene();
	// TODO: Add your message handler code here
	//glViewport(0,0,cx,cy);	


}


void CMainControlDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	//Invalidate(TRUE);
	InvalidateRect(NULL,FALSE);
	CDialogEx::OnTimer(nIDEvent);
}


int CMainControlDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);

	if(SetWindowPixelFormat(hDC)==FALSE)
		return 0;

	if(CreateViewGLContext(hDC)==FALSE)
		return 0;
	SetTimer(0,10,NULL);
	// TODO:  Add your specialized creation code here
	return 0;
}
