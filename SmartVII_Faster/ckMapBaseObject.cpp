#include "StdAfx.h"


#include <fstream>
#include <regex>
#include "ckCoordinate.h"

#include "ckMapBaseObject.h"
//#include <math.h>

#define  _CLOSE_DISTANCE_ 100 //线段距离阈值
#define  _CLOSE_MAHA_DISTANCE_ 100

#define  ANGLEDIF(ag0,ag1) abs(ag1-ag0)>M_PI?(2*M_PI-abs(ag1-ag0)):abs(ag1-ag0)
#define  CKROUND(arg)  (int)(arg+0.5f)
/////////////////////////////////////////////////
// File name handling
CString GetFileDirectory(const CString &path)
{
	ASSERT(path.GetLength());
	int pos=path.ReverseFind('\\');
	if (pos>0)
	{
		return path.Left(pos);
	}
	return "";
}

CString GetFileName(const CString& path)
{
	ASSERT(path.GetLength());
	int pos=path.ReverseFind('\\');
	if (pos>=0)
	{
		return path.Right(path.GetLength()-pos-1);
	}
	return "";
}
CString GetFileExt(const CString &path)
{
	ASSERT(path.GetLength());
	int pos=path.ReverseFind('.');
	if (pos>=0)
	{
		return path.Right(path.GetLength()-pos-1);
	}
	return "";
}
CString GetFileTitle(const CString &path)
{
	ASSERT(path.GetLength());
	CString strResult=GetFileName(path);
	int pos=strResult.ReverseFind('.');
	if (pos>=0)
	{
		return strResult.Left(pos);
	}
	return strResult;
}
vector<CString> ckSplit(CString str,CString split)
{
	CString orzString=str;
	vector<CString> splitvector;
	while(TRUE)
	{
		CString temp=orzString.SpanExcluding(split);
		orzString=orzString.Right(orzString.GetLength()-temp.GetLength()-1);
		splitvector.push_back(temp);
		if (orzString.IsEmpty())
		{
			break;
		}
	}
	return splitvector;

}
BOOL IsSameNode(ckMapPoint pt0,ckMapPoint pt1)
{

	BOOL toward=(sin(pt0.angle)*sin(pt1.angle)+cos(pt0.angle)*cos(pt1.angle))>0?TRUE:FALSE;
	BOOL distance=pow((pt0.x-pt1.x),2)+pow(pt0.y-pt1.y,2);
	if (toward&&distance)
	{
		return TRUE;
	}
	else
		return FALSE;
}
void drawString(const char* str) {
	static int isFirstCall = 1;
	static GLuint lists;

	if( isFirstCall ) { // 如果是第一次调用，执行初始化
		// 为每一个ASCII字符产生一个显示列表
		isFirstCall = 0;

		// 申请MAX_CHAR个连续的显示列表编号
		lists = glGenLists(128);

		// 把每个字符的绘制命令都装到对应的显示列表中
		wglUseFontBitmaps(wglGetCurrentDC(), 0, 128, lists);
	}
	// 调用每个字符对应的显示列表，绘制每个字符
	for(; *str!='\0'; ++str)
		glCallList(lists + *str);
}
void DrawTextLineRGB(float x, float y, float z, float r, float g, float b, char * outputstring)
{
	glPushMatrix();
	glColor3f(r,g,b);	
	wglUseFontBitmaps(wglGetCurrentDC(), 0, 255, 200);

	//glListBase(200);
	glRasterPos3f(x, y, z);
	drawString(outputstring);
	//glCallLists( sizeof(outputstring), GL_UNSIGNED_BYTE,_T(outputstring)); 

	glPopMatrix();
}
ckMapBaseObject::ckMapBaseObject(void)
{
	//	m_nCurrentLine=0;
	m_nMissionId=0;
}


ckMapBaseObject::~ckMapBaseObject(void)
{
}
void DrawPolyLine(vector<ckPloyLineS> line)
{
	for (int i=0;i<line.size();i++)
	{
		ckPloyLineS polyline=line.at(i);

		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
		glLineWidth(5);
		glColor4f(0,1,1,0.7);
		glBegin(GL_LINE_STRIP);
		for(int j=0;j<polyline.ptData.size();j++)
		{
			glVertex3f(polyline.ptData.at(j).x,polyline.ptData.at(j).y,-2.0);
		}
		glEnd();


	}
}

