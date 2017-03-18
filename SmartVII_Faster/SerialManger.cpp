#include "StdAfx.h"
#include "SerialManger.h"
#include "Pcomm.h"
#pragma comment(lib,"lib/Pcomm.lib")
CSerialManger::CSerialManger(void)
{
	m_pDataBuffer=NULL;
	m_nDataLength=0;
	m_nBaudRate=0;
	m_pThreadHandle=NULL;
	InitPortSetting();
	//_CrtSetBreakAlloc(847);
}


CSerialManger::~CSerialManger(void)
{
}
void CSerialManger::InitPortSetting()
{
	m_commData.Port = 1;
	m_commData.ibaudrate  = 12;
	m_commData.iparity = 0;
	m_commData.ibytesize = 3;
	m_commData.istopbits = 0;
	m_commData.BaudRate  = m_nBaudRate;
	m_commData.Parity = P_NONE;
	m_commData.ByteSize = BIT_8;
	m_commData.StopBits = STOP_1;
	m_commData.Hw = FALSE;
	m_commData.Sw = FALSE;
	m_commData.Dtr = TRUE;
	m_commData.Rts = TRUE;
}
void CSerialManger::InitDataBuffer()
{
	/*if (m_nDataLength>0)
	{
	m_pDataBuffer=new unsigned char[m_nDataLength];
	}*/
}
void CSerialManger::DeleteDataBuffer()
{
	//if ((m_nDataLength>0)&&(m_pDataBuffer=NULL))
	//{
	//	delete[] m_pDataBuffer;
	//}
}
BOOL CSerialManger::OpenPort()
{
	//m_pConfigFile=NULL;
	return TRUE;
}
void CSerialManger::ClosePort()
{

}
BOOL CSerialManger::PortSet(int Port)
{
	int port=Port;
	int	mode = m_commData.Parity | m_commData.ByteSize | m_commData.StopBits;
	int	hw = m_commData.Hw ? 3 : 0;	// bit0 and bit1 
	int	sw = m_commData.Sw ? 12 : 0;     // bit2 and bit3
	int	ret ;
	DWORD   tout;

	if((ret=sio_ioctl(port,m_nBaudRate,mode))!=SIO_OK){
		//MxShowError("sio_ioctl",ret);
		return FALSE;
	}

	if((ret=sio_flowctrl(port,hw | sw))!=SIO_OK){
		//MxShowError("sio_flowctrl",ret);
		return FALSE;
	}

	if((ret=sio_DTR(port,(m_commData.Dtr ? 1:0)))!=SIO_OK){
		//MxShowError("sio_DTR",ret);
		return FALSE;
	}

	if(!m_commData.Hw){
		if((ret=sio_RTS(port,(m_commData.Rts ? 1:0)))!=SIO_OK){
			//MxShowError("sio_RTS",ret);
			return FALSE;
		}
	}

	tout = 1000 / sio_getbaud(port);  // ms/byte
	if (tout < 1)
		tout = 1;                 
	tout = tout * 1 * 3;             // 1 byte; '*3' is for delay
	if(tout<100)
		tout = 100;
	sio_SetWriteTimeouts(port, tout);
	return TRUE;
}
BOOL CSerialManger::PortBaudRate(int Port)
{
	int	port = Port;
	int	ret ;
	/*long speed;
	speed=491520L;*/
	if (m_nBaudRate==0)
	{
		return FALSE;
	}
	if((ret=sio_baud(port, m_nBaudRate))!=SIO_OK){
		//MxShowError("sio_baud",ret);
		return FALSE;
	}
	if((ret=sio_getbaud(port))<0){
		//MxShowError("sio_getbaud",ret);
		return FALSE;
	}
	return TRUE;

}


//void CSerialManger::SetConfigFileName(FILE *fp)
//{
//	m_pConfigFile=fp;
//}
void CSerialManger::SetBaudRate(long baud)
{
	m_nBaudRate=baud;
}