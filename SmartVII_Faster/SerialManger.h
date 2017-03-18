/************************************************************************/
// 版权信息：武汉大学 测绘遥感信息工程国家重点实验室
// 作者：周剑
// 创作时间：2013-5
// 完成日期：2013-6-4
// 修改者 :
// 修改时间：
// 描述信息：封装无人车底层数据的读取接口，根据配置文件一次性打开所有传感设备
// 文件名：SerialManger.h
// 当前版本号：Ver1.0
// 历史版本号：Ver1.0                                                                  */
/************************************************************************/
#pragma once
#define _MAX_TP_BUF_SIZE_ 2048
#define _MAX_REC_BUF_SIZE_	4096
#define _MAX_SEQ_BUF_SIZE_ 5120
#define _NOMAL_(i) (i)%_MAX_SEQ_BUF_SIZE_

// #define CRC16_GEN_POL 0x8005
// #define MKSHORT(a,b) ((unsigned short)(a) | ((unsigned short)(b)<<8))

//enum BAUD_RATE {BR9600, BR19200, BR38400, BR500K,};

// typedef struct tagCOMMDATA
// {
// 	int     Port;
// 	int     BaudRate,Parity,ByteSize,StopBits;
// 	int	 ibaudrate,iparity,ibytesize,istopbits;
// 
// 	BOOL     Hw;		// RTS/CTS hardware flow control 
// 	BOOL 	Sw;		// XON/XOFF software flow control 
// 	BOOL    Dtr,Rts;
// }COMMDATA,*LPCOMMDATA;
//串口工作状态
typedef struct tagPortStatus
{
	int Port;
	bool states;
}PortStatus;

//线程工作模式
// enum THREADMODE 
// {
// 	MODENULL, //未创建线程
// 	MODERUN, //线程在运行
// 	MODESUSPEND, //线程暂停
// 	MODESTOP, //线程停止
// };

class CSerialManger
{
public:
	CSerialManger(void);
	virtual ~CSerialManger(void);
	virtual BOOL OpenPort();
	virtual void ClosePort();
	//virtual void ProComData();
protected:
	BOOL		m_bStopThread;
	CWinThread	*m_pThreadHandle;
protected:
	BOOL PortSet(int Port);
	BOOL PortBaudRate(int Port);
		void InitPortSetting();
//	virtual void InitPortSetting();
protected:
	//FILE * m_pConfigFile;
	COMMDATA m_commData;  
	int m_nBaudRate;
public:
	unsigned char *m_pDataBuffer;
	long long m_nDataLength;
	THREADMODE  m_threadMode;
	//
public:
	//记录串口状态信息
	//char m_Msg[1024];
	char	buf[_MAX_REC_BUF_SIZE_]; //接收缓冲区
	//unsigned char bufOK[_MAX_REC_BUF_SIZE_]; //处理好的数据缓冲区
	unsigned char chBuf[_MAX_SEQ_BUF_SIZE_]; //处理缓冲区
	//void SetConfigFileName(FILE *fp);
	void SetBaudRate(long baud);
	void InitDataBuffer();
	void DeleteDataBuffer();

};