void DrawRoadLane(vector<ckPloyLineS> line)
{
	for (int i=0;i<line.size();i++)
	{
		ckPloyLineS polyline=line.at(i);

		glPointSize(2);
		glColor3f(1,1,1);
		glBegin(GL_POINTS);
		/*	glLineWidth(2);
		glColor3f(1,1,1);
		if (polyline.type==0)
		{
		glDisable(GL_BLEND);
		glDisable(GL_LINE_SMOOTH);
		glEnable(GL_LINE_STRIP);
		glLineStipple(1,0x00FF);
		glBegin(GL_LINES);
		}
		else
		{
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
		glBegin(GL_LINE_STRIP);
		}
		*/		
		for(int j=0;j<polyline.ptData.size();j++)
		{
			glVertex3f(polyline.ptData.at(j).x,polyline.ptData.at(j).y,0);
		}
		glEnd();
	}
}
void DrawMapIndex(vector<PointShow> ptcloud)
{
	if (ptcloud.empty())
	{
		return;
	}
	glEnable(GL_DEPTH_TEST);
	for (int i=0;i<ptcloud.size();i++)
	{
		char num[20];
		memset(num,'\0',20);
		sprintf(num,"%d",ptcloud[i].type);
		DrawTextLineRGB(ptcloud[i].x,ptcloud[i].y,ptcloud[i].z,1.0,0.2,0.4,num);
	}
	glDisable(GL_DEPTH_TEST);
}
//添加图层
void ckMapBaseObject::AddLineLayer(CDataShow &cs)
{
	if (!cs.m_bIsCreate)
	{
		AfxMessageBox("视图未创建");
		return;
	}
	if(!cs.UpdateLineLayer("CKMAPLINE",m_drawLineBuffer))
	{
		cs.CreateLineLayer("CKMAPLINE",DrawPolyLine);
		cs.UpdateLineLayer("CKMAPLINE",m_drawLineBuffer);
	}
	if(!cs.UpdateLineLayer("CKMAPROADLANE",m_drawLaneBuffer))
	{
		cs.CreateLineLayer("CKMAPROADLANE",DrawRoadLane);
		cs.UpdateLineLayer("CKMAPROADLANE",m_drawLaneBuffer);
	}
	if(!cs.UpdatePointLayer("CKMAPINDEX",m_drawIndexBuffer))
	{
		cs.CreatePointLayer("CKMAPINDEX",DrawMapIndex);
		cs.UpdatePointLayer("CKMAPINDEX",m_drawIndexBuffer);
	}
	cs.SetLayerStatus("CKMAPINDEX",false);
}
void PointCircle(float x,float y,float r)  //画节点的圆
{
	//画圆
	int n=500;
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_POLYGON);
	for(int i=0;i<n;i++)
		glVertex2f(x+r*cos(2*M_PI/n*i), y+r*sin(2*M_PI/n*i));
	glEnd();
	glFlush();
}
void DrawMissionPoint(vector<ckMissionPoint> mission)
{
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(20);
	glBegin(GL_POINTS);
	for (int i=0;i<mission.size();i++)
	{
		if (mission.at(i).property0==0)
		{
			glColor4f(0,1,0,0.6);
		}
		else if (mission.at(i).property0==7)
		{
			glColor4f(1,0,0,0.6);
		}
		else
			glColor4f(1,1,0,0.6);

		glVertex3f(mission[i].x,mission[i].y,1.0);
		//	PointCircle(mission.at(i).x,mission.at(i).y,1.0/eyehig);
	}
	glEnd();

	//glEnable(GL_DEPTH_TEST);
	for (int i=0;i<mission.size();i++)
	{
		char num[20];
		memset(num,'\0',20);
		sprintf(num,"%d",i);
		DrawTextLineRGB(mission[i].x,mission[i].y,mission[i].hgt,1.0,0.2,0.4,num);
	}
}
void DrawTrackLayer(vector<ckPloyLineS> line)
{

	for (int i=0;i<line.size();i++)
	{
		ckPloyLineS polyline=line.at(i);

		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
		glLineWidth(10);
		glColor4f(0,1,0,0.7);
		glBegin(GL_LINE_STRIP);
		for(int j=0;j<polyline.ptData.size();j++)
		{
			glVertex3f(polyline.ptData.at(j).x,polyline.ptData.at(j).y,0.0);
		}
		glEnd();
	}
}
void ckMapBaseObject::AddMissionLayer(CDataShow &cs)
{
	if (!cs.m_bIsCreate)
	{
		AfxMessageBox("视图未创建");
		return;
	}
	if(!cs.UpdateMissionLayer("CKMISSION",m_drawMissionBuffer))
	{
		cs.CreateMissionLayer("CKMISSION",DrawMissionPoint);
		cs.UpdateMissionLayer("CKMISSION",m_drawMissionBuffer);
	}
	if(!cs.UpdateLineLayer("CKTRACKLANE",m_drawTrackBuffer))
	{
		cs.CreateLineLayer("CKTRACKLANE",DrawTrackLayer);
		cs.UpdateLineLayer("CKTRACKLANE",m_drawTrackBuffer);
	}
}
// Construct map from files
void ckMapBaseObject::BrowseDir(CString dir)
{
	CFileFind ff;

	CString szDir = dir;

	if(dir.Right(1) != "\\")

		szDir += "\\";

	szDir += "*.*";

	BOOL res = ff.FindFile(szDir);

	while(res)
	{
		res = ff.FindNextFile();
		if(ff.IsDirectory() && !ff.IsDots())
		{
			//如果是一个子目录，用递归继续往深一层找
			BrowseDir(ff.GetFilePath());
		}
		else if(!ff.IsDirectory() && !ff.IsDots())
		{
			//显示当前访问的文件,到listbox中
			CString filepath=ff.GetFilePath();
			if (GetFileExt(filepath)=="txt")
			{
				//构建文件列表
				m_mapFileList.push_back(filepath);
			}
		}

	}
	if (m_mapFileList.size()==0)
	{
		AfxMessageBox("工作空间中没有对应数据");
	}
	ff.Close();//关闭
}
void ckMapBaseObject::ReadTrackFile(CString path)
{

	//path->GpsLength=0;
	FILE *fp=fopen(path,"rb");
	ASSERT(fp);
	//获得文件的长度以及保存所有信息到buffer里面
	fseek(fp,0,SEEK_END);
	int Len = ftell(fp);
	int num=Len/sizeof(ckMapPoint);
	fseek(fp,0,SEEK_SET);
	ckMapPoint *pDataBuffer=new ckMapPoint[num];
	fread(pDataBuffer,sizeof(ckMapPoint),num,fp);
	ckMapLine mapline;
	mapline.start=pDataBuffer[0];
	mapline.end=pDataBuffer[num-1];
	//mapline.trackline
	for (int i=0;i<num;i++)
	{
		ckMapPoint pt=pDataBuffer[i];
		mapline.trackline.push_back(pt);

	}
	m_MapLineBuffer.push_back(mapline);
	delete[] pDataBuffer;
	fclose(fp);

}
bool Compare(const ckMifPoint&pt0,const ckMifPoint &pt1)
{
	return pt0.id<pt1.id;
}
//读取路线点文件，用于最低层轨迹导航
void ckMapBaseObject::ReadPointTable(CString path)
{
std:fstream ff;
	ff.open(path,ios::in);
	//fout.open("TestMap.txt",ios::out);
	if (!ff.good())
	{
		AfxMessageBox("导入点数据文件错误");
		return;
	}
	int mapNum;
	while(!ff.eof())
	{
		ckMifPoint pt;
		double lon,lat;
		ff>>pt.id>>lon>>lat;
		ckCoordinate::LongLat2XY(lon,lat,pt.x,pt.y);
		//fout<<pt.x<<"\t"<<pt.y<<"\r\n";
		//	fprintf(fp,"%lf,%lf\n",pt.x,pt.y);
		m_MifPoint.push_back(pt);
	}
	ff.close();
	GetMapCentor(m_centerPoint);
	//sort(m_MifPoint.begin(),m_MifPoint.end(),Compare);

}
//读取路段文件
//int id;
//string name;
//char direction;
//int snode;
//int enode;
//double length;
//short speed;
//short lanenum;
//char elevated;  //是否为高架
//char structure;
//short spdlmt2s; //起点速度限制 
//short spdlmt2e;  //重点速度限制  
void ckMapBaseObject::ReadLineTable(CString path)
{
std:fstream ff;
	ff.open(path,ios::in);
	if (!ff.good())
	{
		AfxMessageBox("导入线表文件错误");
		return;
	}
	//char strc[100];
	//ff.getline(strc,30);
	while(!ff.eof())
	{
		ckMifLine line;
		/*	ff>>line.mapnum>>line.id>>line.name>>line.direction>>line.snode>>line.enode>>\
		line.length>>line.speed>>line.lanenum>>line.elevated>>line.structure\
		>>line.spdlmt2s>>line.spdlmt2e;*/
		string lane;
		ff>>line.id>>line.name>>line.direction>>line.snode>>line.enode>>\
			line.length>>line.lanewidth>>line.lanenum>>line.speed>>line.linkId>>lane;
		vector<CString> lanestr=ckSplit(lane.c_str(),"|");
		for (int i=0;i<lanestr.size();i++)
		{
			int laneid=atoi(lanestr.at(i).GetBuffer(lanestr.at(i).GetLength()));
			line.LaneId.push_back(laneid);
		}
		m_MifLine.push_back(line);
	}
	ff.close();
}
//Link Node

