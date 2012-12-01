#ifndef _IP_C_
#define _IP_C_

#include <nll.h>
#ifdef NO_TOS
	#include <stdio.h>
#endif

BOOL is_ip_packet(void *buffer, u_int_t len, IP *ip_pkt)
{
	
		u_char_t *buf = (u_char_t *)buffer;
  
  		if (ntohs_tos (*((u_int16_t *)(buf + 2*ETH_ADDR_LEN))) != ETHERTYPE_IP)
            	return FALSE;
		
		u_char_t *ipheader = (u_char_t *)(buf + ETH_HEAD_LEN);
		if(((*(ipheader) & 0x0F)*4) < IP_HEAD_MIN_LEN )
				return FALSE;
		ip_pkt->version = ((*(ipheader) >> 4 ) & 0x0F);
		ip_pkt->hdr_len  = (*(ipheader) & 0x0F);
		ip_pkt->tos = *(ipheader + 1) ;
		ip_pkt->len = *((u_int16_t *)(ipheader + 2));
		ip_pkt->id = *((u_int16_t *)(ipheader + 4));
		ip_pkt->offset = *((u_int16_t *)(ipheader + 6));
		ip_pkt->ttl = *(ipheader + 8);
		ip_pkt->protocol = *(ipheader + 9);
		ip_pkt->checksum = *((u_int16_t *)(ipheader + 10));
		memcpy_tos(ip_pkt->src,(u_char_t *)(ipheader + 12),IP_LEN);
		memcpy_tos(ip_pkt->dst,(u_char_t *)(ipheader + 16),IP_LEN);
		if(ip_pkt->checksum != ip_checksum(ip_pkt))
				return FALSE;
		return TRUE;
	}

u_int16_t ip_checksum(IP *ip)
{
	u_int16_t temp = ip->checksum;
	ip->checksum = 0;
	int len = ip->hdr_len <<2;
	unsigned long sum = 0;  /* assume 32 bit long, 16 bit short */
	const u_int16_t *ip1 = (u_int16_t *)ip;

	while(len > 1){
		sum += *(ip1)++;
		if(sum & 0x80000000)   /* if high order bit set, fold */
			sum = (sum & 0xFFFF) + (sum >> 16);
		len -= 2;
	}

	if(len)      /* take care of left over byte */
		sum += (unsigned short) *(unsigned char *)ip;

	while(sum>>16)
		sum = (sum & 0xFFFF) + (sum >> 16);
	ip->checksum = temp;
	return ~sum;
}

u_int16_t ip_checksum_v2(IP *ip)
{
	ip->checksum = 0;
	int len = ip->hdr_len<<2;
	unsigned long  sum = 0;
	unsigned short answer = 0;
	unsigned short *w = (unsigned short *)ip;


	while(len > 1){
		sum += *w++;
		len -= 2;
	}

	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	answer = ~sum;

	return(answer);
}

/* 
 * method to convert an ip from dot decimal notation
 * to an array of 4 bytes.
 */

#define ctod(c) ((c) -'0')

int inet_aton_tos(u_char_t *dot_ip, u_char_t *net_ip)
{
	int base = 10,val;
	u_char_t c = *dot_ip;
	int dot_count = 1;

  for (;;) 
  {
	if (!is_digit(c))
		  return -1;
	val = 0; 
	for (;;) 
	{
		if (is_digit(c)) 
		  {
			  val = (val * base) + ctod(c);
			  if(val > 255)
				  return -1;
			  c = *++dot_ip;
		 	} else break;
	}
	if (c == '.') 
	{
		if (dot_count > 3)
			return -1;
		*net_ip++ = val;
		dot_count++;
		c = *++dot_ip;
	} else break;
 }

if(dot_count < 3)
	return -1;
if (c != '\0' && (!is_ascii(c) || !is_space(c))) 
  	return -1;
  *net_ip = val;
	return 0;
}

int create_ip_hr(u_char_t *src_ip,u_char_t *dst_ip,u_int_t payload_len,IP *packet)
{
	unsigned short packet_len = (sizeof(IP) + payload_len);

	packet->version = IP_V4;
	packet->hdr_len = sizeof(IP) / sizeof(int);
	packet->tos = IP_TOS_MIN_DELAY;
	packet->len = htons_tos(packet_len);
	packet->id =  htons_tos(0xFEED);
	packet->offset = htons_tos(IP_FLAG_DF);
	packet->ttl = IP_DEFAULT_TTL;
	packet->protocol = IP_PROTO_UDP;
	packet->checksum = 0;
	memcpy_tos(packet->src, src_ip, IP_LEN);
	memcpy_tos(packet->dst, dst_ip, IP_LEN);
	packet->checksum = ip_checksum(packet);

	return (int)packet_len;
}


#endif


