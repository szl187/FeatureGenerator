#pragma once
#include<map>
#include"flow.h"
#include <cstring>
#include<algorithm>
extern int j, time_i, allflow,Sort,onlyallflow;
class flow_process
{	
public:
	std::map <struct flow_key, flow>flows;
	std::vector<struct flow_feature> flow_features;
	struct simple_packet_info simple_packet_info;	
	~flow_process();
	all_flow * af;
	flow_process() { af = NULL; };
	
	void on_packet_received();
};




