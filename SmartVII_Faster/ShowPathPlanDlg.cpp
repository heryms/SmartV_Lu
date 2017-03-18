// ShowPathPlan.cpp : 实现文件

#include "stdafx.h"
#include "SmartVII_Faster.h"
#include "ShowPathPlanDlg.h"
#include "afxdialogex.h"



// ShowPathPlan 对话框

IMPLEMENT_DYNAMIC(CShowPathPlanDlg, CDialogEx)

CShowPathPlanDlg::CShowPathPlanDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CShowPathPlanDlg::IDD, pParent)
{
	IsWindowOpen=0;
	IsWindowCreate=0;

	//m_DataShowPro.camPos[2]=-480.0f;
	m_DataShowPro.camRot[0]=0.0f;
	m_RoadDataBuffer=NULL;
	m_RoadNetBuffer=NULL;
	bNetFlag=FALSE;
	m_nNetLength=0;
//	m_ultiPath=new RoadPoint[120];
}

CShowPathPlanDlg::~CShowPathPlanDlg()
{
//	delete[] m_ultiPath;
	if (m_RoadDataBuffer!=NULL)
	{
		delete[] m_RoadDataBuffer;
		m_RoadDataBuffer=NULL;
	}
	if (m_RoadNetBuffer!=NULL)
	{
		delete[] m_RoadNetBuffer;
		m_RoadNetBuffer=NULL;
	}
	
}

void CShowPathPlanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CShowPathPlanDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEHWHEEL()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


// ShowPathPlan 消息处理程序

void CShowPathPlanDlg::Draw_NetPoint(float radious ,RoadNet netpoint)
{
	GLfloat step = 0.1f;
	GLfloat angle;
	glLineWidth(1);
	if (netpoint.roadprop==0)
	{
		glColor3f(0,1,0);
	}
	else if (netpoint.roadprop==1)
	{
		glColor3f(1,0,0);

	}
	else if(netpoint.roadprop==2)
	{
		glColor3f(1,1,0);
	}
	else if(netpoint.roadprop==3)
	{
		glColor3f(0,0,1);
	}
	else
	{
		glColor3f(0.5,1,1);
	}
	glBegin(GL_LINE_STRIP);
	double delx=0,dely=0;
	GussiantoCar(netpoint.x,netpoint.y,&delx,&dely);
	for (angle = 0.0f;angle <= 2.0f*3.1415926f;angle += step)
	{

		glVertex3f((radious)*sin(angle)+delx,(radious)*cos(angle)+dely,0);
	}
	glEnd();
}
void CShowPathPlanDlg::OnPaint()
{
	//if (flag1!=10)
	//{
	//	return;
	//}

	CPaintDC dc(this); // device context for painting

	HWND hWnd = this->GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);
	wglMakeCurrent(NULL, NULL);
	wglMakeCurrent(hDC,	m_DataShowPro.m_hGLContext);

	//绘图区域，图层管理
	m_DataShowPro.InitialScene(dc,hDC);
	m_DataShowPro.Draw_grid(5);
	m_DataShowPro.Draw_coordinate();
	m_DataShowPro.Draw_circle(5,5);
	glPopMatrix();
	SwapBuffers(dc.m_ps.hdc);
	::ReleaseDC(hWnd,hDC);
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()
}



void CShowPathPlanDlg::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// 此功能要求 Windows Vista 或更高版本。
	// _WIN32_WINNT 符号必须 >= 0x0600。
	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	m_DataShowPro.OnMouseWheel(nFlags,zDelta,pt);

	CDialogEx::OnMouseHWheel(nFlags, zDelta, pt);
}


void CShowPathPlanDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	m_DataShowPro.OnLButtonDown(nFlags,point);
	//UpdateWindow();
	//SetCapture();
	//	PostMessage(WM_PAINT,0,0);
	CDialogEx::OnLButtonDown(nFlags, point);
}


BOOL CShowPathPlanDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
  m_DataShowPro.OnMouseWheel(nFlags,zDelta,pt);
	PostMessage(WM_PAINT,0,0);
	return TRUE;
	//return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CShowPathPlanDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值


	//m_DataShowPro.OnLButtonUp(nFlags,point);
	//ReleaseCapture();
	//PostMessage(WM_PAINT,0,0);
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CShowPathPlanDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	m_DataShowPro.OnRButtonDown(nFlags,point);
	//UpdateWindow();
	m_DataShowPro.m_nButtonFlag=rightDown;
	m_DataShowPro.OnRButtonDown(nFlags,point);
	SetCapture();
	CDialogEx::OnRButtonDown(nFlags, point);
}

void NavigationLayer(const vector<PointShow> ptcloud)
{
	glPointSize(5);
	glColor3f(1,1,0);
	glBegin(GL_POINTS);
	//glVertex3f(ptcloud[0].x,ptcloud[0].y,ptcloud[0].z);
	for (int i=0;i<ptcloud.size();i++)
	{
		glVertex3f(ptcloud[i].x,ptcloud[i].y,ptcloud[i].z);
	}
	glEnd();	
	glFlush();
}
void CarLayer(const vector<PointShow> ptcloud)
{
	glPointSize(10);
	glColor3f(1.0,0,1.0);
	glBegin(GL_POINTS);
	//glVertex3f(ptcloud[0].x,ptcloud[0].y,ptcloud[0].z);
	for (int i=0;i<ptcloud.size();i++)
	{
		glVertex3f(ptcloud[i].x,ptcloud[i].y,0);
	}
	glEnd();	
	glFlush();
}


