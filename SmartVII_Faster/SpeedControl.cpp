// SpeedControl.cpp : 实现文件
//

#include "stdafx.h"
#include "SmartVII_Faster.h"
#include "SpeedControl.h"
#include "afxdialogex.h"


#include "stdafx.h"
#include "SmartVII_Faster.h"
#include "SpeedControl.h"
#include "afxdialogex.h"




#include<stdio.h>
#include<stdlib.h>
#define random(x) (rand()%x)
using namespace fl;


// CSpeedControl 对话框

IMPLEMENT_DYNAMIC(CSpeedControl, CDialogEx)

CSpeedControl::CSpeedControl()
{
	wantedspeed=0;
	tui_flag=false;
	BreakTime=0;
	ThrottleControl=0;
	con_flag=2;//0    执行刹车       1    执行油门
	ting=false;
}

CSpeedControl::~CSpeedControl()
{
}


// CSpeedControl 消息处理程序

void CSpeedControl::send_speedcontrol()
{

	while(true)
	{
		//double cur_speed=g_curSpeed;
		clock_t start,finish;
		start=clock();
		double cur_speed=g_CurSpeed/100.0;
		//double a=m_canCtl.m_nCarAngle;
		//int cur_speed=g_curSpeed;
			//m_canCtl.m_nCarSpeed;
		pre_speed=cur_speed;
		Sleep(200);
		cur_speed=g_CurSpeed/100.0;
		finish=clock();
		dif_v=(cur_speed-pre_speed);
		acc_v=(dif_v/3.6)/((finish-start)/1000.0);
		speederror=cur_speed-wantedspeed;
		shan_speedcontrol(wantedspeed,cur_speed,speederror,acc_v);
		//m_canCtl.SendBrakeEx(0);
		if(con_flag==0)
		{
			m_canCtl.SendSpeedEx(0);
			m_canCtl.SendBrakeEx(BreakTime);//0-
		if(con_flag==1)
		{
			m_canCtl.SendBrakeEx(0);
			m_canCtl.SendSpeedEx(ThrottleControl);//0-255
		}
		//Sleep(100);
		if(tui_flag)
		{
			break;
		}
		}
	}
}

