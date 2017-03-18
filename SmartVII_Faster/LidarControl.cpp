#include "stdafx.h"
#include "LidarControl.h"
#include "Laser_sick.h"

 void MyOutData1(unsigned char *pData, int len)
{
	g_Sick.m_bCapture1 =true;
	//fwrite(pData,len,1,g_Sick.m_fp1);
	//	TRACE("sick 1 length =%d \n", len);
	memcpy(g_Sick.m_Data_1,   pData,   len );
	//	Sleep(1);
}

void MyOutData2(unsigned char *pData, int len)
{
	g_Sick.m_bCapture2 =true;

	//fwrite(pData,len,1,g_Sick.m_fp2);
	//	TRACE("sick 2 length =%d \n", len);
	memcpy(g_Sick.m_Data_2,   pData, len );
	//	Sleep(1);
}

void MyOutData3(unsigned char *pData, int len)
{
	g_Sick.m_bCapture3 =true;
	//fwrite(pData,len,1,g_Sick.m_fp3);
	//	TRACE("sick 3 length =%d \n", len);
	memcpy(g_Sick.m_Data_3,pData, len );
	//	Sleep(1);
}

void MyOutData4(unsigned char *pData, int len)
{
	g_Sick.m_bCapture4 =true;
	//fwrite(pData,len,1,g_Sick.m_fp4);
	//	TRACE("sick 4 length =%d \n", len);
	memcpy(g_Sick.m_Data_4,pData, len );
	//	Sleep(1);
}



CLidarControl::CLidarControl(void)
{
	for (int i=0;i<4;i++)
	{
		m_lms291Thread[i].SetPortNum(14+i);
	}

	m_lms291Thread[0].SetCallBack(MyOutData1);
	m_lms291Thread[1].SetCallBack(MyOutData2);
	m_lms291Thread[2].SetCallBack(MyOutData3);
	m_lms291Thread[3].SetCallBack(MyOutData4);

	m_bIsPortOpen=FALSE; // 串口打开标志
	m_bIsDataOut=FALSE; // sick打开标志
	m_bIsSaveData=FALSE;
	//m_LidarOfflineCtrl = NULL; // 离线控制对话框
}


CLidarControl::~CLidarControl(void)
{
 	
	
}



void CLidarControl::OpenCom()
{
//	::AfxMessageBox("OpenCom");
	int flag=0;

	for (int i=0;i<4;i++) // 目前四个sick
	{
		if(i == 0 || i == 3)
		{
			if (!m_lms291Thread[i].IsPortOpen())
			{
				m_lms291Thread[i].OpenPort();

				m_lms291Thread[i].AngleMode_180_050();
				m_lms291Thread[i].SetResolutionMode_180_050();
			}
			if (!m_lms291Thread[i].IsPortOpen())
			{
				flag=1;
			}
		}
	}

	for (int i=1;i<3;i++)
	{
		if (!m_lms291Thread[i].IsPortOpen())
		{
			m_lms291Thread[i].OpenPort();
			m_lms291Thread[i].AngleMode_100_025();
			m_lms291Thread[i].SetResolutionMode_100_025();
		}
		if (!m_lms291Thread[i].IsPortOpen())
		{
			flag=1;
		}
	}

	if (flag==0)
		m_bIsPortOpen=TRUE;
}

void CLidarControl::Closecom()
{
//	::AfxMessageBox("Closecom");
	for (int i=0;i<4;i++)
	{
		if (m_lms291Thread[i].IsPortOpen())
		{
			m_lms291Thread[i].ClosePort();
			m_bIsPortOpen=FALSE;
		}
	}
}

void CLidarControl::OpenSick()
{
	//::AfxMessageBox("Open Sick");
	for (int i=0;i<4;i++)
	{
		if(i == 0 || i == 3)
		{
			m_lms291Thread[i].StartThread();
			m_lms291Thread[i].CtnsDataOutputStart();
		}
	}

	for (int i=1;i<3;i++)
	{
		m_lms291Thread[i].StartThread();
		m_lms291Thread[i].CtnsDataOutputStart();
	}

	m_bIsDataOut = TRUE;

	
}

void CLidarControl::CloseSick()
{
	//::AfxMessageBox("Close Sick");
	for (int i=0; i<4; i++)
	{
		m_lms291Thread[i].CtnsDataOutputStop();
		m_lms291Thread[i].StopThread();
	}

	m_bIsDataOut = FALSE;
}

BYTE realProData[7+732*2+812*2];

void CLidarControl::SickSaveThreadPro(LPVOID lpParam)
{
	CLidarControl* pcontrol=(CLidarControl*)lpParam;

	while(true)
	{
		if (!(pcontrol->m_bIsDataOut&&pcontrol->m_bIsPortOpen&&pcontrol->m_bIsSaveData))
		{
			_endthread();
		}

		SYSTEMTIME st;
		GetSystemTime(&st);

		realProData[0]=0xa5;
		realProData[1]=0xa5;
		realProData[2]=st.wHour;
		realProData[3]=st.wMinute;
		realProData[4]=st.wSecond;
		realProData[5]=(st.wMilliseconds>>8);
		realProData[6]=(st.wMilliseconds&0xff);
		Sleep(30);
	}
}

void CLidarControl::SatrtThread()
{
	if (m_bIsSaveData)
	{
		_beginthread(SickSaveThreadPro,0,this);
	}
	
}

