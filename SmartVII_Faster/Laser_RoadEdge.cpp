#include "StdAfx.h"
#include "Laser_RoadEdge.h"


#define lastnum 500

float temp1 = 1;//�ȶ��㷨�ĵ����ķ�ĸ
int flag_lastpoint = 0;//����ȶ��㷨�Ƕ�10m�ߣ�0������6m�ߣ�1��
CLaser_RoadEdge::CLaser_RoadEdge(void)
{
}


CLaser_RoadEdge::~CLaser_RoadEdge(void)
{
}

UINT CLaser_RoadEdge::SingleRoadSerach(int Num,sPoint*pPoint,RoadParam param,sPoint&point_S ,sPoint& point_E)
{

	int indicator;  // ָʾ��
	indicator = -1;

    MeanFilter(Num,pPoint,5);

	/////////////////////////////////////////////////
	//	����ɨ���ߵ�ֱ�߶β���������
	sPoint temp_S[10],temp_E[10]; // ��ѡ�߶�
	int option_num = 0;        // ��ѡ�߶���
	int abc = 0;
	float stdError=0;     //���ݲ�ͬ����ģʽѡ��·�����ж��޲�
	float distMax=0;
	float distMin=0;
	stdError=param.stdErr;
	distMin=param.disMin;
	distMax=param.disMax;
	int edge=param.nearPoint;
	float roadwidth=param.width;
	float lwidth=param.leftWidth;
	float rwidth=param.rightWidth;

	for(int i = edge;i < Num-edge; i++)
	{
		if(pPoint[i].r > distMax)//��������Զ������80��
		{
			if(indicator != -1)
			{
				point_E = pPoint[i - 1];      // ��i���㲻����Ҫ������ȡǰһ����
				if(point_S.x - point_E.x > roadwidth)
				{
					temp_S[option_num] = point_S;
					temp_E[option_num] = point_E;
					option_num++;
				}
			}
			indicator = -1;
			continue;
		}
		if(pPoint[i].x > rwidth)//�ڳ�������8�׷�Χ��̽��
		{
			indicator = -1;
			continue;
		}
		else if(pPoint[i].x < lwidth  && indicator != -1)
		{
			point_E = pPoint[i - 1];      // ��i���㲻����Ҫ������ȥǰһ����
			if(point_S.x - point_E.x > roadwidth)//��֤�߶εĳ��ȣ��߶���ʼ����յ��x֮��>2.5������2�ף����Ե�·����2.5��
			{
				temp_S[option_num] = point_S;
				temp_E[option_num] = point_E;
				option_num++;
			}
			break;//������8�׾Ͳ���Ҫ���Ǵ�ɨ����
		}
	    float point_r[512];
		for (int i=0;i<Num;i++)
		{
			point_r[i]=pPoint[i].r;
		}
		float std = fabs(cal_std(i - 2,4,point_r)/(pPoint[i - 2].x - pPoint[i + 2].x));//����Ӧ��׼�����ԽԶ��Խϡ�裬�ų�����Ա�׼��ĸ���

		if(std < stdError )//std ���ڽ���9����ķ������ȡ��м�ĵ㣬0.025����ֵ��̫��̽�ⲻ��·Ե��̫С��·�氼͹Ҳ����̽
		{
			// ��߽߱�
			indicator++;
			if(indicator == 0)
			{
				point_S = pPoint[i];
				abc++;
			}
		}
		else
		{
			if(indicator != -1)
			{
				point_E = pPoint[i - 1];      // ��i���㲻����Ҫ������ȥǰһ����
				if(point_S.x - point_E.x > 2.5)//ͬʱҪ��֤��������2.5������Ϊ��ѡ�߶�
				{
					temp_S[option_num] = point_S;
					temp_E[option_num] = point_E;
					option_num++;
					indicator = -1;
				}
				else
					indicator = -1;
			}
		}

	}

	if(option_num == 0)
		return 0;
	//  �ҳ������߶Σ���Ϊ·�棬point_S,point_EΪ����ֵ	
	Road_SearchOptimal(option_num,pPoint,temp_S,temp_E,point_S,point_E);
	return 1;

}

