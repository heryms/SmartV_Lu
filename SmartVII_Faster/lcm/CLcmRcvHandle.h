#if !defined _LCM_RCV_HANDLE_INCLUDE
#define _LCM_RCV_HANDLE_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma once
#include "stdafx.h"
#include <string>
#include <process.h>
#include"lcm/lcm-cpp.hpp"
//#pragma comment(lib,"lib/lcm.lib")
using namespace std;


template<typename DataType>

class LcmRcvHandler 
{
private:
	typedef void (*msg_output)(const DataType *msg);
	msg_output _rcvMsg; //callback function for output data
	string _channel  ;  //channel  id
	lcm::LCM _lcm;
public:
	~LcmRcvHandler() {}
	LcmRcvHandler()
	{
		_channel=" ";
		_rcvMsg=NULL;

	}
	void handleMessage(const lcm::ReceiveBuffer* rbuf,
		const std::string& chan, 
		const DataType * msg)
	{
	
		_rcvMsg(msg);

	}
	//0 lcm²»³É¹¦
	int initialLcm(string channel,msg_output rcv)
	{
		
		_rcvMsg=rcv;
		_channel=channel;
       if (!_lcm.good())
       {
		   return 0;
       }
	   _lcm.subscribe(channel, &LcmRcvHandler<DataType>::handleMessage, this);
	   return 1;
	}
	
	int grab()
	{
		//_beginthread(listen,0,this);
		return _lcm.handle();
	}
};
#endif