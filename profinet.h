/*
DCP macros and essentials

*/
/********************************flags for dcp protocol***************************************************/


#define PF_DCP_SERVICE_GET                      0x03
#define PF_DCP_SERVICE_SET                      0x04
#define PF_DCP_SERVICE_IDENTIFY                 0x05
#define PF_DCP_SERVICE_HELLO                    0x06

#define PF_DCP_BLOCK_PADDING                    0x00

#define PF_DCP_SERVICE_TYPE_REQUEST             0x00
#define PF_DCP_SERVICE_TYPE_SUCCESS             0x01
#define PF_DCP_SERVICE_TYPE_NOT_SUPPORTED       0x05

#define PF_DCP_HEADER_SIZE                      10
#define PF_DCP_BLOCK_HDR_SIZE                   4

#define PF_DCP_HELLO_FRAME_ID                   0xfefc
#define PF_DCP_GET_SET_FRAME_ID                 0xfefd
#define PF_DCP_ID_REQ_FRAME_ID                  0xfefe
#define PF_DCP_ID_RES_FRAME_ID                  0xfeff

#define PF_DCP_OPT_RESERVED_0  0
#define PF_DCP_OPT_IP 0x01
#define PF_DCP_OPT_DEVICE_PROPERTIES 0x02
#define PF_DCP_OPT_DHCP 0x03
#define PF_DCP_OPT_RESERVED_4 0x04
#define PF_DCP_OPT_CONTROL 0x05
#define PF_DCP_OPT_DEVICE_INITIATIVE 0x06
   /*
    * Reserved                0x07 .. 0x7f
    * Manufacturer specific   0x80 .. 0xfe
    */
#define PF_DCP_OPT_ALL 0xff
/*ip properties*/
#define PF_DCP_SUB_ALL 0xff
#define PF_DCP_SUB_DEVICE_INITIATIVE 0x01

#define    PF_DCP_SUB_IP_MAC 0x01                 /* Read */
#define    PF_DCP_SUB_IP_PAR 0x02                       /* Read/Write */
#define    PF_DCP_SUB_IP_SUITE 0x03                       /* Read/(optional Write) */
/*device properties*/
#define    PF_DCP_SUB_DEV_PROP_VENDOR 0x01        /* Read */
#define    PF_DCP_SUB_DEV_PROP_NAME 0x02                /* Read/Write */
#define    PF_DCP_SUB_DEV_PROP_ID 0x03                  /* Read */
#define    PF_DCP_SUB_DEV_PROP_ROLE 0x04                /* Read */
#define    PF_DCP_SUB_DEV_PROP_OPTIONS  0x05            /* Read */
#define    PF_DCP_SUB_DEV_PROP_ALIAS                /* Used as filter only */
#define    PF_DCP_SUB_DEV_PROP_INSTANCE             /* Read, optional */
#define    PF_DCP_SUB_DEV_PROP_OEM_ID               /* Read, optional */
#define    PF_DCP_SUB_DEV_PROP_GATEWAY 
/*control properties*/
#define PF_DCP_SUB_CONTROL_START 0x01          /* Write */
#define    PF_DCP_SUB_CONTROL_STOP 0x02               /* Write */
#define    PF_DCP_SUB_CONTROL_SIGNAL 0x03               /* Write */
#define    PF_DCP_SUB_CONTROL_RESPONSE 0x04
#define    PF_DCP_SUB_CONTROL_FACTORY_RESET 0x05        /* Optional, Write */
#define    PF_DCP_SUB_CONTROL_RESET_TO_FACTORY 0x05     /* Write */
/*error properties*/
#define    PF_DCP_BLOCK_ERROR_NO_ERROR
#define    PF_DCP_BLOCK_ERROR_OPTION_NOT_SUPPORTED
#define    PF_DCP_BLOCK_ERROR_SUBOPTION_NOT_SUPPORTED
#define    PF_DCP_BLOCK_ERROR_SUBOPTION_NOT_SET
#define    PF_DCP_BLOCK_ERROR_RESOURCE_ERROR
#define    PF_DCP_BLOCK_ERROR_SET_NOT_POSSIBLE

#define ETHTYPE_PROFINET 0x8892U

#ifndef CC_H
#define CC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <assert.h>

static inline void cc_assert (int exp)
{
   assert (exp);              // LCOV_EXCL_LINE
}

#define CC_PACKED_BEGIN
#define CC_PACKED_END
#define CC_PACKED             __attribute__((packed))

#define CC_FORMAT(str,arg)    __attribute__((format (printf, str, arg)))

