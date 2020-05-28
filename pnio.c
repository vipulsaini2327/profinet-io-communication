
#include"profinet.h"


#define RESERVED_FOR_IMPL_FLAG 0X0001
#define NO_FACK_FLAG 0X0008
#define ACTIVITY_HINT 0xFFFF
#define INTERFACE_HINT 0XFFFF
#define AUTHORISED_PROTOCOL 0x00
#define SERIAL_HIGH 0X00
#define FRAGMENT_LENGTH 0X54
#define FRAGMENT_NUMBER 0X00
#define SERIAL_LOW 0X00

int send_read_IO_Packet(char *ip, uint32_t api, uint16_t slot, uint16_t subslot, uint8_t *response, int *size)
{
	os_buf_t * buf=NULL;
	buf=os_buf_alloc(2048);

	uint16_t dst_pos=0;

	//uint8_t ip[4]={*(dev->ip), *(dev->ip+1),*(dev->ip+2),*(dev->ip+3)};
	//uint32_t addr=((uint32_t)ip[0])<<24|((uint32_t)ip[1])<<16|((uint32_t)ip[2])<<8|(uint32_t)ip[3];

	// Driver code 
	char hostbuffer[256]; 
	char IPbuffer[15];
	getMyIP(IPbuffer); 
	struct hostent *host_entry; 
	int hostname; 
  	char *nibble=NULL;
	int i=0;
	uint8_t myIP[4]={0};
	os_ipaddr_t *source_ip=NULL;
	//os_ipaddr_t *dest_ip=malloc(4);

	os_ethhdr_t *p_ethhdr;
	os_iphdr_t *iphdr;
	os_udphdr_t *udphdr;
   	os_DCEorRPC_t *dceorrpc;
	os_pnio_t *pnio_t;
	os_read_reqhdr_t *readreqhdr;
	os_blockhdr_t *ioblock;

	uint8_t *p_dst;
	p_dst=(uint8_t *)buf->payload;
 
	long port=34964;
	uint8_t objuuid[16]={0};
	dceorrpc = (os_DCEorRPC_t *)&p_dst[dst_pos];
	dceorrpc->version=4;
	dceorrpc->packet_type=0x00;
	dceorrpc->flag=NO_FACK_FLAG;
	dceorrpc->data_rep[0]=0x10; 
	dceorrpc->data_rep[1]=0x00;
	dceorrpc->data_rep[2]= 0x00;
	dceorrpc->serial_high=SERIAL_HIGH;	

	objuuid[0]=0x00;
	objuuid[1]=0x00;
	objuuid[2]=0xa0;
	objuuid[3]=0xde;
	objuuid[4]=0x97;
	objuuid[5]=0x6c;
	objuuid[6]=0xd1;
	objuuid[7]=0x11;
	objuuid[8]=0x82;
	objuuid[9]=0x71;

	memcpy(dceorrpc->object_uuid, objuuid, 16);

	uint8_t interface[16];
	interface[0]= 0x02;
	interface[1]= 0x00;
	interface[2]= 0xa0;
	interface[3]= 0xde;
	interface[4]= 0x97;
	interface[5]= 0x6c;
	interface[6]= 0xd1;
	interface[7]= 0x11;
	interface[8]= 0x82;
	interface[9]= 0x71;
	interface[10]= 0x00;
	interface[11]= 0xa0;
	interface[12]= 0x24;
	interface[13]= 0x42;
	interface[14]= 0xdf;
	interface[15]= 0x7d;
	
	memcpy(dceorrpc->interface, interface, 16);

	uint8_t activity[16];
	activity[0] = 0xdb;
	activity[1] = 0xab;
	activity[2] = 0xba;
	activity[3] = 0xec;
	activity[4] = 0x1d;
	activity[5] = 0x00;
      	activity[6] = 0x54; 
	activity[7] = 0x43;
	activity[8] = 0x50;
	activity[9] = 0xb2;
	activity[10] = 0x0b;
	activity[11] = 0x01;
	activity[12] = 0x63;
	activity[13] = 0x0a;
	activity[14] = 0xba;
	activity[15] = 0xfd;

	memcpy(dceorrpc->activity,activity,16);
	
	dceorrpc->server_boot=0x00;
	dceorrpc->interface_version=(dceorrpc->interface_version | 0x01);
	dceorrpc->sequence=0;
	dceorrpc->option=0x05;
	dceorrpc->interface_hint=INTERFACE_HINT;
	dceorrpc->activity_hint=ACTIVITY_HINT;
	dceorrpc->fragment_length=FRAGMENT_LENGTH;
	dceorrpc->fragment_num=FRAGMENT_NUMBER;
	dceorrpc->auth_proto=AUTHORISED_PROTOCOL;
	dceorrpc->serial_low=SERIAL_LOW;
	
	dst_pos += sizeof(os_DCEorRPC_t);		
	
	
	pnio_t=(os_pnio_t *)&p_dst[dst_pos];
	pnio_t->max_arg=0x8040;
  	pnio_t->arg_length=0x00000040;
  	pnio_t->maxcount=0x8040;
  	pnio_t->offset=htonl(0);
  	pnio_t->actual_count=0x00000040;
  	readreqhdr=&(pnio_t->read_req_header);

	ioblock=&(readreqhdr->block);
	ioblock->block_type=0x0900;
	ioblock->length=0x3c00;
	ioblock->version_high=1;
	ioblock->version_low=htons(0);
	
	
	readreqhdr->seq_nbr=0x0a00;
	readreqhdr->api=api;
	readreqhdr->slot=slot;
	readreqhdr->subslot=subslot;
	readreqhdr->padding=0x0000;
	readreqhdr->index=0x40;//
	readreqhdr->index=(readreqhdr->index)<<8;
	readreqhdr->index=readreqhdr->index|0xf8;//
	readreqhdr->record_data_length=0x80;
	readreqhdr->record_data_length=(readreqhdr->record_data_length)<<16;
	readreqhdr->padding2[0]=0;
	readreqhdr->padding2[1]=0;
	
	dst_pos+= sizeof(os_pnio_t);
	
	//char plcip[12]={0};
	//memcpy(dest_ip->addr,ip, 4);
	//sprintf(plcip, "%d.%d.%d.%d", dest_ip->addr[0],dest_ip->addr[1],dest_ip->addr[2],dest_ip->addr[3]);
	//printf(plcip, "%d.%d.%d.%d", dest_ip->addr[0],dest_ip->addr[1],dest_ip->addr[2],dest_ip->addr[3]);
	int portno=34964;
	unsigned char *payload=(char *)p_dst;
	size_t len=sizeof(p_dst);
			
	int sockfd, n;

	int serverlen;

	struct sockaddr_in serveraddr;

	struct hostent *server;

 

	/* socket: create the socket */

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sockfd < 0)
		perror("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
	server = gethostbyname(ip);

	if (server == NULL) 
	{

		//fprintf(stderr,"ERROR, no such host as %2d . %2d . %2d . %2d\n", dest_ip->addr[0], dest_ip->addr[1], dest_ip->addr[2], dest_ip->addr[3]);
		return -1;
	}

	/* build the server's Internet address */

	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,(char *)&serveraddr.sin_addr.s_addr, server->h_length);
	serveraddr.sin_port = htons(portno);

	/* send the message to the server */

	serverlen = sizeof(serveraddr);
	n = sendto(sockfd, payload, dst_pos, 0,(struct sockaddr *) &serveraddr, serverlen);
	if (n < 0)
		perror("ERROR in sendto");
	else
	{
	   	printf("send ret: %d\n", n);
		struct timeval waittime;
		socklen_t waitize=sizeof(waittime);
		waittime.tv_sec=5;
		setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &waittime, waitize);
		int length=2048;
		uint8_t buffer[length];	
		n=recvfrom(sockfd, (void *)buffer, length, 0, (struct sockaddr *) &serveraddr, (socklen_t *)&serverlen);
		if(n<0)
			printf("nothing recieved%s\n", buffer);
		else
		{
			printf("%d bytes recieved: \n", n);
			memcpy(response, buffer, n);
			*size = n;
		}			
	}
	
	//free(dest_ip);
	close(sockfd);
	return 0;
}

