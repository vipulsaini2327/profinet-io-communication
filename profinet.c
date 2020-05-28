/*
Purpose: supervisor for all profinet devices over the local network
Author: Vipul Saini
Date: 4 jan 2019

*/

/**************************************headers*******************************************************************/

#include"profinet.h"
extern void*create_mqtt_task(void*);

CC_STATIC_ASSERT(PF_DCP_BLOCK_HDR_SIZE == sizeof(pf_dcp_block_hdr_t));

static int request_interval=0;
void read_configurations()
{
	FILE *file=fopen("config", "r");
	char line[256];
	int linenum=0;
	while(fgets(line, 256, file) != NULL)
	{
	        char key[256], value[256];

	        linenum++;
	        if(line[0] == '#') continue;
		        if(sscanf(line, "%s %s", key, value) != 2)
		        {
		                fprintf(stderr, "Syntax error, line %d\n", linenum);
		                continue;
		        }
		if(!strcmp(key, "IDENT_REQ_TIMEOUT"))
		{
			request_interval=atoi(value);
		}
		else
			if(!strcmp(key, "INTERFACE_NAME"))
			{
				interface_name=malloc(50);
				memcpy(interface_name,value, sizeof(value));
			}
	        printf("%s  %s\n", key, value);	
	}
}
void *discover(void * arg)
{
	while(1)
	{
		discover_devices();
		discover_devices();
		sleep(request_interval);
	}
}
void *recieve(void * arg)
{
	while(1)
	{
		recieve_profinet_packets();
	}
}
int main()
{
	start=NULL;
	read_configurations();
	pthread_t dis, rec, mqtt;
	
	pthread_create(&mqtt, NULL, create_mqtt_task,0);
	pthread_create(&dis, NULL, discover,0);
	pthread_create(&rec, NULL, recieve,0);
	while(1)
	{
		sleep(10);
	}
	//recieve_profinet_packets();
	//check_ip_address();
}




