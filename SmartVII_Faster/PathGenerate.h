#pragma once

//规划部分参数
#define PI 3.1415926
#define STEP 0.1
#define STEPNUM 40
#define POINTNUM 50
#define MAXITER 100
#define eps1 2.2204e-016 
#define inf 10000000
#define CLOTHOIDPOINTNUM 100
#define BETWEENNODES 4
#define GRID_WIDTH 0.2		//网格的宽度 0.2米
#define GRID_WIDTH_X 0.2		
#define GRID_WIDTH_Y 0.2
#define TOL_LINE 1
#define BUFFERWIDTH 8//直线缓冲格网的个数（不是米）
#define BUFFERWIDTHFORCLOTHOID 7//clothoid曲线的缓冲网格个数
#define SAFEDISTANCE 25 //安全距离
#define DELTAANGLE 0.15//规划起始点和目标点的角度差

#define GRID_I 100
#define GRID_J 200
#define GPSLINE_Zanshi 24
#define TOLDISTANCE 6
#define MAXD 100000000
#define MINDIS 36//认为与车距离在2米以内的点都可以认为是目前的大致位置



#include "udpReceive.h"

typedef struct tagRoadPoint
{
	double x;
	double y;
	double angle;
	double changeangle;
	double k;
}RoadPoint;

typedef struct tagNodePoint
{
	RoadPoint point;
	int fatherNode;
	double cost;
}NodePoint;
typedef struct tagObstacle
{
	RoadPoint LUPoint;
	RoadPoint LDPoint;
	RoadPoint RUPoint;
	RoadPoint RDPoint;
	RoadPoint Polygon[4];

}Obstacle;

typedef struct tagVeloGrid
{
	double GauX;
	double GauY;
	double angle;
	BOOL isObs;

}VeloGrid;


struct LINESEG 
{ 
	RoadPoint s; 
	RoadPoint e; 

}; 
typedef struct tabRoadNet
{
	int num;
	double longitude;
	double latitude;
	double height;
	int roadprop;
	double x;
	double y;
	double angle;

}RoadNet;


//枚举地图类型 轨迹数据为TRACK，路网数据为NET
enum  MapType{TRACK=1,NET=2,};

enum SPEEDCONTROL
{
	SPEEDSTABLE,  //保持当前速度
	SPEEDUP,  //加速
	SPEEDDOWN, //减速
	BRAKE,//刹车
};

typedef struct  Grids
{
	int X;
	int Y;
} GridPoint;

typedef void (*CallBack_plan)(RoadPoint *buf, int len) ;



class CPathGenerate
{
public:
	CPathGenerate(void);
	~CPathGenerate(void);
public://转换函数
	static int LongLat2XY(double longitude,double latitude,double &X,double &Y);
	static int XY2LongLat(double X,double Y,double& longitude,double& latitude);

public:

	CString gpsFileName;
	CString roadNetFileName;
	CRITICAL_SECTION m_planCs;
	RoadPoint *m_RoadDataBuffer;
	int fileLength;//路网点扩充的数量或gps点的数量
	int m_netFileLength;	
	RoadNet m_roadNetBuffer[256];
	MapType m_eType;

	int Obs_Avoidance;//避障模式，动态避障模式
	int Obs_Avoid_Time;//动态避障模式持续次数


	//外部变量
public:
	int LoadMapFile(CString path);
	void CalMapCenter();
	void StartCPathGenerate();
	BOOL Cal_PassLine(GridPoint p0,GridPoint p1,int* obs);
	BOOL SearchBestTarget(GridPoint p0,GridPoint p1,int* obs,int width);
	//从外部获得各种数据
	inline RoadPoint GetTarPoint(){return m_tarPoint;}
	inline RoadPoint GetInitialpoint(){return initialPoint;}
	inline RoadPoint* GetFinalPath(){return m_ultiPath;}

	inline RoadPoint* GetLastPath(){return m_lastPath;}
	inline RoadPoint* GetLastGauPath(){return m_lastPath_Gussian;}
	inline int GetlastpathLength(){return m_lastUltiLen;}
	inline int GetBeginGPSPos(){return beginPointGPSPos;}