int send_write_IO_Packet(char *ip, uint32_t api, uint16_t slot, uint16_t subslot, uint8_t *data, uint32_t len, uint8_t *response, int *size)
{
	os_buf_t * buf=NULL;
	buf=os_buf_alloc(2048);


	uint16_t dst_pos=0;

	//uint8_t ip[4]={*(dev->ip), *(dev->ip+1),*(dev->ip+2),*(dev->ip+3)};
	//uint32_t addr=((uint32_t)ip[0])<<24|((uint32_t)ip[1])<<16|((uint32_t)ip[2])<<8|(uint32_t)ip[3];

	char hostbuffer[256]; 
	char IPbuffer[15];
	getMyIP(IPbuffer); 
	struct hostent *host_entry; 
	int hostname; 
  	char *nibble=NULL;
	int i=0;
	uint8_t myIP[4]={0};
	os_ipaddr_t *source_ip=NULL;
	os_ipaddr_t *dest_ip=malloc(4);

	os_ethhdr_t *p_ethhdr;
	os_iphdr_t *iphdr;
	os_udphdr_t *udphdr;
   	os_DCEorRPC_t *dceorrpc;
	os_pnio_t *pnio_t;
	os_write_reqhdr_t *writereqhdr;
	os_blockhdr_t *ioblock;

	uint8_t *p_dst;
	p_dst=(uint8_t *)buf->payload;
 
	long port=34964;

	uint8_t objuuid[16]={0};
	dceorrpc = (os_DCEorRPC_t *)&p_dst[dst_pos];
	dceorrpc->version=4;
	dceorrpc->packet_type=0x00;
	dceorrpc->flag=NO_FACK_FLAG;
	dceorrpc->data_rep[0]=0x10; 
	dceorrpc->data_rep[1]=0x00;
	dceorrpc->data_rep[2]= 0x00;
	dceorrpc->serial_high=SERIAL_HIGH;	

	objuuid[0]=0x00;
	objuuid[1]=0x00;
	objuuid[2]=0xa0;
	objuuid[3]=0xde;
	objuuid[4]=0x97;
	objuuid[5]=0x6c;
	objuuid[6]=0xd1;
	objuuid[7]=0x11;
	objuuid[8]=0x82;
	objuuid[9]=0x71;
	objuuid[10]=0x00;
	objuuid[11]=0x00;
	objuuid[12]=0x00;
	objuuid[13]=0x00;
	objuuid[14]=0x00;
	objuuid[15]=0x00;

	memcpy(dceorrpc->object_uuid, objuuid, 16);

	uint8_t interface[16];

	interface[0]= 0x02;
	interface[1]= 0x00;
	interface[2]= 0xa0;
	interface[3]= 0xde;
	interface[4]= 0x97;
	interface[5]= 0x6c;
	interface[6]= 0xd1;
	interface[7]= 0x11;
	interface[8]= 0x82;
	interface[9]= 0x71;
	interface[10]= 0x00;
	interface[11]= 0xa0;
	interface[12]= 0x24;
	interface[13]= 0x42;
	interface[14]= 0xdf;
	interface[15]= 0x7d;
	
	memcpy(dceorrpc->interface, interface, 16);

	uint8_t activity[16];
	activity[0] = 0xdb;
	activity[1] = 0xab;
	activity[2] = 0xba;
	activity[3] = 0xec;
	activity[4] = 0x1d;
	activity[5] = 0x00;
      	activity[6] = 0x54; 
	activity[7] = 0x43;
	activity[8] = 0x50;
	activity[9] = 0xb2;
	activity[10] = 0x0b;
	activity[11] = 0x01;
	activity[12] = 0x63;
	activity[13] = 0x0a;
	activity[14] = 0xba;
	activity[15] = 0xfd;

	memcpy(dceorrpc->activity,activity,16);
	
	dceorrpc->server_boot=0x00;
	dceorrpc->interface_version=0x01;
	dceorrpc->sequence=0;
	dceorrpc->option=0x03;
	dceorrpc->interface_hint=INTERFACE_HINT;
	dceorrpc->activity_hint=ACTIVITY_HINT;
	dceorrpc->fragment_length=FRAGMENT_LENGTH;
	dceorrpc->fragment_num=FRAGMENT_NUMBER;
	dceorrpc->auth_proto=AUTHORISED_PROTOCOL;
	dceorrpc->serial_low=SERIAL_LOW;
	
	dst_pos += sizeof(os_DCEorRPC_t);		
	
	
	pnio_t=(os_pnio_t *)&p_dst[dst_pos];
	pnio_t->max_arg=0x8040;
  	pnio_t->arg_length=0x00000040;
  	pnio_t->maxcount=0x8040;
  	pnio_t->offset=htonl(0);
  	pnio_t->actual_count=0x00000040;
  	writereqhdr=(os_write_reqhdr_t *)&(pnio_t->read_req_header);

	ioblock=&(writereqhdr->block);
	ioblock->block_type=0x0800;
	ioblock->length=0x3f00;
	ioblock->version_high=1;
	ioblock->version_low=htons(0);
	
	
	writereqhdr->seq_nbr=0x0110;

	uint8_t ar_uuid[16];
	ar_uuid[0] = 0x10;
	ar_uuid[1] = 0x01;
	ar_uuid[2] = 0x00;
	ar_uuid[3] = 0x00;
	ar_uuid[4] = 0x00;
	ar_uuid[5] = 0x00;
      	ar_uuid[6] = 0x00; 
	ar_uuid[7] = 0x00;
	ar_uuid[8] = 0x00;
	ar_uuid[9] = 0x00;
	ar_uuid[10] = 0x00;
	ar_uuid[11] = 0x00;
	ar_uuid[12] = 0x00;
	ar_uuid[13] = 0x00;
	ar_uuid[14] = 0x00;
	ar_uuid[15] = 0x00;

	memcpy(writereqhdr->ar_uuid,ar_uuid,16);

	writereqhdr->api=api;
	writereqhdr->slot=slot;
	writereqhdr->subslot=subslot;
	writereqhdr->padding=0x0000;
	writereqhdr->index=0x40;
	writereqhdr->index=(writereqhdr->index)<<8;
	writereqhdr->index=writereqhdr->index|0xf8;
	//writereqhdr->record_data_length=0x08000000;
	writereqhdr->record_data_length=htonl(len);
	printf("write len = %d\n", len);
	memcpy(writereqhdr->dataValue, data, len);
	/*writereqhdr->dataValue[0]=0x01;
	writereqhdr->dataValue[1]=0x02;
	writereqhdr->dataValue[2]=0x03;
	writereqhdr->dataValue[3]=0x04;
	writereqhdr->dataValue[4]=0x05;
	writereqhdr->dataValue[5]=0x06;
	writereqhdr->dataValue[6]=0x07;
	writereqhdr->dataValue[7]=0x08;*/
	
	
	dst_pos+= sizeof(os_pnio_t);



	//char plcip[12]={0};
	//memcpy(dest_ip->addr,dev->ip, 4);
	//sprintf(plcip, "%d.%d.%d.%d", dest_ip->addr[0],dest_ip->addr[1],dest_ip->addr[2],dest_ip->addr[3]);
	//printf(plcip, "%d.%d.%d.%d", dest_ip->addr[0],dest_ip->addr[1],dest_ip->addr[2],dest_ip->addr[3]);
	int portno=34964;
	unsigned char *payload=(char *)p_dst;
	//size_t len=sizeof(p_dst);
			
	int sockfd, n;

	int serverlen;

	struct sockaddr_in serveraddr;

	struct hostent *server;

 

	/* socket: create the socket */

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sockfd < 0)
		perror("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
	server = gethostbyname(ip);

	if (server == NULL) 
	{

		//fprintf(stderr,"ERROR, no such host as %2d . %2d . %2d . %2d\n", dest_ip->addr[0], dest_ip->addr[1], dest_ip->addr[2], dest_ip->addr[3]);
		return -1;
	}

	/* build the server's Internet address */

	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,(char *)&serveraddr.sin_addr.s_addr, server->h_length);
	serveraddr.sin_port = htons(portno);

	/* send the message to the server */

	serverlen = sizeof(serveraddr);
	n = sendto(sockfd, payload, dst_pos, 0,(struct sockaddr *) &serveraddr, serverlen);
	if (n < 0)
		perror("ERROR in sendto");
	else
	{
	   	printf("send ret: %d\n", n);
		struct timeval waittime;
		socklen_t waitize=sizeof(waittime);
		waittime.tv_sec=5;
		setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &waittime, waitize);
		int length=2048;
		uint8_t buffer[length];	
		n=recvfrom(sockfd, (void *)buffer, length, 0, (struct sockaddr *) &serveraddr, (socklen_t *)&serverlen);
		if(n<0)
			printf("nothing recieved%s\n", buffer);
		else
		{
			printf("%d bytes recieved: \n", n);
			memcpy(response, buffer, n);
			*size = n;
		}			
	}
	
	free(dest_ip);
	close(sockfd);
	return 0;
}
