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
	header_len_ls = simple_packet_info.packet_len - ntohs(simple_packet_info.ip_protocol->ip_length) - 14 + simple_packet_info.header_len;
	
	bandwith = appbandwith = throughput = 0;
	ave_pkt_size = app_ave_pkt_size = ave_interval = ave_windows = 0;
	
	packet_size_vector = std::to_string(simple_packet_info.packet_len)+',';
	
	
	
}


/*
rtp_sub_flow* flow::rtp_process(u_int32_t ssrc,int type)
{
	rtp_sub_flow* r = NULL;

	std::map <u_int32_t, rtp_sub_flow>::iterator it;
	it = rtp_sub_flows.find(ssrc);
	if (it == rtp_sub_flows.end())
	{
		r = new rtp_sub_flow(ssrc,type);
		rtp_sub_flows.insert(std::pair<u_int32_t, rtp_sub_flow>(ssrc, *r));
	}
	else r = &(it->second);
	return r;

}
*/

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
	ave_pkt_size = double(packet_size_ls) / pkt_count;
	app_ave_pkt_size = ave_pkt_size - double(header_len_ls) / pkt_count;
	throughput = pkt_count / duration;
	if (pkt_count > 1)ave_interval = duration / double(pkt_count - 1);
	else ave_interval = duration;
	bandwith = ave_pkt_size * throughput;
	appbandwith = app_ave_pkt_size * throughput;

	
	
	
	if (duration != 0) {
	
		if (ave_pkt_size < lenmax && throughput >= thpmin && pkt_count > cntmin && ave_pkt_size > lenmin) {
			
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
			
			
		}

	}
	
}

void flow::feature_output(simple_packet_info &pkt, int protocol) 
{
	
	//包数+1
	pkt_count++;

	//计算包长总和，首部长度总和
	packet_size_ls += pkt.packet_len;
	header_len_ls += pkt.packet_len - ntohs(pkt.ip_protocol->ip_length) - 14 + pkt.header_len;//10.10 更新applen的算法

	//更新最后时间戳
	this->latest_timestamp = pkt.ts;

	//记录前v个包的长度
	if (pkt_count <= v)
	{
		packet_size_vector += std::to_string(pkt.packet_len);
		if (pkt_count <= v - 1) packet_size_vector += ",";
	}
	
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