//int NodeID;
//CROSSFLAG Cross_flg;
//char Light_flag;
//string XRNodeID;
//string RoadID;
//string XRLinkID;
//string Node_ID;
void ckMapBaseObject::ReadLinkPoint(CString path)
{
std:fstream ff;
	ff.open(path,ios::in);
	if (!ff.good())
	{
		AfxMessageBox("导入连接表文件错误");
		return;
	}
	while(!ff.eof())
	{
		ckLinkNode node;
		string crossinfo,nearstring,crossnear,linknear,laneinfo;
		ff>>node.id>>node.cross_flg>>node.light_flag>>node.cross_info>>node.sideway_num\
			>>nearstring>>crossnear>>linknear>>laneinfo;
		CString str=nearstring.c_str();
		vector<CString> strvector=ckSplit(str,"|");
		for (int i=0;i<strvector.size();i++ )
		{
			int roadid=atoi(strvector[i].GetBuffer(strvector[i].GetLength()));
			node.roadnear.push_back(roadid);
		}
		CString str1=laneinfo.c_str();
		vector<CString> strvector1=ckSplit(str1,"|");
		for (int i=0;i<strvector1.size();i++ )
		{
			int laneid=atoi(strvector1[i].GetBuffer(strvector1[i].GetLength()));
			node.laneinfo.push_back(laneid);
		}
		m_LinkNode.push_back(node);
		//m_MapLinkList.insert(map<int,ckLinkNode>::value_type(node.id,node));
	}
	ff.close();
}
//读取车道信息
//int LaneInfoID;
//char LaneID;
//int Spathline;
//short Node_ID;
//short LaneNum;
//short CLaneNum;
//char Lanetype;
//string Epathline;
//TurnType Arrow;
//float width;
//char Uturnflag;
void ckMapBaseObject::ReadRoadLaneTable(CString path)
{
std:fstream ff;
	ff.open(path,ios::in);
	if (!ff.good())
	{
		AfxMessageBox("导入车道线文件错误");
		return;
	}
	while(!ff.eof())
	{
		ckRoadLane line;
		/*ff>>line.LaneInfoID>>line.LaneID>>line.Spathline>>line.Node_ID>>\
		line.LaneNum>>line.CLaneNum>>line.Lanetype>>line.Epathline>>line.Arrow\
		>>line.width>>line.Uturnflag;*/
		ff>>line.id>>line.snode>>line.enode>>line.lanenum>>line.iscross;
		m_RoadLane.push_back(line);
	}
	ff.close();
}
void ckMapBaseObject::ReadTrackGps(CString path)
{
std:fstream ff;
	ff.open(path,ios::in);
	if (!ff.good())
	{
		AfxMessageBox("导入点数据文件错误");
		return;
	}
	int mapNum;
	while(!ff.eof())
	{
		ckMifPoint pt;
		double lon,lat,amizth;
		ff>>lon>>lat>>amizth;
		ckCoordinate::LongLat2XY(lon,lat,pt.x,pt.y);
		pt.amith=amizth;
		m_MifPoint.push_back(pt);
	}
	ff.close();
	GetMapCentor(m_centerPoint);
}
void ckMapBaseObject::ReadLanePointTable(CString path)
{
std:fstream ff;
	ff.open(path,ios::in);
	if (!ff.good())
	{
		AfxMessageBox("导入点数据文件错误");
		return;
	}
	int mapNum;
	while(!ff.eof())
	{
		ckMifPoint pt;
		double lon,lat;
		ff>>pt.id>>lon>>lat;
		ckCoordinate::LongLat2XY(lon,lat,pt.x,pt.y);
		m_LanePoint.push_back(pt);
	}

	ff.close();
}
//读取人行横道数据
//int sidewayid;
//string linklanes;
//int snode;
//int enode;
//float length;
//float width;
void ckMapBaseObject::ReadSideWayTable(CString path)
{
std:fstream ff;
	ff.open(path,ios::in);
	if (!ff.good())
	{
		AfxMessageBox("导入人道行文件错误");
		return;
	}
	while(!ff.eof())
	{
		ckSideway side;
		ff>>side.sidewayid>>side.linklanes>>side.snode>>side.enode>>\
			side.length>>side.width;
		m_SideWay.push_back(side);
	}
}

