#pragma once
#include<map>
#include"flow.h"
#include <cstring>


class flow_process
{	
public:
	std::map <struct flow_key, flow>flows;
	struct simple_packet_info simple_packet_info;	
	flow_process();

	void on_packet_received();
};

