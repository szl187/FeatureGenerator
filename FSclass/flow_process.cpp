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
		for (auto i = flow_features.begin(); i <= flow_features.begin() + 1 && (*i).thp > 0; i++) {
			if ((*i).avelen1000up > 1000) cnt1000++;
		}
		int cntshare = 0, cntvideo = 0;
		for (auto i = flow_features.begin(); i <= flow_features.begin()+1 && (*i).thp>0 ; i++) {
			//std::cout <<"cnt1000:"<<cnt1000<<" ||"<<std::setw(5)<< (*i).portsrc << "->"<<std::setw(5) << (*i).portdst << " len1000:" << std::setw(7)<<(int)(*i).avelen1000up << " len:" <<std::setw(4)<< (int)(*i).pktlen << " thp:" << (*i).thp << std::endl;
			
			
			if((*i).avelen1000up >1200 ) std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  share\n";
			else if ((*i).avelen1000up > 1000 && (*i).pktlen > 200) std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  video\n";


		}
		if (cnt1000 == 0) {
			auto i = flow_features.begin();
			std::cout << (*i).ipsrc << "->" << (*i).ipdst << "  " << (*i).portsrc << "->" << (*i).portdst << "  voice\n";
		}
		

				

	}


		
}