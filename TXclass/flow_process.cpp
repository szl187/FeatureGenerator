#include "flow_process.h"




void flow_process::on_packet_received()
{
	
	
	flow* f = NULL;

	std::map <struct flow_key, flow>::iterator it;
	it = flows.find(simple_packet_info->flow_key);
	//flows中没有，创建新的流
	if (it == flows.end()) {
		f = new flow(*simple_packet_info);
		flows.insert(std::pair<struct flow_key, flow>(simple_packet_info->flow_key, *f));
	}
		
	else
	{
		//若当前流持续时间超过设定的单位时间，则结束并创建新流
		if (simple_packet_info->ts - (*it).second.start_timestamp > time_i)
		{
			//调用当前流的terminate方法生成flow_feature流特征类并存储
			(*it).second.terminate();
			struct flow_feature last_ff = (*it).second.ff;
			if ((*it).second.ff.valid) flow_features.push_back(last_ff);

			//从流表中删除当前流并创建新的流加入
			flows.erase(it);
			f = new flow(*simple_packet_info,last_ff,d_num);
			flows.insert(std::pair<struct flow_key, flow>(simple_packet_info->flow_key, *f));
		}
		else
		{
			f = &(it->second);
			f->add_packet(*simple_packet_info);
		}
			
			

	}
		
	
	
	

}


flow_process::~flow_process() {

	
	if (flows.size() >= 1) {
		std::map <struct flow_key, flow>::iterator it;


		
		for (it = flows.begin(); it != flows.end(); ++it) {
			it->second.terminate();
			flow_features.push_back((*it).second.ff);
		}
		

		//将输出按每秒包数从大到小排序
		
		sort(flow_features.begin(), flow_features.end());
		reverse(flow_features.begin(), flow_features.end());
		

		//CSV输出或TXT输出
		int cnt = 0;
		for (auto i = flow_features.begin(); i != flow_features.end(); i++) {
			if (i <= flow_features.begin() + 1 && i != flow_features.end() && (*i).avelen1000up > 1000 && (*i).avelen1000up < 2000) {
				cnt++;
				//std::cout << (*i).portsrc << "->" << (*i).portdst << " len:" << (*i).avelen1000up << " bw:" << (*i).bw << " thp:" << (*i).thp << std::endl;
			}
		}
		//out1 << (*i).portsrc << "," << (*i).portdst << "," << (*i).pktlen << "," << (*i).bw << "," << (*i).thp << "," << (*i).cnt26 << "," << (*i).cnt60 << "," << (*i).cnt74 << "," << (*i).cnt54 << "," << (*i).cnt78 << "," << (*i).cnt82 << std::endl;

					//if ((*i).thp > 9) {
		if (cnt == 1) {
			auto i = flow_features.begin();
			while ((*i).avelen1000up < 1000 || (*i).avelen1000up > 2000) i++;
			if ((*i).avelen1000up >= 1180) std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  share\n";
			else  std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  video\n";
			//else std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "有情况没考虑到0\n";

			if((*i).udpnopayloadrate>0) 
				std::cout << (*i).ipdst << "->" << (*i).ipsrc << "  " << (*i).portdst << "->" << (*i).portsrc << "  voice\n";
			else 
				std::cout << (*i).ipdst << "->" << (*i).ipsrc << "  " << (*i).portdst << "->" << (*i).portsrc << "  none\n";
		}
		else if (cnt == 2) {
			auto i = flow_features.begin();
			while ((*i).avelen1000up < 1000 || (*i).avelen1000up > 2000) i++;
			if ((*i).avelen1000up >= 1180) std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  share\n";
			else  std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  video\n";


			if ((*(i + 1)).avelen1000up >= 1180) std::cout << (*(i + 1)).ipsrc << "->" << (*(i + 1)).ipdst << "  " << (*(i + 1)).portsrc << "->" << (*(i + 1)).portdst << "  share\n";
			else  std::cout << (*(i + 1)).ipsrc << "->" << (*(i + 1)).ipdst << "  " << (*(i + 1)).portsrc << "->" << (*(i + 1)).portdst << "  video\n";
			//else std::cout << (*(i + 1)).ipsrc << "->" << (*(i + 1)).ipdst << "  " << (*(i + 1)).portsrc << "->" << (*(i + 1)).portdst << "有情况没考虑到1.2\n";

		}
		else {
			auto i = flow_features.begin();
			while (((*i).portsrc <= 8000 || (*i).portdst <= 8000) && i<flow_features.end()) i++;

			if (i == flow_features.end()) {
				std::cout << "none\n";
				return;
			}

			auto j = i+1;
			while (((*j).portsrc <= 8000 || (*j).portdst <= 8000) && j < flow_features.end()) j++;
			if (j == flow_features.end()) {
				std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  voice\n";
				std::cout << (*i).ipdst << "->" << (*i).ipsrc << "  " << (*i).portdst << "->" << (*i).portsrc << "  voice\n";
				return;
			}

			//std::cout << (*i).portsrc << "->" << (*i).portdst << " nopayload " << (*i).udpnopayloadrate<< std::endl;
			//std::cout << (*(j)).portsrc << "->" << (*(j)).portdst << " nopayload " << (*(j)).udpnopayloadrate << std::endl;

			if ((*i).udpnopayloadrate > 0 && (*(j)).udpnopayloadrate > 0) {
				std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  voice\n";
				std::cout << (*j).ipsrc << "->" << (*j).ipdst << "  " << (*j).portsrc << "->" << (*j).portdst << "  voice\n";
			}
			else if ((*i).udpnopayloadrate > 0 && ( * (j)).udpnopayloadrate == 0) {
				std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  none\n";
				std::cout << (*j).ipsrc << "->" << (*j).ipdst << "  " << (*j).portsrc << "->" << (*j).portdst << "  voice\n";
			}
			else if ((*i).udpnopayloadrate == 0 && (*(j)).udpnopayloadrate > 0) {
				std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  voice\n";
				std::cout << (*j).ipsrc << "->" << (*j).ipdst << "  " << (*j).portsrc << "->" << (*j).portdst << "  none\n";
			}
			else {
				std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  none\n";
				std::cout << (*j).ipsrc << "->" << (*j).ipdst << "  " << (*j).portsrc << "->" << (*j).portdst << "  none\n";
			}
		}
		
		
	}
}