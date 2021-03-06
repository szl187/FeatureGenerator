#include <pcap.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
 #include"flow_process.h"
 
flow_process * flow_p = new flow_process();

void getPacket(u_char * arg, const struct pcap_pkthdr * pkthdr, const u_char * packet)
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
    
    }
    else if(ip_protocol->ip_protocol == 17){
      struct udp_header* udp_protocol;
      udp_protocol = (struct udp_header*)(packet + 14 + 20);
      flow_p->simple_packet_info.protocol = 17;
      flow_p->simple_packet_info.tcp_protocol = NULL;
      flow_p->simple_packet_info.udp_protocol = udp_protocol;
      flow_p->simple_packet_info.flow_key.tcp_source_port = ntohs(udp_protocol->udp_source_port);
      flow_p->simple_packet_info.flow_key.tcp_destination_port = ntohs(udp_protocol->udp_destination_port);
    }

    flow_p->on_packet_received();
}

 

}


std::ofstream out1;
std::string label;
int main(int argc, char* argv[])
{

  char errBuf[PCAP_ERRBUF_SIZE];
  char * file = (char *)malloc(80);
  char * path = (char *)malloc(80);
  char * outputfile1name = (char *)malloc(80);
  getcwd(file,70);
  getcwd(path,70);
  //strcpy(path, 可执行文件和测试pcap文件所在路径);
  //strcpy(file, 可执行文件和测试pcap文件所在路径);
  strcat(file, argv[1]);
  label= argv[2];
  strcpy(outputfile1name, path);
  strcat(outputfile1name, argv[1]);
  strcat(outputfile1name, "res.csv");
  out1.open(outputfile1name);


  pcap_t * device;

  device = pcap_open_offline(file,errBuf); 


  struct bpf_program filter;
  pcap_compile(device, &filter, "tcp and udp", 1, 0);
  pcap_setfilter(device, &filter);

  int id = 0;
  pcap_loop(device, -1, getPacket, (u_char*)&id);
  pcap_close(device);
  
  std::map <struct flow_key, flow>::iterator it;
  for (it = flow_p->flows.begin(); it != flow_p->flows.end(); ++it) {
      it->second.terminate();
  }
 
  return 0;
}
