#include "StdAfx.h"
#include "CanControl.h"
#include "stdafx.h"
#define  _COMMAND_SLEEP 1

//定位刹车返回指令
#define  _REOMTE_BRAKE_DOWN   0x01
#define  _REMOTE_BRAKE_UP    0x03
#define  _REOMTE_UNBRAKE_DOWN  0x04
#define  _REMOTE_UBRAKE_UP     0x06
#define  _SOFTWARE_BRAKE_DOWN         0x08
#define  _SOFTWARE_BRAKE_UP      0x0C
#define   _SOFTWARE_UNBRAKE_DOWN   0x10
#define  _SOFTWARE_UNBRAKE_UP    0x30
const DWORD GCanBrTab[10] = {
	0x060003, 0x060004, 0x060007,
	0x1C0008, 0x1C0011, 0x160023,
	0x1C002C, 0x1600B3, 0x1C00E0,
	0x1C01C1
};
CCanControl::CCanControl(void)
{
	m_rcvHandle=NULL;
	m_sendHandle=NULL;
	m_connect=0;
	m_cannum=0;
	m_devind=0;
	m_devtype=VCI_USBCAN_2E_U;
	m_hRcvHandle=CreateEvent(NULL,TRUE,FALSE,NULL);
	m_nCarAngle=550;
	m_nCarSpeed=0;
	m_nEncodeNum=0;
}


CCanControl::~CCanControl(void)
{
	StopThread();
	CloseCan();
}
UINT CCanControl::RcvCarStatus(LPVOID lpParam)
{
	CCanControl *pCanThread = (CCanControl *)lpParam;

	pCanThread->ProCanRcv();
	pCanThread->m_rcvHandle= NULL;
	pCanThread->m_threadMode = MODENULL;
	pCanThread->m_bStopThread = FALSE;
	return 0;
}
BOOL CCanControl::ConnectCan()
{
	if (m_connect==1)
	{
		m_connect=0;
		Sleep(500);
		VCI_CloseDevice(m_devtype,m_devind);
		return FALSE;
	}
	VCI_INIT_CONFIG init_config;
	int mode,cannum,baud;
	cannum=0;
	baud=GCanBrTab[2];
	mode=0;
	m_cannum=cannum;
	init_config.Mode=mode;

	if (VCI_OpenDevice(m_devtype,m_devind,0)!=STATUS_OK)
	{
		AfxMessageBox("打开设备失败!\n");
		return FALSE;
	}
	if (VCI_SetReference(m_devtype,m_devind, cannum, 0, &baud) != STATUS_OK)
	{
		AfxMessageBox("波特率设置失败!\n");
		VCI_CloseDevice(m_devtype,m_devind);
		return FALSE;
	}
	if(VCI_InitCAN(m_devtype,m_devind,cannum,&init_config)!=STATUS_OK)
	{
		AfxMessageBox("初始化CAN失败!\n");
		VCI_CloseDevice(m_devtype,m_devind);
		return FALSE;
	}
	if(VCI_StartCAN(m_devtype,m_devind,m_cannum)!=STATUS_OK)
	{
		AfxMessageBox("启动CAN失败!\n");
		VCI_CloseDevice(m_devtype,m_devind);
		return FALSE;		
	}

	m_connect=1;
		return TRUE;
}
UINT CCanControl::CloseCan()
{
	if (m_connect)
	{
		VCI_CloseDevice(m_devtype,m_devind);
	}
	return 0;
}

