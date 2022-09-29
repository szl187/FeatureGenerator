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
	bandwith = appbandwith = throughput = 0;
	ave_pkt_size = app_ave_pkt_size = ave_interval = ave_windows = 0;
	ave_rtt = 0;
	rtt_min = 100;
	rtt_max = -1;
	packet_size_vector = std::to_string(simple_packet_info.packet_len)+',';
	if (protocol == 6) {
		max_seq = ntohl(simple_packet_info.tcp_protocol->tcp_acknowledgement);
		last_flag = (ntohs(simple_packet_info.tcp_protocol->tcp_headerlen_flag) & 3);
		seq.insert( std::pair<u_int32_t, time_type>(ntohl(simple_packet_info.tcp_protocol->tcp_acknowledgement) + simple_packet_info.packet_len - simple_packet_info.header_len, simple_packet_info.ts));
		
	}
	
	//tx
	udpnopayload_cnt = 0;
	cnt26=cnt74=cnt60= cnt100down=cnt54=cnt78=cnt82=0;
	cnt1000up = 0;
	avelen1000up = 0;
}

flow::flow(simple_packet_info& simple_packet_info, struct flow_feature last_ff, double d)
{

	this->flow_key = simple_packet_info.flow_key;
	this->protocol = simple_packet_info.protocol;
	latest_timestamp = simple_packet_info.ts;
	start_timestamp = simple_packet_info.ts;
	reverse_latest_timestamp = -1;
	interarrival_time_n = interarrival_time_ls = flow_duration = used_ts = 0;
	pkt_count = 1;
	rtt_count = window_size_ls = retrans = ack_no_payload = 0;
	packet_size_ls = simple_packet_info.packet_len;
	header_len_ls = simple_packet_info.packet_len - ntohs(simple_packet_info.ip_protocol->ip_length) - 14 + simple_packet_info.header_len;

	bandwith = appbandwith = throughput = 0;
	ave_pkt_size = app_ave_pkt_size = ave_interval = ave_windows = 0;
	ave_rtt = 0;
	rtt_min = 100;
	rtt_max = -1;
	packet_size_vector = std::to_string(simple_packet_info.packet_len) + ',';
	if (protocol == 6) {
		max_seq = ntohl(simple_packet_info.tcp_protocol->tcp_acknowledgement);
		last_flag = (ntohs(simple_packet_info.tcp_protocol->tcp_headerlen_flag) & 3);
		seq.insert(std::pair<u_int32_t, time_type>(ntohl(simple_packet_info.tcp_protocol->tcp_acknowledgement) + simple_packet_info.packet_len - simple_packet_info.header_len, simple_packet_info.ts));
	}
	
	ff.appbw = last_ff.appbw;
	ff.applen = last_ff.applen;
	ff.bw = last_ff.bw;
	ff.itvl = last_ff.itvl;
	ff.pktlen = last_ff.pktlen;
	ff.thp = last_ff.thp;
	this->d = d;

	ff.histroyBW = last_ff.histroyBW;
	//tx
	udpnopayload_cnt = 0;
	cnt26 = cnt74 = cnt60 = cnt100down = cnt54 = cnt78 = cnt82 = 0;
	cnt1000up = 0;
	avelen1000up = 0;
}


void flow::add_packet(simple_packet_info &pkt)
{
	feature_output(pkt, pkt.protocol);
}

/*
float flow::count_score(double pktlen, double itvl) {
	float s1, s2;
	if (pktlen > 1400) s1 = 2.5;
	else if (pktlen > 1100) s1 = 2;
	else if (pktlen > 800) s1 = 1.5;
	else if (pktlen > 500) s1 = 1;
	else if (pktlen > 200) s1 = 0.5;
	else s1 = 0;

	if (itvl > 1) s2 = 0;
	else if (itvl > 1 >> 2) s2 = 0.5;
	else if (itvl > 1 >> 3) s2 = 1;
	else if (itvl > 1 >> 4) s2 = 1.5;
	else if (itvl > 1 >> 5) s2 = 2;
	else s2 = 2.5;

	return s1 + s2;
}*/

void flow::terminate()
{
	string ipsrc, ipdst;
	double duration = latest_timestamp - start_timestamp;
	ipsrc = inet_ntoa(flow_key.ip_source_address);
	ipdst = inet_ntoa(flow_key.ip_destination_address);
	ave_pkt_size = double(packet_size_ls )/ pkt_count;

	double ave_app_size = ave_pkt_size - double(header_len_ls) / pkt_count;

	throughput = pkt_count / duration;
	if(rtt_count>0) ave_rtt = rtt_ls / rtt_count;
	else ave_rtt = 0;
	if(pkt_count>1)ave_interval = duration / double(pkt_count-1);
	else ave_interval = duration;
	bandwith = ave_pkt_size *throughput;
	
	
	
	
	if (throughput>9){
		
			
			ff.valid = 1;
			strcpy(ff.ipsrc, ipsrc.c_str());
			strcpy(ff.ipdst, ipdst.c_str());

			
				
				ff.bw = bandwith;
				ff.itvl = ave_interval;
				ff.pktlen = ave_pkt_size;
				ff.thp = throughput;
			
				ff.applen = ave_app_size;
		
			
			ff.portdst = flow_key.tcp_destination_port;
			ff.portsrc = flow_key.tcp_source_port;
			

			//tx
			ff.udpnopayloadrate = double(udpnopayload_cnt) / pkt_count;
			ff.cnt26 = (cnt26);
			ff.cnt60 = (cnt60);
			ff.cnt74 = (cnt74);
			ff.cnt54 = cnt54;
			ff.cnt78 = cnt78;
			ff.cnt82 = cnt82;
			ff.cnt100down = (cnt100down);
			if (cnt1000up >= 1)
				ff.avelen1000up = (double)avelen1000up / cnt1000up;
			else ff.avelen1000up = 0;
			
		}

	

}

void flow::feature_output(simple_packet_info &pkt, int protocol) 
{

	if (pkt.packet_len > 1000) {
		cnt1000up++;
		avelen1000up += pkt.packet_len;
	}

	pkt_count++;
	//如果是无载荷包，则不统计特征
	/*
	if (ntohs(pkt.ip_protocol->ip_length) - pkt.header_len + 14 == 0) { udpnopayload_cnt++; return; }
	

	if (pkt.packet_len <= 200) {
		if (ntohs(pkt.ip_protocol->ip_length) - pkt.header_len + 14 == 74) cnt74++;
		if (pkt.packet_len-pkt.header_len==60) { cnt60++; }
		if (pkt.packet_len - pkt.header_len == 26) cnt26++;
		if (pkt.packet_len - pkt.header_len == 54) cnt54++;
		if (pkt.packet_len - pkt.header_len == 78) cnt78++;
		if (pkt.packet_len - pkt.header_len == 82) cnt82++;
		if (pkt.packet_len - pkt.header_len <= 100) cnt100down++;

		return;
	}
	*/
	//包数+1
	
	
	//计算包长总和，首部长度总和
	packet_size_ls += pkt.packet_len;
	header_len_ls += pkt.packet_len - ntohs(pkt.ip_protocol->ip_length) - 14 + pkt.header_len;

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
