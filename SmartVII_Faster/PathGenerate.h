#pragma once

//�滮���ֲ���
#define PI 3.1415926
#define STEP 0.1
#define STEPNUM 40
#define POINTNUM 50
#define MAXITER 100
#define eps1 2.2204e-016 
#define inf 10000000
#define CLOTHOIDPOINTNUM 100
#define BETWEENNODES 4
#define GRID_WIDTH 0.2		//����Ŀ�� 0.2��
#define GRID_WIDTH_X 0.2		
#define GRID_WIDTH_Y 0.2
#define TOL_LINE 1
#define BUFFERWIDTH 8//ֱ�߻�������ĸ����������ף�
#define BUFFERWIDTHFORCLOTHOID 7//clothoid���ߵĻ����������
#define SAFEDISTANCE 25 //��ȫ����
#define DELTAANGLE 0.15//�滮��ʼ���Ŀ���ĽǶȲ�

#define GRID_I 100
#define GRID_J 200
#define GPSLINE_Zanshi 24
#define TOLDISTANCE 6
#define MAXD 100000000
#define MINDIS 36//��Ϊ�복������2�����ڵĵ㶼������Ϊ��Ŀǰ�Ĵ���λ��



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


//ö�ٵ�ͼ���� �켣����ΪTRACK��·������ΪNET
enum  MapType{TRACK=1,NET=2,};

enum SPEEDCONTROL
{
	SPEEDSTABLE,  //���ֵ�ǰ�ٶ�
	SPEEDUP,  //����
	SPEEDDOWN, //����
	BRAKE,//ɲ��
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
public://ת������
	static int LongLat2XY(double longitude,double latitude,double &X,double &Y);
	static int XY2LongLat(double X,double Y,double& longitude,double& latitude);

public:

	CString gpsFileName;
	CString roadNetFileName;
	CRITICAL_SECTION m_planCs;
	RoadPoint *m_RoadDataBuffer;
	int fileLength;//·���������������gps�������
	int m_netFileLength;	
	RoadNet m_roadNetBuffer[256];
	MapType m_eType;

	int Obs_Avoidance;//����ģʽ����̬����ģʽ
	int Obs_Avoid_Time;//��̬����ģʽ��������


	//�ⲿ����
public:
	int LoadMapFile(CString path);
	void CalMapCenter();
	void StartCPathGenerate();
	BOOL Cal_PassLine(GridPoint p0,GridPoint p1,int* obs);
	BOOL SearchBestTarget(GridPoint p0,GridPoint p1,int* obs,int width);
	//���ⲿ��ø�������
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


	inline BOOL GetHumanExist(){return HumanExist;} //���е��Ƿ��ͨ��
	inline BOOL GetSideWalkAccess(){return SideWalkAccess;}  //���е��Ƿ��ͨ��
	inline BOOL GetRoadAccess(){return RoadAccess;}   //��·�Ƿ����

	//�ֲ��滮����
private://��Ա����
	SPEEDCONTROL flag_SpeedControl;//��ʼΪ0�������仯��Ϊ1ʱ���٣�Ϊ2ʱ���٣�����3ʱɲ��
	RoadPoint *m_invertedRoadDataBuffer;
	//	RoadPoint *m_pogBuffer;//���ڹ滮�д洢���еĵ�
	NodePoint m_pNodes[POINTNUM];  //�滮·���Ľڵ���Ϣ
	//vector<RoadPoint>m_ultiPath; //�������ɵõ���·��
	RoadPoint m_beginPoint;  //�滮�ľֲ����
	RoadPoint m_ultiPath[1000];//��ֲ�����Ĺ滮·��
	RoadPoint m_ultiPath_Gussian[1000];//���˹����Ĺ滮·��
	RoadPoint m_lastPath[1000];//��ֲ�����Ĺ滮·��
	RoadPoint m_lastPath_Gussian[1000];
	RoadPoint m_ultiPath_linshi[1000];//��ֲ�����Ĺ滮·��
	RoadPoint m_ultiPath_Gussian_linshi[1000];//���˹����Ĺ滮·��
	DWORD m_lastUltiLen;//��һ��·���ĳ���
	DWORD m_ultiLen_linshi;//��ʱ�õ�·������
	int currentIndex;//��ǰ����λ������һ·���е����
	int startIndex;//�滮��ʼ������һ·���е����
	int beginPointGPSPos;//�滮��ʼ��λ�������GPS������
	int tarIndex;

