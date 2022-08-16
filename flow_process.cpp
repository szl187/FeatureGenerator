#include "flow_process.h"

flow_process::flow_process()
{	
}


void flow_process::on_packet_received()
{
	

	flow* f = NULL;

	std::map <struct flow_key, flow>::iterator it;
	it = flows.find(simple_packet_info.flow_key);
	//flows中没有，创建新的流
	if (it == flows.end()) {
		f = new flow(simple_packet_info);
		flows.insert(std::pair<struct flow_key, flow>(simple_packet_info.flow_key, *f));
		//printf("new flow\n");
	}
	else
	{
		
		f = &(it->second);
	}
	if (it != flows.end())
	{
		if ((*it).second.latest_timestamp!=0 )
		{
			if (simple_packet_info.ts - (*it).second.start_timestamp > time_i )
			{
				j++;
				(*it).second.terminate(1);
				flows.erase(it);
				f = new flow(simple_packet_info);
				flows.insert(std::pair<struct flow_key, flow>(simple_packet_info.flow_key, *f));
				//printf("overtime\n");
			}
		}
		
	}		
	f->add_packet(simple_packet_info);

	struct in_addr tmp = simple_packet_info.flow_key.ip_source_address;
	simple_packet_info.flow_key.ip_source_address = simple_packet_info.flow_key.ip_destination_address;
	simple_packet_info.flow_key.ip_destination_address = tmp;

	u_int16_t p = simple_packet_info.flow_key.tcp_source_port;
	simple_packet_info.flow_key.tcp_source_port = simple_packet_info.flow_key.tcp_destination_port;
	simple_packet_info.flow_key.tcp_destination_port = p;
	it = flows.find(simple_packet_info.flow_key);
	if (it != flows.end())
	{
		(*it).second.reverse_latest_timestamp = simple_packet_info.ts;
	}


}
