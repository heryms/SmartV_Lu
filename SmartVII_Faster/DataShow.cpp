#include "StdAfx.h"
#include "DataShow.h"
#define  _USE_MATH_DEFINES
#include <math.h>
#define PI 3.14159
static void
	draw_letter_N ()
{
	
	glBegin (GL_QUADS);

	glVertex3f (-1.0, 8.5, 0);
	glVertex3f (-0.5, 8.5, 0);
	glVertex3f (-0.5, 10.5, 0);
	glVertex3f (-1.0, 10.5, 0);

	glVertex3f (1.0, 8.5, 0);
	glVertex3f (0.5, 8.5, 0);
	glVertex3f (0.5, 10.5, 0);
	glVertex3f (1.0, 10.5, 0);

	glVertex3f (-0.5, 10.5, 0);
	glVertex3f (-1.0, 10.5, 0);
	glVertex3f (0.5, 8.5, 0);
	glVertex3f (1.0, 8.5, 0);

	glEnd ();
}




CDataShow::CDataShow(void)
{
	
	camPos[0]=0.0f;
	camPos[1]=0.0f;
	camPos[2]=0.0f;
	camRot[0]=0.0f;
	camRot[1]=0.0f;
	camRot[2]=0.0f;

	scenePos[0]=0.0f;
	scenePos[1]=0.0f;
	scenePos[2]=0.0f;
    sceneRot[0]=0.0f;
	sceneRot[1]=0.0f;
	sceneRot[2]=0.0f;

	vieeDis[0]=-100;
	vieeDis[1]=100;
	// 视点选择的位置
	eyePos[0]=0.0f;
	eyePos[1]=0.0f;
	eyePos[2]=70.0f;
	eyeCenter[0]=0.0f;
	eyeCenter[1]=0.0f;
	eyeCenter[2]=0.0f;
	eyeUp[0]=0.0f;
	eyeUp[1]=1.0f;
	eyeUp[2]=0.0f;

	m_lfRange=10000;
	mouseprevpoint.x=0;
	mouseprevpoint.y=0;
	mouserightdown=FALSE;
	mouseleftdown=FALSE;
	
	//显示每部分的开关
	m_bDrawPoints=TRUE;

	m_bIsCreate=FALSE;
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
	m_nButtonFlag=other;
	m_NumPixel= 5;
}

CDataShow::~CDataShow(void)
{
}
// CMainControlDlg 消息处理程序
BOOL CDataShow::SetWindowPixelFormat(HDC hDC)
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
	
	return TRUE;
}


//********************************************
// CreateViewGLContext
// Create an OpenGL rendering context
//********************************************
BOOL CDataShow::CreateViewGLContext(HDC hDC)
{
	m_hGLContext = wglCreateContext(hDC);
	
	if(m_hGLContext==NULL)
		return FALSE;
	
	if(wglMakeCurrent(hDC,m_hGLContext)==FALSE)
		return FALSE;
	
	return TRUE;  
}