void CCanControl::SendCarSpeed(bool control,unsigned char brake, unsigned short speed)
{
	unsigned int ret;
	unsigned char IDbuff[4],Databuff[8];

	IDbuff[0]=0;
	IDbuff[1]=0;
	IDbuff[2] = 0x05;
	IDbuff[3] = 0x4f;
	Databuff[0] = 0xa5;
	Databuff[1] = 0xa5;
	Databuff[2] = 0xa5;
	Databuff[3] = 0xa5;
	Databuff[4] = control;
	Databuff[5] = brake;
	Databuff[6] = (speed&0xff);
	Databuff[7] = (speed>>8);
	//判定已经处于刹车状态是不响应刹车
	if (brake==3)
	{
		if (m_eBrakes==SOFTDOWN||m_eBrakes==SOFTUP)
		{
			return;
		}
	}
	//判定已经处于松刹车状态是不再松刹车
	else if (brake==4)
	{
		if (m_eBrakes==UNSOFTDOWN||m_eBrakes==UNSOFTUP)
		{
			return;
		}
	}
	VCI_CAN_OBJ vco;
	ZeroMemory(&vco,sizeof(VCI_CAN_OBJ));
	vco.ID=((DWORD)IDbuff[0]<<24)+((DWORD)IDbuff[1]<<16)+((DWORD)IDbuff[2]<<8)+
		((DWORD)IDbuff[3]);
	memcpy(vco.Data,Databuff,8);
	vco.DataLen=8;
	//vco.SendType=2;
	int sendTimeout=10;
	VCI_SetReference(m_devtype,m_devind,m_cannum,3,&sendTimeout);//设置发送超时
	ret=VCI_Transmit(m_devtype,m_devind,m_cannum,&vco,1);
	Sleep(_COMMAND_SLEEP);
}
//编码器查询函数，0 归零 ，1 查询
void CCanControl::SendEncode(bool require)
{
	unsigned int ret=0;
	unsigned char IDbuff[4],Databuff[8];

	IDbuff[0]=0;
	IDbuff[1]=0;
	IDbuff[2] = 0x05;
	IDbuff[3] = 0x7f;
	Databuff[0] = 0xa5;
	Databuff[1] = 0xa5;
	Databuff[2] = 0xa5;
	Databuff[3] = 0xa5;
	Databuff[4] = 0xa5;
	Databuff[5] = 0xa5;
	Databuff[6] = 0xa5;
	Databuff[7] = require;

	VCI_CAN_OBJ vco;
	ZeroMemory(&vco,sizeof(VCI_CAN_OBJ));
	vco.ID=((DWORD)IDbuff[0]<<24)+((DWORD)IDbuff[1]<<16)+((DWORD)IDbuff[2]<<8)+
		((DWORD)IDbuff[3]);
	memcpy(vco.Data,Databuff,8);
	vco.DataLen=8;
	//vco.SendType=2;
	int sendTimeout=10;
	VCI_SetReference(m_devtype,m_devind,m_cannum,3,&sendTimeout);//设置发送超时
	ret=VCI_Transmit(m_devtype,m_devind,m_cannum,&vco,1);
	Sleep(_COMMAND_SLEEP);
}
void CCanControl::SendEncodeEx(int status)//查询编码器状态
{
	unsigned int ret=0;
	unsigned char IDbuff[4],Databuff[8];

	IDbuff[0]=0;
	IDbuff[1]=0;
	IDbuff[2] = 0x05;
	IDbuff[3] = 0x7f;
	Databuff[0] = 0xa5;
	Databuff[1] = 0xa5;
	Databuff[2] = 0xa5;
	Databuff[3] = 0xa5;
	Databuff[4] = 0xa5;
	Databuff[5] = 0xa5;
	Databuff[6] = 0xa5;
	Databuff[7] = status;

	VCI_CAN_OBJ vco;
	ZeroMemory(&vco,sizeof(VCI_CAN_OBJ));
	vco.ID=((DWORD)IDbuff[0]<<24)+((DWORD)IDbuff[1]<<16)+((DWORD)IDbuff[2]<<8)+
		((DWORD)IDbuff[3]);
	memcpy(vco.Data,Databuff,8);
	vco.DataLen=8;
	//vco.SendType=2;
	int sendTimeout=10;
	VCI_SetReference(m_devtype,m_devind,m_cannum,3,&sendTimeout);//设置发送超时
	ret=VCI_Transmit(m_devtype,m_devind,m_cannum,&vco,1);
	Sleep(_COMMAND_SLEEP);
}
void CCanControl::SendBrakeEx(int status)//控制刹车开度 0~100
{
	unsigned int ret;
	unsigned char IDbuff[4],Databuff[8];

	IDbuff[0]=0;
	IDbuff[1]=0;
	IDbuff[2] = 0x05;
	IDbuff[3] = 0x4f;
	Databuff[0] = 0xa5;
	Databuff[1] = 0xa5;
	Databuff[2] = 0xa5;
	Databuff[3] = 0xa5;
	Databuff[4] = 1;
	Databuff[5] = 1;
	Databuff[6] = (status&0xff);
	Databuff[7] = (status>>8);
	VCI_CAN_OBJ vco;
	ZeroMemory(&vco,sizeof(VCI_CAN_OBJ));
	vco.ID=((DWORD)IDbuff[0]<<24)+((DWORD)IDbuff[1]<<16)+((DWORD)IDbuff[2]<<8)+
		((DWORD)IDbuff[3]);
	memcpy(vco.Data,Databuff,8);
	vco.DataLen=8;
	//vco.SendType=2;
	int sendTimeout=10;
	VCI_SetReference(m_devtype,m_devind,m_cannum,3,&sendTimeout);//设置发送超时
	ret=VCI_Transmit(m_devtype,m_devind,m_cannum,&vco,1);
	Sleep(_COMMAND_SLEEP);
}
void CCanControl::SendSpeedEx(int status)//控制油门开度 0~255
{
	unsigned int ret;
	unsigned char IDbuff[4],Databuff[8];

	IDbuff[0]=0;
	IDbuff[1]=0;
	IDbuff[2] = 0x05;
	IDbuff[3] = 0x4f;
	Databuff[0] = 0xa5;
	Databuff[1] = 0xa5;
	Databuff[2] = 0xa5;
	Databuff[3] = 0xa5;
	Databuff[4] = 1;
	Databuff[5] = 2;
	Databuff[6] = (status&0xff);
	Databuff[7] = (status>>8);
	VCI_CAN_OBJ vco;
	ZeroMemory(&vco,sizeof(VCI_CAN_OBJ));
	vco.ID=((DWORD)IDbuff[0]<<24)+((DWORD)IDbuff[1]<<16)+((DWORD)IDbuff[2]<<8)+
		((DWORD)IDbuff[3]);
	memcpy(vco.Data,Databuff,8);
	vco.DataLen=8;
	//vco.SendType=2;
	int sendTimeout=10;
	VCI_SetReference(m_devtype,m_devind,m_cannum,3,&sendTimeout);//设置发送超时
	ret=VCI_Transmit(m_devtype,m_devind,m_cannum,&vco,1);
	Sleep(_COMMAND_SLEEP);
}
	void CCanControl::SendCarAngle(bool control,unsigned short angle)
{
	unsigned int ret=0;
	unsigned char IDbuff[4],Databuff[8];

	IDbuff[0]=0;
	IDbuff[1]=0;
	IDbuff[2] = 0x05;
	IDbuff[3] = 0x7c;
	Databuff[0] = 0xa5;
	Databuff[1] = 0xa5;
	Databuff[2] = 0xa5;
	Databuff[3] = 0xa5;
	Databuff[4] = 0xa5;
	Databuff[5] = control;
	Databuff[6] = (angle&0xff);
	Databuff[7] = (angle>>8);

	VCI_CAN_OBJ vco;
	ZeroMemory(&vco,sizeof(VCI_CAN_OBJ));
	vco.ID=((DWORD)IDbuff[0]<<24)+((DWORD)IDbuff[1]<<16)+((DWORD)IDbuff[2]<<8)+
		((DWORD)IDbuff[3]);
	memcpy(vco.Data,Databuff,8);
	vco.DataLen=8;
	//vco.SendType=2;
	int sendTimeout=10;
	VCI_SetReference(m_devtype,m_devind,m_cannum,3,&sendTimeout);//设置发送超时
	ret=VCI_Transmit(m_devtype,m_devind,m_cannum,&vco,1);
	Sleep(_COMMAND_SLEEP);
}
//CAN	数据接受
void CCanControl::ProCanRcv()
{
	VCI_CAN_OBJ frameinfo[50];
	VCI_ERR_INFO errinfo;
	int len=1;
	int i=0;
	bool angle=false,speed=false;
	unsigned short LastEncode=0;

	while(!m_bStopThread)
	{
		Sleep(1);
//		WaitForSingleObject(m_hRcvHandle,INFINITE);
		if(m_connect==0)
			break;
			long ret=VCI_Receive(m_devtype,m_devind,m_cannum,frameinfo,20,50);
			if(ret<=0)
			{
				//注意：如果没有读到数据则必须调用此函数来读取出当前的错误码，
				//千万不能省略这一步（即使你可能不想知道错误码是什么）
				VCI_ReadErrInfo(m_devtype,m_devind,m_cannum,&errinfo);
			}
			else
			{
				for (i=0;i<len;i++)
				{
					if (frameinfo[i].RemoteFlag==0)
					{
						unsigned char IDbuff[4],Databuff[8];
						IDbuff[0]=frameinfo[i].ID>>24;
						IDbuff[1]=frameinfo[i].ID>>16;
						IDbuff[2]=frameinfo[i].ID>>8;
						IDbuff[3]=frameinfo[i].ID&0xff;

						if (frameinfo[i].DataLen>8)
						{
							frameinfo[i].DataLen=8;
						}
						for (int j=0;j<frameinfo[i].DataLen;j++)
						{
							Databuff[j]=frameinfo[i].Data[j];
						}
						if(IDbuff[2]==0x05&&IDbuff[3]==0x7c)
						{
							m_nCarAngle=Databuff[6]+(Databuff[7]<<8);
							g_CurWheelAngle=m_nCarAngle;
							angle=true;

						}
						if(IDbuff[2]==0x05&&IDbuff[3]==0x7f)
						{
							m_nEncodeNum=Databuff[4]+(Databuff[5]*256)+(Databuff[6]*256*256)+(Databuff[7]*256*256*256);
							//angle=true;
						}
						//接收速度值
						else if (IDbuff[2]==0x05&&IDbuff[3]==0x4f)
						{
							//返回信号为实际速度值
							if (Databuff[5]==0xA5)
							{
								m_nCarSpeed=Databuff[6]+(Databuff[7]<<8);
								g_CurSpeed=m_nCarSpeed;
								speed=true;
							}
							//返回信号为刹车控制值
							else
							{
								int ebrake=Databuff[5];
								switch (ebrake)
								{
								case 0x01:
									m_eBrakes=REMOTEDOWN;
									break;
								case 0x03:
									m_eBrakes=REMOTEUP;
									break;
								case 0x04:
									m_eBrakes=UNREMOTEDOWN;
								    break;
								case 0x06:
									m_eBrakes=UNREMOTEUP;
									break;
								case 0x08:
									m_eBrakes=SOFTDOWN;
									break;
								case 0x0C:
									m_eBrakes=SOFTUP;
									break;
								case  0x10:
									m_eBrakes=UNSOFTDOWN;
									break;
								case 0x30:
									m_eBrakes=UNSOFTUP;
									break;
								default:
									break;

								}

							}
							
						}
					}
				}

			}
			
		//}
		
   }
}
void CCanControl::RequireCarStatus()
{
	while(!m_bStopThread)
	{
		//WaitForSingleObject(g_gpsHandle,50);
		Sleep(5);
		SendCarAngle(false,550);
		Sleep(5);
		SendCarSpeed(false,false,0);
	    Sleep(40);
	}
	
}
UINT CCanControl::QueryCarStatus(LPVOID lpParam)
{
	CCanControl *pCanThread = (CCanControl *)lpParam;

	pCanThread->RequireCarStatus();
	pCanThread->m_sendHandle= NULL;
	pCanThread->m_threadMode = MODENULL;
	pCanThread->m_bStopThread = FALSE;
	return 0;
}
void CCanControl::CreateThread()
{
	if(m_rcvHandle != NULL)
		return;
	if(m_sendHandle != NULL)
		return;
	//	m_pStream = fopen(strHomePath+strFilename, "ab+");

	m_rcvHandle = AfxBeginThread(RcvCarStatus, 
		this,
		THREAD_PRIORITY_HIGHEST, //THREAD_PRIORITY_NORMAL,
		0,
		CREATE_SUSPENDED);
	//m_sendHandle = AfxBeginThread(QueryCarStatus, 
	//	this,
	//	THREAD_PRIORITY_HIGHEST, //THREAD_PRIORITY_NORMAL,
	//	0,
	//	CREATE_SUSPENDED);

	if(m_rcvHandle==NULL) 
	{
		AfxMessageBox("Create thread error!");
		return;
	}
	m_bStopThread = FALSE;
	m_threadMode = MODESUSPEND;

}

void CCanControl::StartThread()
{
	if(m_rcvHandle == NULL)
		CreateThread();

	if(m_rcvHandle != NULL)
	{
		m_rcvHandle->ResumeThread();
	   // m_sendHandle->ResumeThread();
	}
	else
		return;

	m_threadMode = MODERUN;
}

void CCanControl::SuspendThread()
{
	if(m_rcvHandle != NULL && m_threadMode == MODERUN)
		
	{
		m_rcvHandle->SuspendThread();
	  //  m_sendHandle->SuspendThread();
	}
	else 
		return;

	m_threadMode = MODESUSPEND;
}

void CCanControl::ResumeThread()
{
	if(m_rcvHandle != NULL && m_threadMode == MODESUSPEND)
		
	{
		m_rcvHandle->ResumeThread();
	    //m_sendHandle->ResumeThread();
	}
	else
		return;

	m_threadMode = MODERUN;
}

void CCanControl::StopThread()
{
	if(m_rcvHandle == NULL)
		return;

	m_bStopThread = TRUE;

	while(m_rcvHandle)
		Sleep(1);

	m_threadMode = MODESTOP;
}

