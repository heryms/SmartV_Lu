#include "StdAfx.h"
#include "Laser_RoadEdge.h"


#define lastnum 500

float temp1 = 1;//稳定算法的迭代的分母
int flag_lastpoint = 0;//标记稳定算法是对10m线（0）还是6m线（1）
CLaser_RoadEdge::CLaser_RoadEdge(void)
{
}


CLaser_RoadEdge::~CLaser_RoadEdge(void)
{
}

UINT CLaser_RoadEdge::SingleRoadSerach(int Num,sPoint*pPoint,RoadParam param,sPoint&point_S ,sPoint& point_E)
{

	int indicator;  // 指示量
	indicator = -1;

    MeanFilter(Num,pPoint,5);

	/////////////////////////////////////////////////
	//	计算扫描线的直线段并保存下来
	sPoint temp_S[10],temp_E[10]; // 备选线段
	int option_num = 0;        // 备选线段数
	int abc = 0;
	float stdError=0;     //根据不同距离模式选择路面检测判断限差
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
		if(pPoint[i].r > distMax)//激光器最远距离是80米
		{
			if(indicator != -1)
			{
				point_E = pPoint[i - 1];      // 第i个点不满足要求，所以取前一个点
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
		if(pPoint[i].x > rwidth)//在车的左右8米范围内探测
		{
			indicator = -1;
			continue;
		}
		else if(pPoint[i].x < lwidth  && indicator != -1)
		{
			point_E = pPoint[i - 1];      // 第i个点不满足要求，所以去前一个点
			if(point_S.x - point_E.x > roadwidth)//保证线段的长度，线段起始点和终点的x之差>2.5，车宽2米，所以道路至少2.5米
			{
				temp_S[option_num] = point_S;
				temp_E[option_num] = point_E;
				option_num++;
			}
			break;//过了左方8米就不需要考虑此扫描线
		}
	    float point_r[512];
		for (int i=0;i<Num;i++)
		{
			point_r[i]=pPoint[i].r;
		}
		float std = fabs(cal_std(i - 2,4,point_r)/(pPoint[i - 2].x - pPoint[i + 2].x));//自适应标准差，距离越远点越稀疏，排除距离对标准差的干扰

		if(std < stdError )//std 即邻近的9个点的方差，我们取最靠中间的点，0.025经验值，太大探测不出路缘，太小则路面凹凸也会误探
		{
			// 左边边界
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
				point_E = pPoint[i - 1];      // 第i个点不满足要求，所以去前一个点
				if(point_S.x - point_E.x > 2.5)//同时要保证两点间大于2.5，才列为备选线段
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
	//  找出最优线段，即为路面，point_S,point_E为返回值	
	Road_SearchOptimal(option_num,pPoint,temp_S,temp_E,point_S,point_E);
	return 1;

}

void CLaser_RoadEdge::Road_SearchOptimal(int option_num,sPoint pPoint[],
	sPoint temppoint_S[],sPoint temppoint_E[],sPoint &point_S,sPoint &point_E)
{
	int indicator = 0;

	//harry
	//struct sPoint last_S[lastnum],last_E[lastnum];

	if(option_num == 1)//只有一个备选线段
	{
		point_S = temppoint_S[0];
		point_E = temppoint_E[0];

		if (flag_lastpoint == 0)
		{
			point_E.x = point_E.x/(float)temp1+lastpoint_E_10.x*(temp1-1)/(float)temp1;
			point_E.y = point_E.y/(float)temp1+lastpoint_E_10.y*(temp1-1)/(float)temp1;
			point_S.x = point_S.x/(float)temp1+lastpoint_S_10.x*(temp1-1)/(float)temp1;
			point_S.y = point_S.y/(float)temp1+lastpoint_S_10.y*(temp1-1)/(float)temp1;
			//r和其他参数没有修改

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
			//r和其他参数没有修改

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
		if(temppoint_S[i].x > 2.0 && temppoint_E[i].x < -2.0) //中间只要有4米的距离，就从中间走（优先）
		{
			point_S = temppoint_S[i];
			point_E = temppoint_E[i];

			if (flag_lastpoint == 0)
			{
				point_E.x = point_E.x/(float)temp1+lastpoint_E_10.x*(temp1-1)/(float)temp1;
				point_E.y = point_E.y/(float)temp1+lastpoint_E_10.y*(temp1-1)/(float)temp1;
				point_S.x = point_S.x/(float)temp1+lastpoint_S_10.x*(temp1-1)/(float)temp1;
				point_S.y = point_S.y/(float)temp1+lastpoint_S_10.y*(temp1-1)/(float)temp1;
				//r和其他参数没有修改

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
				//r和其他参数没有修改

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
		float averageheight[10];//探测处的路面的平均高度
		int point_num[10];
		sPoint mid_point[10];
		float evaluate_index[10];//判断线段是路面的评价指标
		float RoadWidth[10];
		float max_index = 0.0;//最大评价指标
		float min_dis = 20.0;//备选路段的中间点到车头的最小距离,要求最小值则初值设的大一些，要设最大值则初值设的小一些。20自己定
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

		for(int j = 0; j < option_num;j++)//计算各个备选线段中符合要求的点的高程和
		{
			for(int i = 0; i < 401;i++)
			{
				if(pPoint[i].objectID > temppoint_S[j].objectID //保证i点在线段j内
					&& pPoint[i].objectID < temppoint_E[j].objectID
					&& pPoint[i].r < 80)
				{
					averageheight[j] += pPoint[i].z;
				}
			}

		}
		for(int i = 0;i < option_num;i++)//求出每个备选线段的均高、宽度、中点和所有线段的最小高程和最短距离
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
			float heightratio = exp(-fabs(min_height-averageheight[i]));//高程权值归一化把全都归为（0-1）
			//float distanceratio = exp(-fabs(Distance(&mid_point[i],&Origin_point) - min_dis));
		//	evaluate_index[i] = calc_evaluation_index(heightratio,distanceratio,RoadWidth[i]);//整段路线的权值
			if(evaluate_index[i] > max_index)//选择总权最大的为路面
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
		//r和其他参数没有修改

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
		//r和其他参数没有修改

		temp1++;
		if (temp1>=lastnum)
		{
			temp1=lastnum;
		}
		lastpoint_S_6 = point_S;
		lastpoint_E_6 = point_E;
	}
}
