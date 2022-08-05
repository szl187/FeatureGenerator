#include "flow.h"


flow::flow(simple_packet_info& simple_packet_info)
{

	this->flow_key = simple_packet_info.flow_key;
	this->protocol = simple_packet_info.protocol;
	//std::cout << protocol << std::endl;//8.3
	this->fwd_start_timestamp = simple_packet_info.ts;
	this->reverse = 0;
	this->is_vedio=0;
	this->is_judged=0;
	this->fwd_interarrival_time_n = 0;
	this->fwd_window_size_n = 0;
    this->fwd_packet_szie_n = 0;
    this->fwd_interarrival_time_ls = 0;
    this->fwd_interarrival_time_ss = 0;
    this->fwd_window_size_ls = 0;
    this->fwd_window_size_ss = 0;
    this->fwd_packet_szie_ls = 0;
    this->raw_fwd_packet_szie_ls = 0;
    this->fwd_packet_szie_ss = 0;
    
    this->bwd_start_timestamp = simple_packet_info.ts;
	this->bwd_interarrival_time_n = 0;
	this->bwd_window_size_n = 0;
    this->bwd_packet_szie_n = 0;
    this->bwd_interarrival_time_ls = 0;
    this->raw_bwd_interarrival_time_ls = 0;
    this->bwd_interarrival_time_ss = 0;
    this->bwd_window_size_ls = 0;
    this->bwd_window_size_ss = 0;
    this->bwd_packet_szie_ls = 0;
    this->raw_bwd_packet_szie_ls = 0;
    this->bwd_packet_szie_ss = 0;

    this->tcp_count = 0;
    this->fwd_pkt_count = 0;
    this->bwd_pkt_count = 0;
    this->retrans = 0;
    this->tcp_urgent_count = 0;
	rtt_n = 0;
	rtt_ls = 0;
	rtt_ss = 0;
	rtt2_n = 0;
	rtt2_ls = 0;
	rtt2_ss = 0;
    this->segment_count = 0;
    this->segment_req_interval_n = 0;
    this->segment_req_interval_ls = 0;
    this->segment_req_interval_ss = 0;
    this->segment_size_temp = 0;
    this->segment_size_n = 0;
    this->segment_size_ls = 0;
    this->segment_size_ss = 0;
    this->segment_duration_temp = 0;
    this->segment_duration_n = 0;
    this->segment_duration_ls = 0;
    this->segment_duration_ss = 0;
    this->last_req_time = 0;
	this->segment_pktnum_ls = 0;
	this->segment_pktnum_ss = 0;
	this->segment_pktnum_n = 0;
	this->segment_pktnum_temp = 0;


	fwd_ave_pkt_size = 0;
	fwd_ave_interval = 0;
	fwd_ave_windows = 0;
	fwd_ave_rtt = 0;
	bwd_ave_pkt_size = 0;
	bwd_ave_interval = 0;
	bwd_ave_windows = 0;
	bwd_ave_rtt = 0;
	fwd_count1 = 0;
	fwd_count2 = 0;
	fwd_count3 = 0;
	bwd_count1 = 0;
	bwd_count2 = 0;
	bwd_count3 = 0;
	fwd_rtt_max = bwd_rtt_max = 0.0000001;
	fwd_rtt_min = bwd_rtt_min = 1000;
	fwd_used_ts = bwd_used_ts = -1;
	fwd_app_ave_pkt_size = bwd_app_ave_pkt_size = 0;
	fwd_app_bps = bwd_app_bps = 0;
	fwd_retrans_n=bwd_retrans_n=0;
	fwd_app_size_ls = bwd_app_size_ls = 0;
	fwd_ack_no_payload = bwd_ack_no_payload = 0;
	if (block == 1) block_for_count = 256;
	else block_for_count = 1;
}