	DWORD m_ultiLength;//��������·���ĳ��ȣ������ֽ�������·���ϵ�ĸ������ֽ���Ϊsizeof(RoadPoint)*m_ultiLength)//

	RoadPoint m_clothoidPoint[CLOTHOIDPOINTNUM];//����clothoid�������ɵĵ�
	//Obstacle *m_obstacle;//�ϰ�����Ϣ
	RoadPoint m_tarPoint;   //�滮�ľֲ��յ�
protected:
	RoadPoint initialPoint;//��ͼ�Ļ���
	RoadPoint g_CurrentLocation; //��ǰλ�ã�����������
	RoadPoint m_PurePursuitTar;
	//��־
	BOOL flag_Boundary;//�߽��־
	BOOL flag_Obstacle;//�ϰ����־
	BOOL flag_path;//��·���ɳɹ���ʧ�ܱ�־ȫ������1Ϊ�ɹ���0Ϊʧ��
	short m_TagWheelAngle;//���صķ�����ת��



	//GPS�켣�ϵĵ�һ���ϰ���
	RoadPoint fistObs;

	//////////////���е����˼��/////////
public:
	BOOL HumanExist;//�Ƿ�������
	BOOL SideWalkAccess;//�Ƿ��ͨ��

private:
	int SideWalkMode;//���е����˼��״̬
	int HumanLocationLeft;//�������λ��
	int HumanLocationRight;//�ұ�����λ��
	int LastHumanLocationLeft;
	int LastHumanLocationRight;

	int SideWalkModeTime;
	int leftWide,rightWide,leftLine,rightLine;

	RoadPoint GpsForSideWalkMode[1000];
	int dataCount;

	int flag_TrackBufferInObsGrid(RoadPoint *pointArr,int beginIndex,int length,int LeftOrRight,int &LineNum);
	/////////////////////////////////////

	/////////////��·����ж�//////////////////////
public:
	BOOL RoadAccess;

	//////////////////////////////////

	///////////�յ�ģʽ//////////////
private:
	BOOL EndMode;
	////////////////////////////////



public:
	inline BOOL IsTargetPoint(){return flag_IsCloseTo;}
	inline int GetNetNum(){return CurrentNetPoint;}
private://��Ա����
	void readGPSDataFile(CString path);
	void readRoadNetFile(CString path);
	BOOL PathExtend();
	RoadPoint GetRandomPoint(RoadPoint p);
	DWORD FindNearestPoint(RoadPoint random,DWORD n);
	RoadPoint* GeneratePath(DWORD nearest,RoadPoint end);
	BOOL flag_BoundaryObstacle(RoadPoint Up[],RoadPoint Down[],LINESEG u,RoadPoint paths[],int pointnum);//����1��ʾ����ͨ��������0��ʾû��ͨ������
	void findUltipath(NodePoint *nodes,RoadPoint *clothoid,DWORD finalI);
	void AddNodes(RoadPoint *path,int i);//���ڵ��в���ڵ�
	BOOL flag_PointsInObsGrid(RoadPoint *pointArr,DWORD length);//�����ϰ�����
	int flag_SegPointsInObsGrid(BOOL isGussian,RoadPoint *pointArr,int beginIndex,DWORD length);
	BOOL flag_TargetonobsGrid(RoadPoint tar); //�ж�Ŀ��㸽����û���ϰ���
	int GetCurrentPosition(int currentposi);
	int GetCloseTrackPoint(GpsLine pt);
	void initializepath();
	double AngleTransfer(double angle);

	BOOL PathExtendSpecial();

	BOOL PathExtendHalfGrid();
	void findUltipathHalfGrid(NodePoint *nodes,RoadPoint *clothoid,DWORD finalI);
	RoadPoint m_ultiPath_HalfGrid[CLOTHOIDPOINTNUM+POINTNUM];//��ֲ�����Ĺ滮·����20��*20������
	RoadPoint m_ultiPath_HalfGrid_Gussian[CLOTHOIDPOINTNUM+POINTNUM];//���˹����Ĺ滮·����20��*20������
	BOOL flag_path_HalfGrid;

	BOOL PathExtendWithoutObs();


	///////////������ʻ�㷨����///////////////////////////////

private:
	int obs_grid[GRID_I*GRID_J];
	unsigned char zj_grid[10000];
	int tarI;  //��¼һ����ͨ�е�Ŀ���
	int tarJ;

