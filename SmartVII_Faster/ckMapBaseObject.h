/************************************************************************/
/* ��ͼ����࣬�ܹ�ʵ�ֻ��������ݵ�Ԫ
   ��ǰ��ͼ��Ԫ֧�ֻ���·��

   ��������
   1.�����ͼ���ݣ��������ӵ����ɱ�ʾ���˹�ϵ�ڽӾ���
   2.���������ļ�����ν��ж�λ��ƥ�䵽��Ӧ�ߣ����������ӵ���������������һ�������
   һ�ν��У�֪�������������·��
   3.�������ɵ�·����ȡ��Ӧ�ĳ��������ݲ��������յĹ켣��
   4.��ȡ����ʵʱλ�ã�ƥ�䵽�켣�ϣ���������Ӧ�����ڵ�POI��Ϣ������������Ϣ
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
	unsigned char linetype;//Ŀǰ��������ҪΪֱ�ߡ������·�ڡ���ͷ����
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
	short elevated;  //�Ƿ�Ϊ�߼�
//	short structure;
	//short spdlmt;
	short spdlmt2s; //����ٶ����� 
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
//������ͼ�洢��Ԫ
private:
	//�洢·���ϵĵ�ͼ������
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
	map<int,ckLineSegment> m_MaplineList; //��¼·�μ�����Ϣ
	map<int,ckLinkNode> m_MapLinkList; //��¼���ӵ�����

	//��ͼ���
	
public:
	     ckPointS m_centerPoint;
		 ckRect  m_fullExtent;
private:
	ckLineSegment m_nCurrentLine;
	int m_nMissionId;

	//���ݴ���
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
	void ReadPointTable(CString path); //�ܳ�·���ļ�
	void ReadLineTable(CString path);  //���ɵ��߱�
	void ReadLinkPoint(CString path);  //���ӵ�
	void ReadPOITable(CString path);   //poi
	void ReadRoadLaneTable(CString path);  //������
	void ReadLanePointTable(CString path);  //����������
	void ReadSideWayTable(CString path);  //���к��
public:
	void ReadTrackGps(CString path);
	//����������
	void CreateLineSegment(CString path);
	void GetEnclosRectangle(vector<ckMifPoint> ptline,ckRect& rt);
public:
	void ReadMissionTable(CString path);  //�����ļ�
	// ƥ��·��
	int MappingSection(ckMapLine& missline,ckMapLine line);
	//ƥ��·��
	int MappingCross(ckMapLine& missline,ckMapLine line);
	//ƥ�䵽·��
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
   //ͼ�����
private:
	bool IsConnectLine(ckLineSegment line0,int linkline);
	bool Overlap(ckRect a_rectA, ckRect a_rectB);
    BOOL GetMapCentor(ckPointS &centor);

	bool CreateLineTree(ckLineTree &treenode,vector<vector<ckLineSegment>>lines,int level);
	//����·������������ȡ�ɴ�·��
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

