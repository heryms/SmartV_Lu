/************************************************************************/
/* ��ȡ�����ļ�����ɸ���ָ����������                                                                     */
/************************************************************************/
#pragma once


class CMissionLayer
{

private:
	vector<ckMissionPoint> m_vectorMission;
public:
	CMissionLayer(void);
	~CMissionLayer(void);
public:
	UINT LoadMissionFile(CString path);
	void MissionMsg(EMISSIONTYPE e);
};

