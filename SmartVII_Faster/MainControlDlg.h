#pragma once

// OpenGL ͷ�ļ�
#include "../GL/GL/gl.h"
#include "../GL/GL/glu.h"
#include "../GL/GL/GLAUX.H"

#pragma comment(lib,"../GL/lib/glu32.lib")
#pragma comment(lib,"../GL/lib/opengl32.lib")
#pragma comment(lib,"../GL/lib/glaux.lib")



// ��ʷ�ӽ�λ��
struct ViewFocusM
{
	float x_move,   y_move;	               //��¼ģ��ƽ��
	float m_zoom;                          //�Ŵ���
	float x_rotate,  y_rotate,  z_rotate;  //��¼ģ����ת
};
// CMainControlDlg �Ի���

class CMainControlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainControlDlg)

public:
	CMainControlDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMainControlDlg();

// �Ի�������
	enum { IDD = IDD_SMARTVII_FASTER_FORM };
	
	
	//	udpSendMsg m_udpMsg;
private:
	BOOL m_bSnapScreen;
	BOOL m_bDrawPoints;


	float m_eyex, m_eyey,  m_eyez; 
	float m_centerx,  m_centery, m_centerz; 
	float m_upx, m_upy, m_upz;

	HGLRC m_hGLContext;
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
	// ��ͼ����
	BOOL SetWindowPixelFormat(HDC hDC);
	BOOL CreateViewGLContext(HDC hDC);
	void RenderScene();
	void SetOneViewFocus(int index); // �����ӽ�
	// ����
	void Draw_grid (int meters_per_grid);
	void Draw_compass(double x,double y,double z);
	void Draw_Road_Center_Point();
	void Draw_current_vehicle(float x, float y , float tht);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

public:
	//afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnPaint();

	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
