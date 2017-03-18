#pragma once
#include "afxwin.h"
#include "LMS291Thread.h"
//#include "LidarOffLineCtrlDlg.h"
#include "Laser_sick.h"


void MyOutData1(unsigned char *pData, int len); // �ص���������������
void MyOutData2(unsigned char *pData, int len);
void MyOutData3(unsigned char *pData, int len);
void MyOutData4(unsigned char *pData, int len);


class CLidarControl
{
public:
	CLidarControl(void);
	~CLidarControl(void);

private:
	CWinThread	*m_pThreadHandle;

public:
	
	//Laser_sick m_Sick;
	// ���߲ɼ�
	CLMS291Thread	m_lms291Thread[4];
	BOOL m_bIsPortOpen;
	BOOL m_bIsDataOut;
	BOOL m_bIsSaveData;
	//BOOL m_bIsFileOpen;
	
	// ���ߵ��ԵĿ��ƶԻ���
	//CLidarOffLineCtrlDlg *m_LidarOfflineCtrl;
public:
	/*void ShowOffLineDlg();
	void DeleteOffLineDlg();*/

	//���߲ɼ�
	void OpenCom();
	void Closecom();
	void OpenSick();
	void CloseSick();


	static void SickSaveThreadPro(LPVOID lpParam);
	void SatrtThread();

};

