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
	last_len = simple_packet_info.packet_len;
	
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
	//if (time_i <= 36000) {
	ave_pkt_size = double(packet_size_ls )/ pkt_count;
	app_ave_pkt_size = double(app_packet_size_ls) / pkt_count;
		//printf("haha1\n");
	//}
	throughput = pkt_count / duration;
	ave_interval = interval_ls / double(pkt_count);
	//bandwith = ave_pkt_size / ave_interval;
	bandwith = ave_pkt_size *throughput;
	appbandwith = app_ave_pkt_size * throughput;
	
	
	
	if (duration != 0) {
		/*
		if (csv == 0) {
			if ((allowweakflow==0 && (throughput >= thpmin) && pkt_count > 5)||allowweakflow) {
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
					<< "app packet size(B):" << app_ave_pkt_size << "\n"
					<< "interval(s):" << ave_interval << "\n"
					<< "bandwith(B/s):" << bandwith << "\n"
					<< "application layer bandwith(B/s):" << appbandwith << "\n"
					<< "throughput(num/s):" << throughput << "\n";
				if (protocol == 6)
				{
					out1 << "retrans:" << retrans << "\n"
						<< "retrans rate:" << double(retrans) / pkt_count << "\n"
						<< "ack no payload:" << ack_no_payload << "\n";

					if (rtt_max != -1 && rtt_min != 100) {
						out1 << "average rtt(s):" << ave_rtt << "\n"
							<< "max rtt(s):" << rtt_max << "\n"
							<< "min rtt(s):" << rtt_min << "\n";
					}

				}
				out1 << "length of first some pkt:" << packet_size_vector << "\n";
				out1 << "\n";
			}
		}
		else {
		
			
				
				out1 << ipsrc  << ','<<ipdst<<','<< flow_key.tcp_source_port<<','<<flow_key.tcp_destination_port<<',';
				out1 <<std::fixed<<std::setprecision(6)<< start_timestamp << ','<< latest_timestamp<<',';
				out1 << pkt_count << "," << ave_pkt_size <<','<<app_ave_pkt_size<< "," << ave_interval << "," << bandwith << "," << appbandwith << "," << throughput << ",";
				if (protocol == 6)
				{
					double retrans_rate = double(retrans) / pkt_count;
					double rttdif = rtt_max - rtt_min;
					out1 << retrans << "," << retrans_rate << "," << ack_no_payload << ",";
					if(ave_rtt) out1<< ave_rtt << "," << rtt_max << "," << rtt_min << ","<<rttdif<<",";
					else out1 << -100 << "," << -100 << "," << -100 << ","<<-100<<",";

				}
				else out1 << -100 << ',' << -100 << ',' << -100 << ',' << -100  << ',' << -100 << ','<< - 100 << "," << -100 << ",";
				out1 << packet_size_vector<<"\n";
				
				
			}
		}
		*/
		if ( throughput >= thpmin && pkt_count > cntmin && ave_pkt_size>lenmin) {
			
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
	//out1 << "\n";//8.30test
}

void flow::feature_output(simple_packet_info &pkt, int protocol) 
{
	
	pkt_count++;
	
	unsigned short ip_header_len = (ntohs(pkt.ip_protocol->ip_header_length) >> 8) * 4;//9.1
	
	
	packet_size_ls += pkt.packet_len;
	
	app_packet_size_ls += ntohs(pkt.ip_protocol->ip_length) + 14 - pkt.header_len;//9.1
	

	interval_ls += (pkt.ts - latest_timestamp);//减少除法

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
