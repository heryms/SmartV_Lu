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



void CSmartVII_FasterView::OnCtlturnlane()
{
	// TODO: 在此添加命令处理程序代码
}


void CSmartVII_FasterView::OnCtluturn()
{
	// TODO: 在此添加命令处理程序代码

}


void CSmartVII_FasterView::OnCtlbrake()
{
	// TODO: 在此添加命令处理程序代码
	m_DataCenter.ShowCtlSceneDlg();
	//m_DataCenter.CarSpeedControlled(3,0);
	//while(true)
	//{
	//	if (m_DataCenter.m_dataCan.m_eBrakes==SOFTUP)
	//	{
	//		if ()
	//		{
	//		}
	//		Sleep(5000);
	//		m_DataCenter.CarSpeedControlled(4,0);
	//	}
	//	Sleep(50);
	//}
}


void CSmartVII_FasterView::OnBtnautoctl()
{
	// TODO: 在此添加命令处理程序代码
	CONTROLTYPE type=m_DataCenter.GetControlType();
	if (type==MANUAL)
	{
		m_DataCenter.SetControlModel(SELFDRIVING);

	}
	else  if(type==SELFDRIVING)
	{
		m_DataCenter.SetControlModel(MANUAL);
	}

}

void CSmartVII_FasterView::OnUpdateBtnautoctl(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	CONTROLTYPE e=m_DataCenter.GetControlType();
	if (e==MANUAL)
	{
		pCmdUI->SetText("自动控制");
	}
	else if (e==SELFDRIVING)
	{
		pCmdUI->SetText("手动控制");
	}
}
/*-----------还没有设计好，暂时只加载老地图----------------- */
//加载cs格式地图数据
//测试控制轨迹数据
//新版本地图
void CSmartVII_FasterView::OnBtnaddmap()
{
	// TODO: 在此添加命令处理程序代码
	// TODO: 在此添加命令处理程序代码
	BROWSEINFO bInfo;
	ZeroMemory(&bInfo, sizeof(bInfo));
	bInfo.hwndOwner = m_hWnd;
	bInfo.lpszTitle = _T("请选择路径: ");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST lpDlist; //用来保存返回信息的IDList
	lpDlist = SHBrowseForFolder(&bInfo) ; //显示选择对话框
	char sRead[5120];
	if(lpDlist != NULL)  //用户按了确定按钮
	{
		TCHAR chPath[255]; //用来存储路径的字符串
		SHGetPathFromIDList(lpDlist, chPath);//把项目标识列表转化成字符串
		m_MapObject.BuildTrackFactory(chPath);
		m_MapObject.AddLineLayer(m_DataShowPro);
		m_bmapLoad=TRUE;
		ckRect rt=m_MapObject.m_fullExtent;
		int width=rt.max[0]-rt.min[0];
		int heigth=rt.max[1]-rt.min[1];
		m_DataShowPro.m_lfRange=width>heigth?width:heigth;
		m_bmapLoad=TRUE;
		//PostMessage(WM_SIZE,0,0);
		CRect rect;
		GetClientRect(&rect);
		m_DataShowPro.OnSize(0,rect.Width(),rect.Height());
		PostMessage(WM_PAINT,0,0);
		//m_MapObject.CreateMapLineNet();
	}
}

void CSmartVII_FasterView::OnBtnaddmission()
{
	// TODO: 在此添加命令处理程序代码
	CString filter;  
	filter="任务文件(*.txt)|*.txt||";  
	CFileDialog fdlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,filter);  
	//fdlg.m_ofn.lpstrFilter="GPS File(*.cs)|*.cs|Road Net(*.txt)|*.txt||";
	if(fdlg.DoModal()!=IDOK)
	{
		AfxMessageBox("OPEN MISSION FILE ERROR!");
		return;
	}
	m_MapObject.ReadMissionTable(fdlg.GetPathName());

	vector<ckLineSegment> output;
	vector<ckRoadLane> lanes;
	m_MapObject.CreateMissionLines(output);
	CString str;
	str.Format("%d",output.size());
	MessageBox(str);
	m_MapObject.CalMissionRoad(output,lanes);
	m_MapObject.AddMissionLayer(m_DataShowPro);
	m_MapObject.Convert2TrackLayer(lanes,m_DataCenter.m_trackLayer);
	int ptNum=m_DataCenter.m_trackLayer.size();
	//将路线加载到规划线程
	if (m_DataCenter.m_dataPath.m_RoadDataBuffer==NULL)
	{
		m_DataCenter.m_dataPath.m_RoadDataBuffer=new RoadPoint[ptNum];
		m_DataCenter.m_dataPath.fileLength=ptNum;
		for (int i=0;i<ptNum;i++)
		{
			m_DataCenter.m_dataPath.m_RoadDataBuffer[i].x=m_DataCenter.m_trackLayer[i].Gnx;
			m_DataCenter.m_dataPath.m_RoadDataBuffer[i].y=m_DataCenter.m_trackLayer[i].Gny;
			m_DataCenter.m_dataPath.m_RoadDataBuffer[i].angle=m_DataCenter.m_trackLayer[i].azimuth;
		}
	}
	m_bmissionLoad=TRUE;
	PostMessage(WM_PAINT,0,0);
	SetTimer(4,100,0);
}
