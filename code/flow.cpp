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
	ave_rtt = 0;
	rtt_min = 100;
	rtt_max = -1;
	packet_size_vector = std::to_string(simple_packet_info.packet_len)+',';
	if (protocol == 6) {
		max_seq = ntohl(simple_packet_info.tcp_protocol->tcp_acknowledgement);
		last_flag = (ntohs(simple_packet_info.tcp_protocol->tcp_headerlen_flag) & 3);
	}

	
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
	ave_pkt_size = double(packet_size_ls )/ pkt_count;
	app_ave_pkt_size = ave_pkt_size - double(header_len_ls) / pkt_count;
	throughput = pkt_count / duration;
	if(rtt_count>0) ave_rtt = rtt_ls / rtt_count;
	else ave_rtt = 0;
	if (pkt_count > 1)ave_interval = duration / double(pkt_count - 1);
	else ave_interval = duration;
	bandwith = ave_pkt_size *throughput;
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
			if (protocol == 6) {
				ff.npack = ack_no_payload;
				ff.rtrate = double(retrans) / pkt_count;
				ff.retran = retrans;
				if (ave_rtt) {
					ff.rtt = ave_rtt;
					ff.rttM = rtt_max;
					ff.rttm = rtt_min;
					ff.rttdiff = rtt_max - rtt_min;
				}
			}
		}

	}
	//out1 << "\n";//8.30test
}
void flow::count_rtt(time_type ack_ts, u_int32_t ack) {
	std::map<u_int32_t, time_type>::iterator it;
	it = seq.find(ack);
	if (it != seq.end()) {
		rtt_count++;
		double rtt = ack_ts - it->second;
		rtt_ls += rtt;
		if (rtt < rtt_min) rtt_min = rtt;
		if (rtt > rtt_max) rtt_max = rtt;
		seq.erase(it);
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


	//进行TCP流分析——RTT，重传。重传部分不考虑乱序到达的情况，乱序的包会被识别为重传
	
	
	//插入<下一序号，时间>对。计算rtt时，若反向的包的ack=某个包的下一序号值，则这两个包的时间差构成rtt
	u_int32_t next_seq = ntohl(pkt.tcp_protocol->tcp_acknowledgement) + pkt.packet_len - pkt.header_len;
	//如果当前包是长为0的SYN和ACK，则下一包的序号会+1
	if(next_seq == ntohl(pkt.tcp_protocol->tcp_acknowledgement) && ((ntohs(pkt.tcp_protocol->tcp_headerlen_flag) & 3))) next_seq = next_seq+1;
	seq.insert(std::pair<u_int32_t, time_type>(next_seq, pkt.ts));

	//判断是否IP层载荷是否为0。不使用pkt.packet_len-pkt.headerlen是因为二层可能存在后缀，而headerlen无法统计后缀长度
	if (ntohs(pkt.ip_protocol->ip_length) - pkt.header_len + 14 == 0) ack_no_payload++;

	//若当前包序号大于历史序号，则它不是重传包
	if ((ntohl(pkt.tcp_protocol->tcp_acknowledgement) > max_seq)) max_seq = ntohl(pkt.tcp_protocol->tcp_acknowledgement);

	else {
		//0载荷情况下，若当前包序号等于历史最大序号，若它是FIN或SYN且它的序号和上个包的序号相同，则它是重传包。否则它是一个0载荷的ACK包，且不是重传包
		if ((ntohl(pkt.tcp_protocol->tcp_acknowledgement) == max_seq)) {
			if (ntohs(pkt.ip_protocol->ip_length) - pkt.header_len + 14 == 0 && last_flag > 0 && (ntohs(pkt.tcp_protocol->tcp_headerlen_flag) & 3) == last_flag)
			{
				retrans++;
			}
		}
			//若当前包序号小于历史最大序号，则它是重传包
		else {
			retrans++;
				
		}
	}
	last_flag = (ntohs(pkt.tcp_protocol->tcp_headerlen_flag) & 3);
	

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
