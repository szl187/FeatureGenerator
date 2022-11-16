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
		/*
		//若当前流持续时间超过设定的单位时间time_i，则结束并创建新流(time_i=100000。下面的if不会触发，所有的流直接持续到程序结束)
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
		*/

		//场景分类
		f = &(it->second);
		f->add_packet(*simple_packet_info);

	}
	

	
	

}


flow_process::~flow_process() {


	if (flows.size() >= 1) {
		std::map <struct flow_key, flow>::iterator it;



		for (it = flows.begin(); it != flows.end(); ++it) {
			it->second.terminate();
			if ((*it).second.ff.valid == 1) flow_features.push_back((*it).second.ff);
		}

		if (flow_features.size() >= 1) {
			//将输出按每秒包数从大到小排序

			sort(flow_features.begin(), flow_features.end());
			reverse(flow_features.begin(), flow_features.end());
		}


		if (app == 3) {//feishu 
			//no use
			/*
			int cnt1000 = 0;
			for (auto i = flow_features.begin(); i <= flow_features.begin() + 1 && (*i).thp > 0; i++) {
				if ((*i).avelen1000up > 1000) cnt1000++;
			}
			int cntshare = 0, cntvideo = 0;
			*/
			for (auto i = flow_features.begin(); i <= flow_features.begin() + 1 && (*i).thp > 0; i++) {
				if ((*i).avelen1000up > 1200) std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",share\n";
				else if ((*i).avelen1000up > 1000 && (*i).pktlen > 150) std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",video\n";
				else if ((*i).pktlen > 130) std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",voice\n";
			}
		}
		else if (app == 2) {//dd
			int cntshare = 0, cntvideo = 0, cntsingle = 0, cntvoice = 0;
			for (auto i = flow_features.begin(); i != flow_features.end() && i < flow_features.begin() + 5; i++) {
				if ((*i).pktlen < 161 && (*i).pktlen > 156) {
					cntsingle++;
				}
			}
			for (auto i = flow_features.begin(); i != flow_features.end() && i < flow_features.begin() + 5; i++) {
				if ((*i).avelen1000up > 1200) {//share
					short port1 = (*i).portsrc, port2 = (*i).portdst;
					if (((*i).portsrc < 5000 && (*i).portdst < 60000) || ((*i).portdst < 5000 && (*i).portsrc < 60000)) {//video port
						if (cntsingle == 1) {
							std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",video\n";
							cntvideo++;
						}
						else {
							std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",share\n";
							cntshare++;
						}

					}
					else if (((*i).portsrc < 5000 && (*i).portdst > 60000) || ((*i).portdst < 5000 && (*i).portsrc > 60000)) {
						if (cntsingle == 1) {
							std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",share\n";
							cntshare++;
						}
						else {
							std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",share\n";
							cntshare++;
						}
					}


				}
				if ((*i).avelen1000up < 1200 && (*i).avelen1000up>1000) {

					short port1 = (*i).portsrc, port2 = (*i).portdst;
					if (((*i).portsrc < 5000 && (*i).portdst > 60000) || ((*i).portdst < 5000 && (*i).portsrc > 60000)) {
						if (cntsingle == 1) {
							std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",share\n";
							cntshare++;
						}
						else {
							std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",video\n";
							cntvideo++;
						}

					}
					else if (((*i).portsrc < 5000 && (*i).portdst < 60000) || ((*i).portdst < 5000 && (*i).portsrc < 60000)) {
						if (cntsingle == 1) {
							std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",video\n";
							cntvideo++;
						}
						else {
							std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",video\n";
							cntvideo++;
						}
					}


				}

				if ((*i).pktlen > 200 && (*i).pktlen < 230) {
					std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",voice\n";
					cntvoice++;
				}
			}


			if (cntsingle == 1 && (cntshare == 0 && cntvideo == 0)) {
				for (auto i = flow_features.begin(); i != flow_features.end() && i < flow_features.begin() + 4; i++) {
					if ((*i).pktlen > 122 && (*i).pktlen < 125 && (*i).portdst > 50000) {
						std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",video\n";
					}
					if ((*i).pktlen > 129 && (*i).pktlen < 133 && (*i).portdst > 60000) {
						std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",share\n";
					}
				}
			}
			if (cntsingle == 0 && (cntshare + cntvideo < 2) && cntvoice == 0) {
				for (auto i = flow_features.begin(); i != flow_features.end() && i < flow_features.begin() + 4; i++) {
					if ((*i).pktlen > 129 && (*i).pktlen < 133) {
						std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",share\n";
					}
				}
			}
		}
		else if (app == 1) {//tx
			int cnt = 0;
			
			for (auto i = flow_features.begin(); i <= flow_features.begin()+1; i++) {
				if (cnt < 2 && i != flow_features.end() && (*i).avelen1000up > 1000 && (*i).avelen1000up < 2000) {
					cnt++;
				}

			}
			if (cnt == 1) {
				auto i = flow_features.begin();
				while (((*i).avelen1000up < 1000 || (*i).avelen1000up > 2000) && i != flow_features.end()) i++;
				if ((*i).valid) {
					if ((*i).avelen1000up >= 1180) std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",share\n";
					else  std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",video\n";

					if ((*i).udpnopayloadrate > 0)
						std::cout << (*i).ipdst << "," << (*i).ipsrc << "," << (*i).portdst << "," << (*i).portsrc << ",voice\n";
					else
						std::cout << (*i).ipdst << "," << (*i).ipsrc << "," << (*i).portdst << "," << (*i).portsrc << ",none\n";
				}
			}
			else if (cnt == 2) {
				auto i = flow_features.begin();
				while ((*i).avelen1000up < 1000 || (*i).avelen1000up > 2000) i++;
				if ((*i).avelen1000up >= 1180) std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",share\n";
				else  std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",video\n";

				auto j = i + 1;
				while ((*j).avelen1000up < 1000 || (*j).avelen1000up > 2000) j++;
				if ((*(j)).avelen1000up >= 1180) std::cout << (*(j)).ipsrc << "," << (*(j)).ipdst << "," << (*(j)).portsrc << "," << (*(j)).portdst << ",share\n";
				else  std::cout << (*(j)).ipsrc << "," << (*(j)).ipdst << "," << (*(j)).portsrc << "," << (*(j)).portdst << ",video\n";
		
			}
			else {
				auto i = flow_features.begin();
				std::cout << (*i).ipsrc << "," << (*i).ipdst << "," << (*i).portsrc << "," << (*i).portdst << ",voice\n";
				std::cout << (*i).ipdst << "," << (*i).ipsrc << "," << (*i).portdst << "," << (*i).portsrc << ",none\n";
			}
		}
	}
}


		

