#pragma once
#include<pcap.h>
#include<vector>
#include <unordered_map>
#include<cmath>
#include<string>
#include<fstream>
#include<iostream>
#include<sstream>
#include<iomanip>
//#include "DTree.hpp"
//#include "CDTree.hpp"
//#include "RFCSV.hpp"
#include "time.h"
#define EXPIRED_UPDATE 40
#define SEGMENT_WINDOWS_SIZE 3
//定义连续的时间范围内
#define CLUMP_TIMEOUT 1
//连续发送多个包的最小值
#define BULK_BOUND 4

#define ACTIVE_TIMEOUT 0.005
using std::string;

typedef int packet_count_type;
typedef double time_type;

extern std::string label;
extern std::ofstream out1, out2, out3, out4;
extern int raw, seg_decay, lite, segout, j,block;
//extern  CDTree myCDTree;
enum FlowDirection
{
	forward,backward
};


/*下边是以太网的协议格式 14字节*/
struct ethernet_header
{
    u_int8_t ether_dhost[6];  /*目的以太地址*/
    u_int8_t ether_shost[6];  /*源以太网地址*/
    u_int16_t ether_type;      /*以太网类型*/
};




typedef u_int32_t in_addr_t;

struct ip_header
{
#ifdef WORKS_BIGENDIAN
    u_int8_t ip_version : 4,    /*version:4*/
        ip_header_length : 4; /*IP协议首部长度Header Length*/
#else
    u_int8_t ip_header_length : 4,
        ip_version : 4;
#endif
    u_int8_t ip_tos;         /*服务类型Differentiated Services  Field*/
    u_int16_t ip_length;  /*总长度Total Length*/
    u_int16_t ip_id;         /*标识identification*/
    u_int16_t ip_off;        /*片偏移*/
    u_int8_t ip_ttl;            /*生存时间Time To Live*/
    u_int8_t ip_protocol;        /*协议类型（TCP或者UDP协议）*/
    u_int16_t ip_checksum;  /*首部检验和*/
    struct in_addr  ip_source_address; /*源IP*/
    struct in_addr  ip_destination_address; /*目的IP*/
};

struct udp_header
{
    u_int16_t udp_source_port;
    u_int16_t udp_destination_port;
    u_int16_t udp_length;
    u_int16_t udp_checksum;

};


/*关于tcp头部的定义*/
struct tcp_header
{
    u_int16_t tcp_source_port;		  //源端口号

    u_int16_t tcp_destination_port;	//目的端口号

    u_int32_t tcp_acknowledgement;	//序号

    u_int32_t tcp_ack;	//确认号字段
#ifdef WORDS_BIGENDIAN
    u_int8_t tcp_offset : 4,
        tcp_reserved : 4;
#else
    u_int8_t tcp_reserved : 4,
        tcp_offset : 4;
#endif
    u_int8_t tcp_flags;
    u_int16_t tcp_windows;	//窗口字段
    u_int16_t tcp_checksum;	//检验和
    u_int16_t tcp_urgent_pointer;	//紧急指针字段
};

// RTCP common header word
 struct rtcp_common_header
{
	unsigned char    bVpc;           /// protocol version, padding flag, count
	unsigned char    bPacketType;    ///
	unsigned short    wLen;           /// pkt len in words, w/o this word
};
//RR
typedef struct
{
	unsigned int       dwSsrc;             // data source being reported
	unsigned char        bFraction;          // fraction lost since last SR/RR
	unsigned char        bLost[3];           // cumul. no. pkts lost (signed!)
	unsigned int       dwLastSeq;          // extended last seq. no. received
	unsigned int       dwJitter;           // interarrival jitter
	unsigned int       dwLsr;              // last SR packet from this source
	unsigned int       dwDlsr;             // delay since last SR packet
} RTCP_RR_T;
//SR
typedef struct
{
	unsigned int dwSsrc;           // sender generating this report
	unsigned int dwNtp_sec;        // NTP timestamp    seconds
	unsigned int dwNtp_frac;       //                      fraction
	unsigned int dwRtp_ts;     // RTP timestamp
	unsigned int dwPsent;          // packets sent
	unsigned int dwOsent;          // octets sent
	RTCP_RR_T rr[1];        // variable-length list
} RTCP_SR;

//FB
typedef struct {
	unsigned short wLostSeqNo;
	unsigned short wLostBitMask;
} RTCP_GENERIC_NACK_FB;
typedef struct {
	unsigned int	dwSsrc;                 // data source being reported
	RTCP_GENERIC_NACK_FB nack_fb[3];//8.5 haha
} RTCP_FB;


struct rtcp_header{
	struct rtcp_common_header common;     // common header
	union {
		// sender report (SR)
		RTCP_SR sr;
		// reception report (RR)
		struct {
			unsigned int dwSsrc;// receiver generating this report
			RTCP_RR_T     rr[1];  // variable-length list
		} rr;

		//FB
		struct {
			unsigned int dwSsrc;
			RTCP_FB fb;
		}fb;
	}r;
};















struct flow_key
{
	struct in_addr  ip_source_address; /*源IP*/
	struct in_addr  ip_destination_address; /*目的IP*/
	u_int16_t tcp_source_port;		  //源端口号

	u_int16_t tcp_destination_port;	//目的端口号

	FlowDirection flow_direction;
	