//读取比赛任务文件
void ckMapBaseObject::ReadMissionTable(CString path)
{
std:fstream ff;
	ff.open(path,ios::in);
	if (!ff.good())
	{
		AfxMessageBox("导入任务点数据文件错误");
		return;
	}
	while(!ff.eof())
	{
		ckMissionPoint mission;
		double lon,lat;
		ff>>mission.num>>lon>>lat>>mission.hgt>>\
			mission.property0>>mission.property1;
		ckCoordinate::LongLat2XY(lon,lat,mission.x,mission.y);
		m_MissionPoint.push_back(mission);
		mission.x=mission.x-m_centerPoint.x;
		mission.y=mission.y-m_centerPoint.y;
		m_drawMissionBuffer.push_back(mission);
	}
	ff.close();
}
/************************************************************************/
/*根据文件列表构建数据列表                                                                      */
/************************************************************************/
BOOL ckMapBaseObject::BuildTrackFactory(CString factory)
{
	BrowseDir(factory);
	if (m_mapFileList.size()!=0)
	{
		for (int i=0;i<m_mapFileList.size();i++)
		{
			CString fname=GetFileName(m_mapFileList[i]);
			//ReadTrackFile(m_mapFileList[i]);

			if (fname.Find("LPoint")!=-1)
			{
				ReadPointTable(m_mapFileList[i]);

			}
			else if (fname.Find("Line")!=-1)
			{
				ReadLineTable(m_mapFileList[i]);
			}
			else if (fname.Find("Link")!=-1)
			{
				ReadLinkPoint(m_mapFileList[i]);
			}
			else if (fname.Find("LAPoint")!=-1)
			{
				ReadLanePointTable(m_mapFileList[i]);
			}
			else if (fname.Find("Lane")!=-1)
			{
				ReadRoadLaneTable(m_mapFileList[i]);
			}
			else if (fname=="RD_SideWay.txt")
			{
				ReadSideWayTable(fname);
			}
			else if (fname=="RD_RoadLane.txt")
			{
				ReadRoadLaneTable(m_mapFileList[i]);
			}

		}
		CString indexpath=factory+"\\MapIndex.txt";
		CreateLineSegment(indexpath);
		Convert2IndexLayer(m_ckLineBuffer,m_drawIndexBuffer);
		//	vector<ckPloyLineS> linebufer;
		Convert2LineLayer(m_MifPoint,m_MifLine,m_drawLineBuffer);
		Convert2LaneLayer(m_LanePoint,m_RoadLane,m_drawLaneBuffer);


		return TRUE;
	}

	return FALSE;

}
/************************************************************************/
/*
Param：
missline 途径路线
line 搜索路线
Return:
-1 没有搜索到对应节点
1 成功构建路线
*/
/************************************************************************/
int ckMapBaseObject::MappingSection(ckMapLine & missline,ckMapLine line)
{
	vector<ckMapPoint> templine=line.trackline;
	int startidx=0,endidx=0;
	ckMapPoint startpoint=missline.start;
	ckMapPoint endpoint=missline.end;

	//位置匹配
	for(int i=0;i<templine.size();i++)
	{
		if (IsSameNode(startpoint,templine[i]))
		{
			startidx=i;
			for (int j=i;j<templine.size();j++)
			{
				if (IsSameNode(endpoint,templine[i]))
				{
					endidx=j;
					for (int k=i;k<=j;k++)
					{
						missline.trackline.push_back(templine[k]);
						return 1;
					}
					//return 
				}
			}
		}
	}
	return -1;
}
//根据文件类型构建文件列表
BOOL ckMapBaseObject::BuildMifFactory()
{
	return FALSE;
}
/************************************************************************/
/*   根据已经读取的地图文件生成路网 线只有起点和终点     
遍历每条线的并根据终点的编号获取连接点
构建网络有向图
*/
/************************************************************************/
struct linkcompare: binary_function<ckLinkNode, int,bool>
{
	bool operator()( ckLinkNode &value, int id) const
	{
		if (value.id== id)
			return true;
		else
			return false;
	}
};
int ckMapBaseObject::CreateMapLineNet()
{
	//if (BuildTrackFactory())
	//{
	//	int size=m_MifLine.size();
	//	for (int i=0;i<size;i++)
	//	{
	//		ckMifLine line=m_MifLine[i];
	//		int start=line.snode;
	//		int end=line.enode;
	//		ckLineSegment ls;
	//		ls.lineinfo=line;
	//		//ls.ptrNode=&m_LinkNode[line.linkId];
	//	    //根据id号搜索匹配的道路连接点
	//		vector<ckLinkNode>::iterator linkiter=find_if(m_LinkNode.begin(),m_LinkNode.end(),bind2nd(linkcompare(),line.linkId));
	//		ls.ptrNode=&(*linkiter);
	//		m_ckLineBuffer.push_back(ls);
	//	}
	//	return 1;
	//}
	return -1;
}