#if BYTE_ORDER == LITTLE_ENDIAN
#define CC_TO_LE16(x)         (x)
#define CC_TO_LE32(x)         (x)
#define CC_TO_LE64(x)         (x)
#define CC_FROM_LE16(x)       (x)
#define CC_FROM_LE32(x)       (x)
#define CC_FROM_LE64(x)       (x)
#else
#define CC_TO_LE16(x)         __builtin_bswap16 (x)
#define CC_TO_LE32(x)         __builtin_bswap32 (x)
#define CC_TO_LE64(x)         __builtin_bswap64 (x)
#define CC_FROM_LE16(x)       __builtin_bswap16 (x)
#define CC_FROM_LE32(x)       __builtin_bswap32 (x)
#define CC_FROM_LE64(x)       __builtin_bswap64 (x)
#endif

#define CC_ATOMIC_GET8(p)     __atomic_load_n ((p), __ATOMIC_SEQ_CST)
#define CC_ATOMIC_GET16(p)    __atomic_load_n ((p), __ATOMIC_SEQ_CST)
#define CC_ATOMIC_GET32(p)    __atomic_load_n ((p), __ATOMIC_SEQ_CST)
#define CC_ATOMIC_GET64(p)    __atomic_load_n ((p), __ATOMIC_SEQ_CST)

#define CC_ATOMIC_SET8(p, v)  __atomic_store_n ((p), (v), __ATOMIC_SEQ_CST)
#define CC_ATOMIC_SET16(p, v) __atomic_store_n ((p), (v), __ATOMIC_SEQ_CST)
#define CC_ATOMIC_SET32(p, v) __atomic_store_n ((p), (v), __ATOMIC_SEQ_CST)
#define CC_ATOMIC_SET64(p, v) __atomic_store_n ((p), (v), __ATOMIC_SEQ_CST)

#define CC_ASSERT(exp)        cc_assert (exp)
#ifdef __cplusplus
#define CC_STATIC_ASSERT(exp) static_assert (exp, "")
#else
#define CC_STATIC_ASSERT(exp) _Static_assert (exp, "")
#endif

#ifdef __cplusplus
}
#endif

#endif

#include <errno.h>
#include <sys/time.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <net/if.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/ioctl.h>
#include <netpacket/packet.h>
#include <netinet/in.h>
#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<linux/if_ether.h>
#include<linux/ip.h>
#include<linux/tcp.h>
#include<linux/udp.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <netdb.h>
#include <sys/types.h> 

/**************************************structures*******************************************************************/
typedef struct os_ethaddr
{
  uint8_t addr[6];
} os_ethaddr_t;

typedef struct os_ethhdr
{
  os_ethaddr_t dest;
  os_ethaddr_t src;
  uint16_t  type;
} os_ethhdr_t;

typedef struct os_buf
{
   void * payload;
   uint16_t len;
}os_buf_t;

struct pack
{
	uint8_t source_mac[6];
	uint8_t dest_mac[6];
	uint8_t source[6];
	uint16_t packet_type;
	uint16_t frame_id;
	uint8_t service_id;
	uint8_t service_type;
	uint32_t  xid;
	uint16_t  reserve_or_delay;
	uint16_t  ddl;
};
struct block
{
	uint8_t option;
	uint8_t suboption;
	int block_length;
	uint16_t block_info;	
};

CC_PACKED_BEGIN
typedef struct CC_PACKED pf_dcp_header
{
   uint8_t                 service_id;
   uint8_t                 service_type;
   uint32_t                xid;
   uint16_t                response_delay_factor;
   uint16_t                data_length;
} pf_dcp_header_t;
CC_PACKED_END
CC_STATIC_ASSERT(PF_DCP_HEADER_SIZE == sizeof(pf_dcp_header_t));

