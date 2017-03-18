//////////////////////////////////////////////////////////////////////
// ��Ȩ��Ϣ���人��ѧ ������ϢѧԺ
// ���ߣ�����
// ����ʱ�䣺2007-7
// ������ڣ�2008-8-8
// �޸��� :
// �޸�ʱ�䣺
// ������Ϣ�� �����װ�˶��̷߳�ʽ�ض�ȡ����ɨ�������ݣ��Լ�һЩ��������
// �ļ�����LMS291Thread.h
// ��ǰ�汾�ţ�Ver2.0
// ��ʷ�汾�ţ�Ver1.0
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
//����ص���������ʽ:
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
	inline void SetPortNum(int iPort) {m_commData.Port = iPort;} //���ö˿�
	inline void SetWaitHandle(HANDLE handle) {m_hWaitfor=handle;};
	inline void SetDataGetHandle(HANDLE handle) {m_hGetData=handle;};
	BOOL OpenPort(); //�򿪶˿�
	void ClosePort(); //�رն˿�
	inline BOOL	IsPortOpen() {return m_bOpen;} //�õ��˿�״̬
	//==================================
	//thread function
private:
	BOOL		m_bStopThread;
	CWinThread	*m_pThreadHandle;

public:
	void CreateThread();	//�����̣߳�suspended״̬
	void StartThread();		//��ʼ�̣߳�����߳�δ�������򴴽���Ȼ������Ϊ����״̬
	void SuspendThread(); //��ͣ�߳�
	void ResumeThread(); //�ָ��߳�
	void StopThread(); //ֹͣ�߳�
	static UINT LMS291ThreadPro(LPVOID lpParam); //�̺߳���

	//==================================
	//data function
private:
	inline BOOL IsHead(const unsigned char *const buf, int iPos);
	inline BOOL IsLengthCheckOK(const unsigned char *const buf, int iPos);

	//==================================
	//other function
public:
	THREADMODE	m_threadMode;  //�߳�״̬

public:
	void SetCallBack(CallBack OutData) { m_fnOutData=OutData;}  //���ûص�����d

private:
	CallBack  m_fnOutData;
	//==================================
	//LMS221 function
public:
	void SetMaster(BOOL bMaster=FALSE);
	void PowerOnBaudrate();
	CString GetFilePath(CString FileName);
	void CtnsDataOutputStart();	 //��ʼ�����������
	void CtnsDataOutputStop();  //ֹͣ�����������
	void SetBaudrate9600();	
	void SetBaudrate19200();
	void SetBaudrate38400();
	void SetBaudrate500K();
	void SetUnitMM(); //���õ�λΪmm
	void SetUnitCM(); //���õ�λΪcm
	void SetBaudrateStart();  //�赱ǰֵΪ����������
	void SendOtherCmd(CString szCmd);	 //��������

	void SetResolutionMode_100_100(); //0~100�ȣ�ÿ��1��һ��
	void SetResolutionMode_100_050(); //0~100�ȣ�ÿ��0.5��һ��
	void SetResolutionMode_100_025(); //0~100�ȣ�ÿ��0.25��һ��
	void SetResolutionMode_180_100(); //0~100�ȣ�ÿ��1��һ��
	void SetResolutionMode_180_050(); //0~100�ȣ�ÿ��0.5��һ��

	int m_nAngleMode;
	void AngleMode_100_100();
	void AngleMode_100_050();
	void AngleMode_100_025();
	void AngleMode_180_100();
	void AngleMode_180_050();

	char	buf[_MAX_REC_BUF_SIZE_]; //���ջ�����
	unsigned char bufOK[_MAX_REC_BUF_SIZE_]; //����õ����ݻ�����
	unsigned char chBuf[_MAX_SEQ_BUF_SIZE_]; //��������

private:
	BOOL STR2HEX(CString szTp, char &buf);
	int DECToHEX(int nDe);
	UINT CreateCRC(const unsigned char* const CommData, 
		unsigned int uLen);
};

#endif // !defined(_LMS291THREAD_H_INCLUDED_)

/*------------------------------------------------
������������̡�
		1. ��CMainFrame������һ������Ķ���
		#include "LMS291Thread.h"
		class CMainFrame : public CFrameWnd {
		...
		CLMS221Thread	m_lms291Thread;
		...
		}

		2. ���ûص�����
		static void MyOutData(unsigned char *pData, int len) 
		{
		CMainFrame *pFrm = (CMainFrame *)theApp.m_pMainWnd;
		pFrm->UpdateStatusBar();
		}

		...
		m_lms291Thread.SetCallBack(MyOutData);
		...

		3. ���ú��������߳�
		m_lms291Thread.StartThread();

		4. ���ö˿ڣ����򿪽��ն˿�
		m_lms291Thread.SetPortNum(1);
		m_lms291Thread.StartThread();

		5. ��LMS291Ϊ�������״̬
		m_lms291Thread.CtnsDataOutputStart();

		(6). �˿�����(�˿ڱ��봦�ڹر�״̬)
		HWND	hWnd = GetSafeHwnd(); 
		m_lms291Thread.SetHWnd(hWnd); //���ô��ھ��

		m_lms291Thread.DlgPotSetting();
------------------------------------------------*/
