#include <pcap.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <fstream>
#include <string>
#include <vector>

 #include"flow_process.h"

using  std::cin; using  std::cout; using std::endl;
using std::string; using std::vector;
 
flow_process * flow_p = new flow_process();

void getAllFiles(string path, vector<string>& files) 
//读取文件夹下所有pcap或pcapng文件
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
	for (int i = 0; i < files.size(); i++) {
		cout << files[i] << endl;
	}
  }
}
void getPacket(u_char * arg, const struct pcap_pkthdr * pkthdr, const u_char * packet)
//获取首部信息用于填充flow_p->simple_packet_info结构
{

	int * id = (int *) arg;
    
	flow_p->simple_packet_info.ts = (double)pkthdr->ts.tv_sec + (double)pkthdr->ts.tv_usec / 1000000.0;
	flow_p->simple_packet_info.packet_len = pkthdr->len;
	flow_p->simple_packet_info.flow_direction = forward;
 
	//获取二层首部
	struct ethernet_header* ethernet_protocol; 
	ethernet_protocol = (struct ethernet_header*)packet; 
	u_short ethernet_type;
	ethernet_type = ntohs(ethernet_protocol->ether_type); 

 	if (ethernet_type==0x0800){
     //上层为IPv4，则获取IP首部
		struct ip_header* ip_protocol;  
		ip_protocol = (struct ip_header*)(packet + 14);
		flow_p->simple_packet_info.ip_protocol = ip_protocol;
		flow_p->simple_packet_info.flow_key.ip_source_address = ip_protocol->ip_source_address;
		flow_p->simple_packet_info.flow_key.ip_destination_address = ip_protocol->ip_destination_address;
		string ipsour = inet_ntoa(ip_protocol->ip_source_address);
		string ipdest = inet_ntoa(ip_protocol->ip_destination_address);
	

    //上层为TCP，则获取TCP首部
    if(ip_protocol->ip_protocol == 6){
		struct tcp_header* tcp_protocol; 
		tcp_protocol = (struct tcp_header*)(packet + 14 + 20); 
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
			out3 << "TCP," << ipsour << ',';
			out3 << ipdest << ',' << source_port << ',' << destination_port << ',';
		}
    }
    else if(ip_protocol->ip_protocol == 17){
		struct udp_header* udp_protocol;
		udp_protocol = (struct udp_header*)(packet + 14 + 20);
		flow_p->simple_packet_info.protocol = 17;
		flow_p->simple_packet_info.tcp_protocol = NULL;
		flow_p->simple_packet_info.udp_protocol = udp_protocol;
		flow_p->simple_packet_info.flow_key.tcp_source_port = ntohs(udp_protocol->udp_source_port);
		flow_p->simple_packet_info.flow_key.tcp_destination_port = ntohs(udp_protocol->udp_destination_port);
		if (raw == 1) { 
			out3 << "UDP," << ipsour << ','; 
			out3 << ipdest << ',' << ntohs(udp_protocol->udp_source_port) << ',' << ntohs(udp_protocol->udp_destination_port) << ',';
		}
	}
	if (raw == 1) out3 << pkthdr->len << ",\n";
    flow_p->on_packet_received();
}

 

}
/*用于共享的全局变量*/
char * file = (char *)malloc(80);
char * outputfile1name = (char *)malloc(80);
char * outputfile2name = (char *)malloc(80);
char * outputfile3name = (char *)malloc(80);
string label="",labelClass="";
std::ofstream out1, out2, out3;
int raw = 0;
//CDTree myCDTree(4, 3, 0.0001);//决策树，未完全实现


int main(int argc, char* argv[])
{

	char errBuf[PCAP_ERRBUF_SIZE];
	string realPath;
	char * outputfile1name = NULL;
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
		//cout << "-file:"<<strfile << endl;
		files.push_back(strfile);
		//cout << "files[0]:" << files[0] << endl;
	}

	//cout << "argc:" << argc << endl;
  
	if(argc>3){
		for (int i=3;i<argc;i++){
			if (strcmp(argv[i],"-autocl")==0){
				//根据模型进行流量分类预测
			}
			else if(strcmp(argv[i],"-el")==0){
			//读取人工输入的QoE标签
				i++;
				label = argv[i];
			}
			else if(strcmp(argv[i],"-cl")==0){
				//读取人工输入的分类标签
				i++;
				labelClass = argv[i];
			}
			else if(strcmp(argv[i],"-autoel")==0){
				//根据模型进行QoE预测
			}
			else if (strcmp(argv[i], "-rawdata") == 0) {
				//输出原始数据
				raw = 1;
			}
	
		}
	}
	else{
		//提取全部特征并输出特征文件，并进行QoS打分
	}

	pcap_t * device;

	

	for (int i=0;i<files.size();++i){
		//一次读取候选文件并分别进行处理
		strcpy(outputfile1name,files[i].c_str());
		strcpy(outputfile2name, files[i].c_str());
		strcpy(outputfile3name, files[i].c_str());
		strcat(outputfile1name, "QoEFeat.csv");
		strcat(outputfile2name, "QoSFeat.csv");
		strcat(outputfile3name, "RawData.csv");
		out1.open(outputfile1name);
		//out2.open(outputfile2name);
		if (raw==1) out3.open(outputfile3name);
		cout << "output:" << outputfile1name<< endl;

		device = pcap_open_offline(files[i].c_str(),errBuf);
		struct bpf_program filter;
		pcap_compile(device, &filter, "tcp and udp", 1, 0);
		pcap_setfilter(device, &filter);
		int id = 0;
		pcap_loop(device, -1, getPacket, (u_char*)&id);
		pcap_close(device);
		std::map <struct flow_key, flow>::iterator it;
		for (it = flow_p->flows.begin(); it != flow_p->flows.end(); ++it) {
			it->second.terminate();//输出流量分类特征与QoS评分，并释放空间。未完全实现。
		}
		flow_p = new flow_process();
		out1.close();
		//out2.close();
		if (raw == 1) out3.close();
	}
	return 0;
}
