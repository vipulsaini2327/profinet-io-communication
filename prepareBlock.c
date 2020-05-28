#include "profinet.h"

void pf_put_mem(const void *p_src, uint16_t src_size, /* Bytes to copy */ uint16_t res_len,/* Sizeof p_bytes buf */ uint8_t *p_bytes, uint16_t *p_pos)
{
   if (((*p_pos) + src_size) >= res_len)
   {
      p_bytes = NULL;
   }

   if (p_bytes != NULL)
   {
      memcpy(&p_bytes[*p_pos], p_src, src_size);
      (*p_pos) += src_size;
   }
}

void pf_put_byte(uint8_t val, uint16_t res_len, uint8_t *p_bytes, uint16_t *p_pos)
{
   if (*p_pos >= res_len)
   {
      p_bytes = NULL;
   }

   if (p_bytes != NULL)
   {
      p_bytes[*p_pos] = val;
      (*p_pos)++;
   }
}
void pf_put_uint16(bool is_big_endian, uint16_t val, uint16_t res_len, uint8_t *p_bytes, uint16_t *p_pos)
{
   if (is_big_endian)
   {
      pf_put_byte((val >> 8) & 0xff, res_len, p_bytes, p_pos);
      pf_put_byte(val & 0xff, res_len, p_bytes, p_pos);
   }
   else
   {
      pf_put_byte(val & 0xff, res_len, p_bytes, p_pos);
      pf_put_byte((val >> 8) & 0xff, res_len, p_bytes, p_pos);
   }
}
void pf_put_uint32(bool is_big_endian, uint32_t val, uint16_t res_len, uint8_t *p_bytes, uint16_t *p_pos)
{
   if (is_big_endian)
   {
      pf_put_byte((val >> 24) & 0xff, res_len, p_bytes, p_pos);
      pf_put_byte((val >> 16) & 0xff, res_len, p_bytes, p_pos);
      pf_put_byte((val >> 8) & 0xff, res_len, p_bytes, p_pos);
      pf_put_byte(val & 0xff, res_len, p_bytes, p_pos);
   }
   else
   {
      pf_put_byte(val & 0xff, res_len, p_bytes, p_pos);
      pf_put_byte((val >> 8) & 0xff, res_len, p_bytes, p_pos);
      pf_put_byte((val >> 16) & 0xff, res_len, p_bytes, p_pos);
      pf_put_byte((val >> 24) & 0xff, res_len, p_bytes, p_pos);
   }
}
int pf_dcp_put_block(uint8_t *p_dst, uint16_t *p_dst_pos, uint16_t dst_max, uint8_t opt, uint8_t sub, bool with_block_info, uint16_t block_info, uint16_t value_length, const void *p_value)
{
	int                     ret = -1;
	uint16_t                b_len;

	if ((*p_dst_pos + value_length) < dst_max)
	{
		b_len = value_length;//Adjust written block length if block_info is included 
   
		pf_put_byte(opt, dst_max, p_dst, p_dst_pos);
		pf_put_byte(sub, dst_max, p_dst, p_dst_pos);
		pf_put_uint16(true, b_len, dst_max, p_dst, p_dst_pos);
      
		if ((p_value != NULL) && (value_length > 0))
		{
			pf_put_mem(p_value, value_length, dst_max, p_dst, p_dst_pos);
		}


		while ((*p_dst_pos) & 1)//       Add padding to align on uint16_t 
		{
			pf_put_byte(0, dst_max, p_dst, p_dst_pos);
		}
	}
	
	ret = 0;       // Skip excess data silently !!

	return ret;
}

os_buf_t * os_buf_alloc(uint16_t length)
{
	os_buf_t *p = malloc(sizeof(os_buf_t) + length);

	if (p != NULL)
	{
	memset(p, 0x0, sizeof(os_buf_t) + length);
#if 1
	p->payload = (void *)((uint8_t *)p + sizeof(os_buf_t));  /* Payload follows header struct */
	p->len = length;
#endif
	}
	else
	{
		printf("malloc() failed\n");
	}
	return p;
}

