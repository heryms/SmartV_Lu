//处理车道线接收数据，解算出中心线局部坐标和全局坐标
#pragma once
#include "LcmType_LaneHandle.h"
#include <deque>
#include "DataShow.h"
using namespace std;
typedef struct
{
	float x;
	float y;
}ckLanePoint;
class CRoadLaneData
{
public:
	CRoadLaneData(void);
	~CRoadLaneData(void);
     

private:
	//HANDLE m_hRcvEvent;
	LcmLaneHandler m_lanelcm;
	BOOL m_bStopThread;
	CWinThread* m_pThreadHandle;
	void ExactLanes();
	void DataProFunc();
private://DataManager
	deque<LaneInfo_t> m_laneBuffer;
	int m_nBuffersize;
	float m_fdelY;//Y轴采用间距
public:
	vector<PointShow> m_showBuffer;
	CDataShow* m_csShow;
	GpsLine *m_gpsBuffer;
	int m_nptNum;
	bool m_blineStatus;
public:
	inline void SetBufferSize(int num){m_nBuffersize=num;}
	inline int GetBufferSize(){return m_nBuffersize;}
    inline void SetptNum(int num) {m_nptNum=num;}
	void SetShowdlg(CDataShow *cs);
	void UpdatesLayers();
public:
    BOOL InitialLaneLcm();
	void CreateThread();
	void StartThread();
	void StopThread();
	static UINT RoadLaneThreadPro(LPVOID lpParam);
	
};

