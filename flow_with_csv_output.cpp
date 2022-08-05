#include "flow.h"


flow::flow(simple_packet_info& simple_packet_info)
{

	this->flow_key = simple_packet_info.flow_key;

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
	this->segment_interval_n = 0;
	segment_interval_ls = 0;
	segment_interval_ss = 0;


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
	this->segment_interval_n = 0;
	segment_interval_ls = 0;
	segment_interval_ss = 0;
    this->last_req_time = 0;
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
    

//set fwd data to bwd
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
	printf("judge vedio:%lf,%lf\n",fwd_ave_size,bwd_ave_size);
	if(fwd_ave_size < bwd_ave_size) this->is_vedio=1;
	else  {this->is_vedio=1;this->reverse=1;printf("should reverse\n");this->to_reverse();}
	return;

}


double flow::decay(double decay_factor ,simple_packet_info &pkt)

{
    if (this->flow_key.flow_direction == forward){
        if (this->fwd_packet_szie_n == 0)
        return 1;
	    time_type duration = pkt.ts - this->fwd_latest_timestamp;
	    return pow(2, -decay_factor*duration);
    }
    else{
        if (this->bwd_packet_szie_n == 0)
        return 1;
	    time_type duration = pkt.ts - this->bwd_latest_timestamp;
	    return pow(2, -decay_factor*duration);
    }
}

void flow::add_packet(simple_packet_info &pkt)
{

	double decay = this->decay(1,pkt);

    if (pkt.protocol == 6)
    {
        if (this->flow_key.flow_direction == forward)
            this->fwd_last_TCPseq.push_back(ntohs(pkt.tcp_protocol->tcp_acknowledgement));
        else if (this->flow_key.flow_direction == backward)
            this->bwd_last_TCPseq.push_back(ntohs(pkt.tcp_protocol->tcp_acknowledgement));
    }
    output_packet_c(pkt, decay);


}


void flow::terminate()
{	
	return ;
}

