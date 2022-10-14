#include <pcap.h>
#include <stdlib.h>
#include <stdio.h>
#include<time.h>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <dirent.h>
 #include"flow_process.h"

using  std::cin; using  std::cout; using std::endl;
using std::string; using std::vector;
 
flow_process * flow_p = new flow_process();
int raw = 0, seg_decay = 0, lite = 0, segout = 0, j = 1, block = 0, v = 10, csv = 0,
nortcp = 0, allowweakflow = 0, allflow = 0, onlyallflow = 0, Sort = 0, lenmin = 0,
cntmin = 0, lenmax = 1000000;

double thpmin = 0, time_i = 10, d_num = 0.2, duramin = 0;
string ipsrc = "", ipdst = "";
short portsrc = -1, portdst = -1;



void getAllFiles(string path, vector<string>& files)
{
	DIR* dir;
	struct dirent* ptr;
	string x;
	int len;
	dir = opendir((char*)path.c_str());
	{
		while ((ptr = readdir(dir)) != NULL)
		{
			x = ptr->d_name;
			len = x.size();
			if ((x[len - 1] == 'g' && x[len - 2] == 'n' && x[len - 3] == 'p' && x[len - 4] == 'a' && x[len - 5] == 'c' && x[len - 6] == 'p') && x[len - 7] == '.' || (x[len - 1] == 'p' && x[len - 2] == 'a' && x[len - 3] == 'c' && x[len - 4] == 'p' && x[len - 5] == '.'))
			{
				files.push_back(path + "/" + x);

			}
		}
		sort(files.begin(), files.end());
		cout << "all files:\n";

		for (unsigned int i = 0; i < files.size(); i++) {
			cout << files[i] << endl;
		}
	}
	closedir(dir);
}

void getPacket(u_char * arg, const struct pcap_pkthdr * pkthdr, const u_char * packet)
//获取首部信息用于填充flow_p->simple_packet_info结构
{
	flow_p->simple_packet_info = new struct simple_packet_info;
	int * id = (int *) arg;
	int ip_header_len,header_len;
	flow_p->simple_packet_info->ts = (double)pkthdr->ts.tv_sec + (double)pkthdr->ts.tv_usec / 1000000.0;
	if (raw == 1) {
		out3 << flow_p->simple_packet_info->ts << ',';
	}
	flow_p->simple_packet_info->ts1 = pkthdr->ts;
	flow_p->simple_packet_info->packet_len = pkthdr->len;
	struct ethernet_header* ethernet_protocol;
	ethernet_protocol = (struct ethernet_header*)packet;
	u_short ethernet_type;
	ethernet_type = ntohs(ethernet_protocol->ether_type);

	
	//上层为IPv4，则获取IP首部
	struct ip_header* ip_protocol;
	ip_protocol = (struct ip_header*)(packet + 14);
	ip_header_len = (ntohs(ip_protocol->ip_header_length) >> 8) * 4;
	//cout << ip_header_len<<"\n";
	flow_p->simple_packet_info->ip_protocol = ip_protocol;
	flow_p->simple_packet_info->flow_key.ip_source_address = ip_protocol->ip_source_address;
	flow_p->simple_packet_info->flow_key.ip_destination_address = ip_protocol->ip_destination_address;
		
		
	//printf("%d\n", ntohs(ip_protocol->ip_length));

	//上层为TCP，则获取TCP首部
		
	struct tcp_header* tcp_protocol;
	tcp_protocol = (struct tcp_header*)(packet + 14 + ip_header_len);
	header_len = (ntohs(tcp_protocol->tcp_headerlen_flag) >> 12) * 4 + 14;
	header_len += ip_header_len;
	flow_p->simple_packet_info->header_len = header_len;
			
	u_short source_port;           /*源端口*/
	u_short destination_port;   /*目的端口*/
	source_port = ntohs(tcp_protocol->tcp_source_port);
	destination_port = ntohs(tcp_protocol->tcp_destination_port);

	flow_p->simple_packet_info->protocol = 6;
	flow_p->simple_packet_info->tcp_protocol = tcp_protocol;
	flow_p->simple_packet_info->udp_protocol = NULL;
	flow_p->simple_packet_info->flow_key.tcp_source_port = source_port;
	flow_p->simple_packet_info->flow_key.tcp_destination_port = destination_port;


	flow_p->on_packet_received();
	delete flow_p->simple_packet_info;
	
}
/*用于共享的全局变量*/
/* 10.12
char * file = (char *)malloc(80);
char * outputfile1name = (char *)malloc(80);
char * outputfile3name = (char *)malloc(80);
char * outputfile4name = (char *)malloc(80);
char * outputfile5name = (char *)malloc(80);
*/
string output1, output3, output5;
string label="",labelClass="";
std::ofstream out1, out2, out3,out4,out5;



