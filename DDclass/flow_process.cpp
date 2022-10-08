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
			/*if ((*it).second.ff.valid == 1)*/ flow_features.push_back((*it).second.ff);
		}
		
		if (flow_features.size() >= 1) {
			//将输出按每秒包数从大到小排序

			sort(flow_features.begin(), flow_features.end());
			reverse(flow_features.begin(), flow_features.end());


			//CSV输出或TXT输出

			//int cnt = 0;
			/*
			auto i = flow_features.begin();
			
			while (i < flow_features.end()) {
				if ((*i).avelen1000up == 0) i++;
				else break;
			}
			if (i != flow_features.end() && (*i).valid == 1) std::cout << (*i).portsrc << "->" << (*i).portdst << " len1000:" << (*i).avelen1000up << " thp:" << (*i).thp << "valid: " << (*i).valid << std::endl;
			*/
			int cntshare = 0, cntvideo = 0, cntsingle = 0, cntvoice=0;
			for (auto i = flow_features.begin(); i != flow_features.end() && i < flow_features.begin() + 5; i++) {
				if ((*i).pktlen < 161 && (*i).pktlen > 156) {
					cntsingle++;
				}
			}
			for (auto i = flow_features.begin(); i != flow_features.end() && i<flow_features.begin()+5; i++) {
				//std::cout << (*i).portsrc << "->" << (*i).portdst << " len1000:" << (*i).avelen1000up <<" len:"<<(*i).pktlen << " thp:" << (*i).thp << "valid: " << (*i).valid << std::endl;
				if ((*i).avelen1000up > 1200) {//share
					short port1 = (*i).portsrc, port2 = (*i).portdst;
					if(((*i).portsrc<5000 && (*i).portdst <60000) || ((*i).portdst < 5000 && (*i).portsrc < 60000)) {//video port
						if (cntsingle == 1) {
							std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  video\n";
							cntvideo++;
						}
						else {
							std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  share\n";
							cntshare++;
						}

					}
					else if (((*i).portsrc < 5000 && (*i).portdst > 60000) || ((*i).portdst < 5000 && (*i).portsrc > 60000)) {
						if (cntsingle == 1) {
							std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  share\n";
							cntshare++;
						}
						else {
							std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  share\n";
							cntshare++;
						}
					}
					/*
					auto j = flow_features.begin();
					int isvideo = 0;

					for (; j != flow_features.end(); j++) {
						if ((*j).portsrc == port2 && (*j).portdst == port1) {
							if ((*j).pktlen<103 && (*j).pktlen>99) {//找到了video的反向流，说明是video混淆的
								std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  video\n";
								cntvideo++;
								isvideo = 1;
							}
						}
					}
					if (j == flow_features.end()) {//没找到也正常，可能没录入反向流
						std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  share\n";
						cntshare++;
					}
					else if (isvideo == 0) {
						std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  share\n";
						cntshare++;
					}
					*/
					
				}
				if ((*i).avelen1000up < 1200 && (*i).avelen1000up>1000) {

					short port1 = (*i).portsrc, port2 = (*i).portdst;
					if (((*i).portsrc < 5000 && (*i).portdst > 60000) || ((*i).portdst < 5000 && (*i).portsrc > 60000)) {//找到了video的反向流，说明是video混淆的
						if (cntsingle == 1) {
							std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  share\n";
							cntshare++;
						}
						else {
							std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  video\n";
							cntvideo++;
						}

					}
					else if (((*i).portsrc < 5000 && (*i).portdst < 60000) || ((*i).portdst < 5000 && (*i).portsrc < 60000)) {//没找到也正常，可能没录入反向流
						if (cntsingle == 1) {
							std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  video\n";
							cntvideo++;
						}
						else {
							std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  video\n";
							cntvideo++;
						}
					}
					/*
					auto j = flow_features.begin();
					for (; j != flow_features.end(); j++) {
						if ((*j).portsrc == port2 && (*j).portdst == port1) {
							if ((*j).pktlen < 105 && (*j).pktlen>99) {
								std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  video\n";
								cntvideo++;
								
							}
						}
					}
					if (j == flow_features.end()) {//没找到video的反向流，说明是share混淆的
						std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  share\n";
						cntshare++;
					}
					*/

					
				}
				
				if ((*i).pktlen > 200 && (*i).pktlen < 230) {
					std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  voice\n";
					cntvoice++;
				}
				




				
				/*
				* //TXclass
				if (cnt < 2 && i != flow_features.end() && (*i).avelen1000up > 1000 && (*i).avelen1000up < 2000) {
					cnt++;
				}
				*/
				

			}

			
			if (cntsingle == 1 && (cntshare == 0 && cntvideo == 0)) {
				for (auto i = flow_features.begin(); i != flow_features.end() && i < flow_features.begin() + 4; i++) {
					//std::cout << (*i).portsrc << "->" << (*i).portdst << " len1000:" << (*i).avelen1000up << " len:" << (*i).pktlen << " thp:" << (*i).thp << "valid: " << (*i).valid << std::endl;
					if ((*i).pktlen > 122 && (*i).pktlen < 125 && (*i).portdst > 50000) {
						std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  video\n";
					}
					if ((*i).pktlen > 129 && (*i).pktlen < 133 && (*i).portdst > 60000) {
						std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  share\n";
					}
				}
			}
			if (cntsingle == 0 && (cntshare+cntvideo<2) && cntvoice==0) {
				for (auto i = flow_features.begin(); i != flow_features.end() && i < flow_features.begin() + 4; i++) {
					if ((*i).pktlen > 129 && (*i).pktlen < 133) {
						std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  share\n";
					}
				}
			}
			

				
			/*
			//TXclass
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
			*/
		}
		
		/*
		else {
			std::cout << "no flows has enough throughput(>10), none.\n";
		}*/
		
		
	}
}