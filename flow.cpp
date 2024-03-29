#include "flow.h"


flow::flow(simple_packet_info& simple_packet_info)
{

	this->flow_key = simple_packet_info.flow_key;
	this->protocol = simple_packet_info.protocol;
	latest_timestamp = start_timestamp= simple_packet_info.ts;
	reverse_latest_timestamp = -1;
	interarrival_time_n = interarrival_time_ls = flow_duration  = used_ts = 0;
	pkt_count = 1;
	rtt_count = window_size_ls = retrans = ack_no_payload = 0;
	packet_size_ls = simple_packet_info.packet_len;
	app_packet_size_ls = ntohs(simple_packet_info.ip_protocol->ip_length) + 14 - simple_packet_info.header_len;//9.1
	
	bandwith = appbandwith = throughput = 0;
	ave_pkt_size = app_ave_pkt_size = ave_interval = ave_windows = 0;
	ave_rtt = 0;
	rtt_min = 100;
	rtt_max = -1;
	packet_size_vector = std::to_string(simple_packet_info.packet_len)+',';
	if (protocol == 6) {
		max_seq = ntohl(simple_packet_info.tcp_protocol->tcp_acknowledgement);
		last_flag = (ntohs(simple_packet_info.tcp_protocol->tcp_headerlen_flag) & 3);
	}

	same_len = true;
	cnt1053_1069 = cnt_1267_1269 = 0;
	last_len = simple_packet_info.packet_len;
	cnt7476777980 = 0;
}



void flow::add_packet(simple_packet_info &pkt)
{
	feature_output(pkt, pkt.protocol);
}



void flow::terminate(int overtime)
{
	string ipsrc, ipdst;
	double duration = latest_timestamp - start_timestamp;
	ipsrc = inet_ntoa(flow_key.ip_source_address);
	ipdst = inet_ntoa(flow_key.ip_destination_address);
	ave_pkt_size = double(packet_size_ls )/ pkt_count;
	app_ave_pkt_size = double(app_packet_size_ls) / pkt_count;
	throughput = pkt_count / duration;
	if(rtt_count>0) ave_rtt = rtt_ls / rtt_count;
	else ave_rtt = 0;
	ave_interval = interval_ls / double(pkt_count);
	bandwith = ave_pkt_size *throughput;
	appbandwith = app_ave_pkt_size * throughput;
	
	
	
	
	if (duration != 0) {
		
		
			
			ff.valid = 1;
			strcpy(ff.ipsrc, ipsrc.c_str());
			strcpy(ff.ipdst, ipdst.c_str());
			strcpy(ff.pkt_size_vec, packet_size_vector.c_str());
			ff.appbw = appbandwith;
			ff.applen = app_ave_pkt_size;
			ff.bw = bandwith;
			ff.endts = latest_timestamp;
			ff.itvl = ave_interval;
			ff.pktcnt = pkt_count;
			ff.pktlen = ave_pkt_size;
			ff.startts = start_timestamp;
			ff.thp = throughput;
			ff.portdst = flow_key.tcp_destination_port;
			ff.portsrc = flow_key.tcp_source_port;
			ff.same_len = same_len;
			if (double(cnt7476777980)/pkt_count>0.8 && ave_pkt_size <= 81 && ave_pkt_size >= 74 && app_ave_pkt_size <= 39 && app_ave_pkt_size >= 32) { ff.is3232 = true;  }
			if (same_len == true && (ave_pkt_size ==60 || ave_pkt_size ==54) && app_ave_pkt_size == 12) { ff.is3234or6 = true; }
			if (double(cnt1053_1069) / double(pkt_count) > 0.3) ff.ispayload = true;
			if (double(cnt_1267_1269) / double(pkt_count) > 0.2) { ff.ispayload = true;}
			if (double(cnt1053_1069) / double(pkt_count) > 0.1 && double(cnt1053_1069) / double(pkt_count)<0.3) ff.ispayload = true;
		}

	
	//out1 << "\n";//8.30test
}

void flow::feature_output(simple_packet_info &pkt, int protocol) 
{
	
	pkt_count++;
	unsigned short ip_header_len = (ntohs(pkt.ip_protocol->ip_header_length) >> 8) * 4;

	packet_size_ls += pkt.packet_len;
	
	app_packet_size_ls += ntohs(pkt.ip_protocol->ip_length) + 14 - pkt.header_len;//9.1
	

	interval_ls += (pkt.ts - latest_timestamp);//减少除法

	this->latest_timestamp = pkt.ts;
	
	if (pkt.packet_len == last_len ) same_len = false;
	if (pkt.packet_len == 74 || pkt.packet_len == 76 || pkt.packet_len == 77 || pkt.packet_len == 79 || pkt.packet_len == 80) cnt7476777980++;
	if (pkt.packet_len >= 1053 && pkt.packet_len <= 1069) cnt1053_1069++;
	if (pkt.packet_len >= 1267 && pkt.packet_len <= 1269) cnt_1267_1269++;
}




std::string flow_key::to_string()const
{	
	return std::to_string(ip_source_address.s_addr) + std::to_string(tcp_source_port) + std::to_string(ip_destination_address.s_addr) + std::to_string(tcp_destination_port) ;
}

bool flow_key::operator<(const flow_key& e) const
{		
	if (this->to_string()<e.to_string())
		return true;
	return false;
}
bool flow_key::operator==(const flow_key& e) const
{		
	if (this->to_string()==e.to_string())
		return true;
	return false;
}
bool flow_key::operator>(const flow_key& e) const
{		
	if (this->to_string()>e.to_string())
		return true;
	return false;
}