CC_PACKED_BEGIN
typedef struct CC_PACKED pf_dcp_block_hdr
{
   uint8_t                 option;
   uint8_t                 sub_option;
   uint16_t                block_length;
} pf_dcp_block_hdr_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct CC_PACKED os_ipaddr
{
  uint8_t addr[4];
} os_ipaddr_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct CC_PACKED os_iphdr
{
  uint8_t vrsnANDheadLen;
  uint8_t DiffServField;
  uint16_t length;
  uint16_t identification;
  uint16_t flags;
  uint8_t ttl;	
  uint8_t protocol;	
  uint16_t checksum;
  os_ipaddr_t src;
  os_ipaddr_t dest;

} os_iphdr_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct CC_PACKED DCEorRPC
{
  uint8_t version;//1
  uint8_t packet_type;//1
  uint16_t flag;//1
  uint8_t data_rep[3];//3
  uint8_t serial_high;//1	
  uint8_t object_uuid[16];//16
  uint8_t interface[16];//16
  uint8_t activity[16];//16
  uint32_t server_boot;//4
  uint32_t interface_version;//4
  uint32_t sequence;//4
  uint16_t option;//2
  uint16_t interface_hint;//2
  uint16_t activity_hint;//2
  uint16_t fragment_length;//2
  uint16_t fragment_num;//2
  uint8_t auth_proto;//1
  uint8_t serial_low;//1
} os_DCEorRPC_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct CC_PACKED block_hdr
{
  uint16_t block_type;
  uint16_t length;
  uint8_t version_high;
  uint8_t version_low;
} os_blockhdr_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct CC_PACKED IOD_read_reqhdr
{
  os_blockhdr_t block;
  uint16_t seq_nbr;
  uint8_t ar_uuid[16];
  uint32_t api;
  uint16_t slot;
  uint16_t subslot;
  uint16_t padding;
  uint16_t index;
  uint32_t record_data_length;
  uint8_t target_ar_uuid[16];
  uint32_t padding2[2];
} os_read_reqhdr_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct CC_PACKED IOD_write_reqhdr
{
  os_blockhdr_t block;
  uint16_t seq_nbr;
  uint8_t ar_uuid[16];
  uint32_t api;
  uint16_t slot;
  uint16_t subslot;
  uint16_t padding;
  uint16_t index;
  uint32_t record_data_length;
 // uint8_t target_ar_uuid[16];
  uint8_t dataValue[8];
} os_write_reqhdr_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct CC_PACKED PN_IO
{
  uint32_t max_arg;
  uint32_t arg_length;
  uint32_t maxcount;
  uint32_t offset;
  uint32_t actual_count;
  os_read_reqhdr_t read_req_header;
} os_pnio_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct CC_PACKED os_udphdr
{
  uint16_t src_port;
  uint16_t dst_port;
  uint16_t length;
  uint16_t checksum;
} os_udphdr_t;
CC_PACKED_END

typedef struct device_list
{
	char *name;
	uint8_t *mac;
	uint8_t ip[4];
	uint8_t subnet[4];
	uint8_t gateway[4];
	struct device_list *next;
}device_list;

typedef struct profinet_device
{
	char *device_name;
	uint8_t mac[6];
	uint8_t ip[4];
	uint8_t subnet[4];
	uint8_t gateway[4];
	struct profinet_device *next;
}device;


static device_list *head=NULL;
struct Device *start;
/************************************global variables**************************************************************/

static os_ethaddr_t  dcp_src_address=
{
  {0xc8, 0x1f, 0x66, 0x43, 0x6a, 0x45}
};
static os_ethaddr_t  dcp_mc_address = { {0x01, 0x0e, 0xcf, 0x00, 0x00, 0x00 } };

static char *if_name="enp2s0";
static uint8_t devices[][6]={0};
static int device_count=0;	
char *interface_name;

/************************************functions declarations*********************************************************/
void pf_put_mem(const void *, uint16_t , /* Bytes to copy */ uint16_t ,/* Sizeof p_bytes buf */ uint8_t *, uint16_t *);
void pf_put_byte(uint8_t , uint16_t , uint8_t *, uint16_t *);
void pf_put_uint16(bool , uint16_t , uint16_t , uint8_t *, uint16_t *);
void pf_put_uint32(bool , uint32_t , uint16_t , uint8_t *, uint16_t *);
int pf_dcp_put_block(uint8_t *, uint16_t *, uint16_t , uint8_t , uint8_t , bool , uint16_t , uint16_t , const void *);
void send_set_req(uint8_t []);
void send_get_req(uint8_t []);
void discover_devices();
int check(uint8_t []);
void addDevice(uint8_t []);
int extract_blocks_from_packet(char *, int , device *);
void testIO(uint8_t []);
void recieve_profinet_packets();
void *discover(void *);
void *recieve(void *);
void saveDevice(device_list *);
device_list* createNewDevice(char *, uint8_t *, uint8_t *, uint8_t *, uint8_t *);
void set_ip_config(uint8_t []);
os_buf_t * os_buf_alloc(uint16_t );
void getMyIP(char *);
int send_write_IO_Packet(char *ip, uint32_t api, uint16_t slot, uint16_t subslot, uint8_t *data, uint32_t len, uint8_t *response, int *size);
int send_read_IO_Packet(char *ip, uint32_t api, uint16_t slot, uint16_t subslot, uint8_t *response, int *size);
void checkHostName(int);
void checkHostEntry(struct hostent *); 
void checkIPbuffer(char *);
int check_ip_address();
device* findDeviceByMAC(uint8_t [6]);
device* findDeviceByIP(uint8_t [4]);
device* findDeviceByName(char *);

