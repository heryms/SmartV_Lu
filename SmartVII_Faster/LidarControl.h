#pragma once
#include "afxwin.h"
#include "LMS291Thread.h"
//#include "LidarOffLineCtrlDlg.h"
#include "Laser_sick.h"


void MyOutData1(unsigned char *pData, int len); // 回调函数，接收数据
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
	// 在线采集
	CLMS291Thread	m_lms291Thread[4];
	BOOL m_bIsPortOpen;
	BOOL m_bIsDataOut;
	BOOL m_bIsSaveData;
	//BOOL m_bIsFileOpen;
	
	// 离线调试的控制对话框
	//CLidarOffLineCtrlDlg *m_LidarOfflineCtrl;
public:
	/*void ShowOffLineDlg();
	void DeleteOffLineDlg();*/

	//在线采集
	void OpenCom();
	void Closecom();
	void OpenSick();
	void CloseSick();


	static void SickSaveThreadPro(LPVOID lpParam);
	void SatrtThread();

};

