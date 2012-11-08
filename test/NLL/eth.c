#ifndef _ETH_C
#define _ETH_C

#define INLINE

#include <nll.h>

BOOL is_ethernet_header(void *buffer,u_int_t len ,ETH *ether)
{
	
	u_char_t *buff = (u_char_t *)(buffer);
	memcpy_tos(ether->dst ,buff,ETH_ADDR_LEN);
	memcpy_tos(ether->src , (u_char_t *)(buff + ETH_ADDR_LEN),ETH_ADDR_LEN);
	ether->type = *((u_int16_t *)(buff + 2*ETH_ADDR_LEN));
	
	return TRUE;
	
 }

int create_eth_header(u_char_t *to, u_char_t *host,u_int16_t type,ETH *ether)
{
	memcpy_tos(ether->dst,to,ETH_ADDR_LEN);
	memcpy_tos(ether->src,host,ETH_ADDR_LEN);
	ether->type = htons_tos(type);
	return sizeof(ETH);
}



#endif
