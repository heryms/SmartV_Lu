/************************************************************************/
/*  
   �����Զ���ͼ���ļ�
   �ļ�����Ϊ���ݵ�·��ʻҪ������Զ���
   ���Ʋ��ԡ��ض������Լ�����ʽ
   �Գ���һ��ʱ���ڵ�����״̬���б����ͳ��
*/
/************************************************************************/
#pragma once
#include "ckMapBaseObject.h"
class CMarkLayer:public ckMapBaseObject
{
private:
	//��¼��ǰ��ǵ�λ
	vector<ckMarkPoint> m_MapMarkArray;
	vector<ckMarkPoint> m_MarkLine; 
public:
	CMarkLayer(void);
	~CMarkLayer(void);
public:
	BOOL LoadMarkPoint(CString path);
};

