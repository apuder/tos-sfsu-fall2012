#ifndef _ARP_C
#define _ARP_C

#include <nll.h>

#ifdef NO_TOS
	#include <stdio.h>
#endif

//! The ARP cache maximum entries.
#define ARP_TABLE_SIZE   10


static struct {
    u_char_t ip[IP_LEN];
    u_char_t mac[ETH_ADDR_LEN];

} arp_table[ARP_TABLE_SIZE];

BOOL is_arp_request(void *buffer, u_int_t len, ARP *arp_pkt) 
{

  	//ARP arp_pkt;
  	u_char_t *buf = (u_char_t *)buffer;
  

  	if (ntohs_tos (*((u_int16_t *)(buf + 2*ETH_ADDR_LEN))) != ETHERTYPE_ARP)
            return FALSE;
    u_char_t *arpheader = (u_char_t *)(buf + ETH_HEAD_LEN);
    if (ntohs_tos(*((u_int16_t *) (arpheader + 6))) != ARP_REQUEST)
            return FALSE;
  	arp_pkt->hard_type = ntohs_tos(*((u_int16_t *) (arpheader)));
  	arp_pkt->proto_type = ntohs_tos(*((u_int16_t *) (arpheader + 2)));
  	arp_pkt->hard_size = *((u_char_t *) (arpheader + 4));
  	arp_pkt->proto_size = *((u_char_t *) (arpheader + 5));
  	arp_pkt->op = ntohs_tos(*((u_int16_t *) (arpheader + 6)));
  	memcpy_tos(arp_pkt->eth_source, (u_char_t *) (arpheader + 8), ETH_ADDR_LEN);
  	memcpy_tos(arp_pkt->ip_source, (u_char_t *) (arpheader + 14), IP_LEN);
  	memcpy_tos(arp_pkt->eth_dest, (u_char_t *) (arpheader + 18), ETH_ADDR_LEN);
  	memcpy_tos(arp_pkt->ip_dest, (u_char_t *) (arpheader + 24), IP_LEN);
	#ifdef NO_TOS
  		//printf("   |-Hardware type             : %x\n",arp_pkt.hard_type);
  		//printf("   |-Protocol type             : %x\n",arp_pkt.proto_type);
	#endif
  	
  	return TRUE;
  }

BOOL is_arp_reply(void *buffer, u_int_t len, ARP *arp_pkt)
{
	//ARP arp_pkt;
  	u_char_t *buf = (u_char_t *)buffer;
  

  	if (ntohs_tos (*((u_int16_t *)(buf + 2*ETH_ADDR_LEN))) != ETHERTYPE_ARP)
            return FALSE;
    u_char_t *arpheader = (u_char_t *)(buf + ETH_HEAD_LEN);
    if (ntohs_tos(*((u_int16_t *) (arpheader + 6))) != ARP_REPLY)
            return FALSE;
  	arp_pkt->hard_type = ntohs_tos(*((u_int16_t *) (arpheader)));
  	arp_pkt->proto_type = ntohs_tos(*((u_int16_t *) (arpheader + 2)));
  	arp_pkt->hard_size = *((u_char_t *) (arpheader + 4));
  	arp_pkt->proto_size = *((u_char_t *) (arpheader + 5));
  	arp_pkt->op = ntohs_tos(*((u_int16_t *) (arpheader + 6)));
  	memcpy_tos(arp_pkt->eth_source, (u_char_t *) (arpheader + 8), ETH_ADDR_LEN);
  	memcpy_tos(arp_pkt->ip_source, (u_char_t *) (arpheader + 14), IP_LEN);
  	memcpy_tos(arp_pkt->eth_dest, (u_char_t *) (arpheader + 18), ETH_ADDR_LEN);
  	memcpy_tos(arp_pkt->ip_dest, (u_char_t *) (arpheader + 24), IP_LEN);

  	return TRUE;
}

void arp_add_cache(u_char_t *ip, u_char_t *mac)
{
    int i;
    for (i = 0; i < ARP_TABLE_SIZE; i++) 
   	{
        if (memcmp_tos(arp_table[i].ip, ip, IP_LEN)) {
            memcpy_tos(arp_table[i].mac, mac, ETH_ADDR_LEN);
   	         return;
        }
    }
    for (i = 0; i < ARP_TABLE_SIZE; i++) 
   	{
        if (arp_table[i].ip == NULL) 
		  {
            memcpy_tos(arp_table[i].ip ,ip,IP_LEN);
            memcpy_tos(arp_table[i].mac, mac, ETH_ADDR_LEN);
            return;
        }
    }
}

BOOL arp_ip_to_mac(u_char_t *eth_addr, u_char_t *ip) 
{
	int i;
	for (i = 0; i < ARP_TABLE_SIZE; i++)
        if (memcmp_tos(arp_table[i].ip, ip, IP_LEN)) 
   		{
        	memcpy_tos(eth_addr, arp_table[i].mac, ETH_ADDR_LEN);
            return TRUE;
        }
    return FALSE;
}

u_int_t create_arp_packet(u_char_t *ip_to, u_char_t *eth_to, u_char_t *host_ip, \
  u_char_t *host_mac, u_int16_t arp_op, ARP *packet) 
{
    packet->hard_type = htons_tos(ARPHRD_ETHER);
    packet->proto_type = htons_tos(ETHERTYPE_IP);
    packet->hard_size = ETH_ADDR_LEN;
    packet->proto_size = IP_LEN;
    packet->op = htons_tos(arp_op);
    memcpy_tos(packet->eth_source, host_mac, ETH_ADDR_LEN);
    memcpy_tos(packet->ip_source, host_ip, IP_LEN);
    memcpy_tos(packet->eth_dest, eth_to, ETH_ADDR_LEN);
    memcpy_tos(packet->ip_dest, ip_to, IP_LEN);

    return (sizeof(ARP));
}

#endif