void CLaser_RoadEdge::Road_SearchOptimal(int option_num,sPoint pPoint[],
	sPoint temppoint_S[],sPoint temppoint_E[],sPoint &point_S,sPoint &point_E)
{
	int indicator = 0;

	//harry
	//struct sPoint last_S[lastnum],last_E[lastnum];

	if(option_num == 1)//ֻ��һ����ѡ�߶�
	{
		point_S = temppoint_S[0];
		point_E = temppoint_E[0];

		if (flag_lastpoint == 0)
		{
			point_E.x = point_E.x/(float)temp1+lastpoint_E_10.x*(temp1-1)/(float)temp1;
			point_E.y = point_E.y/(float)temp1+lastpoint_E_10.y*(temp1-1)/(float)temp1;
			point_S.x = point_S.x/(float)temp1+lastpoint_S_10.x*(temp1-1)/(float)temp1;
			point_S.y = point_S.y/(float)temp1+lastpoint_S_10.y*(temp1-1)/(float)temp1;
			//r����������û���޸�

			temp1++;
			if (temp1>=lastnum)
			{
				temp1=lastnum;
			}
			lastpoint_S_10 = point_S;
			lastpoint_E_10 = point_E;
		}
		if (flag_lastpoint == 1)
		{
			point_E.x = point_E.x/(float)temp1+lastpoint_E_6.x*(temp1-1)/(float)temp1;
			point_E.y = point_E.y/(float)temp1+lastpoint_E_6.y*(temp1-1)/(float)temp1;
			point_S.x = point_S.x/(float)temp1+lastpoint_S_6.x*(temp1-1)/(float)temp1;
			point_S.y = point_S.y/(float)temp1+lastpoint_S_6.y*(temp1-1)/(float)temp1;
			//r����������û���޸�

			temp1++;
			if (temp1>=lastnum)
			{
				temp1=lastnum;
			}
			lastpoint_S_6 = point_S;
			lastpoint_E_6 = point_E;
		}
		return;
	}
	for(int i = 0;i < option_num;i++)
	{
		if(temppoint_S[i].x > 2.0 && temppoint_E[i].x < -2.0) //�м�ֻҪ��4�׵ľ��룬�ʹ��м��ߣ����ȣ�
		{
			point_S = temppoint_S[i];
			point_E = temppoint_E[i];

			if (flag_lastpoint == 0)
			{
				point_E.x = point_E.x/(float)temp1+lastpoint_E_10.x*(temp1-1)/(float)temp1;
				point_E.y = point_E.y/(float)temp1+lastpoint_E_10.y*(temp1-1)/(float)temp1;
				point_S.x = point_S.x/(float)temp1+lastpoint_S_10.x*(temp1-1)/(float)temp1;
				point_S.y = point_S.y/(float)temp1+lastpoint_S_10.y*(temp1-1)/(float)temp1;
				//r����������û���޸�

				temp1++;
				if (temp1>=lastnum)
				{
					temp1=lastnum;
				}
				lastpoint_S_10 = point_S;
				lastpoint_E_10 = point_E;
			}
			if (flag_lastpoint == 1)
			{
				point_E.x = point_E.x/(float)temp1+lastpoint_E_6.x*(temp1-1)/(float)temp1;
				point_E.y = point_E.y/(float)temp1+lastpoint_E_6.y*(temp1-1)/(float)temp1;
				point_S.x = point_S.x/(float)temp1+lastpoint_S_6.x*(temp1-1)/(float)temp1;
				point_S.y = point_S.y/(float)temp1+lastpoint_S_6.y*(temp1-1)/(float)temp1;
				//r����������û���޸�

				temp1++;
				if (temp1>=lastnum)
				{
					temp1=lastnum;
				}
				lastpoint_S_6 = point_S;
				lastpoint_E_6 = point_E;
			}

			indicator = 1;
			break;
		}
		else continue;
	}
	if(indicator == 1)
		return;
	else
	{
		float averageheight[10];//̽�⴦��·���ƽ���߶�
		int point_num[10];
		sPoint mid_point[10];
		float evaluate_index[10];//�ж��߶���·�������ָ��
		float RoadWidth[10];
		float max_index = 0.0;//�������ָ��
		float min_dis = 20.0;//��ѡ·�ε��м�㵽��ͷ����С����,Ҫ����Сֵ���ֵ��Ĵ�һЩ��Ҫ�����ֵ���ֵ���СһЩ��20�Լ���
		float min_height = 20.0;
		sPoint Origin_point;
		int optimal_section_no = -1;
		Origin_point.x = 0.0;
		Origin_point.y = 0.0;
		Origin_point.z = 0.0;
		for(int m = 0;m < option_num;m++)
		{
			averageheight[m] = 0.0;
		}

		for(int j = 0; j < option_num;j++)//���������ѡ�߶��з���Ҫ��ĵ�ĸ̺߳�
		{
			for(int i = 0; i < 401;i++)
			{
				if(pPoint[i].objectID > temppoint_S[j].objectID //��֤i�����߶�j��
					&& pPoint[i].objectID < temppoint_E[j].objectID
					&& pPoint[i].r < 80)
				{
					averageheight[j] += pPoint[i].z;
				}
			}

		}
		for(int i = 0;i < option_num;i++)//���ÿ����ѡ�߶εľ��ߡ���ȡ��е�������߶ε���С�̺߳���̾���
		{
			averageheight[i] /= (temppoint_E[i].objectID - temppoint_S[i].objectID);
			RoadWidth[i] = temppoint_S[i].x - temppoint_E[i].x;
			mid_point[i].x = (temppoint_S[i].x + temppoint_E[i].x)/2.0;
			mid_point[i].y = (temppoint_S[i].y + temppoint_E[i].y)/2.0;
			mid_point[i].z = (temppoint_S[i].z + temppoint_E[i].z)/2.0;
			if(averageheight[i] < min_height)
			{
				min_height = averageheight[i];
			}
		/*	if(Distance(&mid_point[i],&Origin_point) < min_dis)
			{
				min_dis = Distance(&mid_point[i],&Origin_point);
			}*/
		}

		for(int i = 0;i < option_num;i++)
		{
			float heightratio = exp(-fabs(min_height-averageheight[i]));//�߳�Ȩֵ��һ����ȫ����Ϊ��0-1��
			//float distanceratio = exp(-fabs(Distance(&mid_point[i],&Origin_point) - min_dis));
		//	evaluate_index[i] = calc_evaluation_index(heightratio,distanceratio,RoadWidth[i]);//����·�ߵ�Ȩֵ
			if(evaluate_index[i] > max_index)//ѡ����Ȩ����Ϊ·��
			{
				max_index = evaluate_index[i];
				optimal_section_no = i;
			}
		}
		point_S = temppoint_S[optimal_section_no];
		point_E = temppoint_E[optimal_section_no];
	}


	if (flag_lastpoint == 0)
	{
		point_E.x = point_E.x/(float)temp1+lastpoint_E_10.x*(temp1-1)/(float)temp1;
		point_E.y = point_E.y/(float)temp1+lastpoint_E_10.y*(temp1-1)/(float)temp1;
		point_S.x = point_S.x/(float)temp1+lastpoint_S_10.x*(temp1-1)/(float)temp1;
		point_S.y = point_S.y/(float)temp1+lastpoint_S_10.y*(temp1-1)/(float)temp1;
		//r����������û���޸�

		temp1++;
		if (temp1>=lastnum)
		{
			temp1=lastnum;
		}
		lastpoint_S_10 = point_S;
		lastpoint_E_10 = point_E;
	}
	if (flag_lastpoint == 1)
	{
		point_E.x = point_E.x/(float)temp1+lastpoint_E_6.x*(temp1-1)/(float)temp1;
		point_E.y = point_E.y/(float)temp1+lastpoint_E_6.y*(temp1-1)/(float)temp1;
		point_S.x = point_S.x/(float)temp1+lastpoint_S_6.x*(temp1-1)/(float)temp1;
		point_S.y = point_S.y/(float)temp1+lastpoint_S_6.y*(temp1-1)/(float)temp1;
		//r����������û���޸�

		temp1++;
		if (temp1>=lastnum)
		{
			temp1=lastnum;
		}
		lastpoint_S_6 = point_S;
		lastpoint_E_6 = point_E;
	}
}