	int TT1,TT2,TT3,TT4,TT5,TT6,TT7,TT8,XXX,XXX1,XXX2,XXX3,XXX4,XXX5,XXX6,XXX7,XXX8,XXX9,XXX10,XXX11,XXX12,XXX13,XXX14,XXX15,XXX16;
	RoadPoint RR1,RR2,RR3,RR4,RR5,RR6;
	/////////////////////////////////////
	int VeloAccCount;//������ӵĴ���
	vector<VeloGrid> VeloAccPoints;//�����������

	int OffsetVal;//�켣ƫ����


	RoadPoint grid0;//����(0,0)������꣨��˹��
	RoadPoint GridLeftDown,GridRightDown,GridLeftUp,GridRightUp;//�����Ķ���
	int flag_Intersect;//��ʼ�����һ·���������������4���ཻ�����1�������񶥲��ཻ��2������߽��ཻ��3�����ұ߽��ཻ��4����һ·������������,�����ߵ�ֱ�߻��붥���ཻ��5��������߽磻6�������ұ߽�
	GridPoint OriginStart,OriginEnd;//���ڿ�ͨ�е��жϵ������յ���������㣬X,Y��������к��б��

	int PJDISTANCE;//·��ƴ�Ӿ���
	
	//////////////////////////////////////

	void VeloAccumulation();//�������ݷ�������


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


	//�̺߳����Ĳ���
public:
	void CreateThread();	//�����̣߳�suspended״̬
	void StartThread();		//��ʼ�̣߳�����߳�δ�������򴴽���Ȼ������Ϊ����״̬
	void SuspendThread(); //��ͣ�߳�
	void ResumeThread(); //�ָ��߳�
	void StopThread(); //ֹͣ�߳�
	udpReceiveMsg m_recvMsg;
private:
	THREADMODE m_threadMode;
	BOOL		m_bStopThread;
	CWinThread	*m_pThreadHandle;
	static UINT PlanThreadPro(LPVOID lpParam); //�̺߳���



	/************************************************************************/
	/* ���˺�clothoid���֣��Լ�һЩ�ⲿӦ�õ�static����                                                                     */
	/************************************************************************/

public:
	static 	double AngleNormalnize(double phi);
	static	double MiddlePointPlan(double firMidPoint_x, double firMidPoint_y,double current_position_x,double
		current_position_y,double angle_sensor);//PID�㷨	
	static	short MiddlePiont_Shan(double firMidPoint_x,double firMidPoint_y);


	/************************************************************************/
	/*Clothoid�㷨����                                                    */
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
	/* �����ж�                                                                     */
	/************************************************************************/	
	//	obstacle GenerateObstacle(double LUx,double LUy,double RDx,double RDy);//�����ϰ���

	bool InsideConvexPolygon(int vcount,RoadPoint polygon[],RoadPoint q);//�������ι�ϵ�ж�
	bool CurveInsects(RoadPoint Up[],RoadPoint Down[],LINESEG u);//
	bool intersect(LINESEG u,LINESEG v);//ֱ���ཻ�㷨
	double Distance(RoadPoint m1,RoadPoint m2);  //ŷ����þ���
	double cosangle(RoadPoint m1,RoadPoint m2,RoadPoint m3); //��������
	double multiply(RoadPoint sp,RoadPoint ep,RoadPoint op) ;
	int sign(double m);
	RoadPoint getIntersectPoint(RoadPoint u1,RoadPoint u2,RoadPoint v1,RoadPoint v2);//�������߶εĽ���
	double PointToSegDist(double x, double y, double x1, double y1, double x2, double y2);//��㣨x,y�����߶εľ���
	void makeline(RoadPoint p1,RoadPoint p2,double &a,double &b,double &c);//��ֱ֪�������㣬��ֱ�߷���ax+by+c=0������a,b,c
	bool Line_Seg_Intersect(double x1,double y1,double x2,double y2,double a,double b,double c);//�ж��߶κ�ֱ���Ƿ��ཻ
	bool LineIntersection(double a1,double b1,double c1,double a2,double b2,double c2,RoadPoint &p);//��ֱ�ߺ�ֱ�ߵĽ���
	bool Angle_Bisector_Intersect(RoadPoint p1,RoadPoint p2,RoadPoint p3,RoadPoint &p);//�������ν�ƽ���ߺͶԱߵĽ���(����p1���ڵĽǣ�
};