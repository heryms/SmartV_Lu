#include "StdAfx.h"
#include "LocationPro.h"
#include "Pcomm.h"
HANDLE g_gpsrcv=NULL;
CLocationPro::CLocationPro(void)
{
	m_nBaudRate=B115200;
//	FILE*fp=fopen("GPSConfig.xml"	)
	ReadConfigFile();
	m_bRecFlag=FALSE;
	m_insStatus=NOSINGAL;
}


CLocationPro::~CLocationPro(void)
{	
	StopThread();
}
#define CRC32_POLYNOMIAL 0xEDB88320L
/* --------------------------------------------------------------------------
Calculate a CRC value to be used by CRC calculation functions.
-------------------------------------------------------------------------- */
unsigned long CRC32Value(int i)
{
	int j;
	unsigned long ulCRC;
	ulCRC = i;
	for ( j = 8 ; j > 0; j-- )
	{
		if ( ulCRC & 1 )
			ulCRC = ( ulCRC >> 1 ) ^ CRC32_POLYNOMIAL;
		else
			ulCRC >>= 1;
	}
	return ulCRC;
}
/* --------------------------------------------------------------------------
Calculates the CRC-32 of a block of data all at once
-------------------------------------------------------------------------- */
unsigned long CalculateBlockCRC32(

	unsigned long ulCount, /* Number of bytes in the data block */
	unsigned char *ucBuffer ) /* Data block */
{
	unsigned long ulTemp1;
	unsigned long ulTemp2;
	unsigned long ulCRC = 0;
	while ( ulCount-- != 0 )
	{
		ulTemp1 = ( ulCRC >> 8 ) & 0x00FFFFFFL;
		ulTemp2 = CRC32Value( ((int) ulCRC ^ *ucBuffer++ ) & 0xff );
		ulCRC = ulTemp1 ^ ulTemp2;
	}
	return( ulCRC );
}
// 打开GPS串口
BOOL CLocationPro::OpenPort()
{
	int     ret;

	 if (!GetPortStatus())
	{
	 
	if((ret=sio_open(m_gpsConfig.port))!=SIO_OK){
		AfxMessageBox("打开GPS串口操作失败!");
		return FALSE;
	 }

	if(!PortSet(m_gpsConfig.port)){
		sio_close(m_gpsConfig.port);

		return  FALSE;

 	}

	if(!PortBaudRate(m_gpsConfig.port)){
		sio_close(m_gpsConfig.port);
		return FALSE;
	 }
	}
	m_gpsConfig.status=true;
	return TRUE;
}
BOOL CLocationPro::GetPortStatus()
{
	return m_gpsConfig.status;
}
void CLocationPro::ClosePort()
{
	if (m_gpsConfig.status)
	{
		sio_close(m_gpsConfig.port);
		m_gpsConfig.status=false;
	}
	return ;
}
void CLocationPro::InitPortSetting()
{
	m_commData.Port = 1;
	m_commData.ibaudrate  = 18;
	m_commData.iparity = 0;
	m_commData.ibytesize = 3;
	m_commData.istopbits = 0;
	m_commData.BaudRate  = B460800;
	m_commData.Parity = P_NONE;
	m_commData.ByteSize = BIT_8;
	m_commData.StopBits = STOP_1;
	m_commData.Hw = FALSE;
	m_commData.Sw = FALSE;
	m_commData.Dtr = TRUE;
	m_commData.Rts = TRUE;
}

