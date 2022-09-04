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
	struct portinfo ports[3];//9.1
	int flag = 0, f3230 = 0, f3232 = 0;

	for (auto i = flow_features.begin(); i != flow_features.end(); i++) {

		if ((*i).pktlen == 214 && f3230 == 0) {

			f3230 = 1;
			std::cout << std::fixed << std::setprecision(6) << (*i).startts << " 端口" << (*i).portsrc << "," << (*i).portdst << " 担任角色：" << "3230\n";
		}

		else if (((*i).pktlen == 60 || (*i).pktlen == 54) && (*i).applen == 12 /*&& (((*i).endts-(*i).startts))<1.2*dura3230  && ((*i).endts - (*i).startts)> 0.8*dura3230*/) {
			if (flag == 0) {
				ports[1] = { (*i).startts ,(*i).portsrc ,(*i).portdst ,0 };
				flag += 1;
			}
			else if (flag == 1 && (*i).portdst != ports[1].portdst)ports[2] = { (*i).startts ,(*i).portsrc ,(*i).portdst ,0 };


		}

		else if (f3232 == 0 && (*i).same_len && (*i).pktlen <= 80 && (*i).pktlen >= 75 && (*i).applen <= 38 && (*i).applen >= 33) {
			f3232 = 1;
			//std::cout << "3232:" << (*i).applen << "\n";
			std::cout << std::fixed << std::setprecision(6) << (*i).startts << " 端口" << (*i).portsrc << "," << (*i).portdst << " 担任角色：" << "3232\n";
			ports[0] = { (*i).startts ,(*i).portsrc ,(*i).portdst ,0 };
		}


	}
	for (auto i = flow_features.begin(); i != flow_features.end(); i++) {
		if ((*i).portsrc == ports[1].portsrc && (*i).portdst == ports[1].portdst) {
			ports[1].totallen += (*i).pktcnt * (*i).pktlen;
			ports[1].applen += (*i).applen;
		}
		else if ((*i).portsrc == ports[1].portdst && (*i).portdst == ports[1].portsrc) {
			ports[1].totallen += (*i).pktcnt * (*i).pktlen;
			ports[1].applen += (*i).applen;
		}
		if ((*i).portsrc == ports[2].portsrc && (*i).portdst == ports[2].portdst) {
			ports[2].totallen += (*i).pktcnt * (*i).pktlen;
			ports[2].applen += (*i).applen;
		}
		else if ((*i).portsrc == ports[2].portdst && (*i).portdst == ports[2].portsrc) {
			ports[2].totallen += (*i).pktcnt * (*i).pktlen;
			ports[2].applen += (*i).applen;
		}
		if ((*i).portsrc == ports[0].portsrc && (*i).portdst == ports[0].portdst) {
			ports[0].applen += (*i).applen;
		}
		else if ((*i).portsrc == ports[0].portdst && (*i).portdst == ports[0].portsrc) {
			ports[0].applen += (*i).applen;
		}
	}
	//9.1
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
	if (ports[0].applen > 200) std::cout << "视频\n";
	if (ports[2].applen + ports[1].applen > 100) std::cout << "共享\n";
	else if (ports[0].applen < 100) { std::cout << "音频\n"; }

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