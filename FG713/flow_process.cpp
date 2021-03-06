#include "flow_process.h"

flow_process::flow_process()
{	
}


void flow_process::on_packet_received()
{
	

	int backward_flag = 0;
	flow* f = NULL;

	std::map <struct flow_key, flow>::iterator it;
	it = flows.find(simple_packet_info.flow_key);

	
	//正向没有找到，则反向再进行查找
	if (it == flows.end()) {
		backward_flag = 1;
		struct in_addr tmp = simple_packet_info.flow_key.ip_source_address;
		simple_packet_info.flow_key.ip_source_address= simple_packet_info.flow_key.ip_destination_address;
		simple_packet_info.flow_key.ip_destination_address=tmp;

		u_int16_t p = simple_packet_info.flow_key.tcp_source_port;
		simple_packet_info.flow_key.tcp_source_port = simple_packet_info.flow_key.tcp_destination_port;
		simple_packet_info.flow_key.tcp_destination_port =p;
		it = flows.find(simple_packet_info.flow_key);
	
		//flows中没有，创建新的流
		if (it == flows.end()) 
		{
			struct in_addr tmp = simple_packet_info.flow_key.ip_source_address;
			simple_packet_info.flow_key.ip_source_address= simple_packet_info.flow_key.ip_destination_address;
			simple_packet_info.flow_key.ip_destination_address=tmp;

			u_int16_t p = simple_packet_info.flow_key.tcp_source_port;
			simple_packet_info.flow_key.tcp_source_port = simple_packet_info.flow_key.tcp_destination_port;
			simple_packet_info.flow_key.tcp_destination_port =p;
		
			simple_packet_info.flow_key.flow_direction = forward;
			f = new flow(simple_packet_info);				

			flows.insert(std::pair<struct flow_key, flow>(simple_packet_info.flow_key, *f));	
			//printf("New flow created\n");
		}	
		else
		{
			struct in_addr tmp = simple_packet_info.flow_key.ip_source_address;
			simple_packet_info.flow_key.ip_source_address= simple_packet_info.flow_key.ip_destination_address;
			simple_packet_info.flow_key.ip_destination_address=tmp;

			u_int16_t p = simple_packet_info.flow_key.tcp_source_port;
			simple_packet_info.flow_key.tcp_source_port = simple_packet_info.flow_key.tcp_destination_port;
			simple_packet_info.flow_key.tcp_destination_port =p;
			simple_packet_info.flow_key.flow_direction = backward;
			f = &(it->second);
			//printf("f->reverse:%d\n",f->reverse);
			if(f->reverse) {simple_packet_info.flow_key.flow_direction = forward; /*printf("Forward packet add to exist flow\n");*/}
			else {simple_packet_info.flow_key.flow_direction = backward;/*printf("Backward packet add to exist flow\n");*/}
		}

	}
	else
	{
		
		f = &(it->second);
		if(f->reverse) {simple_packet_info.flow_key.flow_direction = backward; /*printf("Backward packet add to exist flow\n");*/}
		else {simple_packet_info.flow_key.flow_direction = forward;/*printf("Forward packet add to exist flow\n");*/}
		
	}
	
	if (it != flows.end())
	{
		if ((*it).second.bwd_latest_timestamp!=0 && (*it).second.fwd_latest_timestamp!=0)
		{
			if ((simple_packet_info.ts - (*it).second.bwd_latest_timestamp > 10 || simple_packet_info.ts - (*it).second.fwd_latest_timestamp)>10)
			{
				(*it).second.terminate();
				flows.erase(it);
				simple_packet_info.flow_key.flow_direction = forward;
				f = new flow(simple_packet_info);
				flows.insert(std::pair<struct flow_key, flow>(simple_packet_info.flow_key, *f));
				printf("But overtime, create new flow");
			}
		}
		
	}		
	
	f->add_packet(simple_packet_info);
}
