/************************************************************************/
/* 读取任务文件，完成各种指定场景动作                                                                     */
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

