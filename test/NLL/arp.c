#ifndef _ARP_C
#define _ARP_C

#include <nll.h>


//! The ARP cache maximum entries.
#define ARP_TABLE_SIZE   10


static struct {
    u_char_t ip[IP_LEN];
    u_char_t mac[ETH_ADDR_LEN];

} arp_table[ARP_TABLE_SIZE];



BOOL is_arp_request(void *buffer, u_int_t len, ARP *arp_pkt) {

	ETH ethheader;

	if (is_ethernet_header(buffer, len, &ethheader)) {
#ifndef NO_TOS
		//print_packet((void *)&ethheader,len);
#endif
        if (ntohs_tos (ethheader.type) != ETHERTYPE_ARP)
            return FALSE;
        u_char_t *arpheader = (u_char_t *)(buffer + ETH_HEAD_LEN);
		
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
    } else
        return FALSE;
}

BOOL is_arp_reply(void *buffer, u_int_t len, ARP *arp_pkt) {

	ETH ethheader;

	if (is_ethernet_header(buffer, len,&ethheader)) {

		if (ntohs_tos (ethheader.type) != ETHERTYPE_ARP)
            return FALSE;

        u_char_t *arpheader = (u_char_t *)(buffer + ETH_HEAD_LEN);

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
    } else
        return FALSE;
}

void arp_add_cache(u_char_t *ip, u_char_t *mac) {

    u_int_t i;

    for (i = 0; i < ARP_TABLE_SIZE; i++) {
        if (memcmp_tos(arp_table[i].ip, ip, IP_LEN)) {
            memcpy_tos(arp_table[i].mac, mac, ETH_ADDR_LEN);
            return;
        }
    }
    for (i = 0; i < ARP_TABLE_SIZE; i++) {
        if (arp_table[i].ip == NULL) {
            for (i = 0; i < IP_LEN; i++)
                arp_table[i].ip[i] = ip[i];
            memcpy_tos(arp_table[i].mac, mac, ETH_ADDR_LEN);
            return;
        }
    }
}

BOOL arp_ip_to_mac(u_char_t *eth_addr, u_char_t *ip) {

	u_int_t i;

   /* if (memcmp_tos(ip, get_host_ip(), IP_LEN)) {
        // Maybe we're asking our MAC address (???)             //
        memcpy_tos(eth_addr, get_host_mac(), ETH_ADDR_LEN);
        return TRUE;
    } */

    for (i = 0; i < ARP_TABLE_SIZE; i++)
        if (memcmp_tos(arp_table[i].ip, ip, IP_LEN)) {
        	memcpy_tos(eth_addr, arp_table[i].mac, ETH_ADDR_LEN);
            return TRUE;
        }
    return FALSE;
}

u_int_t create_arp_packet(u_char_t *ip_to, u_char_t *eth_to, u_char_t *host_ip, u_char_t *host_mac, u_int16_t arp_op, ARP *packet) {

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


