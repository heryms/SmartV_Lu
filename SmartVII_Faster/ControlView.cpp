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



void CSmartVII_FasterView::OnCtlturnlane()
{
	// TODO: �ڴ���������������
}


void CSmartVII_FasterView::OnCtluturn()
{
	// TODO: �ڴ���������������

}


void CSmartVII_FasterView::OnCtlbrake()
{
	// TODO: �ڴ���������������
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
	// TODO: �ڴ���������������
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
	// TODO: �ڴ������������û����洦��������
	CONTROLTYPE e=m_DataCenter.GetControlType();
	if (e==MANUAL)
	{
		pCmdUI->SetText("�Զ�����");
	}
	else if (e==SELFDRIVING)
	{
		pCmdUI->SetText("�ֶ�����");
	}
}
/*-----------��û����ƺã���ʱֻ�����ϵ�ͼ----------------- */
//����cs��ʽ��ͼ����
//���Կ��ƹ켣����
//�°汾��ͼ
void CSmartVII_FasterView::OnBtnaddmap()
{
	// TODO: �ڴ���������������
	// TODO: �ڴ���������������
	BROWSEINFO bInfo;
	ZeroMemory(&bInfo, sizeof(bInfo));
	bInfo.hwndOwner = m_hWnd;
	bInfo.lpszTitle = _T("��ѡ��·��: ");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST lpDlist; //�������淵����Ϣ��IDList
	lpDlist = SHBrowseForFolder(&bInfo) ; //��ʾѡ��Ի���
	char sRead[5120];
	if(lpDlist != NULL)  //�û�����ȷ����ť
	{
		TCHAR chPath[255]; //�����洢·�����ַ���
		SHGetPathFromIDList(lpDlist, chPath);//����Ŀ��ʶ�б�ת�����ַ���
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
	// TODO: �ڴ���������������
	CString filter;  
	filter="�����ļ�(*.txt)|*.txt||";  
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
	//��·�߼��ص��滮�߳�
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