void flow::output_packet_c(simple_packet_info &pkt, double decay)
{
    if (pkt.flow_key.flow_direction == forward){
        this->fwd_pkt_count++;
        if (pkt.packet_len) 
        {
		    if (this->fwd_pkt_count== 1) {
				//若为首个包，则进行初始化工作
		    	this->fwd_start_timestamp = pkt.ts;
		    	this->fwd_latest_timestamp = pkt.ts;
		    	this->fwd_packet_szie_n = 1;
				this->fwd_packet_szie_ls += pkt.packet_len ;
				this->fwd_packet_szie_ss += pkt.packet_len * pkt.packet_len ;
		    }
		    else{
				//非首个包，进行特征提取
				this->fwd_packet_szie_n = decay*this->fwd_packet_szie_n +1;
				this->fwd_packet_szie_ls = pkt.packet_len +this->fwd_packet_szie_ls* decay;
				this->fwd_packet_szie_ss = pkt.packet_len * pkt.packet_len +this->fwd_packet_szie_ss* decay;//效率
		
				
            
            
			}
         
            
        }
        if(this->fwd_pkt_count == 1){
        	this->fwd_interarrival_time_n = 1;
            this->fwd_interarrival_time_ls = 0;
            //this->raw_bwd_interarrival_time_ls += 0;
            this->fwd_interarrival_time_ss = 0;
        }
        else
        {
            this->fwd_interarrival_time_n = decay*this->fwd_interarrival_time_n+1;
            this->fwd_interarrival_time_ls = (pkt.ts - this->fwd_latest_timestamp) +this->fwd_interarrival_time_ls* decay;
            //this->raw_bwd_interarrival_time_ls += (pkt.ts - this->bwd_latest_timestamp);
            this->fwd_interarrival_time_ss = (pkt.ts - this->fwd_latest_timestamp) * (pkt.ts - this->fwd_latest_timestamp) +this->fwd_interarrival_time_ss* decay;
        }
        if (pkt.protocol == 6)//TCP
        {
            this->tcp_count++;
            this->fwd_window_size_n = decay*this->fwd_window_size_n+1;
            this->fwd_window_size_ls = ntohs(pkt.tcp_protocol->tcp_windows)+this->fwd_window_size_ls * decay;
            this->fwd_window_size_ss = uint32_t(ntohs(pkt.tcp_protocol->tcp_windows)) * uint32_t(ntohs(pkt.tcp_protocol->tcp_windows))+this->fwd_window_size_ss * decay;
            for(std::vector<packet_count_type>::iterator i=this->fwd_last_TCPseq.begin();i!=this->fwd_last_TCPseq.end();i++){
                if (ntohs(pkt.tcp_protocol->tcp_acknowledgement) == *i) {this->retrans++; break;}
            }
            if (ntohs(pkt.tcp_protocol->tcp_urgent_pointer)) this->tcp_urgent_count++;
        }
        //不区分包信息后，在此处计算
        //this->fwd_flow_duration = get_fwd_flow_duration();
        if (this->fwd_pkt_count == 1)
        {
            this->fwd_flow_bps = 0;
            this->fwd_flow_pps = 0;
        }
        else
        {
            this->bwd_flow_bps = this->raw_bwd_packet_szie_ls / this->bwd_flow_duration;
            double fwd_duration_temp = pkt.ts - this->fwd_start_timestamp;
            if (fwd_duration_temp==0){
            	this->fwd_flow_bps = 0;
            	this->fwd_flow_pps = 0;
            	this->fwd_flow_duration=0;
            }
            else{
            this->fwd_flow_bps = (pkt.packet_len/fwd_duration_temp)+(this->fwd_flow_duration/fwd_duration_temp)*this->fwd_flow_bps;
            
            this->fwd_flow_duration = fwd_duration_temp; 
            
            this->fwd_flow_pps = this->fwd_pkt_count / this->fwd_flow_duration;
            }
        }
        
        this->flow_tcp_rate = this->tcp_count / (this->fwd_pkt_count + this->bwd_pkt_count);

        this->fwd_latest_timestamp = pkt.ts;
    }




    if (pkt.flow_key.flow_direction == backward){
        this->bwd_pkt_count++;
        if (pkt.packet_len) 
        {
		    if (this->bwd_pkt_count == 1) {
				//out2 << "aSize,vSize,Intercal,aInt,vInt,bandwidth,throughput\n";
		    	this->bwd_start_timestamp = pkt.ts;
		    	this->bwd_latest_timestamp = pkt.ts;
		    	this->bwd_packet_szie_n = 1;
				this->bwd_packet_szie_ls += pkt.packet_len ;
				//this->raw_bwd_packet_szie_ls += pkt.packet_len;
				this->bwd_packet_szie_ss += pkt.packet_len * pkt.packet_len ;
		    }
		    else{
				this->bwd_packet_szie_n = decay*this->bwd_packet_szie_n +1;
				this->bwd_packet_szie_ls = pkt.packet_len +this->bwd_packet_szie_ls* decay;
				//this->raw_bwd_packet_szie_ls += pkt.packet_len;
				this->bwd_packet_szie_ss = pkt.packet_len * pkt.packet_len +this->bwd_packet_szie_ss* decay;//效率

            }
			double last_interval = 0;
			double segment_interval_temp;
			if (this->bwd_pkt_count == 1) {
				this->bwd_interarrival_time_n = 1;
				this->bwd_interarrival_time_ls = 0;
				//this->raw_bwd_interarrival_time_ls += 0;
				this->bwd_interarrival_time_ss = 0;
			}
			else
			{
				segment_interval_temp = (pkt.ts - this->bwd_latest_timestamp);
				last_interval = this->bwd_interarrival_time_ls / this->bwd_interarrival_time_n;
				this->bwd_interarrival_time_n = decay * this->bwd_interarrival_time_n + 1;
				this->bwd_interarrival_time_ls = (pkt.ts - this->bwd_latest_timestamp)  + this->bwd_interarrival_time_ls* decay;
				//this->raw_bwd_interarrival_time_ls += (pkt.ts - this->bwd_latest_timestamp);
				this->bwd_interarrival_time_ss = (pkt.ts - this->bwd_latest_timestamp)  * (pkt.ts - this->bwd_latest_timestamp) + this->bwd_interarrival_time_ss* decay;
				this->bwd_latest_timestamp = pkt.ts;//725
			}
			if (this->bwd_pkt_count == 1)
			{
				this->bwd_flow_bps = 0;
				this->bwd_flow_pps = 0;
			}
			else
			{
				double bwd_duration_temp = pkt.ts - this->bwd_start_timestamp;
				if (bwd_duration_temp == 0) {

					this->bwd_flow_bps = 0;
					this->bwd_flow_pps = 0;
					this->bwd_flow_duration = 0;
				}
				else {
					this->bwd_flow_bps = (pkt.packet_len / bwd_duration_temp) + (this->bwd_flow_duration / bwd_duration_temp)*this->bwd_flow_bps;
					this->bwd_flow_duration = bwd_duration_temp;
					this->bwd_flow_pps = this->bwd_pkt_count / this->bwd_flow_duration;
				}
			}

			if (this->is_vedio == 1) {//统计上行数据包的时间间隔，下行包数间隔，下行总比特间隔
				
				
				//std::cout << segment_interval_temp << ',' << last_interval * 5 << std::endl;
				if (segment_interval_temp > last_interval * segInterval) {//725还可进一步优化为间隔大于仅计算非段间隔的间隔的平均的十倍segment_interval_temp  > ave_interval * 2
					//printf("%d", segment_count);
					this->segment_count++;
					
					if (this->segment_count == 1) //first segment
					{
						if (lite == 0) out1 << "序号,时间,平均包长,平均间隔,带宽,吞吐量,平均段内包数,平均段比特数,平均段持续时间,平均段间隔,段内平均包长,段内带宽,\n";
						this->last_req_time = pkt.ts;
						this->last_pkt_num_temp = 0;
						this->segment_interval_n = 1;
						this->segment_interval_ls = 0;
						this->segment_interval_ss = 0;
					}
					else if (this->segment_count > 1 && this->segment_pktnum_temp>=1)//this->segment_count > 1
					{
						
							time_type req_interval_temp = pkt.ts - this->last_req_time;
							//std::cout << "req_interval_temp," << req_interval_temp <<" segment duratemp," << segment_duration_temp << std::endl;
							//std::cout << "1.5," << this->segment_pktnum_temp;
							time_type ave_interval_temp = req_interval_temp / this->segment_pktnum_temp;
							//std::cout << "2" << ave_interval_temp;
							double segment_decay = 1;
							if (seg_decay == 1)	segment_decay = pow(2, -ave_interval_temp);
							//std::cout << "3" << segment_decay;
							this->segment_pktnum_n = segment_decay * this->segment_pktnum_n + 1;
							this->segment_pktnum_ls = segment_decay * this->segment_pktnum_ls + this->segment_pktnum_temp;
							this->segment_pktnum_ss = segment_decay * this->segment_pktnum_ss + this->segment_pktnum_temp * this->segment_pktnum_temp;
							this->segment_size_n = segment_decay * this->segment_size_n + 1;
							this->segment_size_ls = segment_decay * this->segment_size_ls + this->segment_size_temp;
							this->segment_size_ss = segment_decay * this->segment_size_ss + this->segment_size_temp * this->segment_size_temp;
							this->segment_duration_n = segment_decay * this->segment_duration_n + 1;
							this->segment_duration_ls = segment_decay * this->segment_duration_ls + this->segment_duration_temp;
							this->segment_duration_ss = segment_decay * this->segment_duration_ss + this->segment_duration_temp * this->segment_duration_temp;
							this->segment_req_interval_n = segment_decay * this->segment_req_interval_n + 1;
							this->segment_req_interval_ls = segment_decay * this->segment_req_interval_ls + req_interval_temp;
							this->segment_req_interval_ss = segment_decay * this->segment_req_interval_ss + req_interval_temp * req_interval_temp;
							this->segment_interval_n = segment_decay * this->segment_interval_n + 1;
							this->segment_interval_ls = segment_decay * this->segment_interval_ls + segment_interval_temp;
							this->segment_interval_ss = segment_decay * this->segment_interval_ss + segment_interval_temp * segment_interval_temp;


							double ave_segment_pktnum = this->segment_pktnum_ls / this->segment_pktnum_n;
							//std::cout << "4" << ave_segment_pktnum;
							//double vari_segment_pktnum = abs(this->segment_pktnum_ss / this->segment_pktnum_n - (this->segment_pktnum_ls / this->segment_pktnum_n)*(this->segment_pktnum_ls / this->segment_pktnum_n));

							double ave_segment_size = this->segment_size_ls / this->segment_size_n;
							//std::cout << "5" << ave_segment_size;
							//double vari_segment_size = abs(this->segment_size_ss / this->segment_size_n - (this->segment_size_ls / this->segment_size_n)*(this->segment_size_ls / this->segment_size_n));

							double ave_segment_duration = this->segment_duration_ls / this->segment_duration_n;
							//std::cout << "6" << ave_segment_duration;
							//double vari_segment_duration = abs(this->segment_duration_ss / this->segment_duration_n - (this->segment_duration_ls / this->segment_duration_n)*(this->segment_duration_ls / this->segment_duration_n));

							double ave_segment_req_interval = this->segment_req_interval_ls / this->segment_req_interval_n;
							//std::cout << "7" << ave_segment_req_interval;
							//double vari_segment_req_interval = abs(this->segment_req_interval_ss / this->segment_req_interval_n - (this->segment_req_interval_ls / this->segment_req_interval_n)*(this->segment_req_interval_ls / this->segment_req_interval_n));


							double segment_ave_pktsize = this->segment_size_temp / this->segment_pktnum_temp;
							//std::cout << "8" << segment_ave_pktsize;
							double segment_bandwith = this->segment_size_temp / req_interval_temp;//717
							//std::cout << "9" << segment_bandwith;


							double ave_pkt_size = this->bwd_packet_szie_ls / this->bwd_packet_szie_n;
							//std::cout << "10" << ave_pkt_size;
							//double vari_pkt_size = abs(this->bwd_packet_szie_ss / this->bwd_packet_szie_n - (this->bwd_packet_szie_ls / this->bwd_packet_szie_n)*(this->bwd_packet_szie_ls / this->bwd_packet_szie_n));

							double ave_interval = this->bwd_interarrival_time_ls / this->bwd_interarrival_time_n;
							//std::cout << "11" << ave_interval;
							//double vari_interval = abs(this->bwd_interarrival_time_ss / this->bwd_interarrival_time_n - (this->bwd_interarrival_time_ls / this->bwd_interarrival_time_n)*(this->bwd_interarrival_time_ls / this->bwd_interarrival_time_n));

							double ave_segment_interval = this->segment_interval_ls / this->segment_interval_n;
							

							int stall_event = 0;
							if (this->last_pkt_num_temp - this->segment_pktnum_temp > 6) stall_event = 1;

							

							out1<<pkt.id <<','<< pkt.ts1.tv_sec - 1653640000 << '.' << pkt.ts1.tv_usec / 1000 <<','<< ave_pkt_size << ',' << ave_interval << ',' << this->bwd_flow_bps << ',' << this->bwd_flow_pps << ',' << ave_segment_pktnum << ',' << ave_segment_size << ',' << ave_segment_req_interval << ','<<ave_segment_interval << ',' << segment_ave_pktsize << ',' << segment_bandwith;// << ave_segment_req_interval;// << ',' << stall_event;

							//ctime((const time_t*)&(pkt.ts1.tv_sec))

							if (label == "null" || label == "")
							{
								out1 << ",\n";
							}
							else out1 << ',' << label << ",\n";

							this->segment_pktnum_temp = 0;//725
							this->segment_size_temp = 0;
							this->segment_duration_temp = 0;
							this->last_req_time = pkt.ts;
							this->last_pkt_num_temp = this->segment_pktnum_temp;
						

					}
					

				}
				
				else if (segment_count >= 1)
				{
					this->segment_pktnum_temp += 1;
					this->segment_size_temp += pkt.packet_len;
					this->segment_duration_temp += pkt.ts - this->last_req_time;
				}
				
				
			}
            
        }
       
        
        if (pkt.protocol == 6)//TCP
        {
            this->tcp_count++;
            this->bwd_window_size_n = decay*this->bwd_window_size_n+ 1;
            this->bwd_window_size_ls = ntohs(pkt.tcp_protocol->tcp_windows) + (this->bwd_window_size_ls * decay);
            this->bwd_window_size_ss = uint32_t(ntohs(pkt.tcp_protocol->tcp_windows)) * uint32_t(ntohs(pkt.tcp_protocol->tcp_windows)) + this->bwd_window_size_ss * decay;
            
            
            

            for(std::vector<packet_count_type>::iterator i=this->bwd_last_TCPseq.begin();i!=this->bwd_last_TCPseq.end();i++){
                if (ntohs(pkt.tcp_protocol->tcp_acknowledgement) == *i) {this->retrans++; break;}
            }
            if (ntohs(pkt.tcp_protocol->tcp_urgent_pointer)) this->tcp_urgent_count++;
        }
        //if(pkt.ts == this->bwd_start_timestamp)
        //什么时候初始化？ OK
        
        this->flow_tcp_rate = this->tcp_count / (this->fwd_pkt_count + this->bwd_pkt_count);
    }
    
	if(this->bwd_pkt_count==100 && this->is_judged == 0) judge_vedio();
}

