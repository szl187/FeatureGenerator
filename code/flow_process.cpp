#include "flow_process.h"




void flow_process::on_packet_received()
{
	
	if (onlyallflow == 0) {
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
		if (it != flows.end())
		{
			if ((*it).second.latest_timestamp != 0)
			{
				if (simple_packet_info.ts - (*it).second.start_timestamp > time_i)
				{
					j++;
					(*it).second.terminate(1);
					if ((*it).second.ff.valid) flow_features.push_back((*it).second.ff);
					flows.erase(it);
					f = new flow(simple_packet_info);
					flows.insert(std::pair<struct flow_key, flow>(simple_packet_info.flow_key, *f));
					//printf("overtime\n");
				}
			}

		}
		//if (simple_packet_info.flow_key.tcp_source_port == 56430) printf("%d,", f->pkt_count);
		

		if (simple_packet_info.protocol == 6) {
			if (simple_packet_info.packet_len - simple_packet_info.header_len == 0) {
				//std::cout << "ack\n";
				struct in_addr tmp = simple_packet_info.flow_key.ip_source_address;
				simple_packet_info.flow_key.ip_source_address = simple_packet_info.flow_key.ip_destination_address;
				simple_packet_info.flow_key.ip_destination_address = tmp;

				u_int16_t p = simple_packet_info.flow_key.tcp_source_port;
				simple_packet_info.flow_key.tcp_source_port = simple_packet_info.flow_key.tcp_destination_port;
				simple_packet_info.flow_key.tcp_destination_port = p;
				it = flows.find(simple_packet_info.flow_key);
				if (it != flows.end())
				{
					//std::cout << "ack\n";
					(*it).second.count_rtt(simple_packet_info.ts, ntohl(simple_packet_info.tcp_protocol->tcp_ack));
				}
			}
		}
	}
	if (allflow) {
		if (af == NULL) {
			af = new all_flow(simple_packet_info);
			af->add_packet(simple_packet_info);
			
		}
		else af->add_packet(simple_packet_info);
		if (simple_packet_info.ts - af->start_timestamp > time_i) {
			delete af;
			af = NULL;
		}
	}
	


}

flow_process::~flow_process() {
	if (af != NULL) delete af;
	if (flows.size() >= 1) {
		std::map <struct flow_key, flow>::iterator it;
		for (it = flows.begin(); it != flows.end(); ++it) {
			it->second.terminate();

			if ((*it).second.ff.valid) flow_features.push_back((*it).second.ff);
		}
		if (Sort == 1) {
			sort(flow_features.begin(), flow_features.end());
			reverse(flow_features.begin(), flow_features.end());
		}
		if (csv == 1) {
			for (auto i = flow_features.begin(); i != flow_features.end(); i++) {
				out1 << (*i).ipsrc << ',' << (*i).ipdst << ',' << (*i).portsrc << ',' << (*i).portdst << ',';
				out1 << std::fixed << std::setprecision(6) << (*i).startts << ',' << (*i).endts << ',';
				out1 << (*i).pktcnt << "," << (*i).pktlen << ',' << (*i).applen << "," << (*i).itvl << "," << (*i).bw << "," << (*i).appbw << "," << (*i).thp << ","
					<< (*i).retran << "," << (*i).rtrate << "," << (*i).npack << ","
					<< (*i).rtt << "," << (*i).rttM << "," << (*i).rttm << "," << (*i).rttdiff << "," << (*i).pkt_size_vec << "\n";
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
