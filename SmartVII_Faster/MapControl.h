#pragma once


// CMapControl �Ի���
typedef struct 
{
	float x;
	float y;
    int type;
}MapPoint;

class CMapControl : public CDialogEx
{
	DECLARE_DYNAMIC(CMapControl)

public:
	CMapControl(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMapControl();

// �Ի�������
	enum { IDD = IDD_DIALOG_FULLMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	vector<MapPoint> m_vectorMap;
	vector<MapPoint> m_vectorMark;


};