/*
void flow::get_data()
{
    //# Basic IP information
        //"src_ip": self.src_ip,
        //"dst_ip" : self.dest_ip,
        //"src_port" : self.src_port,
        //"dst_port" : self.dest_port,
        //"protocol" : self.protocol,
        // 
    //this->flow_key.ip_source_address.s_addr;
    //this->flow_key.ip_destination_address.s_addr;
    //this->flow_key.tcp_source_port;
    //this->flow_key.tcp_destination_port;
    //this->ip_protocol;

    //# Basic information from packet times
    //    "timestamp": packet_time.get_time_stamp(),
    //    "fwd_flow_duration" : 1e6 * packet_time.get_duration(),
    //    "flow_byts_s" : flow_bytes.get_rate(),
    //    "flow_pkts_s" : packet_count.get_rate(),
    //    "fwd_pkts_s" : packet_count.get_rate(PacketDirection.FORWARD),
    //    "bwd_pkts_s" : packet_count.get_rate(PacketDirection.REVERSE),

    
    if (this->pkt_count == 0)
        return;
    
    //printf("this->simple_packets_info.size()= %d\n", this->simple_packets_info.size());
    time_type timestamp = this->simple_packets_info[0].ts;
    time_type fwd_flow_duration=get_fwd_flow_duration();
    double flow_byts_s=get_flow_byts_s(fwd_flow_duration);
    double flow_pkts_s=get_flow_pkts_s(fwd_flow_duration);
    double fwd_pkts_s = get_flow_byts_s(fwd_flow_duration,1,forward);
    double bwd_pkts_s = get_flow_byts_s(fwd_flow_duration,0,backward);

    //# Count total packets by direction
    //    "tot_fwd_pkts": packet_count.get_total(PacketDirection.FORWARD),
    //    "tot_bwd_pkts" : packet_count.get_total(PacketDirection.REVERSE),
    //    # Statistical info obtained from Packet lengths
    //    "totlen_fwd_pkts": packet_length.get_total(PacketDirection.FORWARD),
    //    "totlen_bwd_pkts" : packet_length.get_total(PacketDirection.REVERSE),
    //    "fwd_pkt_len_max" : float(packet_length.get_max(PacketDirection.FORWARD)),
    //    "fwd_pkt_len_min" : float(packet_length.get_min(PacketDirection.FORWARD)),
    //    "fwd_pkt_len_mean" : float(packet_length.get_mean(PacketDirection.FORWARD)),
    //    "fwd_pkt_len_std" : float(packet_length.get_std(PacketDirection.FORWARD)),
    //    "bwd_pkt_len_max" : float(packet_length.get_max(PacketDirection.REVERSE)),
    //    "bwd_pkt_len_min" : float(packet_length.get_min(PacketDirection.REVERSE)),
    //    "bwd_pkt_len_mean" : float(packet_length.get_mean(PacketDirection.REVERSE)),
    //    "bwd_pkt_len_std" : float(packet_length.get_std(PacketDirection.REVERSE)),
    //    "pkt_len_max" : packet_length.get_max(),
    //    "pkt_len_min" : packet_length.get_min(),
    //    "pkt_len_mean" : float(packet_length.get_mean()),
    //    "pkt_len_std" : float(packet_length.get_std()),
    //    "pkt_len_var" : float(packet_length.get_var()),
    //    "fwd_header_len" : flow_bytes.get_forward_header_bytes(),
    //    "bwd_header_len" : flow_bytes.get_reverse_header_bytes(),
    //    "fwd_seg_size_min" : flow_bytes.get_min_forward_header_bytes(),
    //    "fwd_act_data_pkts" : packet_count.has_payload(PacketDirection.FORWARD),

    printf("%u,%u,%u,%u,%u,%f,%f,%f,%f,%f\n", this->flow_key.ip_source_address.s_addr,
        this->flow_key.ip_destination_address.s_addr,
        this->flow_key.tcp_source_port,
        this->flow_key.tcp_destination_port,
        this->ip_protocol,
        timestamp,
        fwd_flow_duration,
        flow_byts_s,
        flow_pkts_s,
        fwd_pkts_s,
        bwd_pkts_s
    );
}

time_type flow::get_fwd_flow_duration()
{
    time_type min=1e10, max=-1;
    for (unsigned i = 0; i < this->simple_packets_info.size(); i++) {
        if (min > this->simple_packets_info[i].ts)
            min = this->simple_packets_info[i].ts;
        if (max < this->simple_packets_info[i].ts)
            max = this->simple_packets_info[i].ts;
        
    }
    return max - min;
}

double flow::get_flow_byts_s(time_type fwd_flow_duration, unsigned flag , FlowDirection packet_direction)
{
    if (fwd_flow_duration == 0)
        return 0;
    double sum = 0;
    if (flag == 0) {
        for (unsigned i = 0; i < this->simple_packets_info.size(); i++) {
            sum += this->simple_packets_info[i].packet_len;

        }
        return sum / fwd_flow_duration;
    }    
    for (unsigned i = 0; i < this->simple_packets_info.size(); i++) {
        if(this->simple_packets_info[i].flow_direction==packet_direction)
            sum += this->simple_packets_info[i].packet_len;

    }
    return sum / fwd_flow_duration;
}

double flow::get_flow_pkts_s(time_type fwd_flow_duration)
{
    return this->simple_packets_info.size() / fwd_flow_duration;    
}
*/


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
