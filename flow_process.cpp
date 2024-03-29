#include "flow_process.h"




void flow_process::on_packet_received()
{
	
	
		flow* f = NULL;

		std::map <struct flow_key, flow>::iterator it;
		it = flows.find(simple_packet_info.flow_key);
		//flows中没有，创建新的流
		if (it == flows.end()) {
			f = new flow(simple_packet_info);
			flows.insert(std::pair<struct flow_key, flow>(simple_packet_info.flow_key, *f));
			//printf("new flow\n");
			//printf("%d", simple_packet_info.flow_key.tcp_source_port);
		}
		else
		{

			f = &(it->second);
			f->add_packet(simple_packet_info);
		}

		//if (simple_packet_info.flow_key.tcp_source_port == 56430) printf("%d,", f->pkt_count);
		

	


}

void flow_process::portinfo_output(double dura3230)
{
	
	int flag=0;
	struct portinfo port3232[2], port3234[2];
	int payload = 0, is3234 = 0;
	int is3232 = 0;
	for (auto i = flow_features.begin(); i != flow_features.end(); i++) {
		/*
		if ((*i).is3232payload == true) {
			port3232[0] = { 1,(*i).portsrc,(*i).portdst };
		}
		if ((*i).is3232 == true)
		{
			port3232[1] = { 1,(*i).portsrc,(*i).portdst };
		}
		if ((*i).is3234payload == true) {
			port3234[0] = { 1,(*i).portsrc,(*i).portdst };
		}
		*/
		if ((*i).ispayload) payload++;
		if ((*i).is3232) is3232++;
		if ((*i).is3234or6) is3234++;
	
	}
	//std::cout << payload << ',' << is3232 << ',' << is3234 << "\n";
	if (payload==1 )
		if(is3232 == 1)std::cout << "video\n";
		else std::cout << "share\n";
	if (payload == 2) 
		if (is3232 == 0) std::cout << "video, video\n";
		else if (is3232 ==1) std::cout << "share, video\n";
	if (payload == 0) std::cout << "voice\n";

	
	/*
	if (port3232[1].valid && port3234[0].valid) std::cout << port3232[1].portsrc << ','<<port3234[0].portdst << "\n";

	if ((port3232[1].valid && port3232[0].valid)&&(port3232[1].portsrc == port3232[0].portdst)) std::cout << "video\n";
	else if ((port3232[1].valid && port3234[0].valid) && port3232[1].portsrc == port3234[0].portdst) std::cout << "video\n";
	else if (port3232[0].valid) std::cout << "video\n";
	else if (port3234[0].valid) std::cout << "share\n";
	else std::cout << "voice\n";
	*/
	//9.1
	/*
	if (ports[1].totallen > ports[2].totallen) {
		std::cout << std::fixed << std::setprecision(6) << ports[1].startts << " 端口" << ports[1].portsrc << "," << ports[1].portdst << " 担任角色：" << "3234\n";
		std::cout << std::fixed << std::setprecision(6) << ports[2].startts << " 端口" << ports[2].portsrc << "," << ports[2].portdst << " 担任角色：" << "3236\n";
		//std::cout << "共享\n\n";
	}
	else if (ports[1].totallen < ports[2].totallen) {
		std::cout << std::fixed << std::setprecision(6) << ports[1].startts << " 端口" << ports[1].portsrc << "," << ports[1].portdst << " 担任角色：" << "3236\n";
		std::cout << std::fixed << std::setprecision(6) << ports[2].startts << " 端口" << ports[2].portsrc << "," << ports[2].portdst << " 担任角色：" << "3234\n";
		//std::cout << "共享\n\n";
	}
	else {
		std::cout << std::fixed << std::setprecision(6) << ports[1].startts << " 端口" << ports[1].portsrc << "," << ports[1].portdst << " 担任角色：" << "3234or3236\n";
		std::cout << std::fixed << std::setprecision(6) << ports[2].startts << " 端口" << ports[2].portsrc << "," << ports[2].portdst << " 担任角色：" << "3234or3236\n";
		//if (ports[0].totallen > 100) std::cout << "视频\n\n";
		//else  std::cout << "音频\n\n";
	}
	*/
	

}

flow_process::~flow_process() 
{

	if (flows.size() >= 1) {
		std::map <struct flow_key, flow>::iterator it;
		for (it = flows.begin(); it != flows.end(); ++it) {
			it->second.terminate();
			if ((*it).second.ff.valid) flow_features.push_back((*it).second.ff);
		}

		double dura3230 = 0;
		for (auto i = flow_features.begin(); i != flow_features.end(); i++) {
			if ((*i).pktlen == 214) {
				dura3230 = (*i).endts - (*i).startts;
			}
		}
		portinfo_output(dura3230);
	}

}