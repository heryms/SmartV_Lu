#pragma once


// CMapControl 对话框
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
	CMapControl(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMapControl();

// 对话框数据
	enum { IDD = IDD_DIALOG_FULLMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	vector<MapPoint> m_vectorMap;
	vector<MapPoint> m_vectorMark;


};