//根据Link信息构建邻接矩阵
BOOL ckMapBaseObject::ExplainRoadNet()
{
	int size=m_LinkNode.size();
	if(size==0)
	{
		AfxMessageBox("为正确导入Link数据文件");
		return FALSE;
	}
	else
	{

		for (int i=0;i<size;i++)
		{
			//判定节点为过渡节点
			if (0==m_LinkNode[i].cross_flg)
			{

			}
			//判定节点为单一路口点
			else if (1==m_LinkNode[i].cross_flg)
			{

			}
			//判定节点为复合路口点
			else if (2==m_LinkNode[i].cross_flg)
			{

			}
		}
	}

}
//判定直线与点距离和方向是否一致
BOOL ckMapBaseObject::CanddidateLine(vector<ckLineSegment>& line,ckMapPoint pt,int buffersize)
{
	//double vx0=sin(pt.angle);
	//double vy0=cos(pt.angle);
	//double delX0=line.snode.x-pt.x;
	//double delY0=line.snode.y-pt.y;
	//double delX1=line.enode.x-pt.x;
	//double delY1=line.enode.x-pt.y;
	//double delX2=line.enode.x-line.snode.x;
	//double delY2=line.enode.y-line.enode.y;
	////distance dis
	//double d0=sqrt(pow(delX0,2)+pow(delY0,2));
	//double d1=sqrt(pow(delX1,2)+pow(delY1,2));
	//double d2=sqrt(pow(delX2,2)+pow(delY2,2));
	//double mid=(d0+d1+d2)/2;
	//double S=sqrt(mid*(mid-d0)*(mid-d1)*(mid-d2));
	//double dis=S*2/d2;

	//if (d1<_CLOSE_DISTANCE_||d0<_CLOSE_DISTANCE_)
	//{
	//	if (vx0*delX2+vy0*delY2>0)
	//	{
	//		return TRUE;
	//	}
	//}
	//else
	//{
	//	if ((delX0*delY0+delX1*delY1<0)&&(dis<_CLOSE_DISTANCE_))
	//	{
	//		return TRUE;
	//	}
	//}
	ASSERT(!m_ckLineBuffer.empty());
	ckRect ptRect=ckRect(CKROUND(pt.x)-buffersize,CKROUND(pt.y)-buffersize,
		CKROUND(pt.x)+buffersize,CKROUND(pt.y)+buffersize);
	for (int i=0;i<m_ckLineBuffer.size();i++)
	{
		if (Overlap((m_ckLineBuffer.at(i).idxrect),ptRect))
		{
			line.push_back(m_ckLineBuffer.at(i));
		}
	}
	if (line.size()>0)
	{
		return TRUE;
	}
	if (buffersize<_CLOSE_DISTANCE_)
	{
		CanddidateLine(line,pt,buffersize*2);
	}
	else	
		return FALSE;
}
//计算节点的备选路段
int ckMapBaseObject::GetNearLines(vector<ckLineSegment>& lines,ckMapPoint pt)
{
	if(m_ckLineBuffer.size()==0)
	{
		AfxMessageBox("未构建空间索引");
		return -1;
	}
	//vector<ckLineSegment> templine;
	int buffer=8;
	//计算节点的备选路段
	if (CanddidateLine(lines,pt,buffer))
	{
		return 1;
	}
	else
	{
		return 0;

	}	
	//return -1;
}
//计算方位角
double ckMapBaseObject::AmizthCaltulate(double x0,double y0,double x1,double y1)
{
	double delta_x=x1-x0;
	double delta_y=y1-y0;
	double angle=0.0;
	angle=atan(delta_x/delta_y);
	if(delta_y>0&&delta_x>0)
	{
		angle=angle;
	}
	else if(delta_y<0&&delta_x>0)
	{
		angle=angle+M_PI;
	}
	else if(delta_x<0&&delta_y<0)
	{
		angle=angle+M_PI;
	}
	else if(delta_x<0&&delta_y>0)
	{
		angle=angle+2*M_PI;
	}
	return angle;
}
//用于匹配任务点所在的路段，返回路段ID号
int ckMapBaseObject::SearchStartLine(ckLineSegment &sline,ckMissionPoint mission)
{
	ckMissionPoint Smission=m_MissionPoint.at(0); //从起点开始匹配
	ckMapPoint pt;
	pt.x=Smission.x;
	pt.y=Smission.y;
	pt.angle=AmizthCaltulate(m_MissionPoint.at(0).x,m_MissionPoint.at(0).y,m_MissionPoint.at(1).x,m_MissionPoint.at(1).y);
	vector<ckLineSegment> linebuffer;
	/*for(int i=0;i<m_ckLineBuffer.size();i++)
	{
	if (IsCanddidateLine(m_ckLineBuffer.at(i),pt))
	{
	linebuffer.push_back(m_ckLineBuffer.at(i));
	}
	}*/
	if (linebuffer.size()==0)
	{
		//AfxMessageBox("未匹配到正确的道路起点");
		return -1;
	}
	else if (linebuffer.size()==1)
	{
		sline=linebuffer.at(0);
		return 0;
	}
	else
	{
		return 0;
	}
	return -1;
}
struct linecompare: binary_function<ckLineSegment, int,bool>
{
	bool operator()( ckLineSegment &value, int id) const
	{
		if (value.lineinfo.id== id)
			return true;
		else
			return false;
	}
};
//根据初始化的直线搜索生成整条路径
UINT ckMapBaseObject::GeneratePath(vector<ckMapPoint>& path)
{
	// ckLineSegment line;
	// ckLineSegment next;
	// ckLinkNode *link;
	// vector<ckLineSegment> resbuffer;
	// if (SearchStartLine(line,m_MissionPoint[0])!=-1)
	// {
	//  path.push_back(line.snode);
	//  link=line.ptrNode; 
	//  if (link->cross_flg==0)
	//  {
	//   //根据ID号搜索全局路径中的对应路段
	///*   vector<ckLineSegment>::iterator linenode=find_if(m_ckLineBuffer.begin(),m_ckLineBuffer.end(),bind2nd(linecompare(),link->roadnear));
	//   next=(*linenode);
	//   link=next.ptrNode;*/
	//  }
	//   //判断连接点为中继点
	//  while(next.ptrNode->cross_flg!=3)
	//  {
	//   path.push_back(next.snode);
	///*   vector<ckLineSegment>::iterator linenode=find_if(m_ckLineBuffer.begin(),m_ckLineBuffer.end(),bind2nd(linecompare(),link->roadnear));
	//   next=(*linenode);
	//   link=next.ptrNode;  */   
	//  }
	// }
	return 0;
}
/************************************************************************/
/*
Param
start : 任务路段起点
end :任务路段终点

Function:
根据任务文件点序列生成一个路段的路网数据

Output
linepath 匹配得到的线路
*/
/************************************************************************/
double get_maha_distance(ckMapPoint pt0,ckMapPoint pt1)
{
	return abs(pt1.x-pt0.x)+abs(pt1.y-pt0.y);
}
bool ckMapBaseObject::Overlap(ckRect a_rectA, ckRect a_rectB)
{
	//ASSERT(a_rectA && a_rectB);

	for(int index=0; index < 2; ++index)
	{
		if (a_rectA.min[index] > a_rectB.max[index] ||
			a_rectB.min[index] > a_rectA.max[index])
		{
			return false;
		}
	}
	return true;
}
//判定一条直线是否与连接点集相连接
bool ckMapBaseObject::IsConnectLine(ckLineSegment line0,int linkline)
{

	for (int j=0;j<line0.linkNode.roadnear.size();j++)
	{
		vector<int> nearl=line0.linkNode.roadnear;
		if (linkline==nearl.at(j)||linkline==line0.lineinfo.id)
		{
			return true;
		}
	}

	return false;
}
//计算路段最小外接矩形
void ckMapBaseObject::GetEnclosRectangle(vector<ckMifPoint> ptline,ckRect& rt)
{
	int start0=(int)(ptline.at(0).x+0.5f);
	int start1=(int)(ptline.at(0).y+0.5f);
	ckRect rect=ckRect(start0,start1,start0,start1);
	for (int i=0;i<ptline.size();i++)
	{
		double minx,miny,maxx,maxy;
		minx=(int)(ptline.at(i).x);
		miny=(int)(ptline.at(i).y);
		maxx=minx+1;
		maxy=miny+1;
		if (minx<rect.min[0])
		{
			rect.min[0]=minx;
		}
		else if (maxx>rect.max[0])
		{
			rect.max[0]=maxx;
		}
		if (miny<rect.min[1])
		{
			rect.min[1]=miny;
		}
		else if (maxy>rect.max[1])
		{
			rect.max[1]=maxy;
		}
	}
	//if (rect.min[0]==rect.max[0])
	//{
	//	rect.min[0]=rect.min[0]-1;
	//	rect.max[0]=rect.max[0]+1;
	//}
	//if (rect.min[1]=rect.max[1])
	//{
	//	rect.min[1]=rect.min[1]-1;
	//	rect.max[1]=rect.max[1]+1;
	//}

	rt=rect;
}
//构建线索引
void ckMapBaseObject::CreateLineSegment(CString path)
{
	ASSERT(!m_MifLine.empty());
	fstream ff;
	ff.open(path,ios::out);
	if (!ff.good())
	{
		AfxMessageBox("Create Index Error ");
		return;
	}
	for (int i=0;i<m_MifLine.size();i++)
	{
		ckLineSegment lsegment;
		lsegment.lineinfo=m_MifLine.at(i);
		int s,e;
		s=m_MifLine.at(i).snode;
		e=m_MifLine.at(i).enode;
		vector<ckMifPoint> linept;
		if (s<e)
		{
			for (int i=s;i<=e;i++)
			{
				ckMifPoint pt;
				pt.x=m_MifPoint.at(i).x;
				pt.y=m_MifPoint.at(i).y;
				linept.push_back(pt);
			}
		}
		else
		{
			for (int i=s;i>=e;i--)
			{
				ckMifPoint pt;
				pt.x=m_MifPoint.at(i).x;
				pt.y=m_MifPoint.at(i).y;
				linept.push_back(pt);
			}
		}
		ckRect rect;
		GetEnclosRectangle(linept,rect);
		ff<<lsegment.lineinfo.id<<"\t"<<rect.min[0]<<"\t"<<rect.min[1]<<"\t"<<\
			rect.max[0]<<"\t"<<rect.max[1]<<"\n";
		lsegment.idxrect=rect;
		lsegment.linkNode=m_LinkNode.at(m_MifLine.at(i).linkId);
		m_ckLineBuffer.push_back(lsegment);
	}
	ff.close();

}
/************************************************************************/
/* 递归遍历生成的路径树
   1.剔除重复生成路径
   2.根据是否到达终点来判断路径是否有效

*/
/************************************************************************/