void CDataShow::InitialScene(CPaintDC& dc,HDC hDC)
{
	glClearColor(0.0,0.0,0.078,1); //0.192 0.078

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();
	//画格网部分
	//开始画图
	//Draw_compass(20,-15,0.0); // 罗盘
	
	//glLoadIdentity();
	//gluLookAt(eyePos[0],eyePos[1],eyePos[2],eyeCenter[0],eyeCenter[1],eyeCenter[2],eyeUp[0],eyeUp[1],eyeUp[2]);
	::glPushMatrix();
	if (camPos[2]>0)
	{
		glScalef(camPos[2],camPos[2],0);
	}
	else
	{
		glScalef(1.0/(1-camPos[2]),1.0/(1-camPos[2]),0);
	}
	//控制缩放比例
	glTranslatef(camPos[0],camPos[1],0.0f);
	glRotatef(camRot[0],1.0f,0.0f,0.0f);
	glRotatef(camRot[1],0.0f,1.0f,0.0f);
	glRotatef(camRot[2],0.0f,0.0f,1.0f);
	//eyehig=camPos[2];
	//::glPushMatrix();
	//控制旋转参数
	//glTranslatef(scenePos[0],scenePos[1],scenePos[2]);	
	////OnMouseMoveRotate
	//glRotatef(sceneRot[0],1.0f,0.0f,0.0f);
	//glRotatef(sceneRot[1],0.0f,1.0f,0.0f);
	//glRotatef(sceneRot[2],0.0f,0.0f,1.0f);
	
	map<string,bool>::iterator itManger;
	//draw sensor layer
    map<string,point_layer>::iterator itPtLayer;
	for (itPtLayer=m_layersSensor.begin();itPtLayer!=m_layersSensor.end();itPtLayer++)
	{
		map<string,vector<PointShow>>::iterator itPoint;
		
		itPoint=m_sensorBuffer.find(itPtLayer->first);
		itManger=m_layerStatus.find(itPtLayer->first);
		if (itPoint!=m_sensorBuffer.end()&&itManger->second)
		{
			itPtLayer->second(itPoint->second);
		}

	}
	//draw map layer
	map<string,line_layer>::iterator itLineLayer;
	for (itLineLayer=m_layersMapLine.begin();itLineLayer!=m_layersMapLine.end();itLineLayer++)
	{
		map<string,vector<ckPloyLineS>>::iterator itLine;
		itLine=m_maplineBuffer.find(itLineLayer->first);
		itManger=m_layerStatus.find(itLineLayer->first);
		if (itLine!=m_maplineBuffer.end()&&itManger->second)
		{
			itLineLayer->second(itLine->second);
		}

	}
	//draw mission layer
	map<string,mission_layer>::iterator itMissionLayer;
	for (itMissionLayer=m_layersMission.begin();itMissionLayer!=m_layersMission.end();itMissionLayer++)
	{
		map<string,vector<ckMissionPoint>>::iterator itMission;
		itMission=m_missionBuffer.find(itMissionLayer->first);
		itManger=m_layerStatus.find(itMissionLayer->first);

		if (itMission!=m_missionBuffer.end()&&itManger->second)
		{
			itMissionLayer->second(itMission->second);
		}

	}
	
}

/************************************************************************/
/* 鼠标按键操作响应                                                                     */
/************************************************************************/
void CDataShow::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	mouseleftdown=TRUE;
	mouseprevpoint.x=point.x;
	mouseprevpoint.y=point.y;
//	SetCapture();
}
void CDataShow::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

		// TODO: Add your message handler code here and/or call default
	
	mouseleftdown=FALSE;
	SetSceneRot(0,(point.y-mouseprevpoint.y),TRUE,TRUE);
	SetSceneRot(2,(point.x-mouseprevpoint.x),TRUE,TRUE);
}
void CDataShow::OnRButtonDown(UINT nFlags, CPoint point)
{
	mouserightdown=TRUE;
	mouseprevpoint.x=point.x;
	mouseprevpoint.y=point.y;
}
void CDataShow::OnRButtonUp(UINT nFlags, CPoint point)
{
	mouserightdown=FALSE;
	x_move = GLfloat(point.x-mouseprevpoint.x) *1.0;
	y_move = -GLfloat(point.y-mouseprevpoint.y) *1.0;
	SetCamPos(0,(x_move),TRUE,TRUE);
	SetCamPos(1,(y_move),TRUE,TRUE);
}
BOOL CDataShow::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	bool zoomin=(zDelta>0);
	float zoomratio=zDelta/60.0;
	//TRACE("OnMouseWheel %f\n",zoomratio);
	if(zoomin)
	{
		m_zoom=m_zoom*(1+zoomratio);
		//zoomratio=
	}
	else
	{
		m_zoom=m_zoom/(1-zoomratio);
	}
	SetCamPos(2,zoomratio,TRUE,TRUE);
//	this->RenderScene();
	return 1;
}
void CDataShow::SetOneViewFocus(int index)
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

void CDataShow::Draw_compass(double x,double y,double z)
{

}
void CDataShow::Draw_grid (int meters_per_grid)
{
	double pos[3]={0,0,0};
	double grids_per_screen = 10;

	int num_lines = 20;

	double grid_ox = ceil (pos[0] / meters_per_grid) * meters_per_grid;
	double grid_oy = ceil (pos[1] / meters_per_grid) * meters_per_grid;
	double grid_oz = pos[2];

	glPushMatrix ();
	glTranslatef (grid_ox, grid_oy, grid_oz);

	//glColor4f(0.1, 0.2, 0.1, 0.5);
	glColor3f (0.3, 0.3, 0.3);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
	glVertex2f((+num_lines/2)*meters_per_grid, (-num_lines/2)*meters_per_grid);
	glVertex2f((+num_lines/2)*meters_per_grid, (+num_lines/2)*meters_per_grid);
	glVertex2f((+num_lines/2)*meters_per_grid, (+num_lines/2)*meters_per_grid);
	glVertex2f((-num_lines/2)*meters_per_grid, (+num_lines/2)*meters_per_grid);
	glEnd();

	glLineWidth (1);
	glEnable (GL_DEPTH_TEST);
	glBegin (GL_LINES);
	//glColor3f (0.3, 0.3, 0.3);

	for (int i=0; i<num_lines; i++) 
	{
		glVertex2f ((-num_lines/2 + i) * meters_per_grid,
			- num_lines/2 * meters_per_grid);
		glVertex2f ((-num_lines/2 + i) * meters_per_grid,
			num_lines/2 * meters_per_grid);

		glVertex2f (- num_lines/2 * meters_per_grid,
			(-num_lines/2 + i) * meters_per_grid);
		glVertex2f (num_lines/2 * meters_per_grid,
			(-num_lines/2 + i) * meters_per_grid);
	}
	glEnd();
	glDisable (GL_DEPTH_TEST);
	glDisable (GL_BLEND);

	glPopMatrix ();
}
void CDataShow::Draw_Road_Center_Point() //坐标系
{

}