	inline int GetFinalpathLength(){return m_ultiLength;}
	inline RoadPoint GetCurrentPoint(){return g_CurrentLocation;}
	inline RoadNet* GetRoadNetBuffer(){return m_roadNetBuffer;}
	inline SPEEDCONTROL GetSpeedCommand(){return flag_SpeedControl;}
	inline short GetTagWheelAngle(){return m_TagWheelAngle; }
	inline BOOL GetPathFlag(){return flag_path;}
	inline RoadPoint* GetGauPath(){return m_ultiPath_Gussian;}
	inline RoadNet GetCurrentNode(){return m_roadNetBuffer[CurrentNetPoint];}

	
	inline int* GetObsGrid(){return obs_grid;}

	inline RoadPoint* GetFinalPath_HalfGrid(){return m_ultiPath_HalfGrid;}
	inline RoadPoint* GetGauPath_HalfGrid(){return m_ultiPath_HalfGrid_Gussian;}
	inline RoadPoint* GetGPSRoadDataBuffer(){return m_RoadDataBuffer;}
	inline int GetGPSTrackLength(){return fileLength;}


	inline BOOL GetHumanExist(){return HumanExist;} //人行道是否可通行
	inline BOOL GetSideWalkAccess(){return SideWalkAccess;}  //人行道是否可通行
	inline BOOL GetRoadAccess(){return RoadAccess;}   //道路是否阻断

	//局部规划变量
private://成员变量
	SPEEDCONTROL flag_SpeedControl;//初始为0，不做变化。为1时加速，为2时减速，等于3时刹车
	RoadPoint *m_invertedRoadDataBuffer;
	//	RoadPoint *m_pogBuffer;//用于规划中存储所有的点
	NodePoint m_pNodes[POINTNUM];  //规划路径的节点信息
	//vector<RoadPoint>m_ultiPath; //最终生成得到的路径
	RoadPoint m_beginPoint;  //规划的局部起点
	RoadPoint m_ultiPath[1000];//存局部坐标的规划路径
	RoadPoint m_ultiPath_Gussian[1000];//存高斯坐标的规划路径
	RoadPoint m_lastPath[1000];//存局部坐标的规划路径
	RoadPoint m_lastPath_Gussian[1000];
	RoadPoint m_ultiPath_linshi[1000];//存局部坐标的规划路径
	RoadPoint m_ultiPath_Gussian_linshi[1000];//存高斯坐标的规划路径
	DWORD m_lastUltiLen;//上一条路径的长度
	DWORD m_ultiLen_linshi;//临时用的路径长度
	int currentIndex;//当前车体位置在上一路径中的序号
	int startIndex;//规划起始点在上一路径中的序号
	int beginPointGPSPos;//规划起始点位置最近的GPS点的序号
	int tarIndex;

	DWORD m_ultiLength;//最终生成路径的长度（不是字节数，是路径上点的个数，字节数为sizeof(RoadPoint)*m_ultiLength)//

	RoadPoint m_clothoidPoint[CLOTHOIDPOINTNUM];//保存clothoid曲线生成的点
	//Obstacle *m_obstacle;//障碍物信息
	RoadPoint m_tarPoint;   //规划的局部终点
protected:
	RoadPoint initialPoint;//画图的基点
	RoadPoint g_CurrentLocation; //当前位置，可能有问题
	RoadPoint m_PurePursuitTar;
	//标志
	BOOL flag_Boundary;//边界标志
	BOOL flag_Obstacle;//障碍物标志
	BOOL flag_path;//道路生成成功或失败标志全部都是1为成功，0为失败
	short m_TagWheelAngle;//返回的方向盘转角



	//GPS轨迹上的第一个障碍物
	RoadPoint fistObs;

	//////////////人行道行人检测/////////
public:
	BOOL HumanExist;//是否有行人
	BOOL SideWalkAccess;//是否可通行

private:
	int SideWalkMode;//人行道行人检测状态
	int HumanLocationLeft;//左边行人位置
	int HumanLocationRight;//右边行人位置
	int LastHumanLocationLeft;
	int LastHumanLocationRight;

	int SideWalkModeTime;
	int leftWide,rightWide,leftLine,rightLine;

	RoadPoint GpsForSideWalkMode[1000];
	int dataCount;

	int flag_TrackBufferInObsGrid(RoadPoint *pointArr,int beginIndex,int length,int LeftOrRight,int &LineNum);
	/////////////////////////////////////

	/////////////道路阻断判断//////////////////////
public:
	BOOL RoadAccess;

	//////////////////////////////////

