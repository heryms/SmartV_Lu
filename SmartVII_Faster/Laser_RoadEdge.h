#pragma once
#include "lidarobject.h"
class CLaser_RoadEdge :
	public CLidarObject
{
private:
	sPoint m_point_s,m_point_e;
	//cbw
	sPoint lastpoint_E_10;
	sPoint lastpoint_S_10;
	sPoint lastpoint_E_6;
	sPoint lastpoint_S_6;

    RoadParam m_RoadParam;
	sPoint temp_s[10];
	sPoint temp_e[10];
public:

	inline sPoint GetedgePointS(){return m_point_s;}
	inline sPoint GetedgePointE(){return m_point_e;}
	CLaser_RoadEdge(void);
	~CLaser_RoadEdge(void);
	UINT SingleRoadSerach(int Num,sPoint*pPoint,RoadParam param,sPoint&point_S ,sPoint& point_E);
	void Road_SearchOptimal(int option_num,sPoint pPoint[],
		sPoint temppoint_S[],sPoint temppoint_E[],sPoint &point_S,sPoint &point_E);
	///////////////////////////////////////////////////道路跟踪部分//////////////////////////////////////////
	//int Road_SearchForward(sPoint *pPoint,sPoint &point_S,sPoint &point_E);

	//void Road_SearchOptimal(int option_num,sPoint pPoint[],sPoint temppoint_S[],sPoint temppoint_E[],sPoint &point_S,sPoint &point_E);
	//void Road_SearchRefine(sPoint pPoint[],sPoint &point_S,sPoint &point_E);
	//float calc_evaluation_index(float height_ratio,float distance_ratio,float roadwidth);
	//int Road_Search2(sPoint *pPoint,sPoint &point_S,sPoint &point_E);
	void Road_Probe(); //路面探测
};

