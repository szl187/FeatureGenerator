#include "flow.h"


flow::flow(simple_packet_info& simple_packet_info)
{

	this->flow_key = simple_packet_info.flow_key;
	this->protocol = simple_packet_info.protocol;
	latest_timestamp = start_timestamp= simple_packet_info.ts;
	reverse_latest_timestamp = -1;
	interarrival_time_n = interarrival_time_ls = flow_duration  = used_ts = 0;
	pkt_count = rtt_count = packet_szie_ls = window_size_ls = retrans = ack_no_payload = 0;
	bandwith = appbandwith = throughput = 0;
	ave_pkt_size = app_ave_pkt_size = ave_interval = ave_windows = 0;
	ave_rtt = 0;
	rtt_min = 100;
	rtt_max = -1;
	packet_size_vector = "";
	max_seq = 0;
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
	//if (pkt_count < 10) { j--; return; }
	ipsrc = inet_ntoa(flow_key.ip_source_address);
	ipdst = inet_ntoa(flow_key.ip_destination_address);
	//printf("%d,%lf,%lf\n", pkt_count, duration, pkt_count / duration);
	throughput = pkt_count / duration;
	if(rtt_count>0) ave_rtt = rtt_ls / rtt_count;
	else ave_rtt = 0;
	ave_interval = interval_ls / pkt_count;
	//bandwith = ave_pkt_size / ave_interval;
	bandwith = ave_pkt_size *throughput;
	appbandwith = app_ave_pkt_size * throughput;

	
	
	//if (overtime) out1 << " OVERTIME";
	if (duration != 0) {
		if (csv == 0) {
			if (protocol == 6) out1 << " TCP";
			else if (protocol == 17) out1 << " UDP";
			out1.setf(std::ios::fixed);
			out1.precision(5);
			out1 << "flow" << j << ":" << ipsrc << "->" << ipdst << " " << flow_key.tcp_source_port << "->" << flow_key.tcp_destination_port;

			out1 << "\nstart timestamp:" << start_timestamp << "\n"
				<< "end timestamp:" << latest_timestamp << "\n"
				<< "duration:" << duration << "\n"
				<< "packet count:" << pkt_count << "\n"
				<< "packet size(B):" << ave_pkt_size << "\n"
				<< "interval(s):" << ave_interval << "\n"
				<< "bandwith(B/s):" << bandwith << "\n"
				<< "application layer bandwith(B/s):" << appbandwith << "\n"
				<< "throughput(num/s):" << throughput << "\n";
			if (protocol == 6)
			{
				out1 << "retrans:" << retrans << "\n"
					<< "retrans rate:" << double(retrans) / pkt_count << "\n"
					<< "ack no payload:" << ack_no_payload << "\n";


				out1 << "average rtt(s):" << ave_rtt << "\n"
					<< "max rtt(s):" << rtt_max << "\n"
					<< "min rtt(s):" << rtt_min << "\n";


			}
			out1 << "length of first 10 pkt:" << packet_size_vector << "\n";
			out1 << "\n";
		}
		else {
			//if (ipsrc == "6.6.6.1"){
			//if (ave_pkt_size > 1250) {
			if (pkt_count>10){
				out1 << pkt_count << "," << ave_pkt_size << "," << ave_interval << "," << bandwith << "," << appbandwith << "," << throughput << ",";
				if (protocol == 6)
				{
					double retrans_rate = double(retrans) / pkt_count;
					double rttdif = rtt_max - rtt_min;
					out1 << retrans << "," << retrans_rate << "," << ack_no_payload << ",";
					if(ave_rtt) out1<< ave_rtt << "," << rtt_max << "," << rtt_min << ","<<rttdif<<",";
					else out1 << -100 << "," << -100 << "," << -100 << ","<<-100<<",";
					/*[4.9114200e-05 1.4301200e-02 4.2382600e-02 2.1368925e-01 1.8588300e+01]
[0.00017437 0.00090917 0.00277971 0.0078157  0.092751  ]*/
					if (rttdif > 0) {
						int retrans_score, rttdif_score;
						
						if (rttdif <= 4.9114200e-05) rttdif_score = 5;
						else if (rttdif <= 1.4301200e-02) rttdif_score = 4;
						else if (rttdif <= 4.2382600e-02) rttdif_score = 3;
						else if (rttdif <= 2.1368925e-01) rttdif_score = 2;
						else if (rttdif <= 1.8588300e+01) rttdif_score = 1;
						else rttdif_score = 0;

						if (retrans_rate == 0) retrans_score = 5;
						else if (retrans_rate <= 0.00017437) retrans_score = 4;
						else if (retrans_rate <= 0.00090917) retrans_score = 3;
						else if (retrans_rate <= 0.00277971) retrans_score = 2;
						else if (retrans_rate <= 0.0078157) retrans_score = 1;
						else retrans_score = 0;

						out1 << float(retrans_score + rttdif_score) / 2 << ',';



					}
					else out1 << -100 << ',';

				}
				else out1 << -100 << ',' << -100 << ',' << -100 << ',' << -100 << ',' << -100 << ',' << -100 << ','<< - 100 << "," << -100 << ",";
				//out1 << "length of first 10 pkt:" << packet_size_vector << "\n";
				out1 << packet_size_vector<<"\n";
			}
		}
	}
}

void flow::feature_output(simple_packet_info &pkt, int protocol) 
{
	
	pkt_count++;
	int header_len = 28;
	if (protocol == 6)
	{
		
		ave_windows += (ntohs(pkt.tcp_protocol->tcp_windows) - ave_windows) / pkt_count;//窗口byte
		if (reverse_latest_timestamp != -1 && reverse_latest_timestamp != used_ts)
		{
			used_ts = reverse_latest_timestamp;
			rtt_count++;
			double rtt = pkt.ts - reverse_latest_timestamp;
			//ave_rtt = ave_rtt + (rtt - ave_rtt) / rtt_count;//rtt(参考wireshark的计算方式)
			rtt_ls += rtt;//减少除法
			if (rtt < rtt_min) rtt_min = rtt;
			else if (rtt > rtt_max) rtt_max = rtt;
		}

		//std::cout << ntohl(pkt.tcp_protocol->tcp_acknowledgement) << "\n" ;

		if (ntohs(pkt.ip_protocol->ip_length) - 40 == 0) ack_no_payload++;//传输层0载荷包
	
			
		//if (ntohl(pkt.tcp_protocol->tcp_acknowledgement) == 248226263) std::cout << ntohl(pkt.tcp_protocol->tcp_acknowledgement) << "," << max_seq << "," << int(max_seq - ntohl(pkt.tcp_protocol->tcp_acknowledgement)) << "\n";
		if (ntohl(pkt.tcp_protocol->tcp_acknowledgement) >= max_seq) max_seq = ntohl(pkt.tcp_protocol->tcp_acknowledgement);
		else { retrans++; }
		
		
		header_len = 40;
		
	}

	ave_pkt_size += (pkt.packet_len - ave_pkt_size) / pkt_count;//包长byte
	//app_ave_pkt_size += (ntohs(pkt.ip_protocol->ip_length) - header_len - app_ave_pkt_size) / pkt_count;//应用层长度byte
	app_ave_pkt_size = ave_pkt_size - header_len;
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
