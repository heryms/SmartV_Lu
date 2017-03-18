/************************************************************************/
/* 地图标记类，能够实现基本的数据单元
   当前地图单元支持基础路线

   调用流程
   1.导入地图数据，根据连接点生成表示拓扑关系邻接矩阵
   2.导入任务文件，逐次进行定位，匹配到对应线，根据线连接点属性搜索到达下一个任务点
   一次进行，知道完成所有搜索路径
   3.根据生成的路径提取对应的车道线数据并生成最终的轨迹。
   4.获取车辆实时位置，匹配到轨迹上，并搜索对应区域内的POI信息，给出建议信息
*/
/************************************************************************/
#pragma once
#include <map>
#include "MissionLayer.h"
#include "DataShow.h"

using namespace std;

typedef struct 
{
	double x;
	double y;
	double angle;
}ckMapPoint;

typedef struct
{
	ckMapPoint start;
	ckMapPoint end;
	vector<ckMapPoint> trackline;
	unsigned char linetype;//目前的线型主要为直线、弯道、路口、掉头四种
}ckMapLine;
typedef struct
{
	ckMapPoint pt;
	unsigned char marktype;
}ckMarkPoint;
typedef struct 
{
	int id;
	double x;
	double y;
	double amith;
}ckMifPoint;
typedef enum
{
	UNKNOWDIR=0,
	BOTHWAY=1,
	FORWRDONLY=2,
	BACKFORWARD=3,
	ABORT=4,

}CKROADDIRECTION;
typedef enum{
	S130=1,
	S100=2,
	S90=3,
	S70=4,
	S50=5,
	S30=6,
	S11=7,
	S5=8,
}CKSPEEDCLASS;
typedef enum   
{
	LN1=1,
	LN2D3=2,
	LN4D=3,
}CKLANECLASS;
typedef struct
{
	int id;
	string name;
	short direction;
	int snode;
	int enode;
	double length;	
	float lanewidth;
	short speed;
	short lanenum;
	short elevated;  //是否为高架
//	short structure;
	//short spdlmt;
	short spdlmt2s; //起点速度限制 
	short spdlmt2e;  //
    int linkId;
	vector<int> LaneId;
}ckMifLine;
typedef enum
{
	NOTYPE=0,
	COMPOSITECENTOR=1,
	SINGLE=2,
    CONPOSITE=3,
}CROSSFLAG;
typedef struct  
{
	int id;
	short cross_flg;
	short light_flag;
	//short sidway_flag;
	short cross_info;
	short sideway_num;
	vector<int>  roadnear ;
	vector<short>  crossnear;
	vector<short>  linknear;
	vector<int>  laneinfo;

}ckLinkNode;
typedef enum
{
	NO=0,
	LEFT=1,
	RIGHT=2,
	BOTH=3,

}TurnType;
typedef struct  
{
	int id;
    int snode;
	int enode;
	short lanenum;
	short iscross;
	vector<int> lanenear;
	int linkinfo;
}ckRoadLane;

typedef struct  
{
	int sidewayid;
	string linklanes;
	int snode;
	int enode;
	float length;
	float width;
}ckSideway;
struct ckRect
{
	ckRect()  {}

	ckRect(int a_minX, int a_minY, int a_maxX, int a_maxY)
	{
		min[0] = a_minX;
		min[1] = a_minY;

		max[0] = a_maxX;
		max[1] = a_maxY;
	}
	int min[2];
	int max[2];
};
typedef struct  
{
	ckMifLine lineinfo;
	ckRect idxrect;
	ckLinkNode linkNode;
	//vector<ckMifPoint> linebuffer;
}ckLineSegment;
typedef struct  
{
	int mapnum;
	vector<ckMifPoint> point;
	vector<ckMifLine> line;
	vector<ckRoadLane> roadlane;
	vector<ckLinkNode> link;
	vector<ckSideway>  sideway;

}ckMapChart;