	///////////终点模式//////////////
private:
	BOOL EndMode;
	////////////////////////////////



public:
	inline BOOL IsTargetPoint(){return flag_IsCloseTo;}
	inline int GetNetNum(){return CurrentNetPoint;}
private://成员函数
	void readGPSDataFile(CString path);
	void readRoadNetFile(CString path);
	BOOL PathExtend();
	RoadPoint GetRandomPoint(RoadPoint p);
	DWORD FindNearestPoint(RoadPoint random,DWORD n);
	RoadPoint* GeneratePath(DWORD nearest,RoadPoint end);
	BOOL flag_BoundaryObstacle(RoadPoint Up[],RoadPoint Down[],LINESEG u,RoadPoint paths[],int pointnum);//等于1表示可以通过，等于0表示没有通过检验
	void findUltipath(NodePoint *nodes,RoadPoint *clothoid,DWORD finalI);
	void AddNodes(RoadPoint *path,int i);//两节点中插入节点
	BOOL flag_PointsInObsGrid(RoadPoint *pointArr,DWORD length);//格网障碍物检测
	int flag_SegPointsInObsGrid(BOOL isGussian,RoadPoint *pointArr,int beginIndex,DWORD length);
	BOOL flag_TargetonobsGrid(RoadPoint tar); //判断目标点附近有没有障碍物
	int GetCurrentPosition(int currentposi);
	int GetCloseTrackPoint(GpsLine pt);
	void initializepath();
	double AngleTransfer(double angle);

	BOOL PathExtendSpecial();

	BOOL PathExtendHalfGrid();
	void findUltipathHalfGrid(NodePoint *nodes,RoadPoint *clothoid,DWORD finalI);
	RoadPoint m_ultiPath_HalfGrid[CLOTHOIDPOINTNUM+POINTNUM];//存局部坐标的规划路径（20米*20米网格）
	RoadPoint m_ultiPath_HalfGrid_Gussian[CLOTHOIDPOINTNUM+POINTNUM];//存高斯坐标的规划路径（20米*20米网格）
	BOOL flag_path_HalfGrid;

	BOOL PathExtendWithoutObs();


	///////////靠右行驶算法部分///////////////////////////////

private:
	int obs_grid[GRID_I*GRID_J];
	unsigned char zj_grid[10000];
	int tarI;  //记录一个可通行的目标点
	int tarJ;

	int TT1,TT2,TT3,TT4,TT5,TT6,TT7,TT8,XXX,XXX1,XXX2,XXX3,XXX4,XXX5,XXX6,XXX7,XXX8,XXX9,XXX10,XXX11,XXX12,XXX13,XXX14,XXX15,XXX16;
	RoadPoint RR1,RR2,RR3,RR4,RR5,RR6;
	/////////////////////////////////////
	int VeloAccCount;//激光叠加的次数
	vector<VeloGrid> VeloAccPoints;//激光叠加容器

	int OffsetVal;//轨迹偏移量


	RoadPoint grid0;//网格(0,0)点的坐标（高斯）
	RoadPoint GridLeftDown,GridRightDown,GridLeftUp,GridRightUp;//网格四顶点
	int flag_Intersect;//起始点和下一路网点连线与网格的4种相交情况，1：与网格顶部相交；2：与左边界相交；3：与右边界相交；4：下一路网点在网格内,但连线的直线会与顶部相交；5：会与左边界；6：会与右边界
	GridPoint OriginStart,OriginEnd;//用于可通行点判断的起点和终点两个网格点，X,Y是网格的列和行编号

	int PJDISTANCE;//路径拼接距离
	
	//////////////////////////////////////

	void VeloAccumulation();//激光数据放入容器


	void FindAvailableTar(int* obj);
	void FindTarInGrid(int I,int J);
	RoadPoint FindTarByMoveX(int moveWidth);

	void FindAvailableTar(int* obj,int GridI,int GridJ);
	void FindTarInGrid(int I,int J,int GridI,int GridJ);


	RoadPoint FindTarMoveX(int moveWidth);
	RoadPoint FindTarInGridMoveX(int I,int J,int moveWidth);