void flow::to_reverse(){

//printf("This flow reverse!");
//delete delete segment data

	this->segment_count = 0;
    this->segment_req_interval_n = 0;
	this->segment_req_interval_ls = 0;
    this->segment_req_interval_ss = 0;
    this->segment_size_temp = 0;
    this->segment_size_n = 0;
    this->segment_size_ls = 0;
    this->segment_size_ss = 0;
    this->segment_duration_temp = 0;
    this->segment_duration_n = 0;
    this->segment_duration_ls = 0;
    this->segment_duration_ss = 0;
    this->last_req_time = 0;
    this->segment_pktnum_ls = 0;
	this->segment_pktnum_ss = 0;
	this->segment_pktnum_n = 0;
	this->segment_pktnum_temp = 0;
    //reverse feature
    //save
    time_type fwd_latest_timestampTemp=fwd_latest_timestamp;
	time_type fwd_interarrival_time_nTemp=fwd_interarrival_time_n, fwd_interarrival_time_lsTemp=fwd_interarrival_time_ls, fwd_interarrival_time_ssTemp=fwd_interarrival_time_ss; 
	double fwd_packet_szie_nTemp=fwd_packet_szie_n, fwd_packet_szie_lsTemp=fwd_packet_szie_ls,  fwd_packet_szie_ssTemp=fwd_packet_szie_ss;
	double fwd_window_size_nTemp=fwd_window_size_n, fwd_window_size_lsTemp=fwd_window_size_ls, fwd_window_size_ssTemp=fwd_window_size_ss;
	double fwd_flow_bpsTemp=fwd_flow_bps, fwd_flow_ppsTemp=fwd_flow_pps;
	time_type fwd_flow_durationTemp=fwd_flow_duration;
    time_type fwd_start_timestampTemp=fwd_start_timestamp;
    packet_count_type fwd_pkt_countTemp=fwd_pkt_count;
    

//set bwd data to bwd
	fwd_latest_timestamp=bwd_latest_timestamp;
	fwd_interarrival_time_n= bwd_interarrival_time_n;
	fwd_interarrival_time_ls= bwd_interarrival_time_ls;
	fwd_interarrival_time_ss= bwd_interarrival_time_ss; 
	fwd_packet_szie_n= bwd_packet_szie_n;
	fwd_packet_szie_ls= bwd_packet_szie_ls;
	fwd_packet_szie_ss= bwd_packet_szie_ss;
	fwd_window_size_n= bwd_window_size_n;
	fwd_window_size_ls= bwd_window_size_ls;
	fwd_window_size_ss= bwd_window_size_ss;
	fwd_flow_bps= bwd_flow_bps;
	fwd_flow_pps= bwd_flow_pps;
	fwd_flow_duration = bwd_flow_duration;
	fwd_start_timestamp= bwd_start_timestamp;
	fwd_pkt_count = bwd_pkt_count;

//set bwd data to fwd

	bwd_latest_timestamp=fwd_latest_timestampTemp;
	bwd_interarrival_time_n= fwd_interarrival_time_nTemp;
	bwd_interarrival_time_ls= fwd_interarrival_time_lsTemp;
	bwd_interarrival_time_ss= fwd_interarrival_time_ssTemp; 
	bwd_packet_szie_n= fwd_packet_szie_nTemp;
	bwd_packet_szie_ls= fwd_packet_szie_lsTemp;
	bwd_packet_szie_ss= fwd_packet_szie_ssTemp;
	bwd_window_size_n= fwd_window_size_nTemp;
	bwd_window_size_ls= fwd_window_size_lsTemp;
	bwd_window_size_ss= fwd_window_size_ssTemp;
	bwd_flow_bps= fwd_flow_bpsTemp;
	bwd_flow_pps= fwd_flow_ppsTemp;
	bwd_flow_duration = fwd_flow_durationTemp;
	bwd_start_timestamp= fwd_start_timestampTemp;
	bwd_pkt_count=fwd_pkt_countTemp;

//set flow_key
	struct in_addr tmp = flow_key.ip_source_address;
	flow_key.ip_source_address= flow_key.ip_destination_address;
	flow_key.ip_destination_address=tmp;
	u_int16_t p = flow_key.tcp_source_port;
	flow_key.tcp_source_port = flow_key.tcp_destination_port;
	flow_key.tcp_destination_port =p;
	return;
}