struct ckLineTree 
{
	ckLineSegment line;
	vector<ckLineTree> child;
};
class ckMapBaseObject
{
public:
	ckMapBaseObject(void);
    ~ckMapBaseObject(void);
//构建地图存储单元
private:
	//存储路线上的地图点数据
	vector<ckMapPoint> m_MapPointBuffer;
	vector<ckMapLine> m_MapLineBuffer;
	//vector<ckMapLine> m_MapCrossBuffer;
public:
	vector<ckMifPoint> m_MifPoint;
	vector<ckMifLine>  m_MifLine;
	vector<ckRoadLane> m_RoadLane;
	vector<ckMifPoint> m_LanePoint;
	vector<ckSideway> m_SideWay;
	vector<ckLinkNode>m_LinkNode;
    vector<ckMissionPoint> m_MissionPoint;
	vector<ckLineSegment> m_ckLineBuffer;
private:
	map<int,ckLineSegment> m_MaplineList; //记录路段级别信息
	map<int,ckLinkNode> m_MapLinkList; //记录连接点数据

	//地图标记
	
public:
	     ckPointS m_centerPoint;
		 ckRect  m_fullExtent;
private:
	ckLineSegment m_nCurrentLine;
	int m_nMissionId;

	//数据处理
	CString m_workFactory;
	vector<CString> m_mapFileList;
	vector<ckPloyLineS> m_drawLineBuffer;
	vector<ckPloyLineS> m_drawLaneBuffer;
	vector<ckMissionPoint> m_drawMissionBuffer;
	vector<ckPloyLineS> m_drawTrackBuffer;
	vector<PointShow>  m_drawIndexBuffer;
private: //file explore
	void BrowseDir(CString dir);
	void ReadTrackFile(CString path);
	void ReadPointTable(CString path); //跑车路点文件
	void ReadLineTable(CString path);  //生成的线表
	void ReadLinkPoint(CString path);  //连接点
	void ReadPOITable(CString path);   //poi
	void ReadRoadLaneTable(CString path);  //车道线
	void ReadLanePointTable(CString path);  //车道线坐标
	void ReadSideWayTable(CString path);  //人行横道
public:
	void ReadTrackGps(CString path);
	//构建线索引
	void CreateLineSegment(CString path);
	void GetEnclosRectangle(vector<ckMifPoint> ptline,ckRect& rt);
public:
	void ReadMissionTable(CString path);  //任务文件
	// 匹配路段
	int MappingSection(ckMapLine& missline,ckMapLine line);
	//匹配路口
	int MappingCross(ckMapLine& missline,ckMapLine line);
	//匹配到路段
	BOOL BuildMifFactory();
	BOOL ExplainRoadNet();
	BOOL CanddidateLine(vector<ckLineSegment>& line,ckMapPoint pt,int buffersize);
	int GetNearLines(vector<ckLineSegment> &candiline ,ckMapPoint pt);
	UINT CalMissionRoad(vector<ckLineSegment> segments,vector<ckRoadLane>& linepath);

public:
	 inline void SetWorkFactory(CString path){m_workFactory=path;}
public:
	static double AmizthCaltulate(double x0,double y0,double x1,double y1);
	BOOL BuildTrackFactory(CString factory);
	int CreateMapLineNet();
//	BOOL CreateMap(CString path);
    int LocationAt(ckMapPoint pt);
	int SearchStartLine(ckLineSegment &sline,ckMissionPoint mission);
   UINT GeneratePath(vector<ckMapPoint> &path);	
   static void CalLineAzmith(vector<GpsLine> &line);
   //图层更新
private:
	bool IsConnectLine(ckLineSegment line0,int linkline);
	bool Overlap(ckRect a_rectA, ckRect a_rectB);
    BOOL GetMapCentor(ckPointS &centor);

	bool CreateLineTree(ckLineTree &treenode,vector<vector<ckLineSegment>>lines,int level);
	//遍历路径生成树，提取可达路径
	bool SelectOptialLine(vector<ckLineSegment>& outputlines,ckLineTree root,int level);
public:
		BOOL CreateMissionLines(vector<ckLineSegment>& outputLines);
	 UINT Convert2PointLayer(vector<ckMifPoint> ptM,vector<ckPointS>&ptS);
	 UINT Convert2LineLayer(vector<ckMifPoint>ptM,vector<ckMifLine>line,vector<ckPloyLineS>& lineS);
	  UINT Convert2LaneLayer(vector<ckMifPoint>ptM,vector<ckRoadLane>line,vector<ckPloyLineS>& lineS);
	  UINT Convert2IndexLayer(vector<ckLineSegment> lines,vector<PointShow>&ptcloud);
	UINT Convert2TrackLayer(vector<ckRoadLane> line,vector<GpsLine> &trackLine);
	  void AddLineLayer(CDataShow& cs);
	void AddMissionLayer(CDataShow& cs);
//	void AddRoadLaneLayer()
	void Refresh();
};

