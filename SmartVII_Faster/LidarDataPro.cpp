#include "StdAfx.h"
#include "LidarDataPro.h"
#include "Pcomm.h"


CLidarDataPro::CLidarDataPro(void)
{
	m_nBaudRate=491520L;
	ReadConfigFile();
	m_lidarThread[0].SetWaitHandle(g_gpsHandle);
	m_lidarThread[0].SetDataGetHandle(g_lidarHandle1);
	m_lidarThread[0].SetCallBack(Laser_DataIn1);
	m_lidarThread[1].SetWaitHandle(g_gpsHandle);
	m_lidarThread[1].SetDataGetHandle(g_lidarHandle2);
	m_lidarThread[1].SetCallBack(Laser_DataIn2);
	m_lidarThread[2].SetWaitHandle(g_gpsHandle);
	m_lidarThread[2].SetDataGetHandle(g_lidarHandle3);
	m_lidarThread[2].SetCallBack(Laser_DataIn3);
	m_lidarThread[3].SetWaitHandle(g_gpsHandle);
	m_lidarThread[3].SetDataGetHandle(g_lidarHandle4);
	m_lidarThread[3].SetCallBack(Laser_DataIn4);
	
}

CLidarDataPro::~CLidarDataPro(void)
{

}
void CLidarDataPro::Laser_DataIn1(unsigned char *buf, int len)
{
	//m_portArray[0].status=true;
	//memcpy(m_portArray[0].okBuf,buf,len);
}
void CLidarDataPro::Laser_DataIn2(unsigned char *buf, int len)
{
	//m_portArray[1].status=true;
	//memcpy(m_portArray[1].okBuf,buf,len);
}
void CLidarDataPro::Laser_DataIn3(unsigned char *buf, int len)
{
	/*m_portArray[2].status=true;
	memcpy(m_portArray[2].okBuf,buf,len);*/
}
void CLidarDataPro::Laser_DataIn4(unsigned char *buf, int len)
{
	/*m_portArray[3].status=true;
	memcpy(m_portArray[3].okBuf,buf,len);*/
}
//打开串口操作
BOOL CLidarDataPro::OpenPort()
{
	if (1)
	{		
		for (int i=0;i<4;i++)
		{
			/*NLidarNode node=m_portArray[i];*/
			m_lidarThread[i].SetPortNum(m_portArray[i].port);
			if (!m_lidarThread[i].IsPortOpen())
			{
				if (!m_lidarThread[i].OpenPort())
				{
					return FALSE;
				}
				
			}
			switch (m_portArray[i].angle)
			{
			  case A100100:
				m_lidarThread[i].AngleMode_100_100();
				m_lidarThread[i].SetResolutionMode_100_100();
				break;
			  case A100050:
				m_lidarThread[i].AngleMode_100_050();
				m_lidarThread[i].SetResolutionMode_100_050();
				break;
			  case A100025:
				m_lidarThread[i].AngleMode_100_025();
				m_lidarThread[i].SetResolutionMode_100_025();
				break;
			  case A180100:
				m_lidarThread[i].AngleMode_180_100();
				m_lidarThread[i].SetResolutionMode_100_100();
				break;
			  case A180050:
				m_lidarThread[i].AngleMode_180_050();
				m_lidarThread[i].SetResolutionMode_180_050();
				break;
			  default:
				break;
			}

		}
	}
	return TRUE;
}
void CLidarDataPro::ClosePort()
{
	if (1)
	{
	   for (int i=0;i<4;i++)
	   {
		   if (m_lidarThread[i].IsPortOpen())
		   {
			   m_lidarThread->ClosePort();
		   }
	   }
	}
}
//读取配置文件
void CLidarDataPro::ReadConfigFile()
{
	CvFileStorage* fs=cvOpenFileStorage("LidarConfig.xml",NULL,CV_STORAGE_READ);
	int devNum=cvReadIntByName(fs,0,"Num",0);
	if (devNum!=0)
	{
		for (int i=0;i<devNum;i++)
		{	
			char aa[10];
			sprintf(aa,"Com%d",i);
			CvFileNode *fn=cvGetFileNodeByName(fs,0,aa);
			int port=cvReadIntByName(fs,fn,"name",0);
			string str=cvReadStringByName(fs,fn,"Angle"," ");
			int len=cvReadIntByName(fs,fn,"DataLen",0);
			CvSeq* s=cvGetFileNodeByName(fs,fn,"TransMat")->data.seq;
		//	NLidarNode node;
		 m_portArray[i].port=port;
			m_portArray[i].dataLen=len;
			
			for (int j=0;j<s->total;j++)
			{
				m_portArray[i].mat[j]=cvReadReal((CvFileNode*)cvGetSeqElem(s,j));
			}
			if (str=="A100100")
			{
				m_portArray[i].angle=A100100;
			}
			else if (str=="A100050")
			{
				m_portArray[i].angle=A100050;
			}
			else if (str=="A100025")
			{
				m_portArray[i].angle=A100025;
			}
			else if (str=="A180100")
			{
				m_portArray[i].angle=A180100;
			}
			else if (str=="A180050")
			{
				m_portArray[i].angle=A180050;
			}
			m_portArray[i].status=false;
			m_portArray[i].dataFlag=false;
			

		}
	}
	//for (int j=0;j<4;j++)
	//{
	//	m_portArray[j].okBuf=new unsigned char[m_portArray[j].dataLen];
	//}

}

