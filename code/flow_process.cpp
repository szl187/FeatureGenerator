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
		
	//若当前包为TCP包，则判断当前包所属流的反向流中是否有与它匹配的包
	if (simple_packet_info->protocol == 6) {
		if (simple_packet_info->packet_len - simple_packet_info->header_len == 0) {
			struct in_addr tmp = simple_packet_info->flow_key.ip_source_address;
			simple_packet_info->flow_key.ip_source_address = simple_packet_info->flow_key.ip_destination_address;
			simple_packet_info->flow_key.ip_destination_address = tmp;

			u_int16_t p = simple_packet_info->flow_key.tcp_source_port;
			simple_packet_info->flow_key.tcp_source_port = simple_packet_info->flow_key.tcp_destination_port;
			simple_packet_info->flow_key.tcp_destination_port = p;
			it = flows.find(simple_packet_info->flow_key);
			if (it != flows.end() && it->second.pkt_count>1)
			{
				(*it).second.count_rtt(simple_packet_info->ts, ntohl(simple_packet_info->tcp_protocol->tcp_ack));
			}
		}
	}
	
	//输出全流量合计带宽
	if (allflow) {
		if (af == NULL) {
			af = new all_flow(*simple_packet_info);
			af->add_packet(*simple_packet_info);
			
		}
		else af->add_packet(*simple_packet_info);
		
		if (simple_packet_info->ts - af->start_timestamp > time_i) {
			//delete时自动输出时间内所有流量的合计带宽
			delete af;
			af = NULL;
		}
		
	}

}


flow_process::~flow_process() {

	if (af != NULL) //delete时自动输出时间内所有流量的合计带宽
		delete af;
	if (flows.size() >= 1) {
		std::map <struct flow_key, flow>::iterator it;

		//若droplast，则不再统计程序结束时尚未结束的流，否则统计
		if (!droplast) {
			for (it = flows.begin(); it != flows.end(); ++it) {
				it->second.terminate();
				if ((*it).second.ff.valid) flow_features.push_back((*it).second.ff);
			}
		}

		//将输出按每秒包数从大到小排序
		if (Sort == 1) {
			sort(flow_features.begin(), flow_features.end());
			reverse(flow_features.begin(), flow_features.end());
		}

		//CSV输出或TXT输出
		if (csv == 1) {
			for (auto i = flow_features.begin(); i != flow_features.end(); i++) {
				if (vari == 0 || ( (*i).histroyBW.size() > vari)) {
					//计算带宽的标准差（充满重复计算，效率差，待修改）
					double stdev;
					if (vari != 0) {
						double sum = std::accumulate((*i).histroyBW.begin(), (*i).histroyBW.end(), 0.0);
						double mean = sum / (*i).histroyBW.size();
						double accum = 0.0;
						std::for_each((*i).histroyBW.begin(), (*i).histroyBW.end(), [&](const double d) {
							accum += (d - mean)*(d - mean);
						});
						stdev = sqrt(accum / ((*i).histroyBW.size() - 1));
					}

					//输出数据
					out1 << (*i).ipsrc << ',' << (*i).ipdst << ',' << (*i).portsrc << ',' << (*i).portdst << ',';
					out1 << std::fixed << std::setprecision(6) << (*i).startts << ',' << (*i).endts << ',';
					out1 << (*i).pktcnt << "," << (*i).pktlen << ',' << (*i).applen << "," << (*i).itvl << "," << (*i).bw << "," << (*i).appbw << ",";
					if (vari != 0)out1 << stdev << ",";
					out1 << (*i).thp << ","
						<< (*i).retran << "," << (*i).rtrate << "," << (*i).npack << ","
						<< (*i).rtt << "," << (*i).rttM << "," << (*i).rttm << "," << (*i).rttdiff << /*"," << (*i).pkt_size_vec << ','<<(*i).score<<*/"\n";

					//调试用，-i 10000 -sort 输出thp最高的流的带宽和每秒包数
					if (time_i == 10000 && i == flow_features.begin()) std::cout <<(*i).pktlen<<"  " <<(*i).bw << "  " << (*i).thp << std::endl;
				}
				//9.1
				
				
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
					<< "throughput(num/s):" << (*i).thp << "\n"
					<< "retrans:" << (*i).retran << "\n"
					<< "retrans rate:" << (*i).rtrate << "\n"
					<< "ack no payload:" << (*i).npack << "\n"
					<< "average rtt(s):" << (*i).rtt << "\n"
					<< "max rtt(s):" << (*i).rttM << "\n"
					<< "min rtt(s):" << (*i).rttm << "\n"
					<< "length of first some pkt:" << (*i).pkt_size_vec << "\n\n";

			}
		}
	}
}