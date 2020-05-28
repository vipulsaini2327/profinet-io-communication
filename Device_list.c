/*
Purpose: maintain a Linked List of all devices and their properties
Author: Vipul Saini
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct Device
{
	char *name;
	uint8_t mac[6];
	uint8_t ip[4];
	uint8_t subnet[4];
	uint8_t gateway[4];
	struct Device *next;
}devices;


uint8_t gateway[4]={192,168,1,7};
uint8_t ip_range_min[4]={192,168,1,1};
uint8_t ip_range_max[4]={192,168,1,254};
uint8_t subnet[4]={255,255,255,0};



devices *create_device()
{
	devices *node=malloc(sizeof(devices));
	return node;
}

int add_device(devices **head, uint8_t mac[6])
{	
	if(mac)
	{
		devices *node=create_device();
		memcpy(node->mac,mac, 6);
		if(head)
		{
			node->next=*head;
		}
		*head=node;
		return 1;	
	}
	else
		return 0;
}

int add_ip_to_device(devices **head, uint8_t mac[6])
{	
	devices *node;
	int found=0;
	if(head)
	{
		node=*head;
		while(node)
		{
			if(memcmp(node->mac, mac, 6)==0)
			{
				found=1;
				break;
			}
			node=node->next;
		}
		if(found==1)
		{
			if(ip_range_min[3]<ip_range_max[3])// for class one IP Addresses
			{
				memcpy(node->ip, ip_range_min, 4);
				memcpy(node->subnet, subnet, 4);
				memcpy(node->gateway, gateway, 4);
				(ip_range_min[3])+=1;
				printf("assigned an ip :%d.%d.%d.%d\n", ip_range_min[0], ip_range_min[1], ip_range_min[2], ip_range_min[3]);
				printf("assigned a gateway :%d.%d.%d.%d\n", gateway[0], gateway[1], gateway[2], gateway[3]);
				printf("assigned a subnet mask :%d.%d.%d.%d\n", subnet[0], subnet[1], subnet[2], subnet[3]);
			}
			else
			{
				return 0;
				// to do
				if(subnet[3]==0)
				{
				
				}
			}
		}
	}
	else	
		return 0;
}
devices *find_device_by_mac(devices **head, uint8_t mac[6])
{
	int found=0;
	if(*head)
	{
		devices *node=*head;
		node=*head;
		while(node)	
		{
			if(memcmp(mac, node->mac, 6)==0)
			{
				found =1;
				break;
			}
			node=node->next; 
		}
		if(found)
			return node;
		else
			return 0;
	}
	else
	{
		return 0;
	}
}

devices *find_device_by_ip(devices **head, uint8_t ip[4])
{
	if(*head)
	{
		int found=0;
		devices *node=*head;
		node=*head;
		while(node)	
		{
			if(memcmp(ip, node->ip, 4)==0)
			{
				found=1;
				break;
			}
			node=node->next; 
		}
		if(found)
			return node;
		else
			return 0;
	}
	else
	{
		return 0;
	}
}

void show_all_devices(devices **head)
{
	if(*head)
	{
		devices *node=*head;
		node=*head;
		int count=0;
		while(node)	
		{
			printf("Device %d:\nIP- %d.%d.%d.%d\nSubnet Mask- %d.%d.%d.%d\nGateway- %d.%d.%d.%d\nMAC- %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n\n",++count, (node->ip)[0],(node->ip)[1],(node->ip)[2],(node->ip)[3], (node->subnet)[0], (node->subnet)[1], (node->subnet)[2], (node->subnet)[3], (node->gateway)[0], (node->gateway)[1], (node->gateway)[2], (node->gateway)[3], (node->mac)[0], (node->mac)[1], (node->mac)[2], (node->mac)[3], (node->mac)[4], (node->mac)[5]);
			node=node->next; 
		}
	}
	else
	{
		printf("nothing to show, list is empty\n");
	}
}

/*
void main()
{
	devices *head=NULL;
	uint8_t mac[6]={0xac,0xb9,0xff,0x21,0xaf,0xac};
	uint8_t ip[4]={192,168,1,2};
	add_device(&head, mac);
	printf("Device with mac %.2x:%.2x:%.2x:%.2x:%.2x:%.2x added to the list \n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	add_ip_to_device(&head, mac);			
	show_all_devices(&head);
	devices *node=0;
	node=find_device_by_mac(&head, mac);
	if(node)
		printf("device found\n");
	else
		printf("device not found\n");
	node=0;		
	node=find_device_by_ip(&head, ip);
	if(node)
		printf("device found\n");
	else
		printf("device not found\n");
	
	free(head);
}
*/
