#pragma once
#include "stdafx.h"
#include <string>
#include <process.h>
#include "lcm/lcm-cpp.hpp"
#include"lcm/velodynegrid_t.hpp"
#pragma comment(lib,"lib/lcm.lib")
using namespace std;
using namespace ckLcmType;
class LcmVeloHandler 
{
private:
	typedef void (*msg_output)(const velodynegrid_t *msg);
	msg_output _rcvMsg; //callback function for output data
	string _channel  ;  //channel  id
	lcm::LCM *_lcm;
public:

	~LcmVeloHandler() {
		delete _lcm;
	}
	LcmVeloHandler()
	{
		_channel=" ";
		_rcvMsg=NULL;
		_lcm=NULL;
	}

	void handleMessage(const lcm::ReceiveBuffer* rbuf,
		const std::string& chan, 
		const velodynegrid_t * msg)
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
	void sendLcm(velodynegrid_t* lane)
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
		_lcm->subscribe(channel, &LcmVeloHandler::handleMessage, this);
		return 1;
	}

	int grab()
	{
		//_beginthread(listen,0,this);
		 int res=_lcm->handle();
		 return res;
	}
};
