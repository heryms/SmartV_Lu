#pragma once
// OpenGL ͷ�ļ�

#include <vector>
#include <map>
using namespace std;
enum ButtonType
{
	leftDown=0,
	rightDown=1,
	other=2

};

typedef void(*point_layer)(const vector<PointShow> ptcloud);
typedef void(*line_layer) (const vector<ckPloyLineS>linecloud);
typedef void (*mission_layer)(const vector<ckMissionPoint>missionCloud);
class CDataShow
{
public:
	CDataShow(void);
	~CDataShow(void);
public:
	BOOL m_bIsCreate;
	BOOL m_bSnapScreen;
	BOOL m_bDrawPoints;
	float camRot[3];
	float camPos[3];
	float sceneRot[3];
	float scenePos[3];
	float vieeDis[2];
	float eyePos[3];
	float eyeCenter[3];
	float eyeUp[3];
	double m_lfRange;
	BOOL mouserightdown;
	BOOL mouseleftdown;
	CPoint mouseprevpoint;
	
	int m_GLPixelIndex;

	float x_move,y_move;		//��¼ģ��ƽ��
	float x_rotate,y_rotate,z_rotate;  //��¼ģ����ת
	float m_zoom;				//���ű���
	float x_rotate_save,y_rotate_save,z_rotate_save; 
	float x_move_save,y_move_save;
	int x_lbefore,y_lbefore;          //��¼��갴��ʱ������λ��
	int x_rbefore,y_rbefore;
	int z_before1,z_before2;
	int winX,winY;
	int m_NumPixel;
	int snapid;
public:	
	// ��ͼ����
	HGLRC m_hGLContext;
	void InitialScene( CPaintDC& dc,HDC hDC);
	BOOL SetWindowPixelFormat(HDC hDC);
	BOOL CreateViewGLContext(HDC hDC);
     int OnCreate(HWND hWnd);
	
	void SetOneViewFocus(int index); // �����ӽ�
	void OnLButtonDown(UINT nFlags, CPoint point);
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnRButtonDown(UINT nFlags, CPoint point);
    void OnRButtonUp(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	int m_nButtonFlag;
	// ����
	void Draw_grid (int meters_per_grid);
	void Draw_compass(double x,double y,double z);
	void Draw_Road_Center_Point();
	void Draw_coordinate();
	void Draw_current_vehicle(float x, float y , float tht);
	void OnSize(UINT nType, int cx, int cy);
	//��ͼ�ص�����
private:
	//DrawPoint m_fpointFunc;
    map<string,point_layer>  m_layersSensor;
	map<string,vector<PointShow>> m_sensorBuffer;
	map<string,line_layer>  m_layersMapLine;
	map<string,vector<ckPloyLineS>> m_maplineBuffer;
	map<string ,mission_layer> m_layersMission;
	map<string,vector<ckMissionPoint>> m_missionBuffer;
//ͼ�����
private:
	map<string,bool> m_layerStatus;
public:
	inline void SetLayerStatus(string layer,bool status)
	{
		map<string,bool>::iterator it=m_layerStatus.find(layer);
		it->second=status;
	}
public:
	void CreatePointLayer(string name,point_layer layer);
	BOOL UpdatePointLayer(string name,const vector<PointShow> ptcloud);
	void CreateLineLayer(string name,line_layer layer);
	BOOL UpdateLineLayer(string name,const vector<ckPloyLineS> linecloud);
	void CreateMissionLayer(string name,mission_layer layer);
	BOOL UpdateMissionLayer(string name,const vector<ckMissionPoint> missionCloud);
public:
	//int SetPointCallback(DrawPoint dpFunc){ m_fpointFunc=dpFunc;};
	void SetSceneRot(int axis,int value,BOOL increment,BOOL apply);
	void SetCamPos(int axis,int value,BOOL increment,BOOL apply);

	
	void SetView(float f);
	void Draw_circle(int radious, int meters_per_difference);
};