bool ckMapBaseObject::CreateLineTree(ckLineTree &treenode,vector<vector<ckLineSegment>> lines,int level)
{
	if (level==lines.size())
	{
		return true;
	}
	bool isnewnode=false;
	//	int size=treenode.child.size();
	//for (int i=0;i<size;i++)````````
	//{
	vector<ckLineSegment> linebuffer=lines.at(level);		

	for (int k=0;k<linebuffer.size();k++)
	{
		if (IsConnectLine(treenode.line,linebuffer[k].lineinfo.id))
		{
			ckLineTree node;
			node.line=linebuffer.at(k);
			treenode.child.push_back(node);
			isnewnode=true;
		}
	}		
	if (!isnewnode)
	{//所有节点均未匹配到树
		treenode.line.lineinfo.id=-1;
		return false;
	}
	else
	{
		vector<ckLineTree>::iterator itline;
		for (itline=treenode.child.begin();itline<treenode.child.end();itline++)
		{
			/*if (!CreateLineTree(*itline,lines,level+1))
			{
			itline->line.lineinfo.id=-1;
			}*/
			CreateLineTree(*itline,lines,level+1);			
		}
	}

	//	return isnewnode;
}
//计算父子节点中相互连通的线对

void subnode_matched(vector<ckLineSegment> father,vector<ckLineSegment> childs,vector<ckLineSegment>&output)
{
	for (int i=0;i<father.size();i++)
	{
		  vector<short> nearf=father[i].linkNode.linknear;
		for (int j=0;j<childs.size();j++)
		{
		
			for (int k=0;k<nearf.size();k++)
			{
				if (nearf[k]==childs[j].lineinfo.id)
				{
					output.push_back(childs[j]);
				}
			}
		  //vector<int> nearc=childs[j].linkNode.linknear;

		}
	}
}
bool ckMapBaseObject::SelectOptialLine(vector<ckLineSegment>& outputlines,ckLineTree root,int level)
{
	if (level==m_MissionPoint.size())
	{
		return true;
	}
	bool hasline=false;
	for (int j=0;j<root.child.size();j++)
	{
		if (root.child[j].line.lineinfo.id!=-1)
		{
			hasline=true;

			if (outputlines[outputlines.size()-1].lineinfo.id!=root.child[j].line.lineinfo.id)
			{
				level++;
				outputlines.push_back(root.child[j].line);
				SelectOptialLine(outputlines,root.child[j],level);
			}
		//	vector<ckLineTree> temp=nodes[j].child;		//	nodes.clear();		//	nodes.assign(temp.begin(),temp.end());
		}
	}
	//错误节点  前提条件是只有一条，存在二义性的时候
	if (!hasline)
	{
		outputlines.pop_back();
		return false;
	}
}
BOOL ckMapBaseObject::CreateMissionLines(vector<ckLineSegment>& outputLines)
{
	ASSERT(!m_MissionPoint.empty());
	//计算所有任务点的备选路段
	vector<vector<ckLineSegment>> linesbuffer;
	for (int i=0;i<m_MissionPoint.size();i++)
	{
		vector<ckLineSegment> templine;
		ckMapPoint pt;
		pt.x=m_MissionPoint.at(i).x;
		pt.y=m_MissionPoint.at(i).y;
		if (GetNearLines(templine,pt))
		{
			linesbuffer.push_back(templine);
		}
	}
	//改进版程序，通过判断路径是否到达终点为结束条件
	/************************************************************************/
	/* 限制条件
	   路径唯一性：从起点计算到当前匹配点的线路是唯一的能到达所有节点的点
	   顺序关系：当前点只能通过上一个点到达，上一个点不能到达当前点时，采用A*计算最短路径
	   结束判定依据：从起点开始，严格驻点匹配计算路径，并最终能到达终点
	     外循环，终点所在的直线包含在当前路径的子节点中
		 内部判定
		 1.从起点开始，判断所有可能的线，选择距离上与车体最接近的为准
		 2.若判断结果中，节点的邻近线中包含父子关系，子节点保留，父节点选择最邻近线
	*/
	/*******打死不用递归*****************************************************************/
	vector<ckLineSegment> childnodes; //记录所有子节点

	//根据连通关系从备选路段中选择唯一路径
	//int i=1;

	//map<int,vector<ckLineSegment>> resultLines;
	vector<ckLineSegment> resultLines;
	vector<ckLineSegment> startlines=linesbuffer.at(0);

	int searchid=1;
	vector<ckLineTree> trees;
	for (int i=0;i<startlines.size();i++)
	{
		//构建根节点
		ckLineSegment tsline=startlines.at(i);	
		vector<ckLineSegment> mappinglines=linesbuffer.at(searchid);
		//	vector<ckLineTree> childs(0);
		outputLines.push_back(tsline);
		ckLineTree root;
		root.line=tsline;
		if (CreateLineTree(root,linesbuffer,1))
		{
			vector<ckLineTree> nodes=root.child;
			while(!nodes.empty())
			{
				bool hasline=false;
				for (int j=0;j<nodes.size();j++)
				{
					if (nodes[j].line.lineinfo.id!=-1)
					{
						hasline=true;
					
						if (outputLines[outputLines.size()-1].lineinfo.id!=nodes[j].line.lineinfo.id)
						{
							outputLines.push_back(nodes[j].line);
						}
						
						vector<ckLineTree> temp=nodes[j].child;
						nodes.clear();
						nodes.assign(temp.begin(),temp.end());
					}
				}
				if (!hasline)
				{
					nodes.clear();
				}
			}
			return TRUE;
		}	
		
		//	root.child=childs;
		/*	bool isnewnode=false;
		for (int k=0;k<mappinglines.size();k++)
		{
		if (IsConnectLine(root.line,mappinglines[k].lineinfo.id))
		{
		ckLineTree node;
		node.line=mappinglines.at(k);
		root.child.push_back(node);
		isnewnode=true;
		}
		}	*/
		//初始节点找不到子树，判定路径生成失败

		////深度搜索，判定能搜索到终点的路线为最终路线
		//while(searchid<m_MissionPoint.size())
		//{
		//	 mappinglines=linesbuffer.at(searchid);
		//	 vector<ckLineTree> tempnode(root.child.begin(),root.child.end());
		//	// vector<ckLineTree> tchild=root.child;
		//	if (CreateLineTree(tempnode,mappinglines))
		//	{
		//		searchid++;
		//		//CreateLineTree(root.)
		//	}
		//}
	}
	//while(linesbuffer.at(searchid).size()>1)
	//{
	//	//统计下一节点的备选路段ID
	//	vector<int> nextids;
	//	for (int j=0;j<linesbuffer.at(searchid).size();j++)
	//	{
	//		nextids.push_back(linesbuffer.at(searchid).at(j).lineinfo.id);
	//	}
	//          
	//}

	return FALSE;
}

