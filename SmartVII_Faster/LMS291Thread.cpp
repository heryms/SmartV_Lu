// LMS291Thread.cpp: implementation of the CLMS291Thread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LMS291Thread.h"
//#include "ViewVelodyneView.h"
//#include "ViewVelodyne.h"
//#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//extern CViewVelodyneApp theApp;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLMS291Thread::CLMS291Thread()
{
	m_pThreadHandle = NULL;
	m_fnOutData = NULL;

	m_bStopThread = FALSE;

	m_threadMode = MODENULL;

	//port
	m_bOpen = FALSE;
	m_baudRate = BR500K;

	InitPortSetting();

	AngleMode_180_050();
}

void CLMS291Thread::InitPortSetting()
{
	m_commData.Port = 1;
	m_commData.ibaudrate  = 12;
	m_commData.iparity = 0;
	m_commData.ibytesize = 3;
	m_commData.istopbits = 0;
	m_commData.BaudRate  = B9600;
	m_commData.Parity = P_NONE;
	m_commData.ByteSize = BIT_8;
	m_commData.StopBits = STOP_1;
	m_commData.Hw = FALSE;
	m_commData.Sw = FALSE;
	m_commData.Dtr = TRUE;
	m_commData.Rts = TRUE;

	switch(m_baudRate) 
	{
	case BR9600:
		m_commData.ibaudrate  = 12;
		m_commData.BaudRate  = B9600;
		break;
	case BR19200:
		m_commData.ibaudrate  = 13;
		m_commData.BaudRate  = B19200;
		break;
	case BR38400:
		m_commData.ibaudrate  = 14;
		m_commData.BaudRate  = B38400;
		break;
	case BR500K:
		m_commData.ibaudrate  = 18;
		m_commData.BaudRate  = B460800;
		break;
	};
}

CLMS291Thread::~CLMS291Thread()
{
	StopThread();
}

void CLMS291Thread::CreateThread()
{
	if(m_pThreadHandle != NULL)
		return;

	char buffer[_MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), buffer, _MAX_PATH);
	CString strHomePath = GetFilePath(buffer);

