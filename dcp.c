#include"profinet.h"



int extract_blocks_from_packet(char *payload, int length, device *device)
{
	char *pl=malloc(length);
	memcpy(pl, payload, length);
	for(int i=0; i<length; i++)
	{
		struct block b;
		b.option=(uint8_t)pl[0];
		b.suboption=(uint8_t)pl[1];	
		b.block_length=(int)(((uint16_t)(pl[2])<<8)|(uint16_t)pl[3]);
		b.block_info=(uint16_t)(pl[4]<<8)|(uint16_t)pl[5];
		if(b.option!=(uint8_t)0)
		{
			switch(b.option)
			{
				case PF_DCP_OPT_RESERVED_0:
					i=i+b.block_length;					
				break;
				case PF_DCP_OPT_IP:
					switch(b.suboption)
					{
						case PF_DCP_SUB_IP_MAC:
							printf("get MAC address of PLC\n");
							i=i+b.block_length+4;
						break;
						case PF_DCP_SUB_IP_PAR:                       
							printf("IP address: %d.%d.%d.%d\n",(uint8_t)pl[6], (uint8_t)pl[7], (uint8_t)pl[8], (uint8_t)pl[9]);
							printf("Subnet Mask: %d.%d.%d.%d\n",(uint8_t)pl[10], (uint8_t)pl[11], (uint8_t)pl[12], (uint8_t)pl[13]);
							printf("Gateway: %d.%d.%d.%d\n",(uint8_t)pl[14], (uint8_t)pl[15], (uint8_t)pl[16], (uint8_t)pl[17]);
							i=i+b.block_length+4;
							uint8_t ip[4]={(uint8_t)pl[6], (uint8_t)pl[7], (uint8_t)pl[8], (uint8_t)pl[9]};
							uint8_t sn[4]={(uint8_t)pl[10], (uint8_t)pl[11], (uint8_t)pl[12], (uint8_t)pl[13]};
							uint8_t gw[4]={(uint8_t)pl[14], (uint8_t)pl[15], (uint8_t)pl[16], (uint8_t)pl[17]};
							memcpy(device->ip,ip, 4);
							memcpy(device->subnet,sn, 4);
							memcpy(device->gateway,gw, 4);
						break;
						case PF_DCP_SUB_IP_SUITE:                       
							printf("get IP SUITE address of PLC\n");
							i=i+b.block_length+4;
						break;
					}
				break;
				case PF_DCP_OPT_DEVICE_PROPERTIES:	
					switch(b.suboption)
					{
						case PF_DCP_SUB_DEV_PROP_VENDOR:        /* Read */
							printf("device vendor:");
							for(int j=6; j<=b.block_length+5; j++)
								printf("%c", (uint8_t)pl[j]);
							printf("\n");
							i=i+b.block_length+4;
						break;
						case PF_DCP_SUB_DEV_PROP_NAME:                /* Read/Write */
							printf("device name:");
							for(int j=6; j<=b.block_length+5; j++)
								printf("%c", (uint8_t)pl[j]);
							printf("\n");
							i=i+b.block_length+4;
						break;
						case PF_DCP_SUB_DEV_PROP_ID:                  /* Read */
							printf("Vendor ID: ");
								printf("%.2X%.2X\n", (uint8_t)pl[6],(uint8_t)pl[7]);
							printf("Device ID: ");
								printf("%.2X%.2X\n", (uint8_t)pl[8],(uint8_t)pl[9]);
							i=i+b.block_length+4;
						break;
						case PF_DCP_SUB_DEV_PROP_ROLE:                /* Read */
							printf("device role: %.2X\n", (uint8_t)pl[6]);
							i=i+b.block_length+4;
						break;
						case PF_DCP_SUB_DEV_PROP_OPTIONS:            /* Read */
							printf("options available, optional\n");
						break;
					}
				break;
				case PF_DCP_OPT_DHCP:	
					i=i+b.block_length+4;
				break;
				case PF_DCP_OPT_RESERVED_4:	
					i=i+b.block_length+4;
				break;
				case PF_DCP_OPT_CONTROL:	
					switch(b.suboption)
					{
						case PF_DCP_SUB_CONTROL_START:          /* Write */
							printf("get device options of PLC\n");
							i=i+b.block_length+4;
						break;
						case PF_DCP_SUB_CONTROL_STOP:               /* Write */
							printf("get control stop of PLC\n");
							i=i+b.block_length+4;
						break;
						case PF_DCP_SUB_CONTROL_SIGNAL:               /* Write */
							printf("get control signal of PLC\n");
							i=i+b.block_length+4;
						break;
						case PF_DCP_SUB_CONTROL_RESPONSE:
							printf("get control response of PLC\n");
							i=i+b.block_length+4;
						break;
						case PF_DCP_SUB_CONTROL_FACTORY_RESET:        /* Optional, Write */
							printf("get control factory reset of PLC\n");
							i=i+b.block_length+4;
						break;
					}
				break;
				case PF_DCP_OPT_DEVICE_INITIATIVE:	
					i=i+b.block_length+4;
				break;
			
			}
			pl=pl+(b.block_length+4);
		}
		else
		{
			pl=pl+1;//ignoring padding(if any)
			i++;
		}
	}
	return 1;
	printf("\n\n\n");
	
}
void recieve_profinet_packets()
{
	int sock_r;
	sock_r=socket(AF_PACKET, SOCK_RAW, htons(0x8892));
	if(sock_r<0)
	{
		printf("rcv error in socket\n");
	}
	else
	{
		unsigned char *buffer = (unsigned char *) malloc(65536); //to receive data
		memset(buffer,0,65536);
		struct sockaddr socketaddr;
		int socketaddr_len = sizeof (socketaddr);
	 
		struct pack p;
		int buflen=-1;
		int traversed=0;
		//printf("socket listening\n");
		if((buflen=recvfrom(sock_r,buffer,65536,0,&socketaddr,(socklen_t *)&socketaddr_len))<0)
		{
			//will not come into running ever
		}
		else
		{
			
			// checking the packet to have DCP identify response frame id, DCP Identify service id and DCP success service type
			uint8_t mac[6]={(uint8_t)buffer[traversed], (uint8_t)buffer[traversed+1], (uint8_t)buffer[traversed+2], (uint8_t)buffer[traversed+3], (uint8_t)buffer[traversed+4], (uint8_t)buffer[traversed+5]};
			memcpy(p.dest_mac, mac, sizeof(p.dest_mac));
			traversed+=6;

			uint8_t  smac[6]={(uint8_t)buffer[traversed], (uint8_t)buffer[traversed+1], (uint8_t)buffer[traversed+2], (uint8_t)buffer[traversed+3], (uint8_t)buffer[traversed+4], (uint8_t)buffer[traversed+5]};
			memcpy(p.source_mac, smac, sizeof(p.source_mac));
			traversed+=6;

			if((uint16_t)buffer[traversed]==0x81 && (uint16_t)buffer[traversed+1]==0x00)//if packet has VLAN as type it has type=1800 and next 2 bytes will be reserved
			{
				traversed+=4;
			}
			printf("test: 0x%.2x 0x%.2x 0x%.2x 0x%.2x\n", buffer[traversed],buffer[traversed+1],buffer[traversed+2],buffer[traversed+3]);
			p.packet_type=(((uint16_t)buffer[traversed])<<8)|(uint16_t)buffer[traversed+1];
			traversed+=2;
			p.frame_id=((uint16_t)buffer[traversed]<<8)|(uint16_t)buffer[traversed+1];
			traversed+=2;
			p.service_id=(uint8_t)buffer[traversed++];
			p.service_type=(uint8_t)buffer[traversed++];
			p.xid=((uint32_t)buffer[traversed]<<24)|((uint32_t)buffer[traversed+1]<<16)|((uint32_t)buffer[traversed+2]<<8)|((uint32_t)buffer[traversed+3]);
			traversed+=4;
			p.reserve_or_delay=((uint16_t)buffer[traversed]<<8)|((uint16_t)buffer[traversed+1]);
			traversed+=2;
			p.ddl=((uint16_t)buffer[traversed]<<8)|((uint16_t)buffer[traversed+1]);
			traversed+=2;

			int payload_length=(int)p.ddl;
			char *payload=buffer+(traversed);
			
			if(ETHTYPE_PROFINET == p.packet_type | printf(" packet type: %.4x\n", p.packet_type))
			{
				// for a new device that has responded to identify request with identify response
				if( PF_DCP_ID_RES_FRAME_ID == p.frame_id && PF_DCP_SERVICE_IDENTIFY == p.service_id && PF_DCP_SERVICE_TYPE_SUCCESS == p.service_type && !check(p.source_mac))  
				{
					printf("new device found adding to devices\n");
					device *dev=findDeviceByMAC(smac);
					if(!dev)
					{
						dev=(device *)malloc(sizeof(device));
						(dev->mac)[0]=smac[0];(dev->mac)[1]=smac[1];(dev->mac)[2]=smac[2];(dev->mac)[3]=smac[3];(dev->mac)[4]=smac[4];(dev->mac)[5]=smac[5];
					}

					//device_list *dev=malloc(sizeof(device_list));
					extract_blocks_from_packet(payload, (int) p.ddl, dev);
					//addDevice(smac);
					memcpy(dev->mac,smac, 6);
					//addDevice(smac);

					/*int sop=2048;
					uint8_t *response=malloc(sop);
					uint8_t *response1=malloc(sop);
					
					send_read_IO_Packet(dev, response, sop);
					send_write_IO_Packet(dev, response1, sop);
					printf("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x   %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n", response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7], response[8], response[9], response[10], response[11], response[12], response[13], response[14], response[15]);
					//set_ip_config(smac);
					printf("set ip config sent\n");
					*/
				}

				// for an old device that went off power and has come online again will pass a hello request and we add it to the list too {0x01, 0x0e, 0xcf, 0x00, 0x00, 0x00 }
		if( PF_DCP_SERVICE_HELLO == p.frame_id & PF_DCP_SERVICE_HELLO == p.service_id & PF_DCP_SERVICE_TYPE_SUCCESS == p.service_type & !memcmp(p.source_mac, dcp_mc_address.addr, 6) & !check(p.source_mac) )  
				{
					printf("device came back to power\n");
					printf("source mac:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n", (int)smac[0], (int)smac[1], (int)smac[2], (int)smac[3], (int)smac[4], (int)smac[5]);
					printf("destination mac:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n", (int)mac[0], (int)mac[1], (int)mac[2], (int)mac[3], (int)mac[4], (int)mac[5]);
					device *dev=findDeviceByMAC(smac);
					if(!dev)
					{
						dev=(device *)malloc(sizeof(device));
						(dev->mac)[0]=smac[0];(dev->mac)[1]=smac[1];(dev->mac)[2]=smac[2];(dev->mac)[3]=smac[3];(dev->mac)[4]=smac[4];(dev->mac)[5]=smac[5];
						
					}
					extract_blocks_from_packet(payload, (int) p.ddl, dev);
				
					addDevice(smac);
					memcpy(dev->mac,smac, 6);
					/*int sop=1000;
					uint8_t *response=malloc(sop);
					uint8_t *response1=malloc(sop);
					send_read_IO_Packet(dev, response, sop);
					send_write_IO_Packet(dev, response1, sop);
					//printf("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x   %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n", response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7], response[8], response[9], response[10], response[11], response[12], response[13], response[14], response[15]);
					*/
				}
			}
		}
	}
	close(sock_r);
}
void send_set_req(uint8_t dest_mac[])
{
	os_buf_t * buf=NULL;
	uint8_t                 *p_dst;
	uint16_t                dst_pos;
	uint16_t                dst_start_pos;
	os_ethhdr_t             *p_ethhdr;
	pf_dcp_header_t         *p_dcphdr;
	uint8_t                 *p_value=0;

	int * sock;
	int i;
	struct ifreq ifr;
	struct sockaddr_ll sll;
	int ifindex;
	struct timeval timeout;

	sock = calloc (1, sizeof(int));
	*sock = -1;

	*sock = socket(PF_PACKET, SOCK_RAW, htons(ETHTYPE_PROFINET));

	timeout.tv_sec =  0;
	timeout.tv_usec = 1;
	setsockopt(*sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

	i = 1;
	setsockopt(*sock, SOL_SOCKET, SO_DONTROUTE, &i, sizeof(i));


	strcpy(ifr.ifr_name, interface_name);
	ioctl(*sock, SIOCGIFINDEX, &ifr);

	ifindex = ifr.ifr_ifindex;
	strcpy(ifr.ifr_name, interface_name);
	ifr.ifr_flags = 0;
	/* reset flags of NIC interface */
	ioctl(*sock, SIOCGIFFLAGS, &ifr);

	/* set flags of NIC interface, here promiscuous and broadcast */
	ifr.ifr_flags = ifr.ifr_flags | IFF_PROMISC | IFF_BROADCAST;
	ioctl(*sock, SIOCSIFFLAGS, &ifr);

	/* bind socket to protocol, in this case RAW EtherCAT */
	sll.sll_family = AF_PACKET;
	sll.sll_ifindex = ifindex;
	sll.sll_protocol = htons(ETHTYPE_PROFINET);
	bind(*sock, (struct sockaddr *)&sll, sizeof(sll));
	if (*sock > -1)
	{
/******************************sending a set packet**************************************************************/

		buf=os_buf_alloc(60);

		if (buf != NULL)
   		{
			p_dst = (uint8_t *)buf->payload;
			if (p_dst != NULL)
      			{
				dst_pos = 0;
				p_ethhdr = (os_ethhdr_t *)&p_dst[dst_pos];
        	 		memcpy(p_ethhdr->dest.addr, dest_mac, sizeof(p_ethhdr->dest.addr));//add multicast address to packet
				memcpy(p_ethhdr->src.addr,dcp_src_address.addr, sizeof(p_ethhdr->src.addr));//add source mac address to packet
				
				p_ethhdr->type = htons(ETHTYPE_PROFINET);//packet type profinet
				dst_pos += sizeof(os_ethhdr_t);
 	//vipul
				/* Insert FrameId */
				p_dst[dst_pos++] = (PF_DCP_GET_SET_FRAME_ID & 0xff00) >> 8;
				p_dst[dst_pos++] = PF_DCP_GET_SET_FRAME_ID & 0x00ff;
	
				p_dcphdr = (pf_dcp_header_t *)&p_dst[dst_pos];
				p_dcphdr->service_id = PF_DCP_SERVICE_SET;
				p_dcphdr->service_type = PF_DCP_SERVICE_TYPE_REQUEST;
				p_dcphdr->xid = htonl(2);
				p_dcphdr->response_delay_factor = htons(0);
				p_dcphdr->data_length = htons(0);    /* At the moment */
				dst_pos += sizeof(pf_dcp_header_t);

				dst_start_pos = dst_pos;
				//(void)pf_dcp_put_block(p_dst, &dst_pos, 44, PF_DCP_OPT_CONTROL, PF_DCP_SUB_CONTROL_START, true, 0,4, p_value);
				//(void)pf_dcp_put_block(p_dst, &dst_pos, 60, PF_DCP_OPT_CONTROL, PF_DCP_SUB_CONTROL_SIGNAL, true, 0,4, p_value);
				(void)pf_dcp_put_block(p_dst, &dst_pos, 60, PF_DCP_OPT_IP, PF_DCP_SUB_IP_PAR, true, 0,4, p_value);
				//(void)pf_dcp_put_block(p_dst, &dst_pos, 60, PF_DCP_OPT_CONTROL, PF_DCP_SUB_CONTROL_FACTORY_RESET, true, 0,4, p_value);
				//(void)pf_dcp_put_block(p_dst, &dst_pos, 44, PF_DCP_OPT_CONTROL, PF_DCP_SUB_CONTROL_STOP, true, 0,4, p_value);
				//(void)pf_dcp_put_block(p_dst, &dst_pos, 60, PF_DCP_OPT_ALL, PF_DCP_SUB_ALL, true, 0,0, p_value);
				//(void)pf_dcp_put_block(p_dst, &dst_pos, 60, PF_DCP_OPT_ALL, PF_DCP_SUB_ALL, true, 0,0, p_value);
				
			        /* Insert final response length and ship it! */
        	 		p_dcphdr->data_length = htons(dst_pos - dst_start_pos);
        	 		buf->len = dst_pos;
				buf->payload = (void *)((uint8_t *)buf + sizeof(os_buf_t));  /* Payload follows header struct */
				buf->len = 60;
	
				int ret = send (*sock, buf->payload, buf->len, 0);

			   	if (ret>=0)
   				{
   					fflush(stdout);
				//	printf("transmission successful\n");
			   	}
				else
					printf("not transmitted: %d \n", ret);
			}
			else
			{
				printf("buffer payload is null\n");
			}
		}
		else
		{
			printf("buffer is null");
		}
		close(*sock);
    	free(sock);
	}
	else
		printf("socket not created");
}
void send_get_req(uint8_t dest_mac[])
{
	os_buf_t * buf=NULL;
	uint8_t                 *p_dst;
	uint16_t                dst_pos;
	uint16_t                dst_start_pos;
	os_ethhdr_t             *p_ethhdr;
	pf_dcp_header_t         *p_dcphdr;
	uint8_t                 *p_value=0;

	int * sock;
	int i;
	struct ifreq ifr;
	struct sockaddr_ll sll;
	int ifindex;
	struct timeval timeout;

	sock = calloc (1, sizeof(int));
	*sock = -1;

	*sock = socket(PF_PACKET, SOCK_RAW, htons(ETHTYPE_PROFINET));

	timeout.tv_sec =  0;
	timeout.tv_usec = 1;
	setsockopt(*sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

	i = 1;
	setsockopt(*sock, SOL_SOCKET, SO_DONTROUTE, &i, sizeof(i));


	strcpy(ifr.ifr_name, interface_name);
	ioctl(*sock, SIOCGIFINDEX, &ifr);

	ifindex = ifr.ifr_ifindex;
	strcpy(ifr.ifr_name, interface_name);
	ifr.ifr_flags = 0;
	/* reset flags of NIC interface */
	ioctl(*sock, SIOCGIFFLAGS, &ifr);

	/* set flags of NIC interface, here promiscuous and broadcast */
	ifr.ifr_flags = ifr.ifr_flags | IFF_PROMISC | IFF_BROADCAST;
	ioctl(*sock, SIOCSIFFLAGS, &ifr);

	/* bind socket to protocol, in this case RAW EtherCAT */
	sll.sll_family = AF_PACKET;
	sll.sll_ifindex = ifindex;
	sll.sll_protocol = htons(ETHTYPE_PROFINET);
	bind(*sock, (struct sockaddr *)&sll, sizeof(sll));
	if (*sock > -1)
	{
/******************************sending a packet**************************************************************/

		buf=os_buf_alloc(60);

		if (buf != NULL)
   		{
			p_dst = (uint8_t *)buf->payload;
			if (p_dst != NULL)
      			{
				dst_pos = 0;
				p_ethhdr = (os_ethhdr_t *)&p_dst[dst_pos];
        	 		memcpy(p_ethhdr->dest.addr, dest_mac, sizeof(p_ethhdr->dest.addr));//add multicast address to packet
				memcpy(p_ethhdr->src.addr,dcp_src_address.addr, sizeof(p_ethhdr->src.addr));//add source mac address to packet
				
				p_ethhdr->type = htons(ETHTYPE_PROFINET);//packet type profinet
				dst_pos += sizeof(os_ethhdr_t);
 	
				/* Insert FrameId */
				p_dst[dst_pos++] = (PF_DCP_GET_SET_FRAME_ID & 0xff00) >> 8;
				p_dst[dst_pos++] = PF_DCP_GET_SET_FRAME_ID & 0x00ff;
	
				p_dcphdr = (pf_dcp_header_t *)&p_dst[dst_pos];
				p_dcphdr->service_id = PF_DCP_SERVICE_GET;
				p_dcphdr->service_type = PF_DCP_SERVICE_TYPE_REQUEST;
				p_dcphdr->xid = htonl(2);
				p_dcphdr->response_delay_factor = htons(1);
				p_dcphdr->data_length = htons(0);    /* At the moment */
				dst_pos += sizeof(pf_dcp_header_t);

				dst_start_pos = dst_pos;
				(void)pf_dcp_put_block(p_dst, &dst_pos, 60, PF_DCP_OPT_ALL, PF_DCP_SUB_ALL, true, 0,0, p_value);
	
			        /* Insert final response length and ship it! */
        	 		p_dcphdr->data_length = htons(dst_pos - dst_start_pos);
        	 		buf->len = dst_pos;
				buf->payload = (void *)((uint8_t *)buf + sizeof(os_buf_t));  /* Payload follows header struct */
				buf->len = 60;
	
				int ret = send (*sock, buf->payload, buf->len, 0);

			   	if (ret>=0)
   				{
   					fflush(stdout);
					printf("set/ connect and disconnect sent\n");
				//	printf("transmission successful\n");
			   	}
				else
					printf("not transmitted: %d \n", ret);
			}
			else
			{
				printf("buffer payload is null\n");
			}
		}
		else
		{
			printf("buffer is null");
		}
		close(*sock);
    	free(sock);
	}
	else
		printf("socket not created");
}
void discover_devices()
{
/******************************var declarations for packet**************************************************************/
	//printf("started discovering\n");
	os_buf_t * buf=NULL;
	uint8_t                 *p_dst;
	uint16_t                dst_pos;
	uint16_t                dst_start_pos;
	os_ethhdr_t             *p_ethhdr;
	pf_dcp_header_t         *p_dcphdr;
	uint8_t                 *p_value=0;
	
/******************************building a socket**************************************************************/
	int * sock;//socket address
	int i;
	struct ifreq ifr;
	struct sockaddr_ll sll;
	int ifindex;
	struct timeval timeout;

	sock = calloc (1, sizeof(int));
	*sock = -1;

	*sock = socket(PF_PACKET, SOCK_RAW, htons(ETHTYPE_PROFINET));//raw socket with protocol type profinet

	timeout.tv_sec =  0;
	timeout.tv_usec = 1;
	setsockopt(*sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

	i = 1;
	setsockopt(*sock, SOL_SOCKET, SO_DONTROUTE, &i, sizeof(i));


	strcpy(ifr.ifr_name, interface_name);
	ioctl(*sock, SIOCGIFINDEX, &ifr);

	ifindex = ifr.ifr_ifindex;
	strcpy(ifr.ifr_name, interface_name);
	ifr.ifr_flags = 0;
	/* reset flags of NIC interface */
	ioctl(*sock, SIOCGIFFLAGS, &ifr);

	/* set flags of NIC interface, here promiscuous and broadcast */
	ifr.ifr_flags = ifr.ifr_flags | IFF_PROMISC | IFF_BROADCAST;
	ioctl(*sock, SIOCSIFFLAGS, &ifr);

	/* bind socket to protocol, in this case RAW EtherCAT */
	sll.sll_family = AF_PACKET;
	sll.sll_ifindex = ifindex;
	sll.sll_protocol = htons(ETHTYPE_PROFINET);
	bind(*sock, (struct sockaddr *)&sll, sizeof(sll));
	if (*sock > -1)
	{
/******************************sending a packet**************************************************************/
		buf=os_buf_alloc(60);

		if (buf != NULL)
   		{
			p_dst = (uint8_t *)buf->payload;
			if (p_dst != NULL)
      			{
				dst_pos = 0;
				p_ethhdr = (os_ethhdr_t *)&p_dst[dst_pos];
        	 		memcpy(p_ethhdr->dest.addr, dcp_mc_address.addr, sizeof(p_ethhdr->dest.addr));//add multicast address to packet
//discover source mac address, if equal to a default dummy MAC, read a current machine MAC and save it for future  use
				if(dcp_src_address.addr[0]==0xc8 && dcp_src_address.addr[1]==0x1f && dcp_src_address.addr[2]==0x66 && dcp_src_address.addr[3]==0x43 && dcp_src_address.addr[4]==0x6a && dcp_src_address.addr[5]==0x45)
				{
					struct ifreq s;
					int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
					strcpy(s.ifr_name, interface_name);
					if (0 == ioctl(fd, SIOCGIFHWADDR, &s)) 
					{
						
      						memcpy(dcp_src_address.addr,s.ifr_addr.sa_data,sizeof(dcp_src_address.addr));
					}
				}
				/*else
				{
					printf();
				}*/

				memcpy(p_ethhdr->src.addr,dcp_src_address.addr, sizeof(p_ethhdr->src.addr));//add source mac address to packet
				
				p_ethhdr->type = htons(ETHTYPE_PROFINET);//packet type profinet
				dst_pos += sizeof(os_ethhdr_t);
 	
				/* Insert FrameId */
				p_dst[dst_pos++] = (PF_DCP_ID_REQ_FRAME_ID & 0xff00) >> 8;
				p_dst[dst_pos++] = PF_DCP_ID_REQ_FRAME_ID & 0x00ff;
	
				p_dcphdr = (pf_dcp_header_t *)&p_dst[dst_pos];
				p_dcphdr->service_id = PF_DCP_SERVICE_IDENTIFY;
				p_dcphdr->service_type = PF_DCP_SERVICE_TYPE_REQUEST;
				p_dcphdr->xid = htonl(1);
				p_dcphdr->response_delay_factor = htons(1);
				p_dcphdr->data_length = htons(0);    /* At the moment */
				dst_pos += sizeof(pf_dcp_header_t);

				dst_start_pos = dst_pos;
				(void)pf_dcp_put_block(p_dst, &dst_pos, 60, PF_DCP_OPT_ALL, PF_DCP_SUB_ALL, true, 0,0, p_value);
	
			        /* Insert final response length and ship it! */
        	 		p_dcphdr->data_length = htons(dst_pos - dst_start_pos);
        	 		buf->len = dst_pos;
				buf->payload = (void *)((uint8_t *)buf + sizeof(os_buf_t));  /* Payload follows header struct */
				buf->len = 60;
	
				int ret = send (*sock, buf->payload, buf->len, 0);

			   	if (ret>=0)
   				{
   					fflush(stdout);
				//	printf("transmission successful\n");
			   	}
				else
					printf("not transmitted: %d %s\n", ret, interface_name);
			}
			else
			{
				printf("buffer payload is null\n");
			}
		}
		else
		{
			printf("buffer is null");
		}
    	free(sock);
	}
	else
		printf("socket not created");
}
void set_ip_config(uint8_t mac[])
{
	os_buf_t * buf=NULL;
	uint8_t                 *p_dst;
	uint16_t                dst_pos;
	uint16_t                dst_start_pos;
	os_ethhdr_t             *p_ethhdr;
	pf_dcp_header_t         *p_dcphdr;
	uint8_t                 *p_value=0;

	uint8_t ip[4];
	uint8_t subnet[4];
	uint8_t gateway[4];
	
	int * sock;
	int i;
	struct ifreq ifr;
	struct sockaddr_ll sll;
	int ifindex;
	struct timeval timeout;

	sock = calloc (1, sizeof(int));
	*sock = -1;

	*sock = socket(PF_PACKET, SOCK_RAW, htons(ETHTYPE_PROFINET));

	timeout.tv_sec =  0;
	timeout.tv_usec = 1;
	setsockopt(*sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

	i = 1;
	setsockopt(*sock, SOL_SOCKET, SO_DONTROUTE, &i, sizeof(i));


	strcpy(ifr.ifr_name, interface_name);
	ioctl(*sock, SIOCGIFINDEX, &ifr);

	ifindex = ifr.ifr_ifindex;
	strcpy(ifr.ifr_name, interface_name);
	ifr.ifr_flags = 0;
	/* reset flags of NIC interface */
	ioctl(*sock, SIOCGIFFLAGS, &ifr);

	/* set flags of NIC interface, here promiscuous and broadcast */
	ifr.ifr_flags = ifr.ifr_flags | IFF_PROMISC | IFF_BROADCAST;
	ioctl(*sock, SIOCSIFFLAGS, &ifr);

	/* bind socket to protocol, in this case RAW EtherCAT */
	sll.sll_family = AF_PACKET;
	sll.sll_ifindex = ifindex;
	sll.sll_protocol = htons(ETHTYPE_PROFINET);
	bind(*sock, (struct sockaddr *)&sll, sizeof(sll));
	if (*sock > -1)
	{
/******************************sending a set packet**************************************************************/

		buf=os_buf_alloc(60);

		if (buf != NULL)
   		{
			p_dst = (uint8_t *)buf->payload;
			if (p_dst != NULL)
      			{
				dst_pos = 0;
				p_ethhdr = (os_ethhdr_t *)&p_dst[dst_pos];
        	 		memcpy(p_ethhdr->dest.addr, mac, sizeof(p_ethhdr->dest.addr));//add multicast address to packet
				memcpy(p_ethhdr->src.addr,dcp_src_address.addr, sizeof(p_ethhdr->src.addr));//add source mac address to packet
				
				p_ethhdr->type = htons(ETHTYPE_PROFINET);//packet type profinet 8892
				dst_pos += sizeof(os_ethhdr_t);

				/* Insert FrameId */
				p_dst[dst_pos++] = (PF_DCP_GET_SET_FRAME_ID & 0xff00) >> 8;
				p_dst[dst_pos++] = PF_DCP_GET_SET_FRAME_ID & 0x00ff;
	
				p_dcphdr = (pf_dcp_header_t *)&p_dst[dst_pos];
				p_dcphdr->service_id = PF_DCP_SERVICE_SET;
				p_dcphdr->service_type = PF_DCP_SERVICE_TYPE_REQUEST;
				p_dcphdr->xid = htonl(2);
				p_dcphdr->response_delay_factor = htons(0);
				p_dcphdr->data_length = htons(0);    /* At the moment */
				dst_pos += sizeof(pf_dcp_header_t);
				int ip1, ip2, ip3, ip4,sn1, sn2,sn3,sn4,gw1,gw2,gw3,gw4;
				
				printf("please enter an IP Address to be assigned to the new found device");
				scanf("%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);	
				printf("please enter its subnet Mask ");
				scanf("%d.%d.%d.%d", &sn1, &sn2, &sn3, &sn4);	
				printf("please enter its Gateway ");
				scanf("%d.%d.%d.%d", &gw1, &gw2, &gw3, &gw4);
				ip[0]=ip1;ip[1]=ip2;ip[2]=ip3;ip[3]=ip4;	
				subnet[0]=sn1;subnet[1]=sn2;subnet[2]=sn3;subnet[3]=sn4;	
				gateway[0]=gw1;gateway[1]=gw2;gateway[2]=gw3;gateway[3]=gw4;	
				uint8_t perm_or_temp[2]={0x00, 0x0e};
				uint8_t ip_params[14]={perm_or_temp[0],perm_or_temp[1],ip[0], ip[1], ip[2], ip[3], subnet[0], subnet[1], subnet[2], subnet[3], gateway[0], gateway[1], gateway[2], gateway[3]};
				dst_start_pos = dst_pos;
				
				(void)pf_dcp_put_block(p_dst, &dst_pos, 60, PF_DCP_OPT_IP, PF_DCP_SUB_IP_PAR, true, 0,14, ip_params);
				
			
				
				
			        /* Insert final response length and ship it! */
        	 		p_dcphdr->data_length = htons(dst_pos - dst_start_pos);
        	 		buf->len = dst_pos;
				buf->payload = (void *)((uint8_t *)buf + sizeof(os_buf_t));  /* Payload follows header struct */
				buf->len = 60;
	
				int ret = send (*sock, buf->payload, buf->len, 0);

			   	if (ret>=0)
   				{
   					fflush(stdout);
				//	printf("transmission successful\n");
			   	}
				else
					printf("not transmitted: %d\n", ret);
			}
			else
			{
				printf("buffer payload is null\n");
			}
		}
		else
		{
			printf("buffer is null");
		}
    	free(sock);
	}
	else
		printf("socket not created");
}
device_list* createNewDevice(char *name, uint8_t mac[6], uint8_t ip[4], uint8_t subnet[4], uint8_t gateway[4])
{
	device_list *node=malloc(sizeof(device_list));
	node->next=NULL;
	node->name=name;
	memcpy(node->mac,mac,6);
	memcpy(node->ip,ip, 4);
	memcpy(node->subnet,subnet,4);
	memcpy(node->gateway,gateway, 4);
	return node;
}

int check(uint8_t mac[])
{
	int found=0;
	for(int i=0; i<device_count; i++)         
	{
		if(!memcmp(devices[i],mac,6))
		{
			found+=1;
			break;
		}
	}
	return found;
}
void addDevice(uint8_t mac[])
{
	if(!check(mac))
	{
		for(int i=0; i<6; i++)         
		{
			devices[device_count][i]=mac[i];
		}
		device_count++;
	}
}

