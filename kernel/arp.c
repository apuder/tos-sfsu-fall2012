#ifndef _ARP_C
#define _ARP_C

#include "nll.h"

//! The ARP cache maximum entries.
#define ARP_TABLE_SIZE   10

//! ARP cache (here are stored all addresses resolutions IP<->MAC).

static struct {
    u_char_t ip[IP_LEN];
    u_char_t mac[ETH_ADDR_LEN];

} arp_table[ARP_TABLE_SIZE];

BOOL is_arp_request(void *buffer, u_int_t len, ARP arp_pkt) {
    ETH ethheader;
    if (is_ethernet_header(&buffer, &ethheader)) {
        u_char_t *buf = (u_char_t *) (buffer);

        if (ntohs_tos(ethheader->type) != ETHERTYPE_ARP)
            return FALSE;
        u_char_t *arpheader = (buf + ETH_HEAD_LEN);

        if (ntohs_tos(*((u_int16_t *) (arpheader + 6))) != ARP_REQUEST)
            return FALSE;

        arp_pkt->arp_hard_type = ntohs_tos(*((u_int16_t *) (arpheader)));

        arp_pkt->arp_proto_type = ntohs_tos(*((u_int16_t *) (arpheader + 2)));

        arp_pkt->arp_hard_size = *((u_char_t *) (arpheader + 4));

        arp_pkt->arp_proto_size = *((u_char_t *) (arpheader + 5));
        arp_pkt->arp_op = ntohs_tos(*((u_int16_t *) (arpheader + 6)));
        memcpy_tos(arp_pkt->arp_eth_source, (u_char_t *) (arpheader + 8), ETH_ADDR_LEN);
        memcpy_tos(arp_pkt->arp_ip_source, (u_char_t *) (arpheader + 14), IP_LEN);
        memcpy_tos(arp_pkt->arp_eth_dest, (u_char_t *) (arpheader + 18), ETH_ADDR_LEN);
        memcpy_tos(arp_pkt->arp_ip_dest, (u_char_t *) (arpheader + 24), IP_LEN);
        return TRUE;
    } else
        return FALSE;
}

BOOL is_arp_reply(void *buffer, u_int_t len, ARP arp_pkt) {
    ETH ethheader;
    if (is_ethernet_header(&buffer, &ethheader)) {
        u_char_t *buf = (u_char_t *) (buffer);

        if (ntohs_tos(ethheader->type) != ETHERTYPE_ARP)
            return FALSE;
        u_char_t *arpheader = (u_char_t *) (buf + ETH_HEAD_LEN);

        if (ntohs_tos(*((u_int16_t *) (arpheader + 6))) != ARP_REPLY)
            return FALSE;

        arp_pkt->arp_hard_type = ntohs_tos(*((u_int16_t *) (arpheader)));

        arp_pkt->arp_proto_type = ntohs_tos(*((u_int16_t *) (arpheader + 2)));

        arp_pkt->arp_hard_size = *((u_char_t *) (arpheader + 4));

        arp_pkt->arp_proto_size = *((u_char_t *) (arpheader + 5));
        arp_pkt->arp_op = ntohs_tos(*((u_int16_t *) (arpheader + 6)));
        memcpy_tos(arp_pkt->arp_eth_source, (u_char_t *) (arpheader + 8), ETH_ADDR_LEN);
        memcpy_tos(arp_pkt->arp_ip_source, (u_char_t *) (arpheader + 14), IP_LEN);
        memcpy_tos(arp_pkt->arp_eth_dest, (u_char_t *) (arpheader + 18), ETH_ADDR_LEN);
        memcpy_tos(arp_pkt->arp_ip_dest, (u_char_t *) (arpheader + 24), IP_LEN);
        return TRUE;
    } else
        return FALSE;
}

void print_arp(ARP pkt, u_int_t len) {

    kprintf("\n###############################################################\n");

    kprintf("\nARP Header\n");
    kprintf(" |-ARP Packet Total Length   : %u  Bytes(Size of Packet)\n", len);
    kprintf(" |-ARP Operation             : %s\n", pkt->arp_op == ARP_REQUEST ? "ARP REQUEST" : "ARP REPLY");
    kprintf(" |-Sender MAC Address        : %02x:%02x:%02x:%02x:%02x:%02x\n", pkt->arp_eth_source[0], pkt->arp_eth_source[1], pkt->arp_eth_source[2], pkt->arp_eth_source[3], pkt->arp_eth_source[4], pkt->arp_eth_source[5]);
    kprintf(" |-Sender IP Address         : %d.%d.%d.%d\n", pkt->arp_ip_source[0], pkt->arp_ip_source[1], pkt->arp_ip_source[2], pkt->arp_ip_source[3]);
    kprintf(" |-Target MAC Address        : %02x:%02x:%02x:%02x:%02x:%02x\n", pkt->arp_eth_dest[0], pkt->arp_eth_dest[1], pkt->arp_eth_dest[2], pkt->arp_eth_dest[3], pkt->arp_eth_dest[4], pkt->arp_eth_dest[5]);
    kprintf(" |-Target IP Address         : %d.%d.%d.%d\n", pkt->arp_ip_dest[0], pkt->arp_ip_dest[1], pkt->arp_ip_dest[2], pkt->arp_ip_dest[3]);

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
            memcpy_tos(arp_table[i].ip, ip, IP_LEN);
            memcpy_tos(arp_table[i].mac, mac, ETH_ADDR_LEN);
            return;
        }
    }
}

BOOL arp_ip_to_mac(u_char_t *eth_addr, u_char_t *ip) {
    u_int_t i;

    if (memcmp_tos(ip, get_host_ip(), IP_LEN)) {
        // Maybe we're asking our MAC address (???)
        memcpy_tos(eth_addr, get_host_mac(), ETH_ADDR_LEN);
        return TRUE;
    }

    for (i = 0; i < ARP_TABLE_SIZE; i++)
        if (memcmp_tos(arp_table[i].ip, ip, IP_LEN)) {
            // Resolution is found in the cache     //
            memcpy_tos(eth_addr, arp_table[i].mac, ETH_ADDR_LEN);
            return TRUE;
        }
    // Ask in broadcast who has the ip              //
    //send_arp_request(ip, eth_bcast, ARP_REQUEST);
    return FALSE;
}

u_int_t create_arp_packet(u_char_t *ip_to, u_char_t *eth_to, u_char_t *host_ip, u_char_t *host_mac, u_int16_t arp_op, u_char_t *packet) {

    *((u_int16_t *) packet) = htons_tos(ARPHRD_ETHER);
    *((u_int16_t *) (packet + 2)) = htons_tos(ETHERTYPE_IP);
    *(packet + 4) = ETH_ADDR_LEN;
    *(packet + 5) = IP_LEN;
    *((u_int16_t *) (packet + 6)) = htons_tos(arp_op);
    memcpy_tos((packet + 8), host_mac, ETH_ADDR_LEN);
    memcpy_tos((packet + 14), host_ip, IP_LEN);
    memcpy_tos((packet + 18), eth_to, ETH_ADDR_LEN);
    memcpy_tos((packet + 24), ip_to, IP_LEN);

    return (sizeof (struct _arp));
}

#endif


