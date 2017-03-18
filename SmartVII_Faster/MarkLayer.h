/************************************************************************/
/*  
   构造自定义图层文件
   文件属性为根据道路行驶要求进行自定义
   控制策略、特定过程以及处理方式
   对车辆一段时间内的运行状态进行保存和统计
*/
/************************************************************************/
#pragma once
#include "ckMapBaseObject.h"
class CMarkLayer:public ckMapBaseObject
{
private:
	//记录当前标记点位
	vector<ckMarkPoint> m_MapMarkArray;
	vector<ckMarkPoint> m_MarkLine; 
public:
	CMarkLayer(void);
	~CMarkLayer(void);
public:
	BOOL LoadMarkPoint(CString path);
};

