#include "StdAfx.h"
#include "ControlStrategy.h"
#include<fstream>
#include "ckCoordinate.h"
using namespace std;
#define  MAKE_RAD(angle) angle/180.0*3.1415926
HANDLE g_ctlHandle;
CControlStrategy::CControlStrategy(void)
{
	m_nBuffersize=20;
	m_pThreadHandle=NULL;
	m_bStopThread=FALSE;
	m_ncurMisIdx=0;
	m_lfMisDistatce=20.0;
	m_nStartWait=15;
	m_nWaitTime=10;
	m_bFirstStart=TRUE;
	m_lfStopDis=10.0;
	g_ctlHandle=CreateEvent(NULL,TRUE,FALSE,NULL);
}

CControlStrategy::~CControlStrategy(void)
{
	StopThread();
}
//更新角度数据到缓冲区
void CControlStrategy::UpdateWheelAngle(int angle)
{
	if (m_queueAngle.size()<m_nBuffersize)
	{
		m_queueAngle.push_back(angle);
	}
	else
	{
		int out=m_queueAngle.front();
		m_queueAngle.pop_front();
		m_queueAngle.push_back(angle);
	    m_queueAngle.shrink_to_fit();
	}
}
//更新速度数据到缓冲区
void CControlStrategy::UpdateCarSpeed(float speed)
{
	if (m_queueSpeed.size()<m_nBuffersize)
	{
		m_queueSpeed.push_back(speed);
	}
	else

	{
		int out =m_queueSpeed.front();
		m_queueSpeed.pop_front();
		m_queueSpeed.push_back(speed);
		m_queueSpeed.shrink_to_fit();
	}
}
double CControlStrategy::GetDis(GpsLine pt0,GpsLine pt1)
{
	double x=0,y=0;
	x=pt1.Gnx-pt0.Gnx;
	y=pt1.Gny-pt0.Gny;
	return sqrt(pow(x,2)+pow(y,2));
}
//根据实时位置判断是否到达场景
void CControlStrategy::ProAutoControl()
{
	while(!m_bStopThread)
	{
	  ckMissionPoint pt=m_MissionPoint.at(m_ncurMisIdx);
	  GpsLine cur,tag;
	  cur.Gnx=g_curLocation.gauX;
	  cur.Gny=g_curLocation.gauY;
 	  tag.Gnx=pt.x;
	  tag.Gny=pt.y;
	  //判断到达点附近
	  if (m_ncurMisIdx==0)
	  {
		  MissionEvent(m_ncurMisIdx);
		  m_bFirstStart=FALSE;
	  }

	    if (m_bFirstStart&&m_ncurMisIdx!=0)
	  {
		 SendStartStop();
		while(true)
		{   
			if (g_eBrakeStatus==SOFTUP)
			{
                  Sleep(10*1000);
				   SendUnBrake();
				   Sleep(200);
			}
			else if(g_eBrakeStatus==UNSOFTUP)
			{
				//判断送刹车之后开始发送速度
				SendStartSpeed(500);
				Sleep(1000);
				if (GetDis(cur,tag)<m_lfMisDistatce)
				{
					MissionEvent(m_ncurMisIdx);
				}
				else
				{
				   m_ncurMisIdx=m_ncurMisIdx-1;
					MissionEvent(m_ncurMisIdx);
				}
				break;
			}
			else
				Sleep(50);
		}
		  m_bFirstStart=FALSE;
	  }
	  else  if (!m_bFirstStart)
	  {
		  tag.Gnx=m_MissionPoint.at(m_ncurMisIdx).x;
		  tag.Gny=m_MissionPoint.at(m_ncurMisIdx).y;
		  if (GetDis(cur,tag)<m_lfMisDistatce)
		  {
			  MissionEvent(m_ncurMisIdx);
		  }
	  }
	  CString dir;
	  ::GetModuleFileName(AfxGetInstanceHandle(), dir.GetBuffer(MAX_PATH), _MAX_PATH);
	  dir.ReleaseBuffer();
	  int pos=dir.ReverseFind('\\');
	  CString path = dir.Left(pos);
	  path+="\\Map\\MissionIdx.ini";
	  CString idxnum;
	  idxnum.Format("%d",m_ncurMisIdx);
	  WritePrivateProfileString("Mission","num",idxnum,path);	
	  Sleep(50);

	}
}
void CControlStrategy::SendBrake()
{
	if (m_canStatus)
	{
		m_canCtl.SendCarSpeed(true,5,0);
	}
}
void CControlStrategy::SendUnBrake()
{
	if (m_canStatus)
	{
		m_canCtl.SendCarSpeed(true,4,0);
	}
}
void CControlStrategy::SendStartSpeed(int speed)
{
	if (m_canStatus)
	{
		m_canCtl.SendCarSpeed(true,0,speed);
	}
}
BOOL CControlStrategy::TestMissIndexFile()
{

	CString dir;
	::GetModuleFileName(AfxGetInstanceHandle(), dir.GetBuffer(MAX_PATH), _MAX_PATH);
	dir.ReleaseBuffer();
	int pos=dir.ReverseFind('\\');
	CString path = dir.Left(pos);
	path+="\\Map\\MissionIdx.ini";

	if (!PathFileExists(path))
	{
		CreateDirectory(path,NULL);
		WritePrivateProfileString("Mission","num","0",path);
	    return FALSE;
	}
	else 
		return TRUE;
}
//读取默认标记文件
BOOL CControlStrategy::LoadMarkFile()
{
	CString dir;
	::GetModuleFileName(AfxGetInstanceHandle(), dir.GetBuffer(MAX_PATH), _MAX_PATH);
	dir.ReleaseBuffer();
	int pos=dir.ReverseFind('\\');
	CString path = dir.Left(pos);
	path+="\\Map\\Mission.txt";

	if (!PathFileExists(path))
	{
		AfxMessageBox("对应地图目录未发现配置文件");
		return FALSE;
	}
	else
	{
        std:fstream ff;
		ff.open(path,ios::in);
		if (!ff.good())
		{
			AfxMessageBox("导入任务点数据文件错误");
			return FALSE;
		}
		while(!ff.eof())
		{
			ckMissionPoint mission;
			double lon,lat;
			ff>>mission.num>>lon>>lat>>mission.hgt>>\
				mission.property0>>mission.property1;
			ckCoordinate::LongLat2XY(lon,lat,mission.x,mission.y);
			m_markPoints.push_back(mission);			
		}
		ff.close();
	}
	return TRUE;
}
//加载比赛地图文件
BOOL CControlStrategy::LoadMissionFile(CString path)
{
    std:fstream ff;
	ff.open(path,ios::in);
	if (!ff.good())
	{
		AfxMessageBox("导入任务点数据文件错误");
		return FALSE;
	}
	while(!ff.eof())
	{
		ckMissionPoint mission;
		double lon,lat;
		ff>>mission.num>>lon>>lat>>mission.hgt>>\
			mission.property0>>mission.property1;
		ckCoordinate::LongLat2XY(lon,lat,mission.x,mission.y);
		m_MissionPoint.push_back(mission);			
	}
	ff.close();
	return TRUE;
}
//Get Current Control Mark Pos
int CControlStrategy::GetCurrentMissIndex()
{
	CString dir;
	::GetModuleFileName(AfxGetInstanceHandle(), dir.GetBuffer(MAX_PATH), _MAX_PATH);
	dir.ReleaseBuffer();
	int pos=dir.ReverseFind('\\');
	CString path = dir.Left(pos);
	path+="\\Mission\\MissionIdx.ini";
	int posidx=GetPrivateProfileInt("Mission","num",-1,path);
	if (pos==-1)
	{
		AfxMessageBox("任务标记文件加载错误");
		return -1;
	}
	else
		return posidx;
}
double CControlStrategy::GetDoubleDistance(double x0,double y0,double x1,double y1)
{
	double delx=x1-x0;
	double dely=y1-y0;
	return sqrt(pow(delx,2)+pow(dely,2));
}
//更新目标路口的通行状态
int CControlStrategy::UpdateCrossStatus(GpsLine pt)
{
	//判定未经过路口

	if (m_cross.status==0)
	{
       if (GetDoubleDistance(m_cross.start.x,m_cross.start.y,pt.Gnx,pt.Gny)<m_lfMisDistatce)
       {
		   m_cross.status=1;
		   SendStartSpeed(1000);
       }
	}
	else if (m_cross.status==1)
	{
		if (GetDoubleDistance(m_cross.start.x,m_cross.start.y,pt.Gnx,pt.Gny)<m_lfStopDis)
		{
			m_cross.status=2;
			SendBrake();
		}
	}
	else if (m_cross.status==2)
	{
		m_cross.status=3;
		SendUnBrake();
	}
	else if (m_cross.status==3)
	{
		if (GetDoubleDistance(m_cross.end.x,m_cross.end.y,pt.Gnx,pt.Gny))
		{

			m_cross.status=4;
		}
	}
	return m_cross.status;
}
//启动终点停车判断
void CControlStrategy::StartEnding(GpsLine pt)
{
	if (m_cross.status==0)
	{
		if (GetDoubleDistance(m_cross.start.x,m_cross.start.y,pt.Gnx,pt.Gny)<m_lfMisDistatce)
		{
			m_cross.status=1;
			SendStartSpeed(1000);
		}
	}
	else if (m_cross.status==1)
	{
		if (GetDoubleDistance(m_cross.start.x,m_cross.start.y,pt.Gnx,pt.Gny)<m_lfStopDis)
		{
			m_cross.status=2;
			SendBrake();
		}
	}
}
int CControlStrategy::PassNormalPoint(GpsLine pt)
{
	if (m_cross.status==0)
	{
		if (GetDoubleDistance(m_cross.start.x,m_cross.start.y,pt.Gnx,pt.Gny)<m_lfMisDistatce)
		{
			m_cross.status=1;
			SendStartSpeed(1000);
		}
	}
	else if (m_cross.status==1)
	{
		if (GetDoubleDistance(m_cross.start.x,m_cross.start.y,pt.Gnx,pt.Gny)<m_lfMisDistatce)
		{
			m_cross.status=2;
			//SendStartSpeed(1000);
		}
	}
	return m_cross.status;
}
BOOL CControlStrategy::InitialCan(CCanControl can)
{
	if (can.GetConnectStatus()==1)
	{
		m_canCtl=can;
	//	return TRUE;
	}

		return TRUE;
}
void CControlStrategy::SendStartStop()
{
	if (m_canCtl.GetConnectStatus()==1)
	{
		m_canCtl.SendCarSpeed(true,3,0);
	}
}
//根据任务点的类型切换实际的控制策略
int CControlStrategy::MissionEvent(int nidx)
{
	ckMissionPoint pt=m_MissionPoint.at(nidx);
	//任务为起点，进行自动启动
     if (pt.property0==0)
	{
		SendStartStop();
		
		//SetEvent(g_ctlHandle);

		//	WaitForSingleObject(g_ctlHandle,INFINITE);
			//判断刹车已经执行完毕
			/*startCnt++;
			int starttime=startCnt*50;*/
		
                  Sleep(m_nStartWait*1000);
				   SendUnBrake();
				   Sleep(2000);
	
				//判断送刹车之后开始发送速度
				SendStartSpeed(2000); 
				m_ncurMisIdx++;
				return -1;
	
		
	//	m_ncurMisIdx++;
	}
	//判断任务为路口点 
	else if(pt.property0==10)
	{
		//判断执行道路，减速到1500
		if (pt.property1==1)
		{			
			m_canCtl.SendCarSpeed(true,0,1500);
		//	TRACE("speed 1500\n");
		}
		else if (pt.property1==0)
		{
	        m_canCtl.SendCarSpeed(true,0,2500);
			//TRACE("speed 3000\n");
		}
		else if (pt.property1==3)
		{
			m_canCtl.SendCarSpeed(true,0,500);
	    // TRACE("speed 500\n");
		}
		else if (pt.property1==4)
		{
			m_canCtl.SendCarSpeed(true,0,2000);
		}
		else if (pt.property1==6)
		{
			m_canCtl.SendCarSpeed(true,0,500);
		}
		else if (pt.property1==7)
		{
			m_canCtl.SendCarSpeed(true,0,1500);
		}
		else if (pt.property1==8)
		{
			m_canCtl.SendCarSpeed(true,0,1000);
		}
		m_ncurMisIdx++;
	}
	else if (pt.property0==9)
	{
		if (pt.property1==1)
		{	
			m_canCtl.SendCarSpeed(true,0,500);
		}
		else if (pt.property1==9)
		{
			while(true)
			{
				GpsLine curstop,targend;
				curstop.Gnx=g_curLocation.gauX;
				curstop.Gny=g_curLocation.gauY;
				curstop.azimuth=g_curLocation.azimuth;
				targend.Gnx=pt.x;
				targend.Gny=pt.y;
				if (GetDis(curstop,targend)<m_lfStopDis)
				{
					
						SendBrake();
						Sleep(10000);
						SendUnBrake();
						m_canCtl.SendCarSpeed(true,0,1000);
					}
					else
						Sleep(50);

				}
				
			}
		m_ncurMisIdx++;
	
	}
	//判断为路口红绿灯
	else if (pt.property0==1)
	{
		m_canCtl.SendCarSpeed(true,0,1000);
		bool stopflag=false;
		int crossCnt=0;
		while(true)
		{
		
			GpsLine cur,targ;
			cur.Gnx=g_curLocation.gauX;
			cur.Gny=g_curLocation.gauY;
			cur.azimuth=g_curLocation.azimuth;
			targ.Gnx=pt.x;
			targ.Gny=pt.y;
			if (!stopflag)
			{	
				if (GetDis(targ,cur)<m_lfStopDis)
				{
					SendBrake();
					stopflag=true;
				}
				else
					Sleep(50);
			}
			else
			{
				if (g_eBrakeStatus==UNSOFTUP)
				{
					Sleep(m_nWaitTime*1000);
					SendUnBrake();
					Sleep(1000);
					m_canCtl.SendCarSpeed(true,0,1000);
					m_ncurMisIdx++;	
					return -1;
				}
			}
		}
	}
	//判断为侧方停车点
	else if (pt.property0==4)
	{
		GpsLine curpos,tagside;
		curpos.Gnx=g_curLocation.gauX;
		curpos.Gny=g_curLocation.gauY;
		tagside.Gnx=pt.x;
		tagside.Gny=pt.y;
		while(true)
		{
			if (GetDis(curpos,tagside)<5)
			{
				m_canCtl.SendCarSpeed(true,0,500);
				g_ctlTypes=TRACKMODEL;
				m_ncurMisIdx++;
				return -1;
			}
			else
				Sleep(50);
		}
	}
	else if (pt.property0==5)
	{
		GpsLine curpos,tagside;
		curpos.Gnx=g_curLocation.gauX;
		curpos.Gny=g_curLocation.gauY;
		tagside.Gnx=pt.x;
		tagside.Gny=pt.y;
		while(true)
		{
			if (GetDis(curpos,tagside)<5)
			{
				SendBrake();
				Sleep(30000);
			    SendUnBrake();
				m_ncurMisIdx++;
				return -1;
			}
			else
				Sleep(	50);
		}
	}
	else if (pt.property0==6)
	{

		m_canCtl.SendCarSpeed(true,0,500);
		m_ncurMisIdx++;
	}
	//判断为终点停车
	else if (pt.property0==7)
	{
		m_canCtl.SendCarSpeed(true,0,1000);
		bool endBrake=false;
		while(true)
		{
			GpsLine curstop,targend;
			curstop.Gnx=g_curLocation.gauX;
			curstop.Gny=g_curLocation.gauY;
			curstop.azimuth=g_curLocation.azimuth;
            targend.Gnx=pt.x;
			targend.Gny=pt.y;
			if (GetDis(curstop,targend)<m_lfStopDis)
			{
				if (!endBrake)
				{
					SendBrake();
					endBrake=true;				
				}
				else
					Sleep(50);
				
			}
			else Sleep(50);
		}
		
	}

}
UINT CControlStrategy::ControlThreadFunc(LPVOID param)
{
	CControlStrategy *pCtl=(CControlStrategy*)param;
	pCtl->ProAutoControl();
	pCtl->m_pThreadHandle=NULL;
	pCtl->m_bStopThread=FALSE;
	return 1;
}
void CControlStrategy::CreateThread()
{
	if(m_pThreadHandle != NULL)
		return;

	m_pThreadHandle = AfxBeginThread(ControlThreadFunc, 
		this,
		THREAD_PRIORITY_HIGHEST, //THREAD_PRIORITY_NORMAL,
		0,
		CREATE_SUSPENDED);

	if(m_pThreadHandle==NULL) 
	{
		AfxMessageBox("Create thread error!");
		return;
	}

	m_bStopThread = FALSE;
}
void CControlStrategy::StartThread()
{

	if(m_pThreadHandle == NULL)
		CreateThread();

	if(m_pThreadHandle != NULL)
		m_pThreadHandle->ResumeThread();
	else
		return;
}
void CControlStrategy::StopThread()
{
	if(m_pThreadHandle == NULL)
		return;

	m_bStopThread = TRUE;

	while(m_pThreadHandle)
		Sleep(1);
}
