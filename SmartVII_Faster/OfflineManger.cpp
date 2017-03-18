#include "StdAfx.h"
#include "OfflineManger.h"

ckFrame* ckCreateFrame(int lLen,int vLen,int gLen)
{
	ckFrame frame;
	//frame.lidarlen=lLen;
	//frame.videolen=vLen;
	//frame.gpslen=gLen;
	//int header=20;
	//int total=header+lLen+gLen+vLen;
	//frame.framedata=new uchar[vLen];
	return &frame;
}
CK_OFFLINE_RES ckReleaseFrame(ckFrame** frame)
{
	
	/*uchar *ptr=(*frame)->framedata;
	if (ptr!=NULL)
	{
	delete[] ptr; 
	}
	cvFree(frame);*/
	return CK_OK;

}


COfflineManger::COfflineManger(void)
{
	m_bGpsFlags=TRUE;
	m_bLidarFlags=TRUE;
	m_bVideoFlages=TRUE;
	m_pOfflineFile=NULL;
	m_bPlay=FALSE;
	m_bStopThread=TRUE;
	m_nCurPos=0;
	m_nFramesNum=0;
	m_pHandle=NULL;
	m_pLidarBuffer=NULL;
	m_pVideoBuffer=NULL;
	m_pGpsBuffer=NULL;
	m_nGpsLen=0;
	m_nLidarLen=0;
	m_nVideoLen=0;
	InitializeCriticalSection(&m_csData);
}


COfflineManger::~COfflineManger(void)
{
	if (m_pHandle)
	{
		StopThread();
	}
	if (m_pOfflineFile)
	{
		fclose(m_pOfflineFile);
	}
	DeleteCriticalSection(&m_csData);
}

BOOL COfflineManger::AddOfflineFile(CString path)
{
	m_pOfflineFile=fopen(path,"rb");
	if (m_pOfflineFile==NULL)
	{
		return FALSE;
	}
	//ReadData();
	return TRUE;
}
//读取指定指针为数据块
ckFrame* COfflineManger::QueryFrames(long pos)
{
	if (m_pOfflineFile==NULL)
	{
		AfxMessageBox("Data Source Error!");
		return NULL;
	}
	//uchar buffer[]
	int lidarlen=0,gpslen=0,videolen=0;
    //文件指针位移，读取数据之后归位
	fseek(m_pOfflineFile,pos*m_nFrameLen,0);
	unsigned char *buffer=new unsigned char[m_nFrameLen];
	fread(buffer,m_nFrameLen,1,m_pOfflineFile);
	rewind(m_pOfflineFile);
	gpslen=*((int*)(buffer+4));
	lidarlen=*((int*)(buffer+8));
	videolen=*((int*)(buffer+12));
	ckFrame* pFrame=(ckFrame*)cvAlloc(sizeof(ckFrame));
	pFrame->gpslen=gpslen;
	pFrame->lidarlen=lidarlen;
	pFrame->videolen=videolen;
	pFrame->framedata=buffer;
	return pFrame;
}
void COfflineManger::ProPlay()
{
	while(!m_bStopThread)
	{
		if (!m_bPlay)
		{
			continue;
		}
		ckFrame *pframe=QueryFrames(m_nCurPos);
		EnterCriticalSection(&m_csData);
		if(m_nGpsLen>0)
		memcpy(pframe->framedata,m_pGpsBuffer,m_nGpsLen);
		if(m_nLidarLen>0)
		memcpy(pframe->framedata+m_nGpsLen,m_pLidarBuffer,m_nLidarLen);
		if(m_nVideoLen>0)
		memcpy(pframe->framedata+m_nGpsLen+m_nLidarLen,m_pVideoBuffer,m_nVideoLen);
		LeaveCriticalSection(&m_csData);
		ckReleaseFrame(&pframe);
		Sleep(50);
	}
}

//两种方式读取数据文件，通过配置文件可以对之前采集的数据进行兼容
void COfflineManger::ReadConfig(CString path)
{
	CFileFind find;
	if (find.FindFile(path))
	{
		m_nGpsLen=GetPrivateProfileInt("GPS","Len",0,path);
		m_nLidarLen=GetPrivateProfileInt("LIDAR","Len",0,path);
		m_nVideoLen=GetPrivateProfileInt("VIDEO","Len",0,path);
		m_nFramesNum=m_nGpsLen+m_nLidarLen+m_nVideoLen;
	}
	else
	{
	  AfxMessageBox("离线配置文件不存在!");
	}
	
	
}

BOOL COfflineManger::ReadHeader()
{
	if (m_pOfflineFile==NULL)
	{
		AfxMessageBox("Data Source Error!");
		return FALSE;
	}
	unsigned char *buffer=new unsigned char[100];
	fread(buffer,m_nFrameLen,1,m_pOfflineFile);
	if ((buffer[0]==0xaa)&&(buffer[1]==0xaa))
	{
		m_nGpsLen=*((int*)(buffer+4));
		m_nFramesNum+=m_nGpsLen;
		m_nLidarLen=*((int*)(buffer+8));
		m_nFramesNum+=m_nLidarLen;
		m_nVideoLen=*((int*)(buffer+12));
	   m_nFramesNum+=m_nVideoLen;
	}
	rewind(m_pOfflineFile);
	return TRUE;
}

void COfflineManger::InitialBuffer()
{
	if (!m_bBufferFlag)
	{
		if(m_nLidarLen>0)
		m_pLidarBuffer=new unsigned char[m_nLidarLen];
		if(m_nVideoLen>0)
		m_pVideoBuffer=new unsigned char[m_nVideoLen];
		if(m_nGpsLen>0)
		m_pGpsBuffer=new unsigned char[m_nGpsLen];
		m_bBufferFlag=TRUE;
	}
}
void COfflineManger::ReleaseBuffer()
{
	if (m_bBufferFlag)
	{
		if(!m_pGpsBuffer)
		delete[]m_pGpsBuffer;
		if(!m_pLidarBuffer)
		delete[] m_pLidarBuffer;
		if(!m_pVideoBuffer)
		delete[]m_pVideoBuffer;
	}
}
//线程控制函数实现
UINT COfflineManger::OfflineFuncThread(LPVOID param)
{

	return 1;
}
void COfflineManger::CreateThread()
{

}
void COfflineManger::StartThread()
{

}
void COfflineManger::ResumeThread()
{

}
void COfflineManger::SuspendThread()
{

}
void COfflineManger::StopThread()
{

}