int main(int argc, char* argv[])
{
	clock_t start, finish;
	double dura;
	start = clock();
	char errBuf[PCAP_ERRBUF_SIZE];
	string realPath;
	vector<string> files;
	
	
	if (strcmp(argv[1] , "-folder")==0){
		realPath = argv[2];
		getAllFiles(realPath,files);
	}
	else if (strcmp(argv[1], "-file") == 0){
		realPath = argv[2];//绝对路径
		string strfile = argv[2];
		files.push_back(strfile);
	}

  
	if (argc > 3) {
		for (int i = 3; i < argc; i++) {
			if (strcmp(argv[i], "-csv") == 0) {
				//读取人工输入的分类标签
				csv = 1;
			}
			else if (strcmp(argv[i], "-rawdata") == 0) {
				//输出原始数据
				raw = 1;
			}
			else if (strcmp(argv[i], "-v") == 0) {
				//设置输出前v个包的长度（暂时不输出了9.18
				i++;
				v = atoi(argv[i]);
				if (v == 0) {
					cout << "Wrong value for -v(need int>0, get " << argv[i] << "). Use default value 0" << endl;
				}
			}
			else if (strcmp(argv[i], "-i") == 0) {
				//设置单位时间长度
				i++;
				time_i = atof(argv[i]);
				if (abs(time_i) < 1e-15) {
					cout << "Wrong value for -i(need double>0, get " << argv[i] << "). Use default value 10" << endl;
				}
			}
			else if (strcmp(argv[i], "-nortcp") == 0) {
				//不输出rtcp（失效9.18）
				nortcp = 1;
			}
			else if (strcmp(argv[i], "-thpmin") == 0) {
				//设置允许的最低每秒包数
				i++;
				thpmin = atof(argv[i]);
				if (abs(thpmin) < 1e-15) {
					cout << "Wrong value for -thpmin(need double>0, get " << argv[i] << "). Use default value 0" << endl;
				}
			}
			else if (strcmp(argv[i], "-allflow") == 0) {
				//输出总计带宽
				allflow = 1;
			}
			
			
			else if (strcmp(argv[i], "-lenmin") == 0) {
				//设置允许的最小平均包长
				i++;
				lenmin = atoi(argv[i]);
				if (lenmin == 0) {
					cout << "Wrong value for -lenmin(need int>0, get " << argv[i] << " ). Use default value 0" << endl;
				}
			}
			else if (strcmp(argv[i], "-cntmin") == 0) {
				//设置允许的最小包数
				i++;
				cntmin = atoi(argv[i]);
				if (cntmin == 0) {
					cout << "Wrong value for -cntmin(need int>0, get " << argv[i] << " ). Use default value 0" << endl;
				}

			}
			else if (strcmp(argv[i], "-lenmax") == 0) {
				//设置允许的最大平均包长
				i++;
				lenmax = atoi(argv[i]);
				if (lenmax == 0) {
					cout << "Wrong value for -lenmax(need int>0, get " << argv[i] << " ). Use default value 10000" << endl;
				}
			}
			
			
			
			
			else if (strcmp(argv[i], "-duramin") == 0) {
				//设置duration占单位时间的最小比例
				i++;
				duramin = atof(argv[i]);
				if (abs(duramin) < 1e-15 || abs(duramin) >= 1) {
					cout << "Wrong value for -duramin(need double at (0,1), get " << argv[i] << "). Use default value 0" << endl;
				}
			}
			
		}
	}
	

	pcap_t * device;

	

	for (unsigned int i=0;i<files.size();++i){
		
		//一次读取候选文件并分别进行处理
		
		//10.12
		output1 = files[i];
		output3 = files[i];
		output5 = files[i];
		
		
		
		if (csv == 0) output1+= ".txt";
		else output1 += ".csv";
		out1.open(output1);
		if (csv == 1) {
			out1 << "ipsrc,ipdst,portsrc,portdst,startts,endts,pktcnt,pktlen,applen,itvl,bw,appbw,thp,retran, rtrate,npack,rtt,rttM,rttm,rttdif,";//len1,len2,len3,len4,len5,len6,len7,len8,len9,len10,
				/*
				for (int i = 1; i <= v; i++)
				{
					out1 << "len" << i << ',';
				}
				*/
			out1 << endl;
		}
		cout << "output:" << output1 << endl;
		

		output3 += "rawdata.csv";
	
	
		if (raw==1) out3.open(output3);

		

	

		device = pcap_open_offline(files[i].c_str(),errBuf);
		struct bpf_program filter;
		pcap_compile(device, &filter, "tcp", 1, 0);
		pcap_setfilter(device, &filter);
		int id = 0;
		pcap_loop(device, -1, getPacket, (u_char*)&id);
		pcap_close(device);
		
		delete flow_p;
		flow_p = new flow_process();
		out1.close();
		//out2.close();
		if (raw == 1) out3.close();

	}
	finish = clock();
	dura = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("%f seconds\n", dura);
	return 0;
}
