#ifndef _UDP_C
#define _UDP_C

//#define INLINE

#include "nll.h"

BOOL is_udp_packet(void *buffer,u_int_t len,UDP packet)
{

   char *buf = (char *)(buffer);
   ETH ethheader = (ETH)(buf);
  
  if(ntohs_tos(ethheader ->type) != ETHERTYPE_IP)
    return FALSE;
  
   IP ipheader = (IP )(buf + ETH_HEAD_LEN);
   u_int_t ip_header_size = ipheader->ip_hdr_len *4;
   
  if((ipheader->ip_hdr_len *4) < IP_HEAD_MIN_LEN )
    return FALSE;
  if(ipheader->ip_proto != IP_PROTO_UDP)
    return FALSE;
  UDP udpheader = (UDP)(buf + ETH_HEAD_LEN + ip_header_size);
   
	  packet->udp_src_port = udpheader->udp_src_port;
	  packet->udp_dst_port = udpheader->udp_dst_port;
	  packet->udp_len = udpheader->udp_len;
	  packet->udp_checksum = udpheader->udp_checksum;
	  packet->payload = (u_char_t *)(buf + ETH_HEAD_LEN + ip_header_size + 8);
return TRUE;
  
}
void set_pseudo_ip_header(IP ip,PSEUDOIP sip)
{
	memcpy_tos(sip->ip_src,ip->ip_src,IP_LEN);
	memcpy_tos(sip->ip_dst,ip->ip_dst,IP_LEN);
}

  u_int16_t udp_checksum(UDP udp, u_int_t len, PSEUDOIP sip)
     {
           u_int16_t *w = (u_int16_t *)udp;
             u_int16_t *ip_src=(u_int16_t *)(sip->ip_src),
                      *ip_dst=(u_int16_t *)(sip->ip_dst);
             u_int32_t sum;
             u_int_t length=len;


         // Calculate the sum                                      
         sum = 0;
         while (len > 1)
         {
                 sum += *w++;
                 if (sum & 0x80000000)
                         sum = (sum & 0xFFFF) + (sum >> 16);
                 len -= 2;
         }

         if ( len & 1 )
                 // Add the padding if the packet length is odd         
                 sum += *((u_char_t *)w);

         // Add the pseudo-header                                       
         sum += *(ip_src++);
         sum += *ip_src;

         sum += *(ip_dst++);
         sum += *ip_dst;

         sum += IP_PROTO_UDP;
         sum += length;

     // Add the carries                                              
         while (sum >> 16)
                 sum = (sum & 0xFFFF) + (sum >> 16);

         // Return the one's complement of sum                           
         return ( (u_int16_t)(~sum)  );
 }
#endif

