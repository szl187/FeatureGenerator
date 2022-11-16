#include "flow.h"


flow::flow(simple_packet_info& simple_packet_info)
{

	this->flow_key = simple_packet_info.flow_key;
	this->protocol = simple_packet_info.protocol;
	latest_timestamp = simple_packet_info.ts;
	start_timestamp= simple_packet_info.ts;
	reverse_latest_timestamp = -1;
	interarrival_time_n = interarrival_time_ls = flow_duration  = used_ts = 0;
	pkt_count = 1;
	rtt_count = window_size_ls = retrans = ack_no_payload = 0;
	packet_size_ls = simple_packet_info.packet_len;
	header_len_ls = simple_packet_info.packet_len - ntohs(simple_packet_info.ip_protocol->ip_length) - 14 + simple_packet_info.header_len;
	bandwith  = throughput = 0;
	ave_pkt_size = ave_interval = ave_windows = 0;
	udpnopayload_cnt = 0;
	cnt1000up = 0;
	avelen1000up = 0;
}

void flow::add_packet(simple_packet_info &pkt)
{
	feature_output(pkt, pkt.protocol);
}



void flow::terminate()
{
	
	string ipsrc, ipdst;
	double duration = latest_timestamp - start_timestamp;
	ipsrc = inet_ntoa(flow_key.ip_source_address);
	ipdst = inet_ntoa(flow_key.ip_destination_address);

	//计算特征值
	ave_pkt_size = double(packet_size_ls )/ pkt_count;
	if (latest_timestamp == start_timestamp) throughput = 0;
	else throughput = pkt_count / duration;
	if(pkt_count>1)ave_interval = duration / double(pkt_count-1);
	else ave_interval = duration;
	bandwith = ave_pkt_size *throughput;
	
	//特征写入flow_feature
	ff.valid = 1;
	strcpy(ff.ipsrc, ipsrc.c_str());
	strcpy(ff.ipdst, ipdst.c_str());
	ff.bw = bandwith;
	ff.itvl = ave_interval;
	ff.pktlen = ave_pkt_size;
	ff.thp = throughput;
	ff.portdst = flow_key.tcp_destination_port;
	ff.portsrc = flow_key.tcp_source_port;
	if (cnt1000up >= 1)
		ff.avelen1000up = (double)avelen1000up / cnt1000up;
	else ff.avelen1000up = 0;
	//tx
	ff.udpnopayloadrate = double(udpnopayload_cnt) / pkt_count;
	
			


	

}

void flow::feature_output(simple_packet_info &pkt, int protocol) 
{

	if (pkt.packet_len > 1000) {
		cnt1000up++;
		avelen1000up += pkt.packet_len;
	}
	pkt_count++;
	
	//计算包长总和，首部长度总和
	packet_size_ls += pkt.packet_len;

	//更新最后时间戳
	this->latest_timestamp = pkt.ts;

}




std::string flow_key::to_string()const
{	
	return std::to_string(ip_source_address.s_addr) +","+ std::to_string(tcp_source_port)+"," + std::to_string(ip_destination_address.s_addr) +','+ std::to_string(tcp_destination_port) ;
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
