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
		//10.10 修改逻辑，避免一个时间段的最后一个包被当作下一时间段的首个包重复计算
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
				f = new flow(*simple_packet_info);
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

			if ((*it).second.ff.valid) flow_features.push_back((*it).second.ff);
		}
		
		if (csv == 1) {
	
			for (auto i = flow_features.begin(); i != flow_features.end(); i++) {
				out1 << (*i).ipsrc << ',' << (*i).ipdst << ',' << (*i).portsrc << ',' << (*i).portdst << ',';
				out1 << std::fixed << std::setprecision(6) << (*i).startts << ',' << (*i).endts << ',';
				out1 << (*i).pktcnt << "," << (*i).pktlen << ',' << (*i).applen << "," << (*i).itvl << "," << (*i).bw << "," << (*i).appbw << "," << (*i).thp << ",\n";
				
				
				
		
				
			}
		
		}
		else {
			for (auto i = flow_features.begin(); i != flow_features.end(); i++) {
				out1.setf(std::ios::fixed);
				out1.precision(6);
				out1 << "flow" << j << ":" << (*i).ipsrc << "->" << (*i).ipdst << " " << (*i).portsrc << "->" << (*i).portdst;

				out1 << "\nstart timestamp:" << (*i).startts << "\n"
					<< "end timestamp:" << (*i).endts << "\n"
					<< "duration:" << (*i).endts - (*i).startts << "\n"
					<< "packet count:" << (*i).pktcnt << "\n"
					<< "packet size(B):" << (*i).pktlen << "\n"
					<< "app packet size(B):" << (*i).applen << "\n"
					<< "interval(s):" << (*i).itvl << "\n"
					<< "bandwith(B/s):" << (*i).bw << "\n"
					<< "application layer bandwith(B/s):" << (*i).appbw << "\n"
					<< "throughput(num/s):" << (*i).thp << "\n";
		

			}
		}
	}
}