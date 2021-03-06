#pragma once
#include<pcap.h>
#include<vector>
#include <unordered_map>
#include<cmath>
#include<string>
#include<fstream>
#include<iostream>
#include<sstream>
//#include "DTree.hpp"
//#include "CDTree.hpp"
//#include "RFCSV.hpp"
#include <ctime>
#define EXPIRED_UPDATE 40

//定义连续的时间范围内
#define CLUMP_TIMEOUT 1
//连续发送多个包的最小值
#define BULK_BOUND 4

#define ACTIVE_TIMEOUT 0.005
using std::string;

typedef int packet_count_type;
typedef double time_type;

extern std::string label;
extern std::ofstream out1, out2, out3;
extern int raw, seg_decay, lite,segInterval;
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
	int id;
	bpf_u_int32 packet_len;	/* length of this packet (off wire) */
	FlowDirection flow_direction;

    int protocol;

	struct ip_header* ip_protocol;
	struct tcp_header* tcp_protocol;
    struct udp_header* udp_protocol;
};

class flow
{
   

    
public:

	flow(struct simple_packet_info &simple_packet_info);
	struct flow_key flow_key;	
	int reverse, is_vedio,is_judged;
    u_int8_t ip_protocol;
	int bwd_start_sec;
	//RFCSV<MatrixXf, MatrixXi> readcsv;
	string headerString = "id,t,pnb,riu,l";
    //forward flow character
    time_type fwd_latest_timestamp;
    std::vector<packet_count_type> fwd_last_TCPseq;
    packet_count_type raw_fwd_packet_szie_ls;
	time_type fwd_interarrival_time_n, fwd_interarrival_time_ls, fwd_interarrival_time_ss; 
	double fwd_packet_szie_n, fwd_packet_szie_ls,  fwd_packet_szie_ss;
	double fwd_window_size_n, fwd_window_size_ls, fwd_window_size_ss;
	double fwd_flow_bps, fwd_flow_pps;
	time_type fwd_flow_duration;
    time_type fwd_start_timestamp;

    //backward flow character
    time_type bwd_latest_timestamp;
    std::vector<packet_count_type> bwd_last_TCPseq;
    packet_count_type raw_bwd_packet_szie_ls;
    time_type bwd_interarrival_time_n,raw_bwd_interarrival_time_ls, bwd_interarrival_time_ls, bwd_interarrival_time_ss; 
	double bwd_packet_szie_n, bwd_packet_szie_ls, bwd_packet_szie_ss;
	double bwd_window_size_n, bwd_window_size_ls, bwd_window_size_ss;
	double bwd_flow_bps, bwd_flow_pps;
	time_type bwd_flow_duration;
    time_type bwd_start_timestamp;

    //segment character
    packet_count_type segment_count, segment_size_temp, segment_pktnum_temp;
	double segment_pktnum_n, segment_pktnum_ls, segment_pktnum_ss;
    double segment_size_n, segment_size_ls,segment_size_ss;
	double segment_interval_n, segment_interval_ls, segment_interval_ss;
    time_type segment_req_interval_n, segment_req_interval_ls, segment_req_interval_ss;
    time_type last_req_time, segment_duration_temp,segment_duration_n,segment_duration_ls,segment_duration_ss;
	packet_count_type last_pkt_num_temp;

    //flow character related
    
    double flow_tcp_rate;
    packet_count_type retrans; 
    packet_count_type tcp_count, fwd_pkt_count, bwd_pkt_count;
    packet_count_type tcp_urgent_count;


    void add_packet(simple_packet_info &pkt);
	//flow_process中调用，计算新到包的延迟，记录seq并调用output_packet_c计算特征
	double decay(double decay_factor ,simple_packet_info &pkt);//计算延迟系数
	void output_packet_c(simple_packet_info &pkt,double decay);//计算特征并输出
	void terminate();//输出流特征并释放空间（未完成）
	void to_reverse();//将流反向并重置流属性
	void judge_vedio();//判断流是否反向

    /*
    void get_data();
    */

};
