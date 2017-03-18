/************************************************************************/
// ��Ȩ��Ϣ���人��ѧ ���ң����Ϣ���̹����ص�ʵ����
// ���ߣ��ܽ�
// ����ʱ�䣺2014-05-18
// ������ڣ�2014-0530
// �޸��� :
// �޸�ʱ�䣺
// ������Ϣ�� �����װ��SmartVII ���ݵײ�ɼ�����Ҫ�ĺ��������������ļ����õĴ�����Ϣ�������ݲɼ����ֵ�ʱ��ͬ������
// �ļ�����DataCollection.h
// ��ǰ�汾�ţ�Ver1.0
// ��ʷ�汾�ţ�Ver1.0
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
//�̹߳���ģʽ
enum THREADMODE 
{
	MODENULL, //δ�����߳�
	MODERUN, //�߳�������
	MODESUSPEND, //�߳���ͣ
	MODESTOP, //�߳�ֹͣ
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
	void CreateThread();	//�����̣߳�suspended״̬
	void StartThread();		//��ʼ�̣߳�����߳�δ�������򴴽���Ȼ������Ϊ����״̬
	void SuspendThread(); //��ͣ�߳�
	void ResumeThread(); //�ָ��߳�
	void StopThread(); //ֹͣ�߳�
	static UINT DataCollectThreadPro(LPVOID lpParam); //�̺߳���

};

