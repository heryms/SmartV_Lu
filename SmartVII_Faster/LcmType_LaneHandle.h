#pragma once
#include "stdafx.h"
#include <string>
#include <process.h>
#include "lcm/lcm-cpp.hpp"
#include"lcm/LaneInfo_t.hpp"
#pragma comment(lib,"lib/lcm.lib")
using namespace std;
using namespace ckLcmType;
class LcmLaneHandler 
{
private:
	typedef void (*msg_output)(const LaneInfo_t *msg);
	msg_output _rcvMsg; //callback function for output data
	string _channel  ;  //channel  id
	lcm::LCM *_lcm;
public:

	~LcmLaneHandler() {
		delete _lcm;
	}
	LcmLaneHandler()
	{
		_channel=" ";
		_rcvMsg=NULL;
		_lcm=NULL;
	}

	void handleMessage(const lcm::ReceiveBuffer* rbuf,
		const std::string& chan, 
		const LaneInfo_t * msg)
	{

		_rcvMsg(msg);

	}
	int intialSend(string net,string channel)
	{
		_channel=channel;
		_lcm=new lcm::LCM(net);
		if (!_lcm->good())
		{
			return 0;
		}
	}
	void sendLcm(LaneInfo_t* lane)
	{
		_lcm->publish(_channel,lane);
	}
	//0 lcm²»³É¹¦
	int initialLcm(string net, string channel,msg_output rcv)
	{		
		_rcvMsg=rcv;
		_channel=channel;
		_lcm=new lcm::LCM(net);
		if (!_lcm->good())
		{
			return 0;
		}
		_lcm->subscribe(channel, &LcmLaneHandler::handleMessage, this);
		return 1;
	}

	int grab()
	{
		//_beginthread(listen,0,this);
		return _lcm->handle();
	}
};
