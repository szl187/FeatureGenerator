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
			if ((*it).second.ff.valid==1) flow_features.push_back((*it).second.ff);
		}
		
		if (flow_features.size() >= 1) {
			//将输出按每秒包数从大到小排序

			sort(flow_features.begin(), flow_features.end());
			reverse(flow_features.begin(), flow_features.end());


			//CSV输出或TXT输出

			int cnt = 0;
			auto i = flow_features.begin();
			while (i < flow_features.end()) {
				if ((*i).avelen1000up == 0) i++;
				else break;
			}
			if (i != flow_features.end() && (*i).valid == 1) std::cout << (*i).portsrc << "->" << (*i).portdst << " len1000:" << (*i).avelen1000up << " thp:" << (*i).thp << "valid: " << (*i).valid << std::endl;

			for (auto i = flow_features.begin(); i != flow_features.end(); i++) {
				//if ((*i).valid) {
					//std::cout << (*i).portsrc << "->" << (*i).portdst << " len1000:" << (*i).avelen1000up << " thp:" << (*i).thp << "valid: " << (*i).valid << std::endl;
				if (cnt < 2 && i != flow_features.end() && (*i).avelen1000up > 1000 && (*i).avelen1000up < 2000) {
					cnt++;
				}
				//}

			}
			//out1 << (*i).portsrc << "," << (*i).portdst << "," << (*i).pktlen << "," << (*i).bw << "," << (*i).thp << "," << (*i).cnt26 << "," << (*i).cnt60 << "," << (*i).cnt74 << "," << (*i).cnt54 << "," << (*i).cnt78 << "," << (*i).cnt82 << std::endl;

						//if ((*i).thp > 9) {
			if (cnt == 1) {
				auto i = flow_features.begin();
				while (((*i).avelen1000up < 1000 || (*i).avelen1000up > 2000) && i != flow_features.end()) i++;
				if (i == flow_features.end()) {
					std::cout << "error1:cnt=1 but no payload get\n";

				}
				else if ((*i).valid) {
					if ((*i).avelen1000up >= 1180) std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  share\n";
					else  std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  video\n";
					//else std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "有情况没考虑到0\n";

					if ((*i).udpnopayloadrate > 0)
						std::cout << (*i).ipdst << "->" << (*i).ipsrc << "  " << (*i).portdst << "->" << (*i).portsrc << "  voice\n";
					else
						std::cout << (*i).ipdst << "->" << (*i).ipsrc << "  " << (*i).portdst << "->" << (*i).portsrc << "  none\n";
				}
			}
			else if (cnt == 2) {
				auto i = flow_features.begin();
				while ((*i).avelen1000up < 1000 || (*i).avelen1000up > 2000) i++;
				if ((*i).avelen1000up >= 1180) std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  share\n";
				else  std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  video\n";

				auto j = i + 1;
				while ((*j).avelen1000up < 1000 || (*j).avelen1000up > 2000) j++;
				if ((*(j)).avelen1000up >= 1180) std::cout << (*(j)).ipsrc << "->" << (*(j)).ipdst << "  " << (*(j)).portsrc << "->" << (*(j)).portdst << "  share\n";
				else  std::cout << (*(j)).ipsrc << "->" << (*(j)).ipdst << "  " << (*(j)).portsrc << "->" << (*(j)).portdst << "  video\n";
				//else std::cout << (*(i + 1)).ipsrc << "->" << (*(i + 1)).ipdst << "  " << (*(i + 1)).portsrc << "->" << (*(i + 1)).portdst << "有情况没考虑到1.2\n";

			}
			else {
				std::cout << "no flow has packet len >1000\n";
				auto i = flow_features.begin();
				std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  voice\n";
				std::cout << (*i).ipdst << "->" << (*i).ipsrc << "  " << (*i).portdst << "->" << (*i).portsrc << "  none\n";
			}
		}
		else {
			std::cout << "no flows has enough throughput(>10), none.\n";
		}
		
		
	}
}