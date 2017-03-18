//////////////////////////////////////////////////////////////////////
// 版权信息：武汉大学 电子信息学院
// 作者：刘勇
// 创作时间：2007-7
// 完成日期：2008-8-8
// 修改者 :
// 修改时间：
// 描述信息： 该类封装了多线程方式地读取激光扫描仪数据，以及一些常见命令
// 文件名：LMS291Thread.h
// 当前版本号：Ver2.0
// 历史版本号：Ver1.0
//////////////////////////////////////////////////////////////////////

#if !defined(_LMS291THREAD_H_INCLUDED_)
#define _LMS291THREAD_H_INCLUDED_

//#if _MSC_VER > 1000
#pragma once
//#endif // _MSC_VER > 1000

#include "Pcomm.h"
#pragma comment(lib,"lib/Pcomm.lib")
//#define	PI	3.1415926

#define _MAX_TP_BUF_SIZE_ 2048
#define _MAX_REC_BUF_SIZE_	4096
#define _MAX_SEQ_BUF_SIZE_ 512000
#define _NOMAL_(i) (i)%_MAX_SEQ_BUF_SIZE_

#define CRC16_GEN_POL 0x8005
#define MKSHORT(a,b) ((unsigned short)(a) | ((unsigned short)(b)<<8))


//================================================
//定义回调函数的形式:
//void OutData(unsigned char *buf, int len) ;
typedef  void  (*CallBack)(unsigned char *buf, int len) ;
//================================================

class CLMS291Thread : public CObject  
{
public:
	CLMS291Thread();
	virtual ~CLMS291Thread();
	BAUD_RATE	m_baudRate;
	//==================================
	//port function
private:
	COMMDATA	m_commData;
	BOOL		 m_bOpen;
	FILE *      m_pStream;
	HANDLE  m_hWaitfor;
	HANDLE  m_hGetData;
public:
	int _max_tele_length_;  //_MAX_TELE_LENGTH_	732
	int _max_pt_length_; //_MAX_PT_LENGTH_ 361
	int _max_mea_len_; //_MAX_MEA_LEN_ 726

private:
	BOOL PortSet();
	BOOL PortBaudRate();
	void InitPortSetting();
	void ProcComData();
	void MxShowError(LPCTSTR title,int errcode);
	void ShowSysErr(LPCTSTR title);
public:
	inline void SetPortNum(int iPort) {m_commData.Port = iPort;} //设置端口
	inline void SetWaitHandle(HANDLE handle) {m_hWaitfor=handle;};
	inline void SetDataGetHandle(HANDLE handle) {m_hGetData=handle;};
	BOOL OpenPort(); //打开端口
	void ClosePort(); //关闭端口
	inline BOOL	IsPortOpen() {return m_bOpen;} //得到端口状态
	//==================================
	//thread function
private:
	BOOL		m_bStopThread;
	CWinThread	*m_pThreadHandle;

public:
	void CreateThread();	//创建线程，suspended状态
	void StartThread();		//开始线程，如果线程未创建，则创建；然后设置为运行状态
	void SuspendThread(); //暂停线程
	void ResumeThread(); //恢复线程
	void StopThread(); //停止线程
	static UINT LMS291ThreadPro(LPVOID lpParam); //线程函数

	//==================================
	//data function
private:
	inline BOOL IsHead(const unsigned char *const buf, int iPos);
	inline BOOL IsLengthCheckOK(const unsigned char *const buf, int iPos);

	//==================================
	//other function
public:
	THREADMODE	m_threadMode;  //线程状态

public:
	void SetCallBack(CallBack OutData) { m_fnOutData=OutData;}  //设置回调函数d

private:
	CallBack  m_fnOutData;
	//==================================
	//LMS221 function
public:
	void SetMaster(BOOL bMaster=FALSE);
	void PowerOnBaudrate();
	CString GetFilePath(CString FileName);
	void CtnsDataOutputStart();	 //开始数据连续输出
	void CtnsDataOutputStop();  //停止数据连续输出
	void SetBaudrate9600();	
	void SetBaudrate19200();
	void SetBaudrate38400();
	void SetBaudrate500K();
	void SetUnitMM(); //设置单位为mm
	void SetUnitCM(); //设置单位为cm
	void SetBaudrateStart();  //设当前值为开机波特率
	void SendOtherCmd(CString szCmd);	 //其它命令

	void SetResolutionMode_100_100(); //0~100度，每隔1度一个
	void SetResolutionMode_100_050(); //0~100度，每隔0.5度一个
	void SetResolutionMode_100_025(); //0~100度，每隔0.25度一个
	void SetResolutionMode_180_100(); //0~100度，每隔1度一个
	void SetResolutionMode_180_050(); //0~100度，每隔0.5度一个

	int m_nAngleMode;
	void AngleMode_100_100();
	void AngleMode_100_050();
	void AngleMode_100_025();
	void AngleMode_180_100();
	void AngleMode_180_050();

	char	buf[_MAX_REC_BUF_SIZE_]; //接收缓冲区
	unsigned char bufOK[_MAX_REC_BUF_SIZE_]; //处理好的数据缓冲区
	unsigned char chBuf[_MAX_SEQ_BUF_SIZE_]; //处理缓冲区

private:
	BOOL STR2HEX(CString szTp, char &buf);
	int DECToHEX(int nDe);
	UINT CreateCRC(const unsigned char* const CommData, 
		unsigned int uLen);
};

#endif // !defined(_LMS291THREAD_H_INCLUDED_)

/*------------------------------------------------
【程序调用流程】
		1. 在CMainFrame里声明一个该类的对象：
		#include "LMS291Thread.h"
		class CMainFrame : public CFrameWnd {
		...
		CLMS221Thread	m_lms291Thread;
		...
		}

		2. 设置回调函数
		static void MyOutData(unsigned char *pData, int len) 
		{
		CMainFrame *pFrm = (CMainFrame *)theApp.m_pMainWnd;
		pFrm->UpdateStatusBar();
		}

		...
		m_lms291Thread.SetCallBack(MyOutData);
		...

		3. 调用函数启动线程
		m_lms291Thread.StartThread();

		4. 设置端口，并打开接收端口
		m_lms291Thread.SetPortNum(1);
		m_lms291Thread.StartThread();

		5. 设LMS291为连续输出状态
		m_lms291Thread.CtnsDataOutputStart();

		(6). 端口设置(端口必须处于关闭状态)
		HWND	hWnd = GetSafeHwnd(); 
		m_lms291Thread.SetHWnd(hWnd); //设置窗口句柄

		m_lms291Thread.DlgPotSetting();
------------------------------------------------*/
