#include "profinet.h"

static device *top=NULL;
int number_of_devices=0;
int addNewDevice(device *node)
{
	if(NULL ==node)
	{
		return 0;
	}
	else
	{
		device *node=malloc(sizeof(device));
		if(NULL == top)
		{
			top=node;
		}
		else
		{
			device *n=top;
			while(n)
			{
				n=n->next;
			}
			n->next=node;
		}
		number_of_devices++;
		return 1;
	}
}
device* findDeviceByName(char *name)
{
	if(NULL != name)
	{
		if(NULL == top)
		{
			return 0;
		}
		else
		{
			int found=0;
			device *node=top;
			while(node)
			{
				if(strcmp(node->device_name, name)==0)
				{
					found++;
					break;
				}
				node=node->next;
			}
			if(found)
				return node;
		}
	}
	else
		return 0;
}
device* findDeviceByIP(uint8_t ip[4])
{
	if(NULL != ip)
	{
		if(NULL == top)
		{
			return 0;
		}
		else
		{
			int found=0;
			device *node=top;
			while(node)
			{
				if((node->ip)[0]==ip[0] && (node->ip)[1]==ip[1] && (node->ip)[2]==ip[2] && (node->ip)[3]==ip[3])
				{
					found++;
					break;
				}
				node=node->next;
			}
			if(found)
				return node;
		}
	}
	else
		return 0;
}
device* findDeviceByMAC(uint8_t mac[6])
{
	if(NULL != mac)
	{
		if(NULL == top)
		{
			return 0;
		}
		else
		{
			int found=0;
			device *node=top;
			while(node)
			{
				if((node->mac)[0]==mac[0] && (node->mac)[1]==mac[1] && (node->mac)[2]==mac[2] && (node->mac)[3]==mac[3] && (node->mac)[4]==mac[4] && (node->mac)[5]==mac[5])
				{
					found++;
					break;
				}
				node=node->next;
			}
			if(found)
				return node;
		}
	}
	else
		return 0;
}
int checkDEVICEINLIST(device *node)
{
	if(NULL == node)
	{
		return -1;
	}
	else
	{
			
	}
}