void flow::judge_vedio(){
	//修改，总长大的一方为下行
	this->is_judged=1;
	double fwd_ave_size = this->fwd_packet_szie_ls/this->fwd_packet_szie_n;
	double bwd_ave_size = this->bwd_packet_szie_ls/this->bwd_packet_szie_n;
	//printf("judge vedio:%lf,%lf\n",fwd_ave_size,bwd_ave_size);
	if(fwd_ave_size < bwd_ave_size) this->is_vedio=1;
	else  {this->is_vedio=1;this->reverse=1;/*printf("should reverse\n")*/;this->to_reverse();}
	return;

}


double flow::decay(double decay_factor ,simple_packet_info &pkt)

{
    if (pkt.flow_key.flow_direction == forward){
        if (this->fwd_packet_szie_n == 0)
			return 1;
	    time_type duration = pkt.ts - this->fwd_latest_timestamp;
		//std::cout << "fwd duration:" << duration << std::endl;
	    return pow(2, -decay_factor*duration*100);//
    }
    else{
        if (this->bwd_packet_szie_n == 0)
			return 1;
	    time_type duration = pkt.ts - this->bwd_latest_timestamp;
		//std::cout << "bwd duration:" << duration << std::endl;
	    return pow(2, -decay_factor*duration*100);
    }
}

void flow::add_packet(simple_packet_info &pkt)
{

	double decay = this->decay(1,pkt);
	//std::cout << decay << std::endl;

    if (pkt.protocol == 6)
    {
		tcp_feature_output(pkt,decay);
    }
    //output_packet_c(pkt, decay);


}

