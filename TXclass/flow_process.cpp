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
		
			for (auto i = flow_features.begin(); i != flow_features.end(); i++) {
				
					if (i <= flow_features.begin() + 1) //std::cout <<(*i).ipsrc<<"->"<<(*i).ipdst<<"  "<<(*i).portsrc<<"->"<<(*i).portdst<<"  "<<(*i).pktlen<<"  " <<(*i).itvl <<"  "<<(*i).thp<<"  "<<(*i).udpnopayloadrate<< std::endl;
					{
						if ((*i).thp > 9) {
							if ((*i).pktlen < 300 && (*i).udpnopayloadrate>0) std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  none\n";
							else if ((*i).pktlen < 300 && (*i).udpnopayloadrate == 0) std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  voice\n";
							else if ((*i).thp > 100) std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  video\n";
							else if ((*i).pktlen > 570) std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  share\n";
							else if ((*i).pktlen > 350) std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  video\n";
						}
						else std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  none\n";
					}
				
				//9.1
				
				
			}
		
		
	}
}