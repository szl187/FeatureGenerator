#pragma once
#include<map>
#include"flow.h"
#include <cstring>
#include<algorithm>
extern int j;
extern double time_i;
extern std::ofstream out1;

class flow_process
{	
public:
	std::map <struct flow_key, flow>flows;
	std::vector<struct flow_feature> flow_features;
	struct simple_packet_info *simple_packet_info;	//10.10 将s_p_i改为指针存储，每个包处理结束后释放并重新分配，避免一些bug


	~flow_process();

	
	
	void on_packet_received();
};