void CLocationPro::ReadConfigFile()
{
// 	if (m_pConfigFile!=NULL)
// 	{
		CvFileStorage* fs=cvOpenFileStorage("GPSConfig.xml",NULL,CV_STORAGE_READ);	
		CvFileNode *fn=cvGetFileNodeByName(fs,0,"COM");
		int a=cvReadIntByName(fs,fn,"name",0);
		int b=cvReadIntByName(fs,fn,"DataLen",0);
		m_gpsConfig.port=a;
		m_gpsConfig.DataLen=b;
		m_gpsConfig.status=false;	
		cvReleaseFileStorage(&fs);
/*	}*/

}
void CLocationPro::CreateThread()
{
	if(m_pThreadHandle != NULL)
		return;
	   m_pThreadHandle = AfxBeginThread(GPSThreadPro, 
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
	m_threadMode = MODESUSPEND;

}
void CLocationPro::StartThread()
{
	if(m_pThreadHandle == NULL)
		CreateThread();

	if(m_pThreadHandle != NULL)
		m_pThreadHandle->ResumeThread();
	else
		return;

	m_threadMode = MODERUN;
}
void CLocationPro::ResumeThread()
{
	if(m_pThreadHandle != NULL && m_threadMode == MODERUN)
		m_pThreadHandle->SuspendThread();
	else 
		return;

	m_threadMode = MODESUSPEND;
}
void CLocationPro::StopThread()
{
	if(m_pThreadHandle == NULL)
		return;

	m_bStopThread = TRUE;

	while(m_pThreadHandle)
		Sleep(1);

	m_threadMode = MODESTOP;
}
void CLocationPro::SuspendThread()
{
	if(m_pThreadHandle != NULL && m_threadMode == MODERUN)
		m_pThreadHandle->SuspendThread();
	else 
		return;

	m_threadMode = MODESUSPEND;
}

void  CLocationPro::ProcComData()
{
	int	len;
	long i=0, j;

	long lHead = 0;

	long pos1,pos2;
	BOOL bFindData;
	
	BOOL bHeadExist = FALSE;
    while(!m_bStopThread) 
	{

		len = sio_read(m_gpsConfig.port,	buf,
			_MAX_REC_BUF_SIZE_);
		//if the buffer is empty, wait a few monent
	  //  TRACE("Gps Length %d\n",len);
		if(len<1) {	Sleep(10);	continue;}

		//if the buffer is full, maybe lost some data, so can't use
		if(len == _MAX_REC_BUF_SIZE_) 
		{	
			TRACE("**** Receive buff full!\n");	continue;
		}
		//put data in pro buf
		if(lHead+len < _MAX_SEQ_BUF_SIZE_) 
		{
			memcpy(chBuf+lHead, buf, len);
			lHead += len;
		}
		else 
		{
			lHead=0;
			continue;
		}
	#pragma region 数据验证
	while(lHead >= (_INSPVA_MSG_LENGTH+_RESPONSE_HEADER_LENGTH_) && !m_bStopThread) 
		{
			bFindData = FALSE;
			pos1 = -1; 

			bHeadExist = FALSE;
			/////////////////////////
	#pragma region Find Header
		for(i=0; i<lHead; i++) 
			{
			//	Sleep(5);
			//	从头到尾搜索
				if(IsHead(chBuf, i))
				{
					bHeadExist=TRUE;
					bFindData=FALSE;
				//	根据数据长度校验数据是否正确
					if(IsCheckSumOK(chBuf, i)) 
					{
						pos1=i; 
						pos2=i+120;
						m_nWeek=*(UINT*)(chBuf+28);
						memcpy(&m_insPva,chBuf+32,sizeof(INSPVA));
						unsigned char status=*(unsigned char*)(chBuf+84+28);
			        /*	INS_INACTIVE=0,INS_ALIGNING=1,INS_HIGH_VARIANCE=2,INS_SOLUTION_GOOD=3,
							INS_SOLUTION_FREE=4,INS_ALIGNMENT_COMPLETE=5,
							DETERMINING_ORIENTATION=6,WAITING_INITIALPOS=7,*/
						switch(status)
						{ 
						case 0:
							m_insStatus=INS_INACTIVE;
							break;
						case 1:
							m_insStatus=INS_ALIGNING;
							break;
						case 2:
							m_insStatus=INS_HIGH_VARIANCE;
							break;
						case 3:
							m_insStatus=INS_SOLUTION_GOOD;
							break;
						case  4:
							m_insStatus=INS_SOLUTION_FREE;
							break;
						case 5:
							m_insStatus=INS_SOLUTION_FREE;
							break;
						case  6:
							m_insStatus=DETERMINING_ORIENTATION;
							break;
						case 7:
							m_insStatus=WAITING_INITIALPOS;
							break;
						default:
							break;
						}
						bFindData = TRUE;  //表示找到了一段数据，不管长度是否正确。
					//	------------改变原始数据--------------
						memcpy(chBuf, chBuf+pos2, lHead-pos2);
						lHead = lHead-pos2;
						break;
					}  
				//		-----------------------------------------------
					if(bFindData) 
							break; //从新的帧数据开始位置进行判断
					}

				}
		if(!bHeadExist && i==lHead)
		{
			//不存在帧数据头，去掉所有数据
			lHead=0;
		}
		else if(bHeadExist && !bFindData) 
		{
			//存在数据头，不存在数据尾，改变原始数据
			if(i==lHead) 
				lHead=0;
			else 
			{
				memcpy(chBuf, chBuf+pos1, lHead-pos1);
				lHead = lHead-pos1;
			}
		}
#pragma endregion Find Header
	
		if (bHeadExist&&bFindData)
		{
			SetEvent(g_gpsHandle);
		}
	}

		
#pragma endregion 数据验证
			///////////////////////////
		}
	SetEvent(g_gpsHandle);
}
 BOOL CLocationPro::IsHead(const unsigned char *const buf, int iPos)
{


	return (buf[_NOMAL_(iPos)]==0xAA 
		&& buf[_NOMAL_(iPos+1)]==0x44 
		&& buf[_NOMAL_(iPos+2)]==0x12);
	
}

inline BOOL CLocationPro::IsCheckSumOK(const unsigned char *const buf, int iPos)
{
	UINT sumLen, sumDataLen;
	//Length of the total LMS output data string.
	sumLen = buf[_NOMAL_(iPos+3)];
	sumDataLen=(buf[_NOMAL_(iPos+8)])+(buf[_NOMAL_(iPos+9)]<<8);
	//Number of measurement data bytes
	unsigned char crcBuffer[1024];
	memcpy(crcBuffer,buf,sumLen+sumDataLen+1);
	unsigned long crc=CalculateBlockCRC32(sumLen+sumDataLen,crcBuffer);
	return ((sumDataLen == _INSPVA_MSG_LENGTH) && (sumLen == _RESPONSE_HEADER_LENGTH_)
		&&(buf[sumLen+sumDataLen]==(crc&0xff) )&&(buf[sumLen+sumDataLen+1]==(crc>>8&0xff))
		&&(buf[sumLen+sumDataLen+2]==(crc>>16&0xff))&&(buf[sumLen+sumDataLen+3]==(crc>>24&0xff)));
	
}
// inline BOOL CLocationPro::IsCheckSumOK(unsigned long ulCount, unsigned char *ucBuffer)
// {
// 	unsigned long crc=CalculateBlockCRC32(ulCount,ucBuffer);
// }

void _stdcall DataRcv(int port)
{
	//WaitForSingleObject(g_gpsrcv,INFINITE);
	SetEvent(g_gpsrcv);
}
void CLocationPro::StartDataOutput()
{
	g_gpsrcv=CreateEvent(NULL,TRUE,FALSE,NULL);
	char unlogBuf[128];
	sprintf(unlogBuf,"unlogall\r\n");
	 int ret=sio_write(m_gpsConfig.port,unlogBuf,strlen(unlogBuf));
	 Sleep(5);
	char sendbuffer[256];
	sprintf(sendbuffer,"log inspvab ontime 0.05\r\n");
   ret=sio_write(m_gpsConfig.port,sendbuffer,strlen(sendbuffer));
	
 //  ret=sio_cnt_irq(m_gpsConfig.port,DataRcv,10);
    Sleep(5);
 if (ret==SIO_OK)
 {
	 TRACE("gps command succeed!\n");
 }
}
void CLocationPro::StopDataOutput()
{
	char unlogBuf[128];
	sprintf(unlogBuf,"unlogall\r\n");
	int ret=sio_write(m_gpsConfig.port,unlogBuf,strlen(unlogBuf));
	Sleep(5);
}
UINT  CLocationPro::GPSThreadPro(LPVOID lpParam)
{
	CLocationPro *pLocation = (CLocationPro *)lpParam;

	pLocation->ProcComData();

	pLocation->m_pThreadHandle = NULL;
	pLocation->m_threadMode = MODENULL;
	pLocation->m_bStopThread = FALSE;

	
	return 0;
}
void CLocationPro::AlignIMU()
{
	char sendbuffer[256];
	sprintf(sendbuffer,"SETIMUTYPE IMU_ADIS16488\r\nSETIMUORIENTATION 5\r\nVEHICLEBODYROTATION 0 0 0 0 0 0\r\nSETIMUTOANTOFFSET 0 -0.05 1.25 0.1 0.1 0.1");
	sio_write(m_gpsConfig.port,sendbuffer,strlen(sendbuffer));
	Sleep(5);
}
void CLocationPro::ConnectCORS()
{
	char sendbuffer[256];
	sprintf(sendbuffer,"com com2 9600\r\ninterfacemode com2 cmr novatel on\r\nlog com2 gpgga ontime 1");
	sio_write(m_gpsConfig.port,sendbuffer,strlen(sendbuffer));
	Sleep(5);
}