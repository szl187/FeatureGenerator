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
	struct simple_packet_info *simple_packet_info;	//10.10 ��s_p_i��Ϊָ��洢��ÿ��������������ͷŲ����·��䣬����һЩbug


	~flow_process();

	
	
	void on_packet_received();
};