//计算车道网络
UINT ckMapBaseObject::CalMissionRoad(vector<ckLineSegment>segmengs,vector<ckRoadLane> &lane)
{
	ASSERT(!segmengs.empty());
	for (int i=0;i<segmengs.size()-1;i++)
	{
		vector<int> laneids=segmengs[i].linkNode.laneinfo;	
		int id=segmengs[i].lineinfo.LaneId[0];


		lane.push_back(m_RoadLane.at(id));

		vector<int> linklines=segmengs[i].linkNode.roadnear;
		int nextline;
		for (int j=0;j<linklines.size();j++)
		{
			int nextid=segmengs.at(i+1).lineinfo.id;
			if (linklines.at(j)==nextid)
			{
				nextline=laneids.at(j);
			}
		}
		lane.push_back(m_RoadLane.at(nextline));
	}
	int lastlane=segmengs[segmengs.size()-1].lineinfo.LaneId[0];
	lane.push_back(m_RoadLane.at(lastlane));
	Convert2LaneLayer(m_LanePoint,lane,m_drawTrackBuffer);
	return 0;
	//ckLineSegment startline;
	//ckMapPoint pt0,pt1;
	//pt0.x=start.x;
	//pt0.y=start.y;
	//pt1.x=end.x;
	//pt1.y=end.y;
	//pt0.angle=AmizthCaltulate(pt0.x,pt0.y,pt1.x,pt1.y);
	//if (start.property0==0)
	//{
	//	//计算马氏缓冲距离内的备选路段
	//	vector<ckLineSegment> latentlines;
	//	for (int i=0;i<m_ckLineBuffer.size();i++)
	//	{
	//           double mahadis=get_maha_distance(m_ckLineBuffer.at(i).snode,pt0);
	//          if (mahadis<_CLOSE_MAHA_DISTANCE_)
	//          {
	//		   latentlines.push_back(m_ckLineBuffer.at(i));
	//          }
	//	}
	//	//对备选路段与起点方向夹角进行排序，选取最小夹角路段
	//	if (latentlines.size()==0)
	//	{
	//		AfxMessageBox("当前位置未搜索到匹配路段");
	//		return 0;
	//	}
	//	else
	//	{
	//		double minangle=ANGLEDIF(latentlines.at(0).snode.angle,pt0.angle);
	//		int idx=0;
	//		for (int i=1;i<latentlines.size();i++)
	//		{
	//			double delangle=ANGLEDIF(latentlines.at(i).snode.angle,pt0.angle);
	//		    if (minangle>delangle)
	//		    {
	//				minangle=delangle;
	//				idx=i;
	//		    }

	//		}
	//           linepath.push_back(latentlines.at(idx));
	//	}

	//}
	//   //判定为路段入点
	//else if (start.property0==1)
	//{
	//	ckLineSegment outputline;
	//       int outputid;
	//	//根据属性2判定下一路段的选择
	//	if (start.property1==1) //直行
	//	{
	//		outputid=linepath.back().ptrNode->roadnear[1];

	//	}
	//	else if (start.property1==2) //右转
	//	{
	//		outputid=linepath.back().ptrNode->roadnear[0];
	//	}
	//	else if (start.property1==3) //左转
	//	{
	//		outputid=linepath.back().ptrNode->roadnear[2];
	//	}
	//	else if (start.property1==4) //掉头
	//	{
	//		outputid=linepath.back().ptrNode->roadnear[3];
	//	}
	//	map<int,ckLineSegment>::iterator iter=m_MaplineList.find(outputid);
	//    linepath.push_back(iter->second);
	//}	
}
