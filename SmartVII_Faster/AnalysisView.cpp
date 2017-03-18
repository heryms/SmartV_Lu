#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// 归并为数据分析模块
#ifndef SHARED_HANDLERS
#include "SmartVII_Faster.h"
#endif

#include "MainControlDlg.h"

#include "SmartVII_FasterDoc.h"
#include "MainFrm.h"
#include "SmartVII_FasterView.h"

#include "MarkPointDlg.h"
#include "ckCoordinate.h"
#include <fstream>
using namespace std;

BOOL CSmartVII_FasterView::GetMapCentor(ckPointS &centor)
{
	if (m_trackPoints.empty())
	{
		return FALSE;
	}

		double top=0.0,right=0.0;
		double left=m_trackPoints.at(0).y;
		double bottom=m_trackPoints.at(0).x;
		for (int i=0;i<m_trackPoints.size();i++)
		{
			ckMifPoint pt=m_trackPoints.at(i);
			if (pt.y<left)
			{
				left=pt.y;
			}
			if (pt.y>right)
			{
				right=pt.y;
			}
			if (pt.x<bottom)
			{
				bottom=pt.x;
			}
			if (pt.x>top)
			{
				top=pt.x;
			}
		}
		centor.x=(top+bottom)/2.0;
		centor.y=(left+right)/2.0;
		
	
	
	return TRUE;
}
void CSmartVII_FasterView::ReadTrackGps0(CString path)
{
    std:fstream ff;
	ff.open(path,ios::in);
	if (!ff.good())
	{
		AfxMessageBox("导入点数据文件错误");
		return;
	}
	ckMifPoint ptlz;
	double lon0,lat0,amizth0;
	ff>>lon0>>lat0>>amizth0;
	ckCoordinate::LongLat2XY(lon0,lat0,ptlz.x,ptlz.y);
	vector<GpsLine> trackbuffer;
	while(!ff.eof())
	{
	//	ckMifPoint pt;
		GpsLine gpsPt;
		double lon,lat,amizth;
		ff>>lon>>lat>>amizth;
		ckCoordinate::LongLat2XY(lon,lat,gpsPt.Gnx,gpsPt.Gny);
		gpsPt.azimuth=0.0f;
	/*	gpsPt.Gnx=pt.x;
		gpsPt.Gny=pt.y;*/
		trackbuffer.push_back(gpsPt);
		//if ((pow((pt.x-ptlz.x),2)+pow((pt.y-ptlz.y),2))>1)
		//{
		//	m_trackPoints.push_back(pt);
		   /*ptlz.x=pt.x;
		   ptlz.y=pt.y;*/
	//	}
		
	}
	ff.close();
	ckMapBaseObject::CalLineAzmith(trackbuffer);
	for (int i=0;i<trackbuffer.size();i++)
	{
	    ckMifPoint mifPt;
		mifPt.x=trackbuffer[i].Gnx;
		mifPt.y=trackbuffer[i].Gny;
		mifPt.amith=trackbuffer[i].azimuth/3.1415926*180.0;
		m_trackPoints.push_back(mifPt);
	}
   GetMapCentor(m_showCentor);

}
void CSmartVII_FasterView::ReadMissionPt(CString path)
{
    std:fstream ff;
	ff.open(path,ios::in);
	if (!ff.good())
	{
		AfxMessageBox("导入点数据文件错误");
		return;
	}
	while(!ff.eof())
	{
		ckMissionPoint pt;
		int id,pro0,pro1;
		double lon,lat,amizth;
		ff>>id>>lon>>lat>>amizth>>pro0>>pro1;
		ckCoordinate::LongLat2XY(lon,lat,pt.x,pt.y);
		pt.num=id;
		pt.property0=pro0;
		pt.property1=pro1;
		pt.hgt=amizth;
		m_missionPoints.push_back(pt);
	}
	ff.close();
}
void CSmartVII_FasterView::OnBtnmarkadd()
{
	// TODO: 在此添加命令处理程序代码
	CString filter;  
	filter="任务文件(*.txt)|*.txt||";  
	CFileDialog fdlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,filter);  
	//fdlg.m_ofn.lpstrFilter="GPS File(*.cs)|*.cs|Road Net(*.txt)|*.txt||";
	if(fdlg.DoModal()!=IDOK)
	{
		AfxMessageBox("OPEN GPS FILE ERROR!");
		return;
	}
	ReadMissionPt(fdlg.GetPathName());
	if (m_missionPoints.empty())
	{
	   
		AfxMessageBox("任务文件错误");
		return;

	}
	for (int i=0;i<m_missionPoints.size();i++)
	{
		ckMissionPoint mission;
		mission.x=m_missionPoints[i].x-m_showCentor.x;
		mission.y=m_missionPoints[i].y-m_showCentor.y;
		mission.property0=m_missionPoints[i].property0;
		m_showMissionPoints.push_back(mission);
		m_ctlStrate.m_MissionPoint.push_back(m_missionPoints[i]);
	}
	m_DataShowPro.UpdateMissionLayer("CKMARKPOINTS",m_showMissionPoints);
	m_DataShowPro.SetLayerStatus("CKMARKPOINTS",true);
	if (m_DataCenter.m_dataCan.GetConnectStatus()!=1)
	{
		AfxMessageBox("Can未打开");
		return;
	}
	m_bmissionLoad=TRUE;
	//m_ctlStrate.InitialCan(m_DataCenter.m_dataCan);
	CString dir;
	::GetModuleFileName(AfxGetInstanceHandle(), dir.GetBuffer(MAX_PATH), _MAX_PATH);
	dir.ReleaseBuffer();
	int pos=dir.ReverseFind('\\');
	CString path = dir.Left(pos);
	path+="\\Map\\MissionIdx.ini";
	int idx=GetPrivateProfileInt("Mission","num",0,path);
	m_ctlStrate.m_ncurMisIdx=idx;
	m_ctlStrate.m_bFirstStart=TRUE;
	m_ctlStrate.m_canCtl=m_DataCenter.m_dataCan;
	OnDatashow();
	m_ctlStrate.StartThread();

}
void CSmartVII_FasterView::OneKeyRead()
{
	CString dir;
	::GetModuleFileName(AfxGetInstanceHandle(), dir.GetBuffer(MAX_PATH), _MAX_PATH);
	dir.ReleaseBuffer();
	int pos=dir.ReverseFind('\\');
	CString path = dir.Left(pos);
	path+="\\Map\\KCHMAP.txt";
	ReadTrackGps0(path);
	if (m_trackPoints.empty())
	{
		AfxMessageBox("路线数据读取失败");
		return;
	}
	for (int i=0;i<m_trackPoints.size();i++)
	{
		PointShow pt;
		pt.x=m_trackPoints[i].x-m_showCentor.x;
		pt.y=m_trackPoints[i].y-m_showCentor.y;
		m_showTrackPoints.push_back(pt);

	}
	m_DataShowPro.UpdatePointLayer("RoadLane0",m_showTrackPoints);
	int ptNum=m_trackPoints.size();
	if (m_DataCenter.m_dataPath.m_RoadDataBuffer!=NULL)
	{
		delete[] m_DataCenter.m_dataPath.m_RoadDataBuffer;
		m_DataCenter.m_dataPath.m_RoadDataBuffer=new RoadPoint[ptNum];
	}

	m_DataCenter.m_dataPath.m_RoadDataBuffer=new RoadPoint[ptNum];
	m_DataCenter.m_dataPath.fileLength=ptNum;
	for (int i=0;i<ptNum;i++)
	{
		m_DataCenter.m_dataPath.m_RoadDataBuffer[i].x=m_trackPoints[i].x;
		m_DataCenter.m_dataPath.m_RoadDataBuffer[i].y=m_trackPoints[i].y;
		m_DataCenter.m_dataPath.m_RoadDataBuffer[i].angle=CPathGenerate::AngleNormalnize(m_trackPoints[i].amith);
		//m_DataCenter.m_dataPath.m_RoadDataBuffer[i].angle=m_trackPoints[i].amith;
	}
	OnBtnvelo();
	//OnPlanstart();
	m_bmapLoad=TRUE;
	PostMessage(WM_PAINT,0,0);
}
void CSmartVII_FasterView::OnAnaloadmap()
{
	// TODO: 在此添加命令处理程序代码
	CString filter;  
	filter="GPS文件(*.txt)|*.txt||";  
	CFileDialog fdlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,filter);  
	//fdlg.m_ofn.lpstrFilter="GPS File(*.cs)|*.cs|Road Net(*.txt)|*.txt||";
	if(fdlg.DoModal()!=IDOK)
	{
		AfxMessageBox("OPEN GPS FILE ERROR!");
		return;
	}
    ReadTrackGps0(fdlg.GetPathName());
    if (m_trackPoints.empty())
    {
		AfxMessageBox("路线数据读取失败");
		return;
    }
	for (int i=0;i<m_trackPoints.size();i++)
	{
		PointShow pt;
		pt.x=m_trackPoints[i].x-m_showCentor.x;
		pt.y=m_trackPoints[i].y-m_showCentor.y;
		m_showTrackPoints.push_back(pt);
		
	}
	m_DataShowPro.UpdatePointLayer("RoadLane0",m_showTrackPoints);
	  int ptNum=m_trackPoints.size();
	  if (m_DataCenter.m_dataPath.m_RoadDataBuffer!=NULL)
	  {
		  delete[] m_DataCenter.m_dataPath.m_RoadDataBuffer;
		  m_DataCenter.m_dataPath.m_RoadDataBuffer=new RoadPoint[ptNum];
	  }

	  m_DataCenter.m_dataPath.m_RoadDataBuffer=new RoadPoint[ptNum];
	  m_DataCenter.m_dataPath.fileLength=ptNum;
	  for (int i=0;i<ptNum;i++)
	  {
		  m_DataCenter.m_dataPath.m_RoadDataBuffer[i].x=m_trackPoints[i].x;
		  m_DataCenter.m_dataPath.m_RoadDataBuffer[i].y=m_trackPoints[i].y;
		  m_DataCenter.m_dataPath.m_RoadDataBuffer[i].angle=CPathGenerate::AngleNormalnize(m_trackPoints[i].amith);
	  }
	  
	  m_bmapLoad=TRUE;
	  PostMessage(WM_PAINT,0,0);
