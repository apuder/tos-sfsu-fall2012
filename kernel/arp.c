#ifndef _ARP_C_
#define _ARP_C_
/* 
 * functions implementing arp protocol
 */
#include <nll.h>

#ifdef NO_TOS
     #include <stdio.h>
#endif

//! The ARP cache maximum entries.
#define ARP_TABLE_SIZE   10

#define ENTRY_INITIALIZED 0xAA    // flag to indicate initialization of entry

static struct {
    unsigned char init;
    u_char_t ip[IP_LEN];
    u_char_t mac[ETH_ADDR_LEN];

} arp_table[ARP_TABLE_SIZE];

/* printing the entire arp cache */
void show_arp_table() {
    int i;
    for (i = 0; i < 3; i++) {
        kprintf("%d.%d.%d.%d == ", arp_table[i].ip[0], arp_table[i].ip[1], arp_table[i].ip[2], arp_table[i].ip[3]);
        kprintf("%02X:%02X:%02X:%02X:%02X:%02X\n",
                arp_table[i].mac[0], arp_table[i].mac[1], arp_table[i].mac[2],
                arp_table[i].mac[3], arp_table[i].mac[4], arp_table[i].mac[5]);
    }
}

/* 
 * checking if a packet is of type arp request 
 *@ param1 void pointer to a packet 
 * @param2 length of the packet
 * @param3 a pointer to an arp structure 
 * @returns true or false 
 */

BOOL is_arp_request(void *buffer, u_int_t len, ARP *arp_pkt) {

     u_char_t *buf = (u_char_t *)buffer;
  

    if (ntohs_tos(*((u_int16_t *) (buf + 2 * ETH_ADDR_LEN))) != ETHERTYPE_ARP)
        return FALSE;
    u_char_t *arpheader = (u_char_t *) (buf + ETH_HEAD_LEN);
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
    	return TRUE;
}

/* 
 * checking if a packet is of type arp reply 
 *@ param1 void pointer to a packet 
 * @param2 length of the packet
 * @param3 a pointer to an arp structure to be populated with the info.
 * @returns true or false
 */


BOOL is_arp_reply(void *buffer, u_int_t len, ARP *arp_pkt)
{
	
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

/*
 * method used to and to the arp cache. there are two ways of doing it.
 * 1. if there is already ip entry in the cache update it with the new mac address
 * 2. if there is no any slot with the given ip address copy both the ip and mac addresses.
 * @param1 a char pointer to the ip address
 * @param2 char pointer to the mac address
 */


void arp_add_cache(u_char_t *ip, u_char_t *mac) {
    int i;
    for (i = 0; i < ARP_TABLE_SIZE; i++) {
        if (memcmp_tos(arp_table[i].ip, ip, IP_LEN)) //if Ip entry present  copy the mac address
        {
            memcpy_tos(arp_table[i].mac, mac, ETH_ADDR_LEN);
            return;
        }
    }
    for (i = 0; i < ARP_TABLE_SIZE; i++) {
        if (arp_table[i].init != ENTRY_INITIALIZED)  // empty slot copy both the ip and mac
        { 
            memcpy_tos(arp_table[i].ip, ip, IP_LEN);
            memcpy_tos(arp_table[i].mac, mac, ETH_ADDR_LEN);
            arp_table[i].init = ENTRY_INITIALIZED;
            return;
        }
    }
}

/*
 * This method is used for the translation of an ip address to a corresponding mac address.
 * @param1 char pointer to the ethernet address
 * @param2 char pinter to the ip address.
  */

BOOL arp_ip_to_mac(u_char_t *eth_addr, u_char_t *ip) {
    int i;
    for (i = 0; i < ARP_TABLE_SIZE; i++)
        if (memcmp_tos(arp_table[i].ip, ip, IP_LEN)) {
            memcpy_tos(eth_addr, arp_table[i].mac, ETH_ADDR_LEN);
            return TRUE;
        }
    return FALSE;
}

/*
 * method used to create an arp packet.
 * @param1 destination ip address
 * @param2 destination mac address
 * @param3 host or source ip address
 * @param4 source or host mac address
 * @param5 operation type (request or reply)
 * @param6 pointer to a structure of ARP to be populated with info
 * @return the size of the created packet
 */

u_int_t create_arp_packet(u_char_t *ip_to, u_char_t *eth_to, u_char_t *host_ip, \
  u_char_t *host_mac, u_int16_t arp_op, ARP *packet) {
    packet->hard_type = htons_tos(ARPHRD_ETHER);
    packet->proto_type = htons_tos(ETHERTYPE_IP);
    packet->hard_size = ETH_ADDR_LEN;
    packet->proto_size = IP_LEN;
    packet->op = htons_tos(arp_op);
    memcpy_tos(packet->eth_source, host_mac, ETH_ADDR_LEN);
    memcpy_tos(packet->ip_source, host_ip, IP_LEN);
    memcpy_tos(packet->eth_dest, eth_to, ETH_ADDR_LEN);
    memcpy_tos(packet->ip_dest, ip_to, IP_LEN);

    return (sizeof (ARP));
}

#endif