void CSpeedControl::shan_speedcontrol(double WantedSpeed,double CurrentSpeed,double CurrentSpeedError,double CurrentAcceleration)
{

	if (CurrentAcceleration<-1)
	{
		CurrentAcceleration=-0.999;
	}
	if (CurrentAcceleration>1)
	{
		CurrentAcceleration=0.999;
	}
	if (CurrentSpeedError>15)
	{
		CurrentSpeedError=14.999;
	}
	if (CurrentSpeedError<-15)
	{
		CurrentSpeedError=-14.999;
	}
	if (CurrentSpeedError>0)
	{
		fl::Engine* engine = new fl::Engine;
		engine->setName("BreakControl");

		fl::InputVariable* inputVariable1 = new fl::InputVariable;
		inputVariable1->setEnabled(true);
		inputVariable1->setName("CurrentSpeedError");
		inputVariable1->setRange(-20.000, 40.000);
		inputVariable1->addTerm(new fl::Trapezoid("NegSmall", -15.000, -15.000, -2.000, 0.000));
		inputVariable1->addTerm(new fl::Trapezoid("Null", -5.000, -2.000, 2.000, 5.000));
		inputVariable1->addTerm(new fl::Trapezoid("PosSmall", 0.000, 2.000, 5.000, 7.000));
		inputVariable1->addTerm(new fl::Trapezoid("PosLarge", 5.000, 7.000, 15.000, 15.000));
		engine->addInputVariable(inputVariable1);

		fl::InputVariable* inputVariable2 = new fl::InputVariable;
		inputVariable2->setEnabled(true);
		inputVariable2->setName("CurrentAcceleration");
		inputVariable2->setRange(-1.000, 1.000);
		inputVariable2->addTerm(new fl::Trapezoid("Null", -0.300, -0.200, 0.000, 0.100));
		inputVariable2->addTerm(new fl::Trapezoid("Negtive", -1.000, -1.000, -0.300, -0.200));
		inputVariable2->addTerm(new fl::Trapezoid("Pos", 0.000, 0.100, 1.000, 1.000));
		engine->addInputVariable(inputVariable2);

		fl::OutputVariable* outputVariable = new fl::OutputVariable;
		outputVariable->setEnabled(true);
		outputVariable->setName("Break");
		outputVariable->setRange(0.000, 150.000);
		outputVariable->fuzzyOutput()->setAccumulation(NULL);
		outputVariable->setDefuzzifier(new fl::WeightedAverage);
		outputVariable->setDefaultValue(fl::nan);
		//outputVariable->setLockValidOutput(false);
		//outputVariable->setLockOutputRange(false);
		outputVariable->addTerm(new fl::Constant("t00", 0.000));
		outputVariable->addTerm(new fl::Constant("t01", 40.000));
		outputVariable->addTerm(new fl::Constant("t02", 60.000));
		outputVariable->addTerm(new fl::Constant("t03", 70.000));
		engine->addOutputVariable(outputVariable);

		fl::RuleBlock* ruleBlock = new fl::RuleBlock;
		ruleBlock->setEnabled(true);
		ruleBlock->setName("");
		ruleBlock->setConjunction(new fl::Minimum);
		ruleBlock->setDisjunction(new fl::Maximum);
		ruleBlock->setActivation(new fl::Minimum);
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is NegSmall and CurrentAcceleration is Null then Break is t00", engine));
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is Null and CurrentAcceleration is Null then Break is t01", engine));
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is Null and CurrentAcceleration is Negtive then Break is t00", engine));
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is Null and CurrentAcceleration is Pos then Break is t01", engine));
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is PosSmall and CurrentAcceleration is Null then Break is t01", engine));
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is PosSmall and CurrentAcceleration is Negtive then Break is t00", engine));
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is PosSmall and CurrentAcceleration is Pos then Break is t01", engine));
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is PosLarge and CurrentAcceleration is Null then Break is t02", engine));
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is PosLarge and CurrentAcceleration is Negtive then Break is t01", engine));
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is PosLarge and CurrentAcceleration is Pos then Break is t03", engine));
		engine->addRuleBlock(ruleBlock);

		std::string status;
		if (not engine->isReady(&status))
			throw Exception("Engine not ready. "
			"The following errors were encountered:\n" + status, FL_AT);
		inputVariable1->setInputValue(CurrentSpeedError);
		inputVariable2->setInputValue(CurrentAcceleration);

		engine->process();
		BreakTime=outputVariable->getOutputValue();
		con_flag=0;
	}
	//油门控制
	else

	{
		fl::Engine* engine = new fl::Engine;
		engine->setName("throttlecontrol");

		fl::InputVariable* inputVariable1 = new fl::InputVariable;
		inputVariable1->setEnabled(true);
		inputVariable1->setName("CurrentSpeedError");
		inputVariable1->setRange(-20.000, 40.000);
		inputVariable1->addTerm(new fl::Trapezoid("NegSmall", -7.000, -5.000, -2.000, 0.000));
		inputVariable1->addTerm(new fl::Trapezoid("Null", -5.000, -2.000, 2.000, 5.000));
		inputVariable1->addTerm(new fl::Trapezoid("PosSmall", 0.000, 2.000, 15.000, 15.000));
		inputVariable1->addTerm(new fl::Trapezoid("NegLarge", -15.000, -15.000, -7.000, -5.000));
		engine->addInputVariable(inputVariable1);

		fl::InputVariable* inputVariable2 = new fl::InputVariable;
		inputVariable2->setEnabled(true);
		inputVariable2->setName("CurrentAcceleration");
		inputVariable2->setRange(-1.000, 1.000);
		inputVariable2->addTerm(new fl::Trapezoid("Null", -0.300, -0.200, 0.000, 0.100));
		inputVariable2->addTerm(new fl::Trapezoid("Negtive", -1.000, -1.000, -0.300, -0.200));
		inputVariable2->addTerm(new fl::Trapezoid("Pos", 0.000, 0.100, 1.000, 1.000));
		engine->addInputVariable(inputVariable2);

		fl::OutputVariable* outputVariable = new fl::OutputVariable;
		outputVariable->setEnabled(true);
		outputVariable->setName("Throttle");
		outputVariable->setRange(0.000, 150.000);
		outputVariable->fuzzyOutput()->setAccumulation(NULL);
		outputVariable->setDefuzzifier(new fl::WeightedAverage);
		outputVariable->setDefaultValue(fl::nan);
		//outputVariable->setLockValidOutput(false);
		//outputVariable->setLockOutputRange(false);
		outputVariable->addTerm(new fl::Constant("t00", 0.000));
		outputVariable->addTerm(new fl::Constant("t01", 150.000));
		outputVariable->addTerm(new fl::Constant("t02", 170.000));
		outputVariable->addTerm(new fl::Constant("t03", 220.000));
		engine->addOutputVariable(outputVariable);

		fl::RuleBlock* ruleBlock = new fl::RuleBlock;
		ruleBlock->setEnabled(true);
		ruleBlock->setName("");
		ruleBlock->setConjunction(new fl::Minimum);
		ruleBlock->setDisjunction(new fl::Maximum);
		ruleBlock->setActivation(new fl::Minimum);
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is NegSmall and CurrentAcceleration is Null then Throttle is t01", engine));
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is NegSmall and CurrentAcceleration is Negtive then Throttle is t02", engine));
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is NegSmall and CurrentAcceleration is Pos then Throttle is t01", engine));
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is Null and CurrentAcceleration is Null then Throttle is t00", engine));
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is Null and CurrentAcceleration is Negtive then Throttle is t01", engine));
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is Null and CurrentAcceleration is Pos then Throttle is t01", engine));
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is PosSmall then Throttle is t00", engine));
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is NegLarge and CurrentAcceleration is Null then Throttle is t02", engine));
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is NegLarge and CurrentAcceleration is Negtive then Throttle is t03", engine));
		ruleBlock->addRule(fl::Rule::parse("if CurrentSpeedError is NegLarge and CurrentAcceleration is Pos then Throttle is t02", engine));
		engine->addRuleBlock(ruleBlock);
		std::string status;
		if (not engine->isReady(&status))
			throw Exception("Engine not ready. "
			"The following errors were encountered:\n" + status, FL_AT);
		inputVariable1->setInputValue(CurrentSpeedError);
		inputVariable2->setInputValue(CurrentAcceleration);

		engine->process();
		ThrottleControl=outputVariable->getOutputValue();
		con_flag=1;
}
}


#define  _SPEED_TEST_YW_ 0
void CSpeedControl::ywSpeedControl(int tag_speed)
{

	clock_t start,finish;
	start=clock();
	double cur_speed=g_CurSpeed/100.0;
	double wanna_speed=tag_speed/100.0;

	pre_speed=cur_speed;
	Sleep(200);
	cur_speed=g_CurSpeed/100.0;
	finish=clock();
	dif_v=(cur_speed-pre_speed);
	acc_v=(dif_v/3.6)/((finish-start)/1000.0);
	speederror=cur_speed-wanna_speed;
	if ((tag_speed<=0)||(tag_speed>=70))
	{
		m_canCtl.SendSpeedEx(0);
		m_canCtl.SendBrakeEx(100);//0-100
	}
	else
	{
		shan_speedcontrol(wanna_speed,cur_speed,speederror,acc_v);
		if(con_flag==0)
		{
			m_canCtl.SendSpeedEx(0);
			m_canCtl.SendBrakeEx(BreakTime);//0-100
		}
		if(con_flag==1)
		{
			m_canCtl.SendBrakeEx(0);
			m_canCtl.SendSpeedEx(ThrottleControl);//0-255
		}
	}

}




