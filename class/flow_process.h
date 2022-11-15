#pragma once
#include<map>
#include"flow.h"
#include <cstring>
#include<algorithm>
#include<numeric>
#include<iomanip>
extern int j, allflow,Sort,onlyallflow,portinfo,droplast,vari,app;
extern double time_i,d_num;
extern std::ofstream out1;


struct portinfo {
	double startts;
	unsigned short portsrc, portdst;
	double totallen;
	double applen;
};

class flow_process
{	
public:
	std::map <struct flow_key, flow>flows;
	std::vector<struct flow_feature> flow_features;
	struct simple_packet_info * simple_packet_info;	


	~flow_process();
	
	flow_process() { };
	
	void on_packet_received();
};