	void GetRightLine();
	double GetTarAngle();
	double GetAngleFromGEarthPoints();
	int IsCloseToNet(int currentposi);
	double iterGetAngle(int J1,int J2);
	BOOL flag_Available(double x1,double y1,double x2,double y2);
	BOOL GetBufferGrid(double x1,double y1,double x2,double y2);
	void GussianToCar(double Gx,double Gy,double *cx,double *cy);

private:
	BOOL flag_IsCloseTo;
	int CurrentNetPoint;
	int CurrentNetStatus;
	CString GpsLuwangname;
	int obs_grid_simu[GRID_I][GRID_J];
	int LineInRight[GRID_J];

	int m_GENetFileLength;	
	RoadNet m_GoogleEarthRoadNet[200];
	/////////////////////////////////////////////////////////


	//线程函数的部分
public:
	void CreateThread();	//创建线程，suspended状态
	void StartThread();		//开始线程，如果线程未创建，则创建；然后设置为运行状态
	void SuspendThread(); //暂停线程
	void ResumeThread(); //恢复线程
	void StopThread(); //停止线程
	udpReceiveMsg m_recvMsg;
private:
	THREADMODE m_threadMode;
	BOOL		m_bStopThread;
	CWinThread	*m_pThreadHandle;
	static UINT PlanThreadPro(LPVOID lpParam); //线程函数



	/************************************************************************/
	/* 拓扑和clothoid部分，以及一些外部应用的static函数                                                                     */
	/************************************************************************/

public:
	static 	double AngleNormalnize(double phi);
	static	double MiddlePointPlan(double firMidPoint_x, double firMidPoint_y,double current_position_x,double
		current_position_y,double angle_sensor);//PID算法	
	static	short MiddlePiont_Shan(double firMidPoint_x,double firMidPoint_y);


	/************************************************************************/
	/*Clothoid算法部分                                                    */
	/************************************************************************/
private:
	void FresnelCS(double *FresnelC,double *FresnelS,double y);
	void intXY(double *X,double *Y,int nk,double a,double b,double c);
	void FresnelCSk(int nk,double t,double *FresnelC,double *FresnelS);
	void evalXYaLarge(double *X,double *Y,int nk,double a,double b);
	double S(double mu,double nu,double b);
	void evalXYazero(double*X,double*Y,int nk,double b);
	void evalXYaSmall(double *X,double *Y,int nk,double a,double b,double p);
public:
	void buildClothoid(double &k,double &dk,double &L,double x0,double y0,double theta0,double x1,double y1,double theta1,double tol); 
	double normalizeAngle(double phi_in);
	void findA(double &A,double Aguess,double delta,double phi0,double tol);
	double guessA(double phi0,double phi1);
	void pointsOnClothoid(RoadPoint XY[],double x0,double y0,double theta0,double kappa,double dkappa,double L,int npts);	


	/************************************************************************/
	/* 拓扑判断                                                                     */
	/************************************************************************/	
	//	obstacle GenerateObstacle(double LUx,double LUy,double RDx,double RDy);//构造障碍物

	bool InsideConvexPolygon(int vcount,RoadPoint polygon[],RoadPoint q);//点与多边形关系判断
	bool CurveInsects(RoadPoint Up[],RoadPoint Down[],LINESEG u);//
	bool intersect(LINESEG u,LINESEG v);//直线相交算法
	double Distance(RoadPoint m1,RoadPoint m2);  //欧几里得距离
	double cosangle(RoadPoint m1,RoadPoint m2,RoadPoint m3); //三点余弦
	double multiply(RoadPoint sp,RoadPoint ep,RoadPoint op) ;
	int sign(double m);
	RoadPoint getIntersectPoint(RoadPoint u1,RoadPoint u2,RoadPoint v1,RoadPoint v2);//求两条线段的交点
	double PointToSegDist(double x, double y, double x1, double y1, double x2, double y2);//求点（x,y）到线段的距离
	void makeline(RoadPoint p1,RoadPoint p2,double &a,double &b,double &c);//已知直线上两点，求直线方程ax+by+c=0的因子a,b,c
	bool Line_Seg_Intersect(double x1,double y1,double x2,double y2,double a,double b,double c);//判断线段和直线是否相交
	bool LineIntersection(double a1,double b1,double c1,double a2,double b2,double c2,RoadPoint &p);//求直线和直线的交点
	bool Angle_Bisector_Intersect(RoadPoint p1,RoadPoint p2,RoadPoint p3,RoadPoint &p);//求三角形角平分线和对边的交点(角是p1所在的角）
};