void CDataShow::OnMouseMove(UINT nFlags, CPoint point)
{
	if (mouserightdown)
	{
		SetCamPos(2,-(point.y-mouseprevpoint.y),TRUE,TRUE);
	}
	else if(mouseleftdown)
	{
		SetSceneRot(0,(point.y-mouseprevpoint.y),TRUE,TRUE);
		SetSceneRot(2,(point.x-mouseprevpoint.x),TRUE,TRUE);

	}
	mouseprevpoint.x=point.x;
	mouseprevpoint.y=point.y;
}

int CDataShow::OnCreate(HWND hWnd)
{
	
	// TODO:  在此添加您专用的创建代码
	HDC hDC = ::GetDC(hWnd);
	
	if(SetWindowPixelFormat(hDC)==FALSE)
		return 0;

	if(CreateViewGLContext(hDC)==FALSE)
		return 0;
	// TODO:  Add your specialized creation code here
	
	m_bIsCreate=TRUE;
	return 1;
}
void CDataShow::OnSize(UINT nType, int cx, int cy)
{
	glViewport(0,0,cx,cy);

	GLdouble aspect_ratio;
	aspect_ratio=(GLdouble)cx/(GLdouble)cy;
	double left,right,top,bottom;

	if (aspect_ratio<1.0)
	{
		left=-m_lfRange;
		right=m_lfRange;
		top=-aspect_ratio*m_lfRange;
		bottom=aspect_ratio*m_lfRange;

	}
	else
	{
		top=-m_lfRange;
		bottom=m_lfRange;
		left=-aspect_ratio*m_lfRange;
		right=aspect_ratio*m_lfRange; 
	}
	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();

    glOrtho(left,right,top,bottom,vieeDis[0],vieeDis[1]);

//	glOrtho(-50,50,-50,50,-100,100);
//	gluPerspective(40.0f,aspect_ratio,1.0f,100000.0f);
	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();
	
}
void CDataShow::SetCamPos(int axis,int value,BOOL increment,BOOL apply)

{
	if (increment)
	{
		//camPos[axis]+=(float)value*camPos[axis]/100;
		camPos[axis]+=(float)value;
	}
	else
	{
		camPos[axis]-=(float)value
			
			;
	}
	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();

}
void CDataShow::SetSceneRot(int axis,int value,BOOL increment,BOOL apply)
{
	/*if (increment)
	{
	sceneRot[axis]+=(sceneRot[axis]>=360)?(-360+value/2):value/2;
	}
	else
	{
	sceneRot[axis]+=(sceneRot[axis]>=360)?(-360+value/2):value/2;
	}*/
	sceneRot[axis]+=(sceneRot[axis]>=360)?(-360+value/2):value/2;
}

void CDataShow::SetView(float f)
{
	camPos[0]=0.0f;
	camPos[1]=0.0f;
	camPos[2]=0.0f;
	camRot[0]=0.0f;
	camRot[1]=0.0f;
	camRot[2]=0.0f;
	scenePos[0]=0.0f;
	scenePos[1]=0.0f;
	scenePos[2]=0.0f;
	sceneRot[0]=0.0f;
	sceneRot[1]=0.0f;
	sceneRot[2]=0.0f;
}


void CDataShow::Draw_circle(int radious, int meters_per_difference)
{
	glLineWidth(1);
	glColor3f (0.3, 0.3, 0.3);
	glBegin(GL_LINE_STRIP);
	GLfloat step = 0.1f;
	GLfloat angle;
	for (int i = 0;i<meters_per_difference*10;i += meters_per_difference)
	{
		for (angle = 0.0f;angle <= 2.0f*3.1415926f;angle += step)
		{
			glVertex3f((radious+i)*sin(angle),(radious+i)*cos(angle),0);
		}
		glVertex3f((radious+i)*sin(2.0f*3.1415926f),(radious+i)*cos(2.0f*3.1415926f),0);
	}
	glEnd();
}

