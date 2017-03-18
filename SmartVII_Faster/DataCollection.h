/************************************************************************/
// 版权信息：武汉大学 测绘遥感信息工程国家重点实验室
// 作者：周剑
// 创作时间：2014-05-18
// 完成日期：2014-0530
// 修改者 :
// 修改时间：
// 描述信息： 该类封装了SmartVII 数据底层采集所需要的函数，根据配置文件设置的串口信息进行数据采集部分的时间同步处理
// 文件名：DataCollection.h
// 当前版本号：Ver1.0
// 历史版本号：Ver1.0
/************************************************************************/
#pragma once
#include <vector>
using namespace std;
#define _MAX_TP_BUF_SIZE_ 2048
#define _MAX_REC_BUF_SIZE_	4096
#define _MAX_SEQ_BUF_SIZE_ 512000
#define _NOMAL_(i) (i)%_MAX_SEQ_BUF_SIZE_

#define CRC16_GEN_POL 0x8005
#define MKSHORT(a,b) ((unsigned short)(a) | ((unsigned short)(b)<<8))

enum BAUD_RATE {BR9600, BR19200, BR38400, BR500K,};
enum DATA_TYPE {LIDAR=0, VIDEO=1, GPS=2,};
typedef struct tagCOMMDATA
{
	int     Port;
	int     BaudRate,Parity,ByteSize,StopBits;
	int	 ibaudrate,iparity,ibytesize,istopbits;

	BOOL     Hw;		// RTS/CTS hardware flow control 
	BOOL 	Sw;		// XON/XOFF software flow control 
	BOOL    Dtr,Rts;
}COMMDATA,*LPCOMMDATA;
typedef struct tagCollection
{
	//COMMDATA comData;
	int Port;
	int BaudRate;
	DATA_TYPE Dtype;
	long DataLen;

	int PortStates;
	char *pBuffer;
	
}PortParamList;
//线程工作模式
enum THREADMODE 
{
	MODENULL, //未创建线程
	MODERUN, //线程在运行
	MODESUSPEND, //线程暂停
	MODESTOP, //线程停止
};
class CDataCollection
{
public:
	CDataCollection(void);
	~CDataCollection(void);

private:
	BAUD_RATE	m_baudRate;
	//COMMDATA	m_commData;
	vector<PortParamList> m_portList;
	BOOL		 m_bOpen;
	BOOL   m_bGpsStates;
	FILE *      m_pStream; 
private:
	BOOL ReadParamFile(char* file);

	BOOL PortSet(int num);
	BOOL PortBaudRate(int baud);	
public:
	BOOL ConnectPort();
	BOOL ReleaseConnect();
	void CreateThread();	//创建线程，suspended状态
	void StartThread();		//开始线程，如果线程未创建，则创建；然后设置为运行状态
	void SuspendThread(); //暂停线程
	void ResumeThread(); //恢复线程
	void StopThread(); //停止线程
	static UINT DataCollectThreadPro(LPVOID lpParam); //线程函数

};