	std::string to_string()const;
	bool operator < (const struct flow_key& e) const;
	bool operator == (const struct flow_key& e) const;
	bool operator > (const struct flow_key& e) const;
};

struct simple_packet_info
{
	struct flow_key flow_key;
	time_type ts;	/* time stamp */
	struct timeval ts1;
	bpf_u_int32 packet_len;	/* length of this packet (off wire) */
	FlowDirection flow_direction;

    int protocol;

	struct ip_header* ip_protocol;
	struct tcp_header* tcp_protocol;
    struct udp_header* udp_protocol;
	struct rtcp_header* rtcp_header;
};

struct segment_info
{
	packet_count_type seg_pkt_num, seg_byte_cnt;
	time_type seg_req;//下个段段请求的时间戳，即当前上行包的时间戳
	time_type start_time, end_time;//该段第一个和最后一个下行包的时间戳
	time_type seg_dur, req_int;
};

class flow
{
   

    
public:

	flow(struct simple_packet_info &simple_packet_info);
	struct flow_key flow_key;	
	int reverse, is_vedio,is_judged;
	int protocol;
	


    //forward flow character
    time_type fwd_latest_timestamp;
    //std::vector<packet_count_type> fwd_last_TCPseq;
    packet_count_type raw_fwd_packet_szie_ls;
	time_type fwd_interarrival_time_n, fwd_interarrival_time_ls, fwd_interarrival_time_ss; 
	double fwd_packet_szie_n, fwd_packet_szie_ls,  fwd_packet_szie_ss;
	double fwd_window_size_n, fwd_window_size_ls, fwd_window_size_ss;
	double fwd_flow_bps, fwd_flow_pps;
	time_type fwd_flow_duration;
    time_type fwd_start_timestamp;
	//8.3
	double fwd_ave_pkt_size, fwd_ave_interval, fwd_ave_windows, fwd_ave_rtt;
	double bwd_ave_pkt_size, bwd_ave_interval, bwd_ave_windows, bwd_ave_rtt;
	int fwd_count1, fwd_count2, fwd_count3, bwd_count1, bwd_count2, bwd_count3;
	double fwd_rtt_min, fwd_rtt_max, bwd_rtt_min, bwd_rtt_max;
	double fwd_used_ts, bwd_used_ts;
	int fwd_app_ave_pkt_size, bwd_app_ave_pkt_size;
	//double fwd_app_bps, bwd_app_bps;
	//8.4
	int fwd_retrans_n, bwd_retrans_n;
	std::vector<u_int32_t> fwd_seq, bwd_seq;
	std::vector<u_int32_t>::iterator it;
	int fwd_app_size_ls, bwd_app_size_ls;
	double fwd_app_bps, bwd_app_bps;
	double fwd_count3_start_ts, bwd_count3_start_ts;
	int fwd_ack_no_payload, bwd_ack_no_payload;
    //backward flow character
    time_type bwd_latest_timestamp;
    //std::vector<packet_count_type> bwd_last_TCPseq;
    packet_count_type raw_bwd_packet_szie_ls;
    time_type bwd_interarrival_time_n,raw_bwd_interarrival_time_ls, bwd_interarrival_time_ls, bwd_interarrival_time_ss; 
	double bwd_packet_szie_n, bwd_packet_szie_ls, bwd_packet_szie_ss;
	double bwd_window_size_n, bwd_window_size_ls, bwd_window_size_ss;
	double bwd_flow_bps, bwd_flow_pps;
	time_type bwd_flow_duration;
    time_type bwd_start_timestamp;
	int block_for_count;
    //segment character
    packet_count_type segment_count, segment_size_temp, segment_pktnum_temp;
	double segment_pktnum_n, segment_pktnum_ls, segment_pktnum_ss;
    double segment_size_n, segment_size_ls,segment_size_ss;
    time_type segment_req_interval_n, segment_req_interval_ls, segment_req_interval_ss;
    time_type last_req_time, segment_duration_temp,segment_duration_n,segment_duration_ls,segment_duration_ss;
	packet_count_type last_pkt_num_temp;
	//新增
	std::vector<struct segment_info> segments;
	
    //flow character related
    
    double flow_tcp_rate;
    packet_count_type retrans; 
    packet_count_type tcp_count, fwd_pkt_count, bwd_pkt_count;
    packet_count_type tcp_urgent_count;
	int rtt_n;
	double rtt_ls, rtt_ss;
	int rtt2_n;
	double rtt2_ls, rtt2_ss;
	//u_int32_t fwd_ack, fwd_seq, bwd_ack, bwd_seq;



    void add_packet(simple_packet_info &pkt);
	//flow_process中调用，计算新到包的延迟，记录seq并调用output_packet_c计算特征
	double decay(double decay_factor ,simple_packet_info &pkt);//计算延迟系数
	void output_packet_c(simple_packet_info &pkt,double decay);//计算特征并输出
	void tcp_feature_output(simple_packet_info &pkt, double decay);
	void tcp_segment_feature_output(simple_packet_info &pkt);
	struct segment_info init_segment_info(time_type req);
	void terminate();//输出流特征并释放空间（未完成）
	void tcp_terminate();
	void udp_terminate();
	void to_reverse();//将流反向并重置流属性
	void judge_vedio();//判断流是否反向

    /*
    void get_data();
    */

};
