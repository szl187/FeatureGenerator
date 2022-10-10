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
			if ((*it).second.ff.valid == 1) flow_features.push_back((*it).second.ff);
		}
		
		if (flow_features.size() >= 1) {
			//将输出按每秒包数从大到小排序

			sort(flow_features.begin(), flow_features.end());
			reverse(flow_features.begin(), flow_features.end());
		}

		

		int cnt1000 = 0;
		for (auto i = flow_features.begin(); i != flow_features.end(); i++) {
			if ((*i).avelen1000up > 1000) cnt1000++;
		}
		
		for (auto i = flow_features.begin(); i != flow_features.end() ; i++) {
			//std::cout << (*i).portsrc << "->" << (*i).portdst << " len1000:" << (*i).avelen1000up << " len:" << (*i).pktlen << " thp:" << (*i).thp << std::endl;
			if ((*i).avelen1000up > 1000 && (*i).pktlen > 750) { 
				if (cnt1000 == 2) {
					auto j = flow_features.begin();
					
					for (; j != flow_features.end(); j++) {
						if ((*j).portsrc == (*i).portdst && (*j).portdst == (*i).portsrc) {

							

							if ((*j).pktlen<80 && (*j).pktlen>76) {//找到了share的反向流
								std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  share\n";
							}
							else if ((*j).avelen1000up > 1000 && (*j).pktlen < 750) {
								//!!视频反向有负载，但负载很低，可能是音频。
								std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  video\n";
								std::cout << (*j).ipsrc << "->" << (*j).ipdst << "  " << (*j).portsrc << "->" << (*j).portdst << "  voice\n";
								return;
							}
							else std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  video\n";
						}
					}
					
				}
				else std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  video\n";
			}
			if ((*i).pktlen < 750 && (*i).avelen1000up > 1000) {
				if (cnt1000 == 2) {
					
					auto j = flow_features.begin();
					for (; j != flow_features.end(); j++) {
						if ((*j).portsrc == (*i).portdst && (*j).portdst == (*i).portsrc) {

						

							if ((*j).pktlen>110) {//找到了video的反向流
								std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  video\n";
							}
							else std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  share\n";
						}
					}
				}
				else std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  share\n";
			}
			if((*i).avelen1000up < 1000 && (*i).pktlen<300 &&(*i).pktlen>200) std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  voice\n";

		}
			
				
				
				
			
				

	}

			

				
			
	
		
}