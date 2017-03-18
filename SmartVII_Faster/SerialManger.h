/************************************************************************/
// ��Ȩ��Ϣ���人��ѧ ���ң����Ϣ���̹����ص�ʵ����
// ���ߣ��ܽ�
// ����ʱ�䣺2013-5
// ������ڣ�2013-6-4
// �޸��� :
// �޸�ʱ�䣺
// ������Ϣ����װ���˳��ײ����ݵĶ�ȡ�ӿڣ����������ļ�һ���Դ����д����豸
// �ļ�����SerialManger.h
// ��ǰ�汾�ţ�Ver1.0
// ��ʷ�汾�ţ�Ver1.0                                                                  */
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
//���ڹ���״̬
typedef struct tagPortStatus
{
	int Port;
	bool states;
}PortStatus;

//�̹߳���ģʽ
// enum THREADMODE 
// {
// 	MODENULL, //δ�����߳�
// 	MODERUN, //�߳�������
// 	MODESUSPEND, //�߳���ͣ
// 	MODESTOP, //�߳�ֹͣ
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
	//��¼����״̬��Ϣ
	//char m_Msg[1024];
	char	buf[_MAX_REC_BUF_SIZE_]; //���ջ�����
	//unsigned char bufOK[_MAX_REC_BUF_SIZE_]; //����õ����ݻ�����
	unsigned char chBuf[_MAX_SEQ_BUF_SIZE_]; //��������
	//void SetConfigFileName(FILE *fp);
	void SetBaudRate(long baud);
	void InitDataBuffer();
	void DeleteDataBuffer();

};

