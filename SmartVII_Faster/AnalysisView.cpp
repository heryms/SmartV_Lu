#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// �鲢Ϊ���ݷ���ģ��
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
		AfxMessageBox("����������ļ�����");
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
		AfxMessageBox("����������ļ�����");
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
	// TODO: �ڴ���������������
	CString filter;  
	filter="�����ļ�(*.txt)|*.txt||";  
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
	   
		AfxMessageBox("�����ļ�����");
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
		AfxMessageBox("Canδ��");
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
		AfxMessageBox("·�����ݶ�ȡʧ��");
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
	// TODO: �ڴ���������������
	CString filter;  
	filter="GPS�ļ�(*.txt)|*.txt||";  
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
		AfxMessageBox("·�����ݶ�ȡʧ��");
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
	// TODO: �ڴ���������������
//	m_DataCenter.SetControlModel(SELFDRIVING);
  g_ctlTypes=SELFDRIVING;
	m_DataCenter.StartPathPlan();
	//By Lu 10.23
	SetTimer(4,50,0);
}

//��ʼ�ɼ���ͼ������
void CSmartVII_FasterView::OnBtncaiji()
{
	// TODO: �ڴ���������������
	//�ж��Ƿ��Ѿ��ڼ�¼ 
	if(!m_DataCenter.GetGpsPortStatus())
	{
		MessageBox("GPSδ�򿪣���ֹ�ɼ�");
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

//����ɼ�֮��ĵ�ͼ���ݣ����ɵ�ͼ
/** ��ʱ�������ϳ��� **/
void CSmartVII_FasterView::OnBtnopera()
{
	// TODO: �ڴ���������������
//�Ľ���ͼ���ݴ���ģ��
	/************************************************************************/
	/* 1.ԭ�������ļ����ȡ��Ϊͬһ�ļ���
	/* 2. �Զ����ɿռ�������Ԥ��Shapefile�ӿ�
	   3. ԭʼ�����ļ���ʽ�Ѹ��ģ�ֻ�� X,Y,TYPE
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
				//���ļ�
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
			AfxMessageBox("��ͼ�������");
		}
		else
		{
			MessageBox("û�е����ͼ�ļ�");
		}
	}

}

//�����··�ڡ���־�Ƶȵ�λ��Ϣ
void CSmartVII_FasterView::OnBtnmartpoint()
{
	// TODO: �ڴ���������������
	CMarkPointDlg dlg;
	dlg.DoModal();

}


void CSmartVII_FasterView::OnUpdateBtncaiji(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	BOOL flag=m_DataCenter.GetRecordStatus();
	if (!flag)
	{
		pCmdUI->SetText("��ͼ�ɼ�");

	}
	else
		pCmdUI->SetText("ֹͣ");
}