//	SetTimer(5,200,0);

}


void CSmartVII_FasterView::OnAnaautotest()
{
	// TODO: 在此添加命令处理程序代码
//	m_DataCenter.SetControlModel(SELFDRIVING);
  g_ctlTypes=SELFDRIVING;
	m_DataCenter.StartPathPlan();
	//By Lu 10.23
	SetTimer(4,50,0);
}

//开始采集地图点数据
void CSmartVII_FasterView::OnBtncaiji()
{
	// TODO: 在此添加命令处理程序代码
	//判断是否已经在记录 
	if(!m_DataCenter.GetGpsPortStatus())
	{
		MessageBox("GPS未打开，终止采集");
		return;
	}
	BOOL flag=m_DataCenter.GetRecordStatus();
	if (!flag)
	{
		CFileDialog dlg(FALSE);
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
			m_DataCenter.SetGpsFile(path);
			m_DataCenter.EnableRecord(TRUE);
		}
		
	}
	else
	{
		m_DataCenter.SetGpsFile("");
		m_DataCenter.EnableRecord(FALSE);
	}
	

}

//处理采集之后的地图数据，生成地图
/** 暂时还是用老程序 **/
void CSmartVII_FasterView::OnBtnopera()
{
	// TODO: 在此添加命令处理程序代码
//改进地图数据处理模块
	/************************************************************************/
	/* 1.原有两个文件别读取改为同一文件名
	/* 2. 自动生成空间索引，预留Shapefile接口
	   3. 原始数据文件格式已更改，只有 X,Y,TYPE
	/************************************************************************/
	CFileDialog fpdlg(TRUE);
//	fpdlg.m_ofn.lpstrFilter="Curising Style(*txt)|*.txt";
	FILE *fpRead,*fpWrite;
	CString strRead,strWrite;
	if (fpdlg.DoModal()==IDOK)
	{
		strRead=fpdlg.GetPathName();
		int pos=strRead.ReverseFind('.');
		strWrite=strRead.Left(pos)+".cs";
		fpRead=fopen(strRead,"r");
		fpWrite=fopen(strWrite,"ab+");
	
		
		if (fpRead!=NULL)
		{
			long i=0;
		//	GpsLine lastpt;
			GpsLine pt;
			while(!feof(fpRead))
			{
			
				if(fpRead==NULL)break;
				//读文件
				fscanf(fpRead,"%lf%lf%lf\n",&pt.Gnx,&pt.Gny,&pt.azimuth);
				i++;
				if ((i-1)%5!=0)
				{
					continue;
				}
				fwrite(&pt.Gnx,1,sizeof(double),fpWrite);
				fwrite(&pt.Gny,1,sizeof(double),fpWrite);
				fwrite(&pt.azimuth,1,sizeof(double),fpWrite);
			//	lastpt=pt;
			}

		//	fwrite(&i,sizeof(long),1,fpWrite);
			fclose(fpRead);
			fclose(fpWrite);
			AfxMessageBox("地图制作完成");
		}
		else
		{
			MessageBox("没有导入地图文件");
		}
	}

}

//导入道路路口、标志牌等点位信息
void CSmartVII_FasterView::OnBtnmartpoint()
{
	// TODO: 在此添加命令处理程序代码
	CMarkPointDlg dlg;
	dlg.DoModal();

}


void CSmartVII_FasterView::OnUpdateBtncaiji(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	BOOL flag=m_DataCenter.GetRecordStatus();
	if (!flag)
	{
		pCmdUI->SetText("地图采集");

	}
	else
		pCmdUI->SetText("停止");
}
