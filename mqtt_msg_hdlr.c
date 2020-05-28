#include <stdio.h>
#include <string.h>
#include<json-c/json.h>

extern int send_mqtt_pub_default(char*,int);

typedef struct
{
    char ip[32];
    char op[18];
    char api[64];
    char slot[16];
    char sub_slot[16];
    char data[128];
}profinet_req_t;

int json_to_struct(char *buffer, profinet_req_t *out)
{
	struct json_object *parsed_json= NULL;
	struct json_object *ip = NULL, *op= NULL, *api= NULL, *slot= NULL, *sub_slot = NULL, *data=NULL;
	size_t i;	

	parsed_json = json_tokener_parse(buffer);

	if(!parsed_json)
	{
		return -1;
	}
	json_object_object_get_ex(parsed_json, "ip", &ip);
	json_object_object_get_ex(parsed_json, "op", &op);
	json_object_object_get_ex(parsed_json, "api", &api);
	json_object_object_get_ex(parsed_json, "slot", &slot);
	json_object_object_get_ex(parsed_json, "sub_slot", &sub_slot);
	json_object_object_get_ex(parsed_json, "data", &data);

	if(ip)
	{
		printf("IP: %s\n", json_object_get_string(ip));
		strcpy(out->ip, json_object_get_string(ip));
		json_object_put(ip);
	}
	if(op)
	{
		printf("OP: %s\n", json_object_get_string(op));
		strcpy(out->op, json_object_get_string(op));
		json_object_put(op);
	}
	if(api)
	{
		printf("API: %s\n", json_object_get_string(api));
		strcpy(out->api, json_object_get_string(api));
		json_object_put(api);
	}
	if(slot)
	{
		printf("SLOT: %s\n", json_object_get_string(slot));
		strcpy(out->slot, json_object_get_string(slot));
		json_object_put(slot);
	}
	if(sub_slot)
	{
		printf("SUB Slot: %s\n", json_object_get_string(sub_slot));
		strcpy(out->sub_slot, json_object_get_string(sub_slot));
		json_object_put(sub_slot);
	}
	if(data)
	{
		printf("Data: %s\n", json_object_get_string(data));
		strcpy(out->data, json_object_get_string(data));
		json_object_put(data);
	}

	return 0;
}


int HexToInt( char* Hex )
{
  int nLen = strlen( Hex );
  int nDigitMult = 1;
  int nResult = 0;
  for( int i = nLen - 1; i >= 0; i-- )
  {
    char ch = Hex[ i ];
    if( '0' <= ch && ch <= '9' )
      nResult += (ch - '0') * nDigitMult;
    else if( 'a' <= ch && ch <= 'f' )
      nResult += (ch - 'a' + 10) * nDigitMult;
    else if( 'A' <= ch && ch <= 'F' )
      nResult += (ch - 'A' + 10) * nDigitMult;
    nDigitMult *= 16;
  }
  return nResult;
}

int mqtt_msg_hdlr(char *payload, int len)
{
    profinet_req_t out;

    memset(&out, 0x0, sizeof(out));
    json_to_struct(payload, &out);
    printf("IP - %s, OP - %s\n", out.ip, out.op);
    if(strcasecmp(out.op, "read") == 0)
    {
	/* call read */
        int i = 0, len = 0;
        char response[2048]={0}, mqtt_res[2048] = {0};

	send_read_IO_Packet(out.ip, ntohl(atoi(out.api)), ntohs(atoi(out.slot)), ntohs(atoi(out.sub_slot)), response, &len);

	printf("response = [");
	for(i=0;i<len;i++)
        {
		printf("%02X ", response[i] & 0xff);
	}
	printf("]\n");

	strcpy(mqtt_res, "{\"response\": \"");
	for(i=0;i<len;i++)
	{
		char hex[8] ={0};
		sprintf(hex, "%02X", response[i] & 0xff);
		strcat(mqtt_res, hex);
		if(i+1 < len)
			strcat(mqtt_res, " ");
	}
	strcat(mqtt_res, "\" }");
	//printf("json res = %s \n", mqtt_res);
        send_mqtt_pub_default(mqtt_res, strlen(mqtt_res));
    }
    else if(strcasecmp(out.op, "write") == 0)
    {
	    /* call write */
	int i = 0, len = 0, count = 0;
        char response[2048]={0}, mqtt_res[2048] = {0};
	unsigned char string[128]={0};
	char hex[100] = {0};

	/*const int size=sizeof(out.data);
	char datawrite[size]={0};
	for(int i=0; i<size; i++)
	{
		datawrite[size-i]=out.data[i];
	}*/

	const char s[2] = " ";
	char *token;

	/* get the first token */
	token = strtok(out.data, s);

	/* walk through other tokens */
	i=0;
	while( token != NULL ) {
		printf( " %s\n", token );
		string[i++] = HexToInt(token);
		token = strtok(NULL, s);
	}
	count = i;
	printf("input req = [");
	for(i=0;i<count;i++)
        {
                printf("%02X ", (unsigned char)string[i] & 0xff);
        }
	printf("]\n");

	send_write_IO_Packet(out.ip, ntohl(atoi(out.api)), ntohs(atoi(out.slot)),
		ntohs(atoi(out.sub_slot)), string, count, response, &len);

        strcpy(mqtt_res, "{\"response\": \"");
        for(i=0;i<len;i++)
        {
		memset(hex, 0x0, sizeof(hex));
                sprintf(hex, "%02X", (unsigned char)response[i] & 0xff);
                strcat(mqtt_res, hex);
                if(i+1 < len)
                        strcat(mqtt_res, " ");
        }
        strcat(mqtt_res, "\" }");
        //printf("json res = %s \n", mqtt_res);
        send_mqtt_pub_default(mqtt_res, strlen(mqtt_res));

    }
    else
    {
	    printf("Unsupported Operation : %s\n", out.op);
	    return -1;
    }

    return 0;
}