void CLidarDataPro::InitPortSetting()
{

}
////
////void CLidarDataPro::SetResolutionMode_100_100(NLidarNode node) //0~100度，每隔1度一个
////{
////	if(!node.status)
////		return;
////
////	int 	len;
////	char 	buf[11];
////	buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x05;buf[3] = 0x00;
////	buf[4] = 0x3B;buf[5] = 0x64;buf[6] = 0x00;buf[7] = 0x64;
////	buf[8] = 0x00;buf[9] = 0x1D;buf[10] = 0x0F;
////
////	for(int i=0; i<5; i++) {
////		len = sio_write(node.port, buf, 11);
////		if(len>=0)
////			break;
////		Sleep(20);
////	}
////
////	Sleep(1000);
////}
////
////void CLidarDataPro::SetResolutionMode_100_050(NLidarNode node) //0~100度，每隔0.5度一个
////{
////	if(!node.status)
////		return;
////
////	int 	len;
////	char 	buf[11];
////	buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x05;buf[3] = 0x00;
////	buf[4] = 0x3B;buf[5] = 0x64;buf[6] = 0x00;buf[7] = 0x32;
////	buf[8] = 0x00;buf[9] = 0xB1;buf[10] = 0x59;
////
////	for(int i=0; i<5; i++) {
////		len = sio_write(node.port, buf, 11);
////		if(len>=0)
////			break;
////		Sleep(20);
////	}
////
////	Sleep(1000);
////}
////
////void CLidarDataPro::SetResolutionMode_100_025(NLidarNode node) //0~100度，每隔0.25度一个
////{
////	if(!node.status)
////		return;
////
////	int 	len;
////	char 	buf[11];
////	buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x05;buf[3] = 0x00;
////	buf[4] = 0x3B;buf[5] = 0x64;buf[6] = 0x00;buf[7] = 0x19;
////	buf[8] = 0x00;buf[9] = 0xE7;buf[10] = 0x72;
////
////	for(int i=0; i<5; i++) {
////		len = sio_write(node.port, buf, 11);
////		if(len>=0)
////			break;
////		Sleep(20);
////	}
////}
////
////void CLidarDataPro::SetResolutionMode_180_100(NLidarNode node) //0~180度，每隔1度一个
////{
////	if(!node.status)
////		return;
////
////	int 	len;
////	char 	buf[11];
////	buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x05;buf[3] = 0x00;
////	buf[4] = 0x3B;buf[5] = 0xB4;buf[6] = 0x00;buf[7] = 0x64;
////	buf[8] = 0x00;buf[9] = 0x97;buf[10] = 0x49;
////
////	for(int i=0; i<5; i++) {
////		len = sio_write(node.port, buf, 11);
////		if(len>=0)
////			break;
////		Sleep(20);
////	}
////	//Sleep(1000);
////}
////
////void CLidarDataPro::SetResolutionMode_180_050(NLidarNode node) //0~180度，每隔0.5度一个
////{
////	if(!node.status)
////		return;
////
////	int 	len;
////	char 	buf[11];
////	buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x05;buf[3] = 0x00;
////	buf[4] = 0x3B;buf[5] = 0xB4;buf[6] = 0x00;buf[7] = 0x32;
////	buf[8] = 0x00;buf[9] = 0x3B;buf[10] = 0x1F;
////
////	for(int i=0; i<5; i++) {
////		len = sio_write(node.port, buf, 11);
////		if(len>=0)
////			break;
////		Sleep(20);
////	}
////}
//
//UINT CLidarDataPro::LidarThreadPro(LPVOID lpParam)
//{
//
//	return 1;
//}
//
//inline BOOL CLidarDataPro::IsHead(NLidarNode node ,const unsigned char *const buf, int iPos)
//{
//
//	if(node.angle==A100100)  
//	{
//		return (buf[_NOMAL_(iPos)]==0x02 
//			&& buf[_NOMAL_(iPos+1)]==0x80 
//			&& buf[_NOMAL_(iPos+2)]==0xCE
//			&& buf[_NOMAL_(iPos+3)]==0x00
//			&& buf[_NOMAL_(iPos+4)]==0xB0
//			&& buf[_NOMAL_(iPos+5)]==0x65
//			&& (buf[_NOMAL_(iPos+6)]==0x00 || buf[_NOMAL_(iPos+6)]==0x40));
//	}
//	else if(node.angle==A100050)  
//	{
//		return (buf[_NOMAL_(iPos)]==0x02 
//			&& buf[_NOMAL_(iPos+1)]==0x80 
//			&& buf[_NOMAL_(iPos+2)]==0x96
//			&& buf[_NOMAL_(iPos+3)]==0x01
//			&& buf[_NOMAL_(iPos+4)]==0xB0
//			&& buf[_NOMAL_(iPos+5)]==0xC9
//			&& (buf[_NOMAL_(iPos+6)]==0x00 || buf[_NOMAL_(iPos+6)]==0x40));
//	}
//	else if(node.angle==A100025)  
//	{
//		return (buf[_NOMAL_(iPos)]==0x02 
//			&& buf[_NOMAL_(iPos+1)]==0x80 
//			&& buf[_NOMAL_(iPos+2)]==0x26
//			&& buf[_NOMAL_(iPos+3)]==0x03
//			&& buf[_NOMAL_(iPos+4)]==0xB0
//			&& buf[_NOMAL_(iPos+5)]==0x91
//			&& ( buf[_NOMAL_(iPos+6)]==0x01 || buf[_NOMAL_(iPos+6)]==0x41));
//	}
//	else if(node.angle==A180100)  
//	{
//		return (buf[_NOMAL_(iPos)]==0x02 
//			&& buf[_NOMAL_(iPos+1)]==0x80 
//			&& buf[_NOMAL_(iPos+2)]==0x6E
//			&& buf[_NOMAL_(iPos+3)]==0x01
//			&& buf[_NOMAL_(iPos+4)]==0xB0
//			&& buf[_NOMAL_(iPos+5)]==0xB5
//			&& ( buf[_NOMAL_(iPos+6)]==0x00 || buf[_NOMAL_(iPos+6)]==0x40));
//	}
//	else if(node.angle==A180050)  
//	{
//		return (buf[_NOMAL_(iPos)]==0x02 
//			&& buf[_NOMAL_(iPos+1)]==0x80 
//			&& buf[_NOMAL_(iPos+2)]==0xD6
//			&& buf[_NOMAL_(iPos+3)]==0x02
//			&& buf[_NOMAL_(iPos+4)]==0xB0
//			&& buf[_NOMAL_(iPos+5)]==0x69
//			&& ( buf[_NOMAL_(iPos+6)]==0x01 || buf[_NOMAL_(iPos+6)]==0x41));
//	}
//}
//
//inline BOOL CLidarDataPro::IsLengthOK(NLidarNode node,const unsigned char *const buf, int iPos)
//{
//	UINT sumLen, sumDataLen;
//	//Length of the total LMS output data string.
//	sumLen = buf[_NOMAL_(iPos+3)]*256+buf[_NOMAL_(iPos+2)];
//
//	//Number of measurement data bytes
//	unsigned char tp = buf[_NOMAL_(iPos+6)] & 0x0F;
//	sumDataLen = tp*256+buf[_NOMAL_(iPos+5)];
//	int ptnum=(node.dataLen-10)/2;
//	int  meaLen=node.angle-6;
//	return ((sumDataLen == ptnum) && (sumLen == meaLen));
//}
//
//void CLidarDataPro::ProcommData()
//{
//	int	len;
//	long i, j;
//
//	long lHead = 0;
//
//	long pos1, pos2;
//	BOOL bFindData;
//
//	BOOL bHeadExist = FALSE;
//	long ticknew, tickold = GetTickCount();
//	double begin=0;
//	double end=0;
//	double Time=0;
//	while(!m_bStopThread) 
//	{
//		//WaitForSingleObject(g_gpsEvent,INFINITE);
//		for (int i=0;i<m_portArray.size();i++)
//		{
//			Sleep(5);
//			//get data from comm port
//			len = sio_read(m_portArray[i].port,	buf,
//				_MAX_REC_BUF_SIZE_);
//
//			//if the buffer is empty, wait a few monent
//			if(len<1) {	Sleep(50);	continue;}
//
//
//			//if the buffer is full, maybe lost some data, so can't use
//			if(len == _MAX_REC_BUF_SIZE_) 
//			{	
//				TRACE("**** Receive buff full!\n");	continue;
//			}
//
//			//put data in pro buf
//			if(lHead+len < _MAX_SEQ_BUF_SIZE_) 
//			{
//				memcpy(chBuf+lHead, buf, len);
//				lHead += len;
//			}
//			else 
//			{
//				lHead=0;
//				continue;
//			}
////
////	#pragma region 数据验证
////		while(lHead >= m_portArray[i].dataLen && !m_bStopThread) 
////			{
////				bFindData = FALSE;
////				pos1 = -1; pos2 = -1;
////
////				bHeadExist = FALSE;
////				/////////////////////////////
////				for(i=0; i<lHead; i++) 
////				{
////					//从头到尾搜索
////					if(IsHead(m_portArray[i],chBuf, i))
////					{
////						//是否找到帧数据开始,如果不是就判断下一个位置
////						if(IsLengthOK(m_portArray[i],chBuf, i)) 
////						{
////							//是否长度数据正确,如果不是就判断下一个位置
////							pos1 = i; //一帧的开始位置
////
////							bHeadExist = TRUE;
////							//-----------------------------------------------
////							bFindData = FALSE;
////							for(j=i+1; j<lHead; j++ )
////							{
////								//从当前位置开始找下一个帧数据开始断下一个位置
////										pos2 = j; //有一个帧的开始位置
////
////										if(pos2-pos1==m_portArray[i].dataLen) 
////										{
////											//如果长度正确
////											memcpy(m_portArray[i].okBuf, chBuf+pos1, m_portArray[i].dataLen);
////										}
////
////										bFindData = TRUE;  //表示找到了一段数据，不管长度是否正确。
////										//------------改变原始数据--------------
////										memcpy(chBuf, chBuf+pos2, lHead-pos2);
////										lHead = lHead-pos2;
////										//----------------------------------------
////										break; //中止循环
////									}
////								}
////							}
////							//-----------------------------------------------
////							if(bFindData) 
////								if(IsHead(m_portArray[i],chBuf, j)) 
////								{
////									//是否找到帧数据开始,如果不是就判断下一个位置
////									if(IsLengthOK(m_portArray[i],chBuf, j))
////									{
////										//是否长度数据正确,如果不是就判
////								break; //从新的帧数据开始位置进行判断
////						}
////					}
////				}
////
////				if(!bHeadExist && i==lHead)
////				{
////					//不存在帧数据头，去掉所有数据
////					lHead=0;
////				}
////				else if(bHeadExist && !bFindData) 
////				{
////					//存在数据头，不存在数据尾，改变原始数据
////					if(i==lHead) 
////						lHead=0;
////					else 
////					{
////						memcpy(chBuf, chBuf+pos1, lHead-pos1);
////						lHead = lHead-pos1;
////					}
////				}
////				///////////////////////////////////
////			}
////#pragma endregion 数据验证
//		}
//
//	//	ResetEvent(g_gpsEvent);
//	//	SetEvent(g_lidarEvent);
//		
//	}
//}

void CLidarDataPro::StartThread()
{
	for (int i=0;i<4;i++)
	{
		m_lidarThread[i].StartThread();
	}
}

void CLidarDataPro::CreateThread()
{

	for (int i=0;i<4;i++)
	{
		m_lidarThread[i].CreateThread();
	}
}

void CLidarDataPro::ResumeThread()
{
	for (int i=0;i<4;i++)
	{
		m_lidarThread[i].ResumeThread();
	}
}

void CLidarDataPro::SuspendThread()
{
	for (int i=0;i<4;i++)
	{
		m_lidarThread[i].SuspendThread();
	}
}

void CLidarDataPro::StopThread()
{
	for (int i=0;i<4;i++)
	{
		m_lidarThread[i].StopThread();
	}
}

void CLidarDataPro::CtnsDataOutputStart()
{
	for (int i=0;i<4;i++)
	{
		m_lidarThread[i].CtnsDataOutputStart();
	}
}

void CLidarDataPro::CtnsDataOutputStop(NLidarNode node)
{
	for (int i=0;i<4;i++)
	{
		m_lidarThread[i].CtnsDataOutputStop();
	}
}