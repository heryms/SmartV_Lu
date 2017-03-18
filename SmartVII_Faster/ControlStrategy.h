/************************************************************************/
/* �ṩ�������ƶ����Ŀ��Ʋ��Բ��Ժͷ�������
   �ܹ��Դ������ݽ���ͳ�Ʒ����������쳣����
*/
/************************************************************************/
#pragma once

#include "CanControl.h"
#include "MissionLayer.h"
#include <deque>
typedef struct 
{
	ckMissionPoint start;
	ckMissionPoint end;
	int status;//��¼·��ͨ��״̬��0Ϊδ���1��ʾ���룬2��ʾͨ��
}ckCrossStatus;
typedef struct
{
	ckMissionPoint in;
	ckMissionPoint pos;
	ckMissionPoint out;
	int status;
}ckRightStop;
class CControlStrategy
{
public:
	CControlStrategy(void);
	~CControlStrategy(void);
private:
	int m_nCurrentAngle;
	int m_nWaitTime;
	int m_nStartWait;
	float m_fCurrentSpeed;
	int m_nTargetAngle;
	float m_fTargetSpeed;
	int m_nBuffersize;
	CWinThread *m_pThreadHandle;
	BOOL m_bStopThread;
	CMissionLayer m_missionLayer;
public:
	vector<ckMissionPoint> m_MissionPoint;
	vector<ckMissionPoint> m_markPoints;
	int m_ncurMisIdx;
	double m_lfMisDistatce;
	double m_lfStopDis;
	int m_nStartSpeed;
	ckCrossStatus m_cross;
private:
	void SendStartStop();
	void SendBrake(); //ɲ��
	void SendUnBrake(); //��ɲ��
	void DecelerateCommand();
	void AccelarateCommand();
	int MissionEvent(int nidx);
	void SendStartSpeed(int speed);
	int UpdateCrossStatus(GpsLine pt);
	void StartEnding(GpsLine pt);
	int PassNormalPoint(GpsLine pt);
	BOOL m_canStatus;
public:
	BOOL m_bFirstStart;
public:
	CCanControl m_canCtl;
	CMissionLayer m_missionData;
public:
	inline void SetBufferSize(int size) {m_nBuffersize=size;}
	inline int GetBufferSize(){return m_nBuffersize;}
	//���¿���ָ�
    void UpdateWheelAngle(int angle);
	void UpdateCarSpeed(float speed);
	BOOL InitialCan(CCanControl can);
	int GetCurrentMissIndex();
	BOOL TestMissIndexFile();
	BOOL LoadMarkFile();
	BOOL LoadMissionFile(CString path);
public:
	deque<int> m_queueAngle;
	deque<int> m_queueSpeed;
	//�߳̿���
public:
	double GetDis(GpsLine pt0,GpsLine pt1);
	double GetDoubleDistance(double x0,double y0,double x1,double y1);
	void ProAutoControl();
	static UINT ControlThreadFunc(LPVOID param);
	void CreateThread();
	void StartThread();
	void StopThread();

};