void flow::tcp_terminate()
{
	string ipsrc, ipdst;
	u_short tcpsrc, tcpdst;
	double start_ts,end_ts;
	start_ts = fwd_start_timestamp;

	end_ts = fwd_latest_timestamp > bwd_latest_timestamp ? fwd_latest_timestamp : bwd_latest_timestamp;
	//std::cout << fwd_ave_pkt_size << ',' << bwd_ave_pkt_size << std::endl;

	if ((fwd_pkt_count < 2 && bwd_pkt_count < 2) ) return;
	ipsrc = inet_ntoa(flow_key.ip_source_address);
	ipdst = inet_ntoa(flow_key.ip_destination_address);
	if (block == 1) {
		//std::cout << 2;
		fwd_ave_pkt_size += (fwd_packet_szie_ls - fwd_ave_pkt_size * fwd_count3) / fwd_count1;
		fwd_app_ave_pkt_size += (fwd_app_size_ls - fwd_app_ave_pkt_size * fwd_count3) / fwd_count1;
		//std::cout << 3;
		fwd_ave_interval += (fwd_interarrival_time_ls - fwd_ave_interval * fwd_count3) / fwd_count1;
		fwd_ave_windows += (fwd_window_size_ls - fwd_ave_windows * fwd_count3) / fwd_count1;
		double fwd_duration_temp = fwd_latest_timestamp - fwd_count3_start_ts;
		//td::cout << 4;
		this->fwd_flow_duration += fwd_duration_temp;
		this->fwd_flow_bps += (fwd_ave_pkt_size / fwd_flow_duration) + (fwd_duration_temp / fwd_flow_duration)*this->fwd_flow_bps;
		this->fwd_app_bps += (fwd_app_ave_pkt_size / fwd_flow_duration) + (fwd_duration_temp / fwd_flow_duration)*this->fwd_app_bps;
		this->fwd_flow_pps = this->fwd_pkt_count / this->fwd_flow_duration;
		//std::cout << 5;

		bwd_ave_pkt_size += (bwd_packet_szie_ls - bwd_ave_pkt_size * bwd_count3) / bwd_count1;
		bwd_app_ave_pkt_size += (bwd_app_size_ls - bwd_app_ave_pkt_size * bwd_count3) / bwd_count1;
		bwd_ave_interval += (bwd_interarrival_time_ls - bwd_ave_interval * bwd_count3) / bwd_count1;
		//std::cout << 6;

		bwd_ave_windows += (bwd_window_size_ls - bwd_ave_windows * bwd_count3) / bwd_count1;
		double bwd_duration_temp = bwd_latest_timestamp - bwd_count3_start_ts;
		this->bwd_flow_duration += bwd_duration_temp;
		//std::cout << 7;
		this->bwd_flow_bps += (bwd_ave_pkt_size / bwd_flow_duration) + (bwd_duration_temp / bwd_flow_duration)*this->bwd_flow_bps;
		this->bwd_app_bps += (bwd_app_ave_pkt_size / bwd_flow_duration) + (bwd_duration_temp / bwd_flow_duration)*this->bwd_app_bps;
		this->bwd_flow_pps = this->bwd_pkt_count / this->bwd_flow_duration;
	}
	out1 << "flow" << j << ".1,";
	out1 << std::fixed << std::setprecision(5)<< fwd_start_timestamp;
	out1 << ',';
	out1 << std::fixed << std::setprecision(5) << fwd_latest_timestamp;
	out1 << ',' << ipsrc << ',' << ipdst << ','
		<< flow_key.tcp_source_port << ',' << flow_key.tcp_destination_port << ','
		<< fwd_ave_pkt_size << ',' << fwd_ave_interval << ',' << fwd_ave_windows << ','
		<< bwd_ave_rtt << ',' << bwd_rtt_max << ',' << bwd_rtt_min << ',' << fwd_retrans_n <<','<< double(fwd_retrans_n) / fwd_pkt_count<<','
		<<fwd_ack_no_payload <<','<< fwd_flow_bps << ',' << fwd_flow_pps << ','<< std::endl;

	out1 << "flow" << j << ".2,";
	out1 << std::fixed << std::setprecision(5) << bwd_start_timestamp;
	out1 << ',';
	out1 << std::fixed << std::setprecision(5) << bwd_latest_timestamp;
	out1 << ',' << ipdst << ',' << ipsrc << ','
		<< flow_key.tcp_destination_port << ',' << flow_key.tcp_source_port << ','
		<< bwd_ave_pkt_size << ',' << bwd_ave_interval << ',' << bwd_ave_windows << ','
		<< fwd_ave_rtt << ',' << fwd_rtt_max << ',' << fwd_rtt_min << ',' << bwd_retrans_n << ',' << double(bwd_retrans_n) / bwd_pkt_count << ','
		<< bwd_ack_no_payload << ',' << bwd_flow_bps << ',' << bwd_flow_pps << ',' << std::endl;
	
}

void flow::udp_terminate() {}

void flow::terminate()
{
	std::cout << "terminate" << j << std::endl;
	
	if (this->protocol == 6) tcp_terminate();
	else if (this->protocol == 17) udp_terminate();
	return ;
}


/*
struct segment_info
{
	//特征
	packet_count_type seg_pkt_num, seg_byte_cnt;

	//信息
	time_type nxt_seg_req;//下个段段请求的时间戳，即当前上行包的时间戳
	time_type start_time, end_time;//该段第一个和最后一个下行包的时间戳

};
*/
struct segment_info flow::init_segment_info(time_type req)
{
	struct segment_info seg = { 0,0,req,0,0,0,0};
	return seg;
}

