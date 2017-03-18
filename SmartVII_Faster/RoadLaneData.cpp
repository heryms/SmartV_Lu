#include "StdAfx.h"
#include "RoadLaneData.h"
#include "ckCoordinate.h"
LaneInfo_t g_laneInfo;
CRITICAL_SECTION g_cs;
CCriticalSection g_criSection;
CRoadLaneData::CRoadLaneData(void)
{
	m_pThreadHandle=NULL;
	m_nBuffersize=5;
	m_fdelY=0.4;
	m_nptNum=100;
	m_gpsBuffer=NULL;
	m_blineStatus=false;
	InitializeCriticalSection(&g_cs);
	
}


CRoadLaneData::~CRoadLaneData(void)
{
	StopThread();
	if (m_gpsBuffer!=NULL)
	{
		delete[]m_gpsBuffer;
	}
    DeleteCriticalSection(&g_cs);
}
void roadlane_message(const LaneInfo_t*lane)
{
	//EnterCriticalSection(&g_cs);
	
	//g_criSection.Lock();
	g_laneInfo=*lane;
   SetEvent(g_laneHandle);
}
void roadlane_thread(void *param)
{
	LcmLaneHandler* handle=(LcmLaneHandler*)param;
	while(0==handle->grab());
}
BOOL CRoadLaneData::InitialLaneLcm()
{
	CString dir;
	::GetModuleFileName(AfxGetInstanceHandle(), dir.GetBuffer(MAX_PATH), _MAX_PATH);
	dir.ReleaseBuffer();
	int pos=dir.ReverseFind('\\');
	CString path = dir.Left(pos);

	path+="\\IPConfig.ini";
	int ipstatus=GetPrivateProfileInt("Roadlane","this",0,path);
	if (ipstatus==0)//在本机进行数据广播
	{

		if (!m_lanelcm.initialLcm("","CKLCMROADLANE",roadlane_message))
		{
			return FALSE;
		}
	}
	else
	{
		char ipaddr[50];
		DWORD strnum=GetPrivateProfileString("Roadlane","addr","udpm://192.168.127.2:8801",ipaddr,sizeof(ipaddr),path); 
		if (!m_lanelcm.initialLcm(ipaddr,"CKLCMROADLANE",roadlane_message))
		{
			return FALSE;
		}
	}
	_beginthread(roadlane_thread,0,&m_lanelcm);
	StartThread();
	return TRUE;
}
int cntgy=0;
void CRoadLaneData::ExactLanes()
{
	//构建数据缓冲区
	cntgy++;
	//TRACE("lane param id %d ",cntgy);
	//LaneInfo_t lane=g_laneInfo;
	//g_criSection.Unlock();
	//TRACE("\t0 ");
	//int size=m_laneBuffer.size();
	//if (size<m_nBuffersize)
	//{
	//	m_laneBuffer.push_back(g_laneInfo);
	//}
	//else
	//{
	//	m_laneBuffer.pop_front();
	//	m_laneBuffer.push_back(g_laneInfo);
	//}
//	TRACE("\t1 ");
	if (g_laneInfo.leftLaneOK&&g_laneInfo.rightLaneOK)
	{
		/*	{
		vector<PointShow>().swap(m_showBuffer);
		}*/
		
	//	int ptnum=lane.nLeftLanePts;
	/*	float nearpt[2];
		nearpt[0]=lane.leftNearestPt[0];
		nearpt[1]=lane.leftNearestPt[1];
		float farthpt[2];
		farthpt[0]=lane.leftFarthestPt[0];
		farthpt[1]=lane.leftFarthestPt[1];*/
		m_blineStatus=true;
		float y0=0;
		if (m_gpsBuffer!=NULL)
		{
			delete[]m_gpsBuffer;
		}
		m_gpsBuffer=new GpsLine[m_nptNum];
		//ckPointShow pt0=PointShow(nearpt[0],nearpt[1]);
	//	m_showBuffer.push_back(pt0);
	   OriginPt org;
	   org.X=g_laneInfo.position[0];
	   org.Y=g_laneInfo.position[1];
	   org.Angle=g_laneInfo.orientation[0]/180.0*M_PI;
		//SYSTEMTIME st;
	//	GetSystemTime(&st);
		//EnterCriticalSection(&g_cs);
		m_fdelY=40.0/(float)m_nptNum;
		//TRACE("start time %d\t",st.wMilliseconds);
		
		for (int i=0;i<m_nptNum;i++)
		{
			y0+=m_fdelY;
			float x=g_laneInfo.centerPolyPara[0]+y0*g_laneInfo.centerPolyPara[1]+pow(y0,2)*g_laneInfo.centerPolyPara[2];
			//	float x=0;
			double gnx=0.0,gny=0.0;
		///	ckCoordinate::MaptoWorld(org,x,y0,gnx,gny);
			m_gpsBuffer[i].Gnx=x;
			m_gpsBuffer[i].Gny=y0;
		}
		/*	GetSystemTime(&st);
		TRACE("%d\t",st.wMilliseconds);*/
	//	LeaveCriticalSection(&g_cs);
	}
	else
	{
		m_blineStatus=false;
	}
//	TRACE("\t2 ");
	//TRACE("\n ");
}
void CRoadLaneData::DataProFunc()
{
	while(!m_bStopThread)
	{
		WaitForSingleObject(g_laneHandle,INFINITE);
		ExactLanes();
	//	UpdatesLayers();
	//	Sleep(50);
		ResetEvent(g_laneHandle);
	}
	
}
UINT CRoadLaneData::RoadLaneThreadPro(LPVOID lpParam)
{
	CRoadLaneData* pRoadlane=(CRoadLaneData*) lpParam;
    pRoadlane->DataProFunc();
	pRoadlane->m_pThreadHandle=NULL;
    pRoadlane->m_bStopThread=FALSE;
	return 0;
}
void CRoadLaneData::CreateThread()
{
	m_pThreadHandle=AfxBeginThread(RoadLaneThreadPro,this,THREAD_PRIORITY_HIGHEST,0,CREATE_SUSPENDED);
	if (m_pThreadHandle==NULL)
	{
		AfxMessageBox("Create thread error!");
		return;
	}
	m_bStopThread=FALSE;

}
void CRoadLaneData::SetShowdlg(CDataShow* cs)
{
	if (!cs->m_bIsCreate)
	{
		AfxMessageBox("视图未创建");
		return;
	}
}

void CRoadLaneData::UpdatesLayers()
{
	//if (!m_csShow->m_bIsCreate)
	//{
	//	AfxMessageBox("视图未创建");
	//	return;
	//}
	//if(!m_csShow->UpdatePointLayer("CKROADLANE",m_showBuffer))
	//{
	//	
	//	m_csShow->CreatePointLayer("CKROADLANE",DrawRoadlane);
	//	//cs.SetLayerStatus("CKROADLANE",true);
	//	m_csShow->UpdatePointLayer("CKROADLANE",m_showBuffer);
	//}
}
void CRoadLaneData::StartThread()
{
	if (m_pThreadHandle==NULL)
	{
		CreateThread();
	}
	if (m_pThreadHandle!=NULL)
	{
		m_pThreadHandle->ResumeThread();
	}
	else
		return;
		

}
void CRoadLaneData::StopThread()
{
	if (m_pThreadHandle==NULL)
	{
		return;
	}
	m_bStopThread=TRUE;
	while(m_pThreadHandle)
		Sleep(1);
}