void LocalPathLayer(const vector<PointShow> ptcloud)
{
	glPointSize(3);
	glColor3f(8,0,0);
	glBegin(GL_POINTS);
	//glVertex3f(ptcloud[0].x,ptcloud[0].y,ptcloud[0].z);
	for (int i=0;i<ptcloud.size();i++)
	{
		glVertex3f(ptcloud[i].x,ptcloud[i].y,ptcloud[i].z);
	}
	glEnd();
	glFlush();
}

void LastPathLayer(const vector<PointShow> ptcloud)
{
	glPointSize(2);
	glColor3f(8,8,8);
	glBegin(GL_POINTS);
	//glVertex3f(ptcloud[0].x,ptcloud[0].y,ptcloud[0].z);
	for (int i=0;i<ptcloud.size();i++)
	{
		glVertex3f(ptcloud[i].x,ptcloud[i].y,ptcloud[i].z);
	}
	glEnd();	
	glFlush();
}

void VeloLayer(const vector<PointShow> ptcloud)
{
	glPointSize(2);
	glColor3f(1,1,1);
	glBegin(GL_POINTS);
	//glVertex3f(ptcloud[0].x,ptcloud[0].y,ptcloud[0].z);
	for (int i=0;i<ptcloud.size();i++)
	{
		glVertex3f(ptcloud[i].x,ptcloud[i].y,ptcloud[i].z);
	}
	glEnd();	
	glFlush();
}

void RoadNetLayer(const vector<PointShow> ptcloud)
{
	glPointSize(3);
	glColor3f(0,0,5);
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);
	
	glBegin(GL_POINTS);
	//glVertex3f(ptcloud[0].x,ptcloud[0].y,ptcloud[0].z);
	for (int i=1;i<ptcloud.size()-1;i++)
	{
		glVertex3f(ptcloud[i].x,ptcloud[i].y,ptcloud[i].z);
	}
	glEnd();	

	glPointSize(25);
	glColor3f(6,0,6);
	glBegin(GL_POINTS);
	glVertex3f(ptcloud[0].x,ptcloud[0].y,ptcloud[0].z);
	glEnd();

	glPointSize(15);
	glColor3f(6,6,0);
	glBegin(GL_POINTS);
	glVertex3f(ptcloud[ptcloud.size()-2].x,ptcloud[ptcloud.size()-2].y,ptcloud[ptcloud.size()-2].z);
	glEnd();

	glPointSize(15);
	glColor3f(0,6,6);
	glBegin(GL_POINTS);
	glVertex3f(ptcloud[ptcloud.size()-1].x,ptcloud[ptcloud.size()-1].y,ptcloud[ptcloud.size()-1].z);
	glEnd();

	glFlush();

}
void RoadLaneLayer(const vector<PointShow> laneCloud)
{
	glPointSize(5);
	glColor3f(0,1,0);
	glBegin(GL_POINTS);
	//glVertex3f(ptcloud[0].x,ptcloud[0].y,ptcloud[0].z);
	for (int i=0;i<laneCloud.size();i++)
	{
		glVertex3f(laneCloud[i].x,laneCloud[i].y,laneCloud[i].z);
	}
	glEnd();
	glFlush();
}
int CShowPathPlanDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);
	m_DataShowPro.OnCreate(hWnd);
	m_DataShowPro.m_lfRange=50;
	// TODO:  在此添加您专用的创建代码
	m_DataShowPro.CreatePointLayer("NAVILINE",NavigationLayer);
	m_DataShowPro.CreatePointLayer("CURCAR",CarLayer);
	m_DataShowPro.CreatePointLayer("LocalPath",LocalPathLayer);//创建路径层
	m_DataShowPro.CreatePointLayer("LastPath",LastPathLayer);//创建上一路径层
	m_DataShowPro.CreatePointLayer("VeloLayer",VeloLayer);
	m_DataShowPro.CreatePointLayer("RoadNet",RoadNetLayer);
	m_DataShowPro.CreatePointLayer("RoadLane",RoadLaneLayer);
	return 0;
}


void CShowPathPlanDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	m_DataShowPro.OnSize(nType,cx,cy);
	// TODO: 在此处添加消息处理程序代码
}



void CShowPathPlanDlg::GussiantoCar(double Gx,double Gy,double *cx,double *cy)
{
	/*double dx=Gx-initialPoint.x;
	double dy=Gy-initialPoint.y;
	*cx=dx*cos(initialPoint.angle/180*PI)-dy*sin(initialPoint.angle/180*PI);
	*cy=dx*sin(initialPoint.angle/180*PI)+dy*cos(initialPoint.angle/180*PI);*/

	*cx=Gx-initialPoint.x;
	*cy=Gy-initialPoint.y;

	
}


void CShowPathPlanDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_DataShowPro.OnRButtonUp(nFlags,point);
    ReleaseCapture();
	PostMessage(WM_PAINT,0,0);
	CDialogEx::OnRButtonUp(nFlags, point);
}
