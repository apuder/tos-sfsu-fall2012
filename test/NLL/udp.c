#ifndef _UDP_C
#define _UDP_C

#include <nll.h>
#ifdef NO_TOS
	#include <stdio.h>
#endif

BOOL is_udp_packet(void *buffer,u_int_t len,UDP *packet)
{
	IP ip_header;
		
	if(is_ip_packet(buffer,len,&ip_header))
   	{
		if(ip_header.protocol != IP_PROTO_UDP)
			return FALSE;
		u_char_t *udpheader = (u_char_t *)(buffer + ETH_HEAD_LEN + IP_HEAD_MIN_LEN);
		int udp_payload_len = ((ntohs_tos(ip_header.len))-(IP_HEAD_MIN_LEN + UDP_HEAD_MIN_LEN));
		packet->src_port = *((u_int16_t *)(udpheader));
		packet->dst_port = *((u_int16_t *)(udpheader + 2));
		packet->len = *((u_int16_t *)(udpheader + 4));
		packet->checksum = *((u_int16_t *)(udpheader + 6));
		memcpy_tos(packet->payload,(u_char_t *)(udpheader + UDP_HEAD_MIN_LEN),udp_payload_len);
	
		 return TRUE;
	}
	else
		return FALSE;
}

 u_int16_t udp_checksum(UDP *udp,u_char_t *src_ip,u_char_t *dst_ip, int len) // udp length header + payload
 {
	unsigned short temp = udp->checksum;
	udp->checksum = 0;
	int length = len;
	u_int16_t *w = (u_int16_t *)udp;
	u_int16_t *src = (u_int16_t *)src_ip;
	u_int16_t *dst = (u_int16_t *)dst_ip;
    u_int32_t sum = 0;
    unsigned short answer = 0;

   	while (length > 1)
   	{
		sum += *w++;
	  	if (sum & 0x80000000)
		   sum = (sum & 0xFFFF) + (sum >> 16);
	   length -= 2;
  	}	

	if ( length & 1 )
		 // Add the padding if the packet length is odd         
		 //sum += *((u_char_t *)w);
	sum +=0;

	sum += *(src++);
 	sum += *src;

	sum += *(dst++);
	sum += *dst;

	sum += 0x00;
	sum += htons_tos(IP_PROTO_UDP);
	sum += htons_tos(length);
	 
	 // Add the carries                                              
	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	answer = ~sum;
	udp->checksum = temp;
	return (answer);
 }

int create_udp_hr(u_int16_t src_port,u_int16_t dst_port,u_int_t payload_len,void *payload,UDP *ud)
{
	unsigned short packet_len = (payload_len + UDP_HEAD_MIN_LEN);
	
	ud->src_port = htons_tos(src_port);
	ud->dst_port = htons_tos(dst_port);
	ud->len = htons_tos(packet_len);
	ud->checksum = 0;
	memcpy_tos(ud->payload,(u_char_t *)payload,payload_len);

	return (int)packet_len;
}

int create_udp_packet(u_int16_t sp,u_int16_t dp,u_char_t *sip, u_char_t *dip,\
  u_int_t p_len,void *payload,udp_packet *packet)
{
	IP ip;
	UDP ud;

	int udp_len = create_udp_hr(sp,dp,p_len,payload,&ud);
	int ip_len = create_ip_hr(sip,dip,(u_int16_t)udp_len,&ip);
	memcpy_tos(packet->buffer,(u_char_t *)&ip,IP_HEAD_MIN_LEN);
	memcpy_tos((packet->buffer+IP_HEAD_MIN_LEN),(u_char_t *)&ud,udp_len);

 return ip_len;
	
	
}

#endif