void flow::tcp_segment_feature_output(simple_packet_info &pkt)//当判断当前流量为按段传输的视频流量时，计算段特征。逻辑修改为不段特征不计算延迟，而是直接计算（附近三个段的）均值。并且应输出每个段的原始特征。
{
	if (this->segment_count == 0) //第一个段请求，无需输出特征
	{
		if (lite == 1) out1 << "平均包长,平均间隔,平均窗口大小,rtt,rtt方差,带宽,吞吐量,平均段内包数,平均段比特数,平均段持续时间,平均段间隔,\n";
	}
	else //非第一个段请求，计算上一个段请求后的段的特征
	{
		//更新segment_info内部数据
		struct segment_info seg = segments[(segment_count - 1) % SEGMENT_WINDOWS_SIZE];
		segments[(segment_count - 1) % SEGMENT_WINDOWS_SIZE].seg_dur = seg.end_time - seg.start_time;
		segments[(segment_count - 1) % SEGMENT_WINDOWS_SIZE].req_int = pkt.ts - seg.seg_req;
		seg = segments[(segment_count - 1) % SEGMENT_WINDOWS_SIZE];
		
		if (segout == 1) // 向out4输出当前段的原始特征
		{
			out4 << seg.seg_pkt_num << ',' << seg.seg_byte_cnt << ',' << seg.seg_dur << ',' << seg.req_int << ',';
		}

		//统计平均特征
		int n = segment_count < 3 ? segment_count: 3;//统计窗口平均特征时，若窗口不满，则按实际个数计算。
		int total_byte = 0, total_pkt_num = 0;
		double duration_sum = 0, req_int_sum = 0;
		for (int i = 0; i < n; i++) {//计算窗口内段特征的和以计算均值
			total_byte += seg.seg_byte_cnt;
			total_pkt_num += seg.seg_pkt_num;
			duration_sum += seg.seg_dur;
			req_int_sum += seg.seg_req;
		}
		
		//计算段特征和少许流特征
		double ave_segment_pktnum = total_pkt_num / n;
		double ave_segment_size = total_byte / n;
		double ave_segment_duration = duration_sum / n;
		double ave_segment_req_interval = req_int_sum / n;
		double ave_pkt_size = this->bwd_packet_szie_ls / this->bwd_packet_szie_n;
		double ave_interval = this->bwd_interarrival_time_ls / this->bwd_interarrival_time_n;
		double ave_windows = this->bwd_window_size_ls / this->bwd_window_size_n;
		double rtt = this->rtt_ls / this->rtt_n;
		double vari_rtt = abs(this->rtt_ss / this->rtt_n - rtt*rtt);
		

		out1 << /*pkt.ts1.tv_sec - 1653640000 << '.' <<pkt.ts1.tv_usec / 1000 << ',' << */
			ave_pkt_size << ',' << ave_interval << ',' << ave_windows << ','<<rtt<<','<<vari_rtt<<','<< this->bwd_flow_bps << ',' << this->bwd_flow_pps << ','
			<< ave_segment_pktnum << ',' << ave_segment_size << ',' << ave_segment_duration << ',' << ave_segment_req_interval;

		if (label == "null" || label == "")
		{
			out1 << ",\n";
		}
		else out1 << ',' << label << ",\n";

		
	}
	segments[segment_count%SEGMENT_WINDOWS_SIZE] = init_segment_info(pkt.ts);//初始化一个段结构，在后续下行流量到达时不断更新
}

