#include <pcap.h>
#include <stdlib.h>
#include <stdio.h>
#include<time.h>
#include <dirent.h>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
 #include"flow_process.h"

using  std::cin; using  std::cout; using std::endl;
using std::string; using std::vector;
 
flow_process * flow_p = new flow_process();
int raw = 0, seg_decay = 0, lite = 0, segout = 0, j = 1, block = 0, v = 10, csv = 0,
nortcp = 0, allowweakflow = 0, allflow = 0, onlyallflow=0,  Sort = 0, rtinfo = 0, lenmin = 0, 
cntmin = 10, portinfo = 0, lenmax = 10000, droplast = 0, decay=0, vari=0,app=0;

double thpmin = 0, time_i = 100000, d_num=0.2, duramin=0;
string ipsrc = "", ipdst = "";
short portsrc = -1, portdst = -1;


void getAllFiles(string path, vector<string>& files)
{
  DIR* dir;
  struct dirent * ptr;
  string x;
  int len;
  dir = opendir((char *)path.c_str());
  {
	while((ptr=readdir(dir))!=NULL)
	{
	  x = ptr->d_name;
	  len = x.size();
	  if((x[len-1]=='g' && x[len-2]=='n'&& x[len-3]=='p' && x[len-4]=='a' && x[len-5]=='c' && x[len-6]=='p') && x[len-7]=='.'||(x[len-1]=='p' && x[len-2]=='a' && x[len-3]=='c' && x[len-4]=='p' && x[len-5]=='.'))
	  {
		files.push_back(path+"/"+x);

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
	flow_p->simple_packet_info->packet_len = pkthdr->len;
	
	struct ethernet_header* ethernet_protocol;
	ethernet_protocol = (struct ethernet_header*)packet;
	u_short ethernet_type;
	ethernet_type = ntohs(ethernet_protocol->ether_type);

	if (ethernet_type == 0x0800) {
		//上层为IPv4，则获取IP首部
		struct ip_header* ip_protocol;
		ip_protocol = (struct ip_header*)(packet + 14);
		ip_header_len = (ntohs(ip_protocol->ip_header_length) >> 8) * 4;
		flow_p->simple_packet_info->ip_protocol = ip_protocol;
		flow_p->simple_packet_info->flow_key.ip_source_address = ip_protocol->ip_source_address;
		flow_p->simple_packet_info->flow_key.ip_destination_address = ip_protocol->ip_destination_address;
		string ipsour = inet_ntoa(ip_protocol->ip_source_address);
		string ipdest = inet_ntoa(ip_protocol->ip_destination_address);
	


		//上层为TCP，则获取TCP首部
		if (ip_protocol->ip_protocol == 6) {
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
		
		}
		else if (ip_protocol->ip_protocol == 17) 
		{
			struct udp_header* udp_protocol;
			udp_protocol = (struct udp_header*)(packet + 14 + ip_header_len);
			flow_p->simple_packet_info->protocol = 17;
			flow_p->simple_packet_info->tcp_protocol = NULL;
			flow_p->simple_packet_info->udp_protocol = udp_protocol;
			flow_p->simple_packet_info->flow_key.tcp_source_port = ntohs(udp_protocol->udp_source_port);
			flow_p->simple_packet_info->flow_key.tcp_destination_port = ntohs(udp_protocol->udp_destination_port);
			header_len = 8 + 14;
			header_len += ip_header_len;
			
			flow_p->simple_packet_info->header_len = header_len;
		}
	
		flow_p->on_packet_received();
	}
	delete flow_p->simple_packet_info;
}

/*用于共享的全局变量*/

char * outputfile1name = (char *)malloc(80);
std::ofstream out1;


int main(int argc, char* argv[])
{
	clock_t start, finish;
	double dura;
	start = clock();
	char errBuf[PCAP_ERRBUF_SIZE];
	string realPath;
	vector<string> files;
	
	outputfile1name = (char *)malloc(80);

	if (strcmp(argv[1] , "-folder")==0){
		realPath = argv[2];
		getAllFiles(realPath,files);
	}
	else if (strcmp(argv[1], "-file") == 0){
		realPath = argv[2];//绝对路径
		string strfile = argv[2];

		files.push_back(strfile);
	}
	else {
		cout << "need -folder or -file as the first arg" << endl;
		return 1;
	}
	if (strcmp(argv[3], "-app") == 0) {
		
		string appstr = argv[4];
		if (appstr == "tx") app = 1;
		else if (appstr == "dd") app = 2;
		else if (appstr == "fs") app = 3;
		else {
			cout << "app: [tx,dd,fs]" << endl;
			return 1;
		}
		
	}
	else {
		cout << "need -app as the second arg. app: [tx,dd,fs]" << endl;
		return 1;
	}

	
	pcap_t * device;

	for (unsigned int i=0;i<files.size();++i){
		
		//一次读取候选文件并分别进行处理
		cout << "output:" << files[i] << endl;
		device = pcap_open_offline(files[i].c_str(),errBuf);
		if (device == NULL) cout << errBuf << endl;
		else {
			struct bpf_program filter;
			pcap_compile(device, &filter, "tcp and udp", 1, 0);
			pcap_setfilter(device, &filter);
			int id = 0;
			pcap_loop(device, -1, getPacket, (u_char*)&id);
			pcap_close(device);
			delete flow_p;
			flow_p = new flow_process();
			
		}
		
	}
	finish = clock();
	dura = (double)(finish - start) / CLOCKS_PER_SEC;
	return 0;
}