void CDataShow::Draw_coordinate()
{	
	glLineWidth(1);

	glColor3f(1,0,0);
	glBegin(GL_LINES);
	glVertex3d(0,0,0);
	glVertex3d(6,0,0);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex3f(6,-0.5,0);
	glVertex3f(6,0.5,0);
	glVertex3f(8,0,0);
	glEnd();

	glColor3f(1,1,0);
	glBegin(GL_LINES);
	glVertex3d(0,0,0);
	glVertex3d(0,6,0);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex3f(-0.5,6,0);
	glVertex3f(0.5,6,0);
	glVertex3f(0,8,0);
	glEnd();

	draw_letter_N();
	//glutSolidCone(0.5,1.5,20,20);
}
//create new pointcloud to show
void CDataShow::CreatePointLayer(string name,point_layer layer)
{
	map<string,point_layer>::iterator itl;
	itl=m_layersSensor.find(name);
	if (itl==m_layersSensor.end())
	{
		m_layersSensor.insert(map<string,point_layer>::value_type(name,layer));
		m_layerStatus.insert(map<string,bool>::value_type(name,true));
    }
} 
//update layer point on the screen
BOOL CDataShow::UpdatePointLayer(string name,const vector<PointShow> ptCloud)
{
	map<string,point_layer>::iterator itl;
	itl=m_layersSensor.find(name);
	if (itl==m_layersSensor.end())
	{
		return FALSE;
	}
	else
	{
       map<string,vector<PointShow>>::iterator itlPt;
	   itlPt=m_sensorBuffer.find(name);
	   if (itlPt==m_sensorBuffer.end())
	   {
		   m_sensorBuffer.insert(map<string,vector<PointShow>>::value_type(name,ptCloud));
	   }
	   else
	   {
		   m_sensorBuffer.erase(name);
		   m_sensorBuffer.insert(map<string,vector<PointShow>>::value_type(name,ptCloud));
	   }
	}
	return TRUE;
}
void CDataShow::CreateLineLayer(string name,line_layer layer)
{
	map<string,line_layer>::iterator itl;
	itl=m_layersMapLine.find(name);
	if (itl==m_layersMapLine.end())
	{
		m_layersMapLine.insert(map<string,line_layer>::value_type(name,layer));
		m_layerStatus.insert(map<string,bool>::value_type(name,true));
	}
}
BOOL CDataShow::UpdateLineLayer(string name,const vector<ckPloyLineS> linecloud)
{
	map<string,line_layer>::iterator itl;
	itl=m_layersMapLine.find(name);
	if (itl==m_layersMapLine.end())
	{
		return FALSE;
	}
	else
	{
		map<string,vector<ckPloyLineS>>::iterator itlPt;
		itlPt=m_maplineBuffer.find(name);
		if (itlPt==m_maplineBuffer.end())
		{
			m_maplineBuffer.insert(map<string,vector<ckPloyLineS>>::value_type(name,linecloud));
		}
		else
		{
			m_maplineBuffer.erase(name);
			m_maplineBuffer.insert(map<string,vector<ckPloyLineS>>::value_type(name,linecloud));
		}
	}
	return TRUE;
}
void CDataShow::CreateMissionLayer(string name,mission_layer layer)
{
	map<string,mission_layer>::iterator itl;
	itl=m_layersMission.find(name);
	if (itl==m_layersMission.end())
	{
		m_layersMission.insert(map<string,mission_layer>::value_type(name,layer));
		m_layerStatus.insert(map<string,bool>::value_type(name,true));
	}
}
BOOL CDataShow::UpdateMissionLayer(string name,const vector<ckMissionPoint> linecloud)
{
	map<string,mission_layer>::iterator itl;
	itl=m_layersMission.find(name);
	if (itl==m_layersMission.end())
	{
		return FALSE;
	}
	else
	{
		map<string,vector<ckMissionPoint>>::iterator itlPt;
		itlPt=m_missionBuffer.find(name);
		if (itlPt==m_missionBuffer.end())
		{
			m_missionBuffer.insert(map<string,vector<ckMissionPoint>>::value_type(name,linecloud));
		}
		else
		{
			m_missionBuffer.erase(name);
			m_missionBuffer.insert(map<string,vector<ckMissionPoint>>::value_type(name,linecloud));
		}
	}
	return TRUE;
}