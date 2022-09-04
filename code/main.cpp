#include <pcap.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/io.h>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <dirent.h>
 #include"flow_process.h"
 #include<time.h>

using  std::cin; using  std::cout; using std::endl;
using std::string; using std::vector;
 
flow_process * flow_p = new flow_process();
int raw = 0, seg_decay = 0, lite = 0, segout = 0, j = 1, block = 0, v = 10, time_i = 10, csv = 0, nortcp = 0, allowweakflow = 0,allflow=0,onlyallflow=0,Sort=0,rtinfo=0,lenmin=0;
int cntmin=10;
double thpmin = 0;
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
    cout << "all files:\n";
    for (unsigned int i = 0; i < files.size(); i++) {
	cout << files[i] << endl;
    }
  }
  closedir(dir);
} 
//读取文件夹下所有pcap或pcapng文件
/*
{
  long hFile = 0;
  struct _finddata_t fileinfo; 

  string p;
  string type1 = ".pcap", type2 = ".pcapng";
  if (((hFile = _findfirst(p.assign(path).append("\\*" +type1).c_str(), &fileinfo)) != -1) || ((hFile = _findfirst(p.assign(path).append("\\*" +type2).c_str(), &fileinfo)) != -1)) {
    do {
      // 保存文件的全路径
      files.push_back(p.assign(path).append("\\").append(fileinfo.name));

      } while (_findnext(hFile, &fileinfo) == 0); //寻找下一个，成功返回0，否则-1

    _findclose(hFile);
	cout << "all files:\n";
	for (unsigned int i = 0; i < files.size(); i++) {
		cout << files[i] << endl;
	}
  }
}
*/
void getPacket(u_char * arg, const struct pcap_pkthdr * pkthdr, const u_char * packet)
//获取首部信息用于填充flow_p->simple_packet_info结构
{

	int * id = (int *) arg;
	int ip_header_len,header_len;
	flow_p->simple_packet_info.ts = (double)pkthdr->ts.tv_sec + (double)pkthdr->ts.tv_usec / 1000000.0;
	if (raw == 1) {
		out3 << flow_p->simple_packet_info.ts << ',';
	}
	flow_p->simple_packet_info.ts1 = pkthdr->ts;
	flow_p->simple_packet_info.packet_len = pkthdr->len;
	//if (onlyallflow == 0) {//若不仅需要全流信息
		//获取二层首部
		struct ethernet_header* ethernet_protocol;
		ethernet_protocol = (struct ethernet_header*)packet;
		u_short ethernet_type;
		ethernet_type = ntohs(ethernet_protocol->ether_type);

		if (ethernet_type == 0x0800) {
			//上层为IPv4，则获取IP首部
			struct ip_header* ip_protocol;
			ip_protocol = (struct ip_header*)(packet + 14);
			ip_header_len = (ntohs(ip_protocol->ip_header_length) >> 8) * 4;
			//cout << ip_header_len<<"\n";
			flow_p->simple_packet_info.ip_protocol = ip_protocol;
			flow_p->simple_packet_info.flow_key.ip_source_address = ip_protocol->ip_source_address;
			flow_p->simple_packet_info.flow_key.ip_destination_address = ip_protocol->ip_destination_address;
			string ipsour = inet_ntoa(ip_protocol->ip_source_address);
			string ipdest = inet_ntoa(ip_protocol->ip_destination_address);
			if (raw == 1) {
				out3 << ipsour << ',';
				out3 << ipdest << ',';
			}
			//printf("%d\n", ntohs(ip_protocol->ip_length));

		//上层为TCP，则获取TCP首部
			if (ip_protocol->ip_protocol == 6) {
				struct tcp_header* tcp_protocol;
				tcp_protocol = (struct tcp_header*)(packet + 14 + ip_header_len);
				header_len = (ntohs(tcp_protocol->tcp_headerlen_flag) >> 12) * 4 + 14;
				header_len += ip_header_len;
				flow_p->simple_packet_info.header_len = header_len;
				//cout << header_len << "\n";
				//if (header_len == 32) cout << header_len << "\n";
				//cout << header_len << "\n";
				u_short source_port;           /*源端口*/
				u_short destination_port;   /*目的端口*/
				source_port = ntohs(tcp_protocol->tcp_source_port);
				destination_port = ntohs(tcp_protocol->tcp_destination_port);

				flow_p->simple_packet_info.protocol = 6;
				flow_p->simple_packet_info.tcp_protocol = tcp_protocol;
				flow_p->simple_packet_info.udp_protocol = NULL;
				flow_p->simple_packet_info.flow_key.tcp_source_port = source_port;
				flow_p->simple_packet_info.flow_key.tcp_destination_port = destination_port;
				if (raw == 1) {
					out3 << "TCP," << ',' << source_port << ',' << destination_port << ',';
				}
			}
			else if (ip_protocol->ip_protocol == 17) {
				struct udp_header* udp_protocol;
				udp_protocol = (struct udp_header*)(packet + 14 + 20);
				flow_p->simple_packet_info.protocol = 17;
				flow_p->simple_packet_info.tcp_protocol = NULL;
				flow_p->simple_packet_info.udp_protocol = udp_protocol;
				flow_p->simple_packet_info.flow_key.tcp_source_port = ntohs(udp_protocol->udp_source_port);
				flow_p->simple_packet_info.flow_key.tcp_destination_port = ntohs(udp_protocol->udp_destination_port);
				header_len = 8 + 14;
				header_len += ip_header_len;
				flow_p->simple_packet_info.header_len = header_len;
				//cout << header_len << "\n";
				if (raw == 1) {
					out3 << "UDP," << ',' << ntohs(udp_protocol->udp_source_port) << ',' << ntohs(udp_protocol->udp_destination_port) << ',';
				}

				//8.4 RTCP解析

				if (nortcp)//判断是否为rtp或rtcp 8.6todo
				{
					struct rtcp_header * rtcp_protocol;
					rtcp_protocol = (struct rtcp_header*)(packet + 14 + 20 + 8);
					int type = ntohs(rtcp_protocol->common.bPacketType) & 0xff00;//52480=CD=205 FB,51456=C9=201 RR,51200=C8=200 SR
					//printf("%d ", type>>8);
					if (type >= 51200) return;//若为rtcp则不处理

				}
			}
			if (raw == 1) out3 << pkthdr->len << ",\n";
			flow_p->on_packet_received();
		}
		
	//}
	
}
/*用于共享的全局变量*/
char * file = (char *)malloc(80);
char * outputfile1name = (char *)malloc(80);
char * outputfile2name = (char *)malloc(80);
char * outputfile3name = (char *)malloc(80);
char * outputfile4name = (char *)malloc(80);
char * outputfile5name = (char *)malloc(80);
string label="",labelClass="";
std::ofstream out1, out2, out3,out4,out5;




int main(int argc, char* argv[])
{
clock_t start,finish;
double dura;
start = clock();
	char errBuf[PCAP_ERRBUF_SIZE];
	string realPath;
	vector<string> files;
	
	outputfile1name = (char *)malloc(80);
	if (strcmp(argv[1] , "-folder")==0){
	  // 若文件夹中文件数少于2，则失效
		realPath = argv[2];
		getAllFiles(realPath,files);
	}
	else if (strcmp(argv[1], "-file") == 0){
		realPath = argv[2];//绝对路径
		string strfile = argv[2];
	
		files.push_back(strfile);
		
	}

	
  
	if(argc>3){
		for (int i=3;i<argc;i++){
			if(strcmp(argv[i],"-csv")==0){
				//csv
				csv = 1;
			}
	
			else if (strcmp(argv[i], "-rawdata") == 0) {
				//输出原始数据
				raw = 1;
			}
		
			else if (strcmp(argv[i], "-v") == 0) {
				//输出原始数据
				i++;
				v = atoi(argv[i]);
			}
			else if (strcmp(argv[i], "-i") == 0) {
				//输出原始数据
				i++;
				time_i = atoi(argv[i]);
			}
			else if (strcmp(argv[i], "-lenmin") == 0) {
				//输出原始数据
				i++;
				lenmin = atoi(argv[i]);
			}
			else if (strcmp(argv[i], "-nortcp") == 0) {
				//输出原始数据
				nortcp = 1;
			}
			else if (strcmp(argv[i], "-thpmin") == 0) {//若未设置允许弱流，设置允许的最低每秒包数，默认为10
				//输出原始数据
				i++;
				thpmin = atof(argv[i]);
			}
			else if (strcmp(argv[i], "-allflow") == 0) {//若未设置允许弱流，设置允许的最低每秒包数，默认为10
				//输出原始数据
				allflow = 1;
				i++;
				onlyallflow = atoi(argv[i]);
			}
			else if (strcmp(argv[i], "-sort") == 0) {//若未设置允许弱流，设置允许的最低每秒包数，默认为10
				//输出原始数据
				Sort = 1;
			}
			else if (strcmp(argv[i], "-retraninfo") == 0) {
				rtinfo = 1;
			}
			else if (strcmp(argv[i], "-cntmin") == 0) {//若未设置允许弱流，设置允许的最低每秒包数，默认为10
				//输出原始数据
	
				i++;
				cntmin = atoi(argv[i]);
			}
		}
	}
	
	pcap_t * device;



	for (unsigned int i=0;i<files.size();++i){
		
		//一次读取候选文件并分别进行处理
		
		strcpy(outputfile1name,files[i].c_str());
		
		strcpy(outputfile3name, files[i].c_str());
		strcpy(outputfile5name, files[i].c_str());
		//strcpy(outputfile4name, files[i].c_str());
		if (onlyallflow == 0) {
			if (csv == 0) strcat(outputfile1name, ".txt");
			else  strcat(outputfile1name, ".csv");
			out1.open(outputfile1name);
			if (csv == 1) {
				out1 << "ipsrc,ipdst,portsrc,portdst,startts,endts,pktcnt,pktlen,applen,itvl,bw,appbw,thp,retran, rtrate,npack,rtt,rttM,rttm,rttdif,";
				for (int i = 1; i <= v; i++)
				{
					out1 << "len" << i << ',';
				}
				out1 << endl;
			}
			cout << "output:" << outputfile1name << endl;
		}
		
		strcat(outputfile3name, "RawData.csv");
		if (allflow == 1) { 
			strcat(outputfile5name, "allflow.csv"); 
			out5.open(outputfile5name);
			out5 << "startts,endts,bandwith,\n";
			cout << "output5:" << outputfile5name << endl;
		}
		//strcat(outputfile4name, "SegData.csv");
		//cout << "haha1";
		//cout << "output1:" << outputfile1name << endl;
		
		
		//cout << "haha";
		//out2.open(outputfile2name);
		if (raw==1) out3.open(outputfile3name);
		if (segout == 1) out4.open(outputfile4name);
		

	

		device = pcap_open_offline(files[i].c_str(),errBuf);
		struct bpf_program filter;
		pcap_compile(device, &filter, "tcp and udp", 1, 0);
		pcap_setfilter(device, &filter);
		int id = 0;
		pcap_loop(device, -1, getPacket, (u_char*)&id);
		pcap_close(device);
		
		delete flow_p;
		flow_p = new flow_process();
		if (onlyallflow == 0) out1.close();
		//out2.close();
		if (raw == 1) out3.close();
		if (segout == 1) out4.close();
		if (allflow == 1)out5.close();
	}
	finish=clock();
	dura = (double)(finish-start)/CLOCKS_PER_SEC;
	printf("%f seconds\n",dura);
	return 0;
}

