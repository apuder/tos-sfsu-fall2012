#ifndef _ETH_C
#define _ETH_C

#define INLINE

#include <nll.h>

#ifdef NO_TOS
	#include <stdio.h>
#endif


BOOL is_ethernet_header(void *buffer,ETH ether)
{
	u_char_t *buff = (u_char_t *)(buffer);
	memcpy_tos(ether->dst ,buff,ETH_ADDR_LEN);
	memcpy_tos(ether->src , (buff + ETH_ADDR_LEN),ETH_ADDR_LEN);
	ether->type = *((u_int16_t *)(buff + 2*ETH_ADDR_LEN));
	return TRUE;
 }

u_int_t send_eth_packet(u_char_t *to, u_char_t *host, void *data, u_int_t len, u_int16_t type,u_char_t *packet)
{
		u_int_t length;
		length = MIN(len, ETH_MAX_TRANSFER_UNIT);
        memcpy_tos(packet, to, ETH_ADDR_LEN);
        memcpy_tos((packet + ETH_ADDR_LEN), host, ETH_ADDR_LEN);
        *((u_int16_t *)(packet + (2 * ETH_ADDR_LEN))) = htons_tos(type);
         memcpy_tos((packet + ETH_HEAD_LEN), data, len);
         length += ETH_HEAD_LEN;
         while (length < ETH_MIN_LEN)
                 packet[length++] = '\0';
         return length;
	}

#endif