void flow::tcp_feature_output(simple_packet_info &pkt, double decay)//计算TCP特征并输出
{
	if (pkt.flow_key.flow_direction == forward)
	{
		this->fwd_pkt_count++;
		if (this->fwd_pkt_count == 1)//首个上行数据包，进行初始化
		{
			this->fwd_start_timestamp = pkt.ts;
			this->fwd_latest_timestamp = pkt.ts;
			this->fwd_flow_bps = 0;
			this->fwd_flow_pps = 0;
			this->fwd_app_bps = 0;
			this->fwd_flow_duration = 0.01;
		}
		else
		{
			if (fwd_count3 == 0) {
				fwd_count3_start_ts = pkt.ts;
			}
			fwd_count1++;
			fwd_count3++;
			this->fwd_packet_szie_ls += pkt.packet_len;
			this->fwd_app_size_ls += ntohs(pkt.ip_protocol->ip_length) - 54;
			this->fwd_interarrival_time_ls += (pkt.ts - this->fwd_latest_timestamp);
			this->fwd_latest_timestamp = pkt.ts;
			this->fwd_window_size_ls += ntohs(pkt.tcp_protocol->tcp_windows);

			if (fwd_count3 == block_for_count)
			{
				//std::cout << fwd_count1 << std::endl;
				fwd_ave_pkt_size += (fwd_packet_szie_ls - fwd_ave_pkt_size * fwd_count3) / fwd_count1;
				fwd_app_ave_pkt_size += (fwd_app_size_ls - fwd_app_ave_pkt_size * fwd_count3) / fwd_count1;
				fwd_ave_interval += (fwd_interarrival_time_ls - fwd_ave_interval * fwd_count3) / fwd_count1;
				fwd_ave_windows += (fwd_window_size_ls - fwd_ave_windows * fwd_count3) / fwd_count1;
				double fwd_duration_temp = fwd_latest_timestamp - fwd_count3_start_ts;
				this->fwd_flow_duration += fwd_duration_temp;
				this->fwd_flow_bps += (fwd_ave_pkt_size / fwd_flow_duration) + (fwd_duration_temp / fwd_flow_duration)*this->fwd_flow_bps;
				this->fwd_app_bps += (fwd_app_ave_pkt_size / fwd_flow_duration) + (fwd_duration_temp / fwd_flow_duration)*this->fwd_app_bps;
				this->fwd_flow_pps = this->fwd_pkt_count / this->fwd_flow_duration;

				fwd_count3 = 0;
				fwd_window_size_ls = 0;
				fwd_interarrival_time_ls = 0;
				fwd_packet_szie_ls = 0;
				fwd_app_size_ls = 0;
			}
		}
		/*
		if (is_vedio && pkt.packet_len > 120)//包长在50到100，为无载荷的ACK包；包长在150以上，为疑似段请求的TLS包。此处以120为界划分
		{
			tcp_segment_feature_output(pkt);
			segment_count++;
		}

		else*/


		if (fwd_pkt_count > 1 && bwd_pkt_count > 1 && bwd_latest_timestamp != fwd_used_ts)
		{
			fwd_used_ts = bwd_latest_timestamp;
			fwd_count2++;
			double rtt = pkt.ts - bwd_latest_timestamp;
			fwd_ave_rtt = fwd_ave_rtt + (rtt - fwd_ave_rtt) / fwd_count2;
			if (rtt < fwd_rtt_min) fwd_rtt_min = rtt;
			else if (rtt > fwd_rtt_max) fwd_rtt_max = rtt;

		}
		if (ntohs(pkt.ip_protocol->ip_length) - 40 == 0) fwd_ack_no_payload++;
		else if(ntohs(pkt.ip_protocol->ip_length) - 100 <= 0){//不能捕获重发ack的情况，可以考虑外加一个存储ack的vector，同时匹配ack和seq，但可能引入更多误判
			it = std::find(fwd_seq.begin(), fwd_seq.end(), pkt.tcp_protocol->tcp_acknowledgement);
			if (it != fwd_seq.end()) fwd_retrans_n++;
			fwd_seq.push_back(pkt.tcp_protocol->tcp_acknowledgement);
		}

	}

	else//计算下行流量特征
	{
		this->bwd_pkt_count++;
		if (this->bwd_pkt_count == 1)//首个上行数据包，进行初始化
		{
			//初始化时间信息
			this->bwd_start_timestamp = pkt.ts;
			this->bwd_latest_timestamp = pkt.ts;
			//初始化流量特征
			this->bwd_flow_bps = 0;
			this->bwd_flow_pps = 0;
			this->bwd_app_bps = 0;
			this->bwd_flow_duration = 0.1;
		}
		else//非首个包，进行特征计算。该步骤不直接输出特征，而是计算用于计算特征的一些数据。因此在计算特征时可以减少计算量。
		{
			if (bwd_count3 == 0) {
				bwd_count3_start_ts = pkt.ts;
			}
			bwd_count1++;
			bwd_count3++;
			this->bwd_packet_szie_ls += pkt.packet_len;
			this->bwd_app_size_ls += ntohs(pkt.ip_protocol->ip_length) - 54;
			this->bwd_interarrival_time_ls += (pkt.ts - this->bwd_latest_timestamp);
			this->bwd_latest_timestamp = pkt.ts;
			this->bwd_window_size_ls += ntohs(pkt.tcp_protocol->tcp_windows);

			if (bwd_count3 == block_for_count)
			{
				//std::cout << bwd_count1<<std::endl;
				bwd_ave_pkt_size += (bwd_packet_szie_ls - bwd_ave_pkt_size * bwd_count3) / bwd_count1;
				bwd_app_ave_pkt_size += (bwd_app_size_ls - bwd_app_ave_pkt_size * bwd_count3) / bwd_count1;
				bwd_ave_interval += (bwd_interarrival_time_ls - bwd_ave_interval * bwd_count3) / bwd_count1;
				bwd_ave_windows += (bwd_window_size_ls - bwd_ave_windows * bwd_count3) / bwd_count1;
				double bwd_duration_temp = bwd_latest_timestamp - bwd_count3_start_ts;
				this->bwd_flow_duration += bwd_duration_temp;
				this->bwd_flow_bps += (bwd_ave_pkt_size / bwd_flow_duration) + (bwd_duration_temp / bwd_flow_duration)*this->bwd_flow_bps;
				this->bwd_app_bps += (bwd_app_ave_pkt_size / bwd_flow_duration) + (bwd_duration_temp / bwd_flow_duration)*this->bwd_app_bps;
				this->bwd_flow_pps = this->bwd_pkt_count / this->bwd_flow_duration;

				bwd_count3 = 0;
				bwd_window_size_ls = 0;
				bwd_interarrival_time_ls = 0;
				bwd_packet_szie_ls = 0;
				bwd_app_size_ls = 0;
			}
		}
		if (fwd_pkt_count > 1 && bwd_pkt_count > 1 && fwd_latest_timestamp != bwd_used_ts)
		{
			//可改进：1.decay 2.基于ack号计算可避免乱序包的影响
			bwd_used_ts = fwd_latest_timestamp;
			bwd_count2++;
			double rtt = pkt.ts - bwd_latest_timestamp;
			//this->rtt_n = decay * this->rtt_n + 1;
			//this->rtt_ls = rtt + this->rtt_ls* decay;
			//this->rtt_ss = rtt * rtt + this->rtt_ss* decay;
			bwd_ave_rtt = bwd_ave_rtt + (rtt - bwd_ave_rtt) / bwd_count2;
			if (rtt < bwd_rtt_min) bwd_rtt_min = rtt;
			else if (rtt > bwd_rtt_max) bwd_rtt_max = rtt;
		}
		/*
		if (is_vedio && segment_count>=1)//流量方向调整完毕且接受到段请求后，开始统计段特征
		{
			int i = (segment_count - 1) % SEGMENT_WINDOWS_SIZE;
			if (segments[i].seg_pkt_num == 0) 
			{
				segments[i].start_time = pkt.ts;
			}
			segments[i].end_time = pkt.ts;
			segments[i].seg_pkt_num++;
			segments[i].seg_byte_cnt += pkt.packet_len;
		}
		*/
		if (ntohs(pkt.ip_protocol->ip_length) - 54 == 0) bwd_ack_no_payload++;
		else {
			it = std::find(bwd_seq.begin(), bwd_seq.end(), pkt.tcp_protocol->tcp_acknowledgement);
			if (it != bwd_seq.end()) bwd_retrans_n++;
			bwd_seq.push_back(pkt.tcp_protocol->tcp_acknowledgement);
		}
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
