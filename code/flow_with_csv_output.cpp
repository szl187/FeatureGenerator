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
	last_len = simple_packet_info.packet_len;
	header_len_ls = 0;
	
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
	app_packet_size_ls = ntohs(simple_packet_info.ip_protocol->ip_length) + 14 - simple_packet_info.header_len;//9.1

	bandwith = appbandwith = throughput = 0;
	ave_pkt_size = app_ave_pkt_size = ave_interval = ave_windows = 0;
	ave_rtt = 0;
	rtt_min = 100;
	rtt_max = -1;
	packet_size_vector = std::to_string(simple_packet_info.packet_len) + ',';
	if (protocol == 6) {
		max_seq = ntohl(simple_packet_info.tcp_protocol->tcp_acknowledgement);
		last_flag = (ntohs(simple_packet_info.tcp_protocol->tcp_headerlen_flag) & 3);
	}
	same_len = true;
	last_len = simple_packet_info.packet_len;
	header_len_ls = 0;
	ff.appbw = last_ff.appbw;
	ff.applen = last_ff.applen;
	ff.bw = last_ff.bw;
	ff.itvl = last_ff.itvl;
	ff.pktlen = last_ff.pktlen;
	ff.thp = last_ff.thp;
	this->d = d;

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
}

void flow::terminate(int overtime)
{
	string ipsrc, ipdst;
	double duration = latest_timestamp - start_timestamp;
	//if (pkt_count < 10) { j--; return; }
	ipsrc = inet_ntoa(flow_key.ip_source_address);
	ipdst = inet_ntoa(flow_key.ip_destination_address);
	//printf("%d,%lf,%lf\n", pkt_count, duration, pkt_count / duration);
	//if (time_i <= 36000) {
	ave_pkt_size = double(packet_size_ls )/ pkt_count;
	app_ave_pkt_size = ave_pkt_size - double(header_len_ls) / pkt_count;
		//printf("haha1\n");
	//}
	throughput = pkt_count / duration;
	if(rtt_count>0) ave_rtt = rtt_ls / rtt_count;
	else ave_rtt = 0;
	ave_interval = interval_ls / double(pkt_count);
	//bandwith = ave_pkt_size / ave_interval;
	bandwith = ave_pkt_size *throughput;
	appbandwith = app_ave_pkt_size * throughput;
	
	
	
	if (duration!=0) {
		if (ave_pkt_size < lenmax && throughput >= thpmin && pkt_count > cntmin && ave_pkt_size>lenmin && duration>=time_i*duramin) {
			
			ff.valid = 1;
			strcpy(ff.ipsrc, ipsrc.c_str());
			strcpy(ff.ipdst, ipdst.c_str());
			strcpy(ff.pkt_size_vec, packet_size_vector.c_str());
			if (decay == 0) {
				ff.appbw = appbandwith;
				ff.applen = app_ave_pkt_size;
				ff.bw = bandwith;
				ff.itvl = ave_interval;
				ff.pktlen = ave_pkt_size;
				ff.thp = throughput;
			}
			else {
				ff.appbw = (d*ff.appbw + (1 - d)*appbandwith);
				ff.applen = (d*ff.applen + (1 - d)*app_ave_pkt_size);
				ff.bw = (d*ff.bw + (1 - d)*bandwith);
				ff.itvl = (d*ff.itvl + (1 - d)*ave_interval) ;
				ff.pktlen = (d*ff.pktlen + (1 - d)*ave_pkt_size);
				ff.thp = (d*ff.thp + (1 - d)*throughput);
			}
			ff.pktcnt = pkt_count;
			ff.startts = start_timestamp;
			ff.endts = latest_timestamp;
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
			ff.same_len = same_len;
			ff.score = count_score(ave_pkt_size, ave_interval);
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
		/*
		if (rtt > 0.1391&& rtt < 0.14) { 
		
			std::cout << ack   << "\n"; 
			printf("%lf,%lf", ack_ts,it->second);
		}
		*/
		//ave_rtt = ave_rtt + (rtt - ave_rtt) / rtt_count;//rtt(参考wireshark的计算方式)
		rtt_ls += rtt;//减少除法
		if (rtt < rtt_min) rtt_min = rtt;
		else if (rtt > rtt_max) rtt_max = rtt;
		seq.erase(it);
	}
	
}
void flow::feature_output(simple_packet_info &pkt, int protocol) 
{
	
	pkt_count++;
	//问题 会出现两次pktcnt=1
	//if (pkt_count < 3)printf("%d,%u,%u\n", pkt_count, max_seq, ntohl(pkt.tcp_protocol->tcp_acknowledgement));
	unsigned short ip_header_len = (ntohs(pkt.ip_protocol->ip_header_length) >> 8) * 4;//9.1
	if (protocol == 6)
	{
		//ave_windows += (ntohs(pkt.tcp_protocol->tcp_windows) - ave_windows) / pkt_count;//窗口byte
		seq.insert(std::pair<u_int32_t, time_type>(ntohl(pkt.tcp_protocol->tcp_acknowledgement) + pkt.packet_len - pkt.header_len, pkt.ts));


		if (ntohs(pkt.ip_protocol->ip_length) - ip_header_len == 0) ack_no_payload++;//9.1
		if ((ntohl(pkt.tcp_protocol->tcp_acknowledgement) > max_seq)) max_seq = ntohl(pkt.tcp_protocol->tcp_acknowledgement);
		else { 
			if ((ntohl(pkt.tcp_protocol->tcp_acknowledgement) == max_seq)) {
				if (last_flag==0 && (ntohs(pkt.tcp_protocol->tcp_headerlen_flag) & 3)==last_flag)
				{ 
					retrans++; 
					out1.setf(std::ios::fixed);
					out1.precision(5);
					if (rtinfo) out1 << (flow_key.tcp_source_port) << ',' << ntohl(pkt.tcp_protocol->tcp_acknowledgement) <<','<<pkt.ts <<"\n"; 
				}
			}
			else {
				retrans++;
				out1.setf(std::ios::fixed);
				out1.precision(5);
				if (rtinfo) out1 << (flow_key.tcp_source_port) << ',' << ntohl(pkt.tcp_protocol->tcp_acknowledgement) << ','<<pkt.ts<<"\n";
			}
		}
		last_flag = (ntohs(pkt.tcp_protocol->tcp_headerlen_flag) & 3);
		

		
	}
	/*

		ave_pkt_size += (pkt.packet_len - ave_pkt_size) / pkt_count;//包长byte
		//app_ave_pkt_size += (ntohs(pkt.ip_protocol->ip_length) - header_len - app_ave_pkt_size) / pkt_count;//应用层长度byte
		//int app_len = pkt.packet_len - pkt.header_len;

		app_ave_pkt_size += (pkt.packet_len - pkt.header_len - app_ave_pkt_size) / pkt_count;
	*/
	packet_size_ls += pkt.packet_len;
	//if (flow_key.tcp_destination_port == 3232) std::cout <<pkt_count<<','<< pkt.packet_len << "\n";

	//计算applen方法1
	//app_packet_size_ls += ntohs(pkt.ip_protocol->ip_length) + 14 - pkt.header_len;//9.1

	//计算applen方法2
	header_len_ls += pkt.packet_len - ntohs(pkt.ip_protocol->ip_length) - 14 + pkt.header_len;

	//if (flow_key.tcp_destination_port==3236) {
		//std::cout << ntohs(pkt.ip_protocol->ip_length) <<','<<pkt.header_len <<','<< pkt_count<<"\n";
	//}
	//app_packet_size_ls -= ;//9.1

	interval_ls += (pkt.ts - latest_timestamp);//减少除法
	//ave_interval += ((pkt.ts - latest_timestamp) - ave_interval) / pkt_count;//间隔s
	
	/*
	double duration_temp = pkt.ts - latest_timestamp;
	flow_duration += duration_temp;
	if (flow_duration != 0) {
		bandwith += (ave_pkt_size / flow_duration) + (duration_temp / flow_duration)*bandwith;//带宽byte/s
		appbandwith += (app_ave_pkt_size / flow_duration) + (duration_temp / flow_duration)*appbandwith;//应用层带宽byte/s
	}
	*/
	this->latest_timestamp = pkt.ts;
	
	if (pkt_count <= v) 
	{
		packet_size_vector += std::to_string(pkt.packet_len);
		if (pkt_count <= v - 1) packet_size_vector += ",";
	}
	if (pkt.packet_len != last_len) same_len = false;
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
all_flow::~all_flow()
{

	double dura = latest_timestamp - start_timestamp;
	if (dura > 1e-6) {
		double bw = double(total_pktlen) / dura;
		//double appbw = double(total_app_pktlen) / dura;

		out5 << std::fixed << std::setprecision(6) << start_timestamp << ',' << latest_timestamp << ',' << bw <<  ",\n";
		/*
		std::set<struct flow_key>::iterator it;
		for (it = flow_keys.begin(); it != flow_keys.end(); it++) {
			string ipsrc, ipdst;
			ipsrc = inet_ntoa((*it).ip_source_address);
			ipdst = inet_ntoa((*it).ip_destination_address);
			out5 << ipsrc << "->" << ipdst << ' ' << (*it).tcp_source_port << "->" << (*it).tcp_destination_port << ',';
		}
		out5 << "\n";
		*/
	}
	else out5 << "to short\n";
}