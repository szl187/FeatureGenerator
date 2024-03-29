#pragma once
#include<pcap.h>
#include<vector>
//#include <unordered_map>
#include<cmath>
#include<string>
#include<fstream>
#include<iostream>
#include<sstream>
#include<iomanip>
#include<map>
#include <string.h>
#include<set>
//#include "DTree.hpp"
//#include "CDTree.hpp"
//#include "RFCSV.hpp"
//#include "time.h"
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
extern std::ofstream out1, out2, out3, out4,out5;
extern int raw,  j,  v, csv, allowweakflow, time_i,rtinfo, lenmin,cntmin;
extern double thpmin;




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


struct rtcp_header {
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


/*关于tcp头部的定义*/
struct tcp_header
{
	u_int16_t tcp_source_port;		  //源端口号

	u_int16_t tcp_destination_port;	//目的端口号

	u_int32_t tcp_acknowledgement;	//序号

	u_int32_t tcp_ack;	//确认号字段
	/*
#ifdef WORDS_BIGENDIAN
	u_int8_t tcp_offset : 4,
		tcp_reserved : 4;
#else
	u_int8_t tcp_reserved : 4,
		tcp_offset : 4;
#endif
	u_int8_t tcp_flags;
	*/
	u_int16_t tcp_headerlen_flag;
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

	//FlowDirection flow_direction;

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

	int header_len;//2,3,4 header_len
	int protocol;

	struct ip_header* ip_protocol;
	struct tcp_header* tcp_protocol;
	struct udp_header* udp_protocol;
	
};


struct flow_feature
{
	char ipsrc[16];
	char ipdst[16];
	u_int16_t portsrc, portdst;
	double startts, endts;
	int pktcnt;
	double pktlen, applen;
	double itvl, bw, appbw, thp;
	
	double rtrate = -100;
	int npack = -100;
	int retran = -100;
	double rtt = -100;
	double rttM = -100;
	double rttm = -100;
	double rttdiff = -100;
	char pkt_size_vec[128];
	int valid = 0;
	bool operator<(const struct flow_feature &other)const {
		return thp < other.thp;
	}
	bool same_len;
	bool ispayload=false;
	bool is3232=false,is3234or6=false;

};

class flow
{
public:

	flow(struct simple_packet_info &simple_packet_info);
	struct flow_key flow_key;
	struct flow_feature  ff;
	int protocol;
	bool same_len;
	int last_len;

	time_type latest_timestamp, interarrival_time_n, interarrival_time_ls, flow_duration, start_timestamp, used_ts;
	packet_count_type pkt_count,rtt_count, window_size_ls, retrans, ack_no_payload;
	long long int packet_size_ls, app_packet_size_ls;
	double bandwith, appbandwith ,throughput;
	double ave_pkt_size, app_ave_pkt_size,ave_interval, ave_windows, ave_rtt, rtt_min, rtt_max, interval_ls, rtt_ls, pkt_size_ls_right_10;
	std::map<u_int32_t, time_type> seq;
	u_int32_t max_seq;
	std::string packet_size_vector;
	time_type reverse_latest_timestamp;
	int last_flag;
	void feature_output(simple_packet_info &pkt, int protocol);
	void add_packet(simple_packet_info &pkt);
	void count_rtt(time_type ack_ts, u_int32_t ack);
	void terminate(int overtime=0);//
	int is3232payload, is3236payload;
	int cnt7476777980;
	int cnt1053_1069;
	int cnt_1267_1269;
};



