#ifndef _ARP_H
#define _ARP_H

#include "nll.h"

#define ARPHRD_ETHER 1

#define ARP_REQUEST 1   
#define ARP_REPLY 2 



struct _arp;
typedef struct _arp* ARP;

typedef struct _arp
{
         
         u_int16_t arp_hard_type;
         u_int16_t arp_proto_type;
         u_char_t  arp_hard_size;
         u_char_t  arp_proto_size;
         u_int16_t arp_op;
         u_char_t  arp_eth_source[6];
	 //u_int32_t arp_ip_source;
         u_char_t 	arp_ip_source[4];
         u_char_t  	arp_eth_dest[6];
         u_char_t 	arp_ip_dest[4];
	 //u_int32_t arp_ip_dest;
 }arp_t;
 
 // --- Prototypes ----------------------------------------------------- //


BOOL is_arp_request(void *buffer, u_int_t len,ARP arp_packet);
BOOL is_arp_reply(void *buffer, u_int_t len,ARP arp_packet);
void arp_add_cache(u_char_t *ip, u_char_t *mac);
void send_arp_request(u_char_t *ip_to, const u_char_t *eth_to, u_int16_t arp_op);
void send_arp_reply(u_char_t *ip_to, const u_char_t *eth_to, u_int16_t arp_op);
BOOL arp_ip_to_mac(u_char_t *eth_addr, u_char_t *ip_addr);
int send_arp_packet(u_char_t *ip_to, u_char_t *eth_to, u_int16_t arp_op,ARP packet);
void print_arp_request(ARP pkt, u_int_t len);
void print_arp_reply(ARP pkt, u_int_t len);


#endif