//	m_pStream = fopen(strHomePath+strFilename, "ab+");

	m_pThreadHandle = AfxBeginThread(LMS291ThreadPro, 
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

void CLMS291Thread::StartThread()
{
	if(m_pThreadHandle == NULL)
		CreateThread();

	if(m_pThreadHandle != NULL)
		m_pThreadHandle->ResumeThread();
	else
		return;

	m_threadMode = MODERUN;
}

void CLMS291Thread::SuspendThread()
{
	if(m_pThreadHandle != NULL && m_threadMode == MODERUN)
		m_pThreadHandle->SuspendThread();
	else 
		return;

	m_threadMode = MODESUSPEND;
}

void CLMS291Thread::ResumeThread()
{
	if(m_pThreadHandle != NULL && m_threadMode == MODESUSPEND)
		m_pThreadHandle->ResumeThread();
	else
		return;

	m_threadMode = MODERUN;
}

void CLMS291Thread::StopThread()
{
	if(m_pThreadHandle == NULL)
		return;

	m_bStopThread = TRUE;

	while(m_pThreadHandle)
		Sleep(1);

	m_threadMode = MODESTOP;
}

inline BOOL CLMS291Thread::IsHead(const unsigned char *const buf, int iPos)
{
	if(m_nAngleMode==0)  
	{
		return (buf[_NOMAL_(iPos)]==0x02 
			&& buf[_NOMAL_(iPos+1)]==0x80 
			&& buf[_NOMAL_(iPos+2)]==0xCE
			&& buf[_NOMAL_(iPos+3)]==0x00
			&& buf[_NOMAL_(iPos+4)]==0xB0
			&& buf[_NOMAL_(iPos+5)]==0x65
			&& (buf[_NOMAL_(iPos+6)]==0x00 || buf[_NOMAL_(iPos+6)]==0x40));
	}
	else if(m_nAngleMode==1)  
	{
		return (buf[_NOMAL_(iPos)]==0x02 
			&& buf[_NOMAL_(iPos+1)]==0x80 
			&& buf[_NOMAL_(iPos+2)]==0x96
			&& buf[_NOMAL_(iPos+3)]==0x01
			&& buf[_NOMAL_(iPos+4)]==0xB0
			&& buf[_NOMAL_(iPos+5)]==0xC9
			&& (buf[_NOMAL_(iPos+6)]==0x00 || buf[_NOMAL_(iPos+6)]==0x40));
	}
	else if(m_nAngleMode==2)  
	{
		return (buf[_NOMAL_(iPos)]==0x02 
			&& buf[_NOMAL_(iPos+1)]==0x80 
			&& buf[_NOMAL_(iPos+2)]==0x26
			&& buf[_NOMAL_(iPos+3)]==0x03
			&& buf[_NOMAL_(iPos+4)]==0xB0
			&& buf[_NOMAL_(iPos+5)]==0x91
			&& ( buf[_NOMAL_(iPos+6)]==0x01 || buf[_NOMAL_(iPos+6)]==0x41));
	}
	else if(m_nAngleMode==3)  
	{
		return (buf[_NOMAL_(iPos)]==0x02 
			&& buf[_NOMAL_(iPos+1)]==0x80 
			&& buf[_NOMAL_(iPos+2)]==0x6E
			&& buf[_NOMAL_(iPos+3)]==0x01
			&& buf[_NOMAL_(iPos+4)]==0xB0
			&& buf[_NOMAL_(iPos+5)]==0xB5
			&& ( buf[_NOMAL_(iPos+6)]==0x00 || buf[_NOMAL_(iPos+6)]==0x40));
	}
	else if(m_nAngleMode==4)  
	{
		return (buf[_NOMAL_(iPos)]==0x02 
			&& buf[_NOMAL_(iPos+1)]==0x80 
			&& buf[_NOMAL_(iPos+2)]==0xD6
			&& buf[_NOMAL_(iPos+3)]==0x02
			&& buf[_NOMAL_(iPos+4)]==0xB0
			&& buf[_NOMAL_(iPos+5)]==0x69
			&& ( buf[_NOMAL_(iPos+6)]==0x01 || buf[_NOMAL_(iPos+6)]==0x41));
	}
}

inline BOOL CLMS291Thread::IsLengthCheckOK(const unsigned char *const buf, int iPos)
{
	UINT sumLen, sumDataLen;
	//Length of the total LMS output data string.
	sumLen = buf[_NOMAL_(iPos+3)]*256+buf[_NOMAL_(iPos+2)];

	//Number of measurement data bytes
	unsigned char tp = buf[_NOMAL_(iPos+6)] & 0x0F;
	sumDataLen = tp*256+buf[_NOMAL_(iPos+5)];
	unsigned int crc=CreateCRC(buf,_max_mea_len_+4);
	return ((sumDataLen == _max_pt_length_) && (sumLen == _max_mea_len_)&&(buf[_max_tele_length_-2]==(crc&0xff))&&(buf[_max_tele_length_-1]==(crc>>8)));
}

void CLMS291Thread::ProcComData()
{
	int	len;
	long i, j;

	long lHead = 0;

	long pos1, pos2;
	BOOL bFindData=FALSE;

	BOOL bHeadExist = FALSE;
	double begin=0;
	double end=0;
	while(!m_bStopThread) 
	{
		Sleep(1);
	    DWORD ret=	WaitForSingleObject(m_hWaitfor,50);
	    ResetEvent(m_hGetData);
		len = sio_read(m_commData.Port,	buf,
			_MAX_REC_BUF_SIZE_);
		//if the buffer is empty, wait a few monent
		if(len<1) {	Sleep(50);	continue;}
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

		while(lHead >= _max_tele_length_ && !m_bStopThread) 
		{
			bFindData = FALSE;
			pos1 = -1; pos2 = -1;

			bHeadExist = FALSE;
			/////////////////////////////
	#pragma region 解析数据
		    for(i=0; i<=lHead-_max_tele_length_; i++) 
			{
				//从头到尾搜索
				if(IsHead(chBuf, i))
				{
					bHeadExist=TRUE;
					//若找到数据头，判定数据头之后自己是否为完整包
					if (lHead-i>=_max_tele_length_)
					{		//是否长度数据和检校位正确			
						if(IsLengthCheckOK(chBuf, i)) 
						{
							
							pos1 = i; 
							////一帧的开始位置
							//bHeadExist = FALSE;
							//-----------------------------------------------
							bFindData = TRUE;
							pos2=pos1+_max_tele_length_;
							//  //如果长度正确
							memcpy(bufOK, chBuf+pos1, _max_tele_length_);
					
							//更改搜索数组和数据长度
							memcpy(chBuf, chBuf+pos2, lHead-pos2);
							lHead = lHead-pos2;
							//----------------------------------------
							break; //中止循环
						}
					}	
					//找到数据头，但是数据长度不够
					else
					{
						lHead=0;
						bFindData=FALSE;
						break;
					}
				}
			}
#pragma endregion 解析数据

           lHead=0;
		}
		if (bFindData)
		{
			SetEvent(m_hGetData);
		//	SetEvent(g_gpsHandle);
		}
	
	}
}

UINT CLMS291Thread::LMS291ThreadPro(LPVOID lpParam)
{
	CLMS291Thread *pLMS221Thread = (CLMS291Thread *)lpParam;

	pLMS221Thread->ProcComData();

	pLMS221Thread->m_pThreadHandle = NULL;
	pLMS221Thread->m_threadMode = MODENULL;
	pLMS221Thread->m_bStopThread = FALSE;

	return 0;
}

void CLMS291Thread::ClosePort()
{
	sio_close(m_commData.Port);

	m_bOpen = FALSE;
}

BOOL CLMS291Thread::OpenPort()
{
	int     ret;

	if((ret=sio_open(m_commData.Port))!=SIO_OK){
		MxShowError("打开串口操作失败!",ret);
		return FALSE;
	}

	if(!PortSet()){
		sio_close(m_commData.Port);
		return FALSE;
	}

	if(!PortBaudRate()){
		sio_close(m_commData.Port);
		return FALSE;
	}

	m_bOpen = TRUE;

	return TRUE;
}

BOOL CLMS291Thread::PortSet()
{
	int	port = m_commData.Port;
	int	mode = m_commData.Parity | m_commData.ByteSize | m_commData.StopBits;
	int	hw = m_commData.Hw ? 3 : 0;	// bit0 and bit1 
	int	sw = m_commData.Sw ? 12 : 0;     // bit2 and bit3
	int	ret ;
	DWORD   tout;

	if((ret=sio_ioctl(port,m_commData.BaudRate,mode))!=SIO_OK){
		MxShowError("sio_ioctl",ret);
		return FALSE;
	}

	if((ret=sio_flowctrl(port,hw | sw))!=SIO_OK){
		MxShowError("sio_flowctrl",ret);
		return FALSE;
	}

	if((ret=sio_DTR(port,(m_commData.Dtr ? 1:0)))!=SIO_OK){
		MxShowError("sio_DTR",ret);
		return FALSE;
	}

	if(!m_commData.Hw){
		if((ret=sio_RTS(port,(m_commData.Rts ? 1:0)))!=SIO_OK){
			MxShowError("sio_RTS",ret);
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

BOOL CLMS291Thread::PortBaudRate()
{
	int	port = m_commData.Port;
	int	ret ;

	long speed;
	switch(m_baudRate) {
	case BR9600:
		speed=9600L; 
		break;
	case BR19200:
		speed=19200L;
		break;
	case BR38400:
		speed=38400L;
		break;
	case BR500K:
		speed=491520L;
		break;
	};

	if((ret=sio_baud(port, speed))!=SIO_OK){
		MxShowError("sio_baud",ret);
		return FALSE;
	}

	if((ret=sio_getbaud(port))<0){
		MxShowError("sio_getbaud",ret);
		return FALSE;
	}

	return TRUE;
}

// Function name	: MxShowError
// Description	    : 
// Return type		: void 
// Argument         : LPCTSTR title
// Argument         : int errcode
// Come from		: PCommLt11
void CLMS291Thread::MxShowError(LPCTSTR title,int errcode)
{
	char	buf[200];

	if(errcode != SIO_WIN32FAIL){
		switch(errcode){
		case SIO_BADPORT:
			sprintf(buf,"[COM%d] Port number is invalid or port is not opened in advance!\n[COM%d] 端口无效或端口不能被继续打开！", m_commData.Port, m_commData.Port);
			//lstrcpy(buf,"Port number is invalid or port is not opened in advance!\n端口无效或端口不能被继续打开！");
			break;
		case SIO_OUTCONTROL:
			lstrcpy(buf,"The board does not support this function!\n此MOXA卡不支持该操作！");
			break;
		case SIO_NODATA:
			lstrcpy(buf,"No data to read!\n无数据可读！");
			break;
		case SIO_OPENFAIL:
			sprintf(buf,"[COM%d] Port number is invalid or port is not opened in advance!\n[COM%d] 端口无效或端口不能被继续打开！", m_commData.Port, m_commData.Port);
			//lstrcpy(buf,"No such port or port is occupied by other program!\n无此端口或端口被其它程序占用！");
			break;
		case SIO_RTS_BY_HW:
			lstrcpy(buf,"RTS can't be set because RTS/CTS Flowctrl");
			break;
		case SIO_BADPARM:
			lstrcpy(buf,"Bad parameter!\n参数错误！");
			break;
		case SIO_BOARDNOTSUPPORT:
			lstrcpy(buf,"The board does not support this function!\n此MOXA卡不支持该操作！");
			break;
		case SIO_ABORTWRITE:
			lstrcpy(buf,"Write has blocked, and user abort write!\n写数据受阻，用户终止了写操作！");
			break;
		case SIO_WRITETIMEOUT:
			lstrcpy(buf,"Write timeout has happened!\n写数据超时！");
			break;
		default:
			wsprintf(buf,"Unknown Error:%d\n未知名的错误：%d", errcode, errcode);
			break;
		}
		MessageBox(NULL,buf,title,MB_OK|MB_ICONEXCLAMATION);
	}else
		ShowSysErr(title);
}

// Function name	: ShowSysErr
// Description	    : 
// Return type		: void 
// Argument         : LPCTSTR title
// Come from		: PCommLt11
void CLMS291Thread::ShowSysErr(LPCTSTR title)
{
	LPSTR	lpMsgBuf;
	DWORD	syserr;

	syserr = GetLastError();
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		syserr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language */
		(LPTSTR) &lpMsgBuf,
		0,
		NULL
		);
	// Display the string.
	MessageBox(NULL,lpMsgBuf,title,MB_OK|MB_ICONEXCLAMATION);
	// Free the buffer.
	LocalFree(lpMsgBuf);
}

void CLMS291Thread::CtnsDataOutputStart()
{
	if(!m_bOpen)
		return;

	int 	len;
	char 	buf[8];
	buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x02;buf[3] = 0x00;
	buf[4] = 0x20;buf[5] = 0x24;buf[6] = 0x34;buf[7] = 0x08;

	for(int i=0; i<5; i++) {
		len = sio_write(m_commData.Port, buf, 8);
		if(len>=0 && i>3)
			break;
		Sleep(20);
	}
}

void CLMS291Thread::CtnsDataOutputStop()
{
	if(!m_bOpen)
		return;

	int 	len;
	char 	buf[8];
	buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x02;buf[3] = 0x00;
	buf[4] = 0x20;buf[5] = 0x25;buf[6] = 0x35;buf[7] = 0x08;

	for(int i=0; i<5; i++) {
		len = sio_write(m_commData.Port, buf, 8);
		if(len>=0)
			break;
		Sleep(20);
	}
}

void CLMS291Thread::SetMaster(BOOL bMaster)
{
	if(!m_bOpen)
		return;

	///////////////////////////////////
	//master 
	if(bMaster) {
		char tpCh[] = {(char)0x02, (char)0x00, (char)0x23, (char)0x00, 
			(char)0x77, (char)0x00, (char)0x00, (char)0x46, (char)0x00, 
			(char)0x00, (char)0x00, (char)0x00, (char)0x00,
			(char)0x00, (char)0x02, (char)0x42, (char)0x02, (char)0x00, 
			(char)0x00, (char)0x0A, (char)0x0A, (char)0x50, (char)0x64, 
			(char)0x00, (char)0x0A, (char)0x0A, (char)0x50, (char)0x64, 
			(char)0x00, (char)0x0A, (char)0x0A, (char)0x50, (char)0x64, 
			(char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x02, (char)0x00,
			(char)0x1A, (char)0x13};

		for(int i=0; i<5; i++) {
			int len = sio_write(m_commData.Port, tpCh, 41);
			if(len>=0)
				break;
			Sleep(20);
		}
	}
	else {
		char tpCh[] = {(char)0x02, (char)0x00, (char)0x23, (char)0x00, 
			(char)0x77, (char)0x00, (char)0x00, (char)0x46, (char)0x00, 
			(char)0x00, (char)0x00, (char)0x00, (char)0x00,
			(char)0x00, (char)0x02, (char)0x02, (char)0x02, (char)0x00, 
			(char)0x00, (char)0x0A, (char)0x0A, (char)0x50, (char)0x64, 
			(char)0x00, (char)0x0A, (char)0x0A, (char)0x50, (char)0x64, 
			(char)0x00, (char)0x0A, (char)0x0A, (char)0x50, (char)0x64, 
			(char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x02, (char)0x00,
			(char)0x0A, (char)0x00};

		for(int i=0; i<5; i++) {
			int len = sio_write(m_commData.Port, tpCh, 41);
			if(len>=0)
				break;
			Sleep(20);
		}
	}
}

void CLMS291Thread::SetBaudrate9600()
{
	if(!m_bOpen)
		return;

	int 	len;
	char 	buf[8];
	buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x02;buf[3] = 0x00;
	buf[4] = 0x20;buf[5] = 0x42;buf[6] = 0x52;buf[7] = 0x08;

	for(int i=0; i<5; i++) {
		len = sio_write(m_commData.Port, buf, 8);
		if(len>=0)
			break;
		Sleep(20);
	}

	///////////////////////
	//0: set statue
	Sleep(20);
	m_baudRate = BR9600;
	//1: close port
	ClosePort();
	//2: Open port
	Sleep(500);
	OpenPort();
}

void CLMS291Thread::SetBaudrate19200()
{
	if(!m_bOpen)
		return;

	int 	len;
	char 	buf[8];
	buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x02;buf[3] = 0x00;
	buf[4] = 0x20;buf[5] = 0x41;buf[6] = 0x51;buf[7] = 0x08;

	for(int i=0; i<5; i++) {
		len = sio_write(m_commData.Port, buf, 8);
		if(len>=0)
			break;
		Sleep(20);
	}

	///////////////////////
	//0: set statue
	Sleep(20);
	m_baudRate = BR19200;
	//1: close port
	ClosePort();
	//2: Open port
	Sleep(500);
	OpenPort();
}

void CLMS291Thread::SetBaudrate38400()
{
	if(!m_bOpen)
		return;

	int 	len;
	char 	buf[8];
	buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x02;buf[3] = 0x00;
	buf[4] = 0x20;buf[5] = 0x40;buf[6] = 0x50;buf[7] = 0x08;

	for(int i=0; i<5; i++) {
		len = sio_write(m_commData.Port, buf, 8);
		if(len>=0)
			break;
		Sleep(20);
	}

	///////////////////////
	//0: set statue
	Sleep(20);
	m_baudRate = BR38400;
	//1: close port
	ClosePort();
	//2: Open port
	Sleep(500);
	OpenPort();
}

void CLMS291Thread::SetBaudrate500K()
{
	if(!m_bOpen)
		return;

	int 	len;
	char 	buf[8];
	buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x02;buf[3] = 0x00;
	buf[4] = 0x20;buf[5] = 0x48;buf[6] = 0x58;buf[7] = 0x08;

	for(int i=0; i<5; i++) {
		len = sio_write(m_commData.Port, buf, 8);
		if(len>=0)
			break;
		Sleep(20);
	}

	///////////////////////
	//0: set statue
	Sleep(20);
	m_baudRate = BR500K;
	//1: close port
	ClosePort();
	//2: Open port
	Sleep(500);
	OpenPort();
}

void CLMS291Thread::PowerOnBaudrate()
{
	int i;
	if(!m_bOpen)
		return;

	////////////////////////////////////////////////////////////////////
	int 	len;
	char 	buf[16];
	//command 1
	buf[ 0] = 0x02;buf[ 1] = 0x00;buf[ 2] = 0x0A;buf[ 3] = 0x00;
	buf[ 4] = 0x20;buf[ 5] = 0x00;buf[ 6] = 0x53;buf[ 7] = 0x49;
	buf[ 8] = 0x43;buf[ 9] = 0x4B;buf[10] = 0x5F;buf[11] = 0x4C;
	buf[12] = 0x4D;buf[13] = 0x53;buf[14] = 0xBE;buf[15] = 0xC5;
	len = 16;
	for(i=0; i<5; i++) 
	{
		len = sio_write(m_commData.Port, buf, 16);
		if(len>=0)
			break;
		Sleep(20);
	}
	Sleep(2000);

	//command 2
	///////////////////////////////////////////////////////////////////
	buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x02;buf[3] = 0x00;
	buf[4] = 0x66;buf[5] = 0x01;buf[6] = 0x9D;buf[7] = 0x4E;
	len = 8;
	for( i=0; i<5; i++) 
	{
		len = sio_write(m_commData.Port, buf, 8);
		if(len>=0)
			break;
		Sleep(20);
	}

	Sleep(2000);
}

void CLMS291Thread::SetUnitMM()
{
	int i;
	if(!m_bOpen)
		return;

	int 	len;
	char 	buf[48];

	//command 1
	buf[ 0] = (char)0x02;buf[ 1] = (char)0x00;buf[ 2] = (char)0x0A;buf[ 3] = (char)0x00;
	buf[ 4] = (char)0x20;buf[ 5] = (char)0x00;buf[ 6] = (char)0x53;buf[ 7] = (char)0x49;
	buf[ 8] = (char)0x43;buf[ 9] = (char)0x4B;buf[10] = (char)0x5F;buf[11] = (char)0x4C;
	buf[12] = (char)0x4D;buf[13] = (char)0x53;buf[14] = (char)0xBE;buf[15] = (char)0xC5;
	for(i=0; i<5; i++) 
	{
		len = sio_write(m_commData.Port, buf, 16);
		if(len>=0)
			break;
		Sleep(20);
	}

	Sleep(2000);
	//command 2
	char tpCh[] = {(char)02, (char)00, (char)0x21, (char)00, (char)0x77,
		(char)00, (char)00, (char)00, (char)00, 
		(char)00, (char)00, (char)01, (char)00,
		(char)00, (char)02, (char)02, (char)00, 
		(char)00, (char)00, (char)00, (char)00, 
		(char)00, (char)00, (char)00, (char)00, 
		(char)00, (char)00, (char)00, (char)00, 
		(char)00, (char)00, (char)00, (char)00, 
		(char)00, (char)00, (char)00, (char)00, 
		(char)0xFC, (char)0x7E};
	for( i=0; i<5; i++)
	{
		len = sio_write(m_commData.Port, tpCh, 39);
		if(len>=0)
			break;
		Sleep(20);
	}

	Sleep(8000);
}

void CLMS291Thread::SetUnitCM()
{
	int i;
	if(!m_bOpen)
		return;

	int 	len;
	char 	buf[48];

	//command 1
	buf[ 0] = (char)0x02;buf[ 1] = (char)0x00;buf[ 2] = (char)0x0A;buf[ 3] = (char)0x00;
	buf[ 4] = (char)0x20;buf[ 5] = (char)0x00;buf[ 6] = (char)0x53;buf[ 7] = (char)0x49;
	buf[ 8] = (char)0x43;buf[ 9] = (char)0x4B;buf[10] = (char)0x5F;buf[11] = (char)0x4C;
	buf[12] = (char)0x4D;buf[13] = (char)0x53;buf[14] = (char)0xBE;buf[15] = (char)0xC5;

	for( i=0; i<5; i++) 
	{
		len = sio_write(m_commData.Port, buf, 16);
		if(len>=0)
			break;
		Sleep(20);
	}

	Sleep(2000);
	//command 2
	char tpCh[] = {(char)02, (char)00, (char)0x21, (char)00, 
		(char)0x77, (char)00, (char)00, (char)00, 
		(char)00, (char)00, (char)00, (char)00, (char)00,
		(char)00, (char)02, (char)02, (char)0, (char)00, 
		(char)00, (char)00, (char)00, (char)00, (char)00, 
		(char)00, (char)00, (char)00, (char)00, (char)00, 
		(char)00, (char)00, (char)00, (char)00, (char)00, 
		(char)00, (char)00, (char)00, (char)00, (char)0xE8, (char)0x72};

	for( i=0; i<5; i++)
	{
		len = sio_write(m_commData.Port, tpCh, 39);
		if(len>=0)
			break;
		Sleep(20);
	}

	Sleep(8000);
}

void CLMS291Thread::SetBaudrateStart()
{
	int i;
	if(!m_bOpen)
		return;

	int 	len;
	char 	buf[16];
	//command 1
	buf[ 0] = (char)0x02;buf[ 1] = (char)0x00;buf[ 2] = (char)0x0A;buf[ 3] = (char)0x00;
	buf[ 4] = (char)0x20;buf[ 5] = (char)0x00;buf[ 6] = (char)0x53;buf[ 7] = (char)0x49;
	buf[ 8] = (char)0x43;buf[ 9] = (char)0x4B;buf[10] = (char)0x5F;buf[11] = (char)0x4C;
	buf[12] = (char)0x4D;buf[13] = (char)0x53;buf[14] = (char)0xBE;buf[15] = (char)0xC5;

	for( i=0; i<5; i++) 
	{
		len = sio_write(m_commData.Port, buf, 16);
		if(len>=0)
			break;
		Sleep(20);
	}

	//command 2
	buf[0] = (char)0x02;buf[1] = (char)0x00;buf[2] = (char)0x02;buf[3] = (char)0x00;
	buf[4] = (char)0x66;buf[5] = (char)0x01;buf[6] = (char)0x9D;buf[7] = (char)0x4E;

	for( i=0; i<5; i++)
	{
		len = sio_write(m_commData.Port, buf, 8);
		if(len>=0)
			break;
		Sleep(20);
	}
}

void CLMS291Thread::SendOtherCmd(CString szCmd)
{
	int i;
	if(!m_bOpen)
		return;

	int 	len;
	char 	buf[1024];

	int iLen = szCmd.GetLength();
	if(iLen%3 != 2) 
	{
		AfxMessageBox("命令格式有误!");
		return;
	}
	else 
	{
		len = (iLen+1)/3;

		for( i=1; i<len; i++) 
		{
			if(szCmd.GetAt(i*3-1) != ' ') 
			{
				AfxMessageBox("命令格式有误!");
				return;
			}
		}

		CString szTp;
		for( i=0; i<len; i++) 
		{
			szTp = szCmd.Mid(i*3, 2);
			if(!STR2HEX(szTp, buf[i])) 
			{
				AfxMessageBox("命令格式有误!");
				return;
			}
		}

		for( i=0; i<5; i++) {
			len = sio_write(m_commData.Port, buf, len);
			if(len>=0)
				break;
			Sleep(20);
		}
	}
}

BOOL CLMS291Thread::STR2HEX(CString szTp, char &buf)
{
	char ch = szTp.GetAt(0);
	if(ch>47 && ch<58)
		buf = ((int)ch-48)*16;
	else if(ch>64 && ch<71)
		buf = ((int)ch-55)*16;
	else if(ch>96 && ch<103)
		buf = ((int)ch-87)*16;
	else
		return FALSE;

	ch = szTp.GetAt(1);
	if(ch>47 && ch<58)
		buf += (int)ch-48;
	else if(ch>64 && ch<71)
		buf += ((int)ch-55);
	else if(ch>96 && ch<103)
		buf += ((int)ch-87);
	else
		return FALSE;

	return TRUE;
}

int CLMS291Thread::DECToHEX(int nDe)
{
	int Hex=10*(nDe/16)+nDe%16;

	return Hex;
}

UINT CLMS291Thread::CreateCRC(const unsigned char* const CommData, unsigned int uLen)
{
	unsigned short uCrc16 = 0;
	unsigned char abData[2];
	const unsigned char *pData = CommData;

	abData[0] = 0;
	while(uLen--) 
	{
		abData[1] = abData[0];
		abData[0] = *pData++;

		if(uCrc16&0x8000) 
		{
			uCrc16 = (uCrc16 & 0x7fff)<<1;
			uCrc16 ^= CRC16_GEN_POL;
		}
		else 
		{
			uCrc16 <<= 1;
		}
		uCrc16 ^= MKSHORT(abData[0], abData[1]);
	}

	return (uCrc16);
}

CString CLMS291Thread::GetFilePath(CString FileName)
{
	CString tmpStr;
	char drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
	char path_buffer[_MAX_PATH];
	strcpy((char*)path_buffer,FileName);
	_splitpath(path_buffer,drive,dir,fname,ext);

	tmpStr = drive;
	tmpStr += dir;
	return tmpStr;
}

void CLMS291Thread::SetResolutionMode_100_100() //0~100度，每隔1度一个
{
	if(!m_bOpen)
		return;

	int 	len;
	char 	buf[11];
	buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x05;buf[3] = 0x00;
	buf[4] = 0x3B;buf[5] = 0x64;buf[6] = 0x00;buf[7] = 0x64;
	buf[8] = 0x00;buf[9] = 0x1D;buf[10] = 0x0F;

	for(int i=0; i<5; i++) {
		len = sio_write(m_commData.Port, buf, 11);
		if(len>=0)
			break;
		Sleep(5);
	}

//	Sleep(1000);
}

void CLMS291Thread::SetResolutionMode_100_050() //0~100度，每隔0.5度一个
{
	if(!m_bOpen)
		return;

	int 	len;
	char 	buf[11];
	buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x05;buf[3] = 0x00;
	buf[4] = 0x3B;buf[5] = 0x64;buf[6] = 0x00;buf[7] = 0x32;
	buf[8] = 0x00;buf[9] = 0xB1;buf[10] = 0x59;

	for(int i=0; i<5; i++) {
		len = sio_write(m_commData.Port, buf, 11);
		if(len>=0)
			break;
		Sleep(5);
	}

//	Sleep(1000);
}

void CLMS291Thread::SetResolutionMode_100_025() //0~100度，每隔0.25度一个
{
	if(!m_bOpen)
		return;

	int 	len;
	char 	buf[11];
	buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x05;buf[3] = 0x00;
	buf[4] = 0x3B;buf[5] = 0x64;buf[6] = 0x00;buf[7] = 0x19;
	buf[8] = 0x00;buf[9] = 0xE7;buf[10] = 0x72;

	for(int i=0; i<5; i++) {
		len = sio_write(m_commData.Port, buf, 11);
		if(len>=0)
			break;
		Sleep(5);
	}

	//Sleep(1000);
}

void CLMS291Thread::SetResolutionMode_180_100() //0~180度，每隔1度一个
{
	if(!m_bOpen)
		return;

	int 	len;
	char 	buf[11];
	buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x05;buf[3] = 0x00;
	buf[4] = 0x3B;buf[5] = 0xB4;buf[6] = 0x00;buf[7] = 0x64;
	buf[8] = 0x00;buf[9] = 0x97;buf[10] = 0x49;

	for(int i=0; i<5; i++) {
		len = sio_write(m_commData.Port, buf, 11);
		if(len>=0)
			break;
		Sleep(5);
	}

//	Sleep(1000);
}

void CLMS291Thread::SetResolutionMode_180_050() //0~180度，每隔0.5度一个
{
	if(!m_bOpen)
		return;

	int 	len;
	char 	buf[11];
	buf[0] = 0x02;buf[1] = 0x00;buf[2] = 0x05;buf[3] = 0x00;
	buf[4] = 0x3B;buf[5] = 0xB4;buf[6] = 0x00;buf[7] = 0x32;
	buf[8] = 0x00;buf[9] = 0x3B;buf[10] = 0x1F;

	for(int i=0; i<5; i++) {
		len = sio_write(m_commData.Port, buf, 11);
		if(len>=0)
			break;
		Sleep(5);
	}

	//Sleep(1000);
}

void CLMS291Thread::AngleMode_100_100() 
{
	m_nAngleMode = 0;

	_max_tele_length_ = 212;
	_max_pt_length_ = 101;
	_max_mea_len_ = 206;
}

void CLMS291Thread::AngleMode_100_050() 
{
	m_nAngleMode = 1;

	_max_tele_length_ = 412;
	_max_pt_length_ = 201;
	_max_mea_len_ = 406;
}

void CLMS291Thread::AngleMode_100_025() 
{
	m_nAngleMode = 2;

	_max_tele_length_ = 812;
	_max_pt_length_ = 401;
	_max_mea_len_ = 806;
}

void CLMS291Thread::AngleMode_180_100() 
{
	m_nAngleMode = 3;

	_max_tele_length_ = 372;
	_max_pt_length_ = 181;
	_max_mea_len_ = 366;
}

void CLMS291Thread::AngleMode_180_050() 
{
	m_nAngleMode = 4;

	_max_tele_length_ = 732;
	_max_pt_length_ = 361;
	_max_mea_len_ = 726;
}
