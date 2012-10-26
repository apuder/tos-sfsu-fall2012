#ifndef _TEST_PRINT_C
#define __TEST_PRINT_C

#include <nll.h>
#include <kernel.h>

void print_arp(ARP pkt, u_int_t len)
 {
    kprintf("\n###############################################################\n");
    kprintf("\nARP Header\n");
    kprintf(" |-ARP Packet Total Length   : %d  Bytes(Size of Packet)\n", len);
    kprintf(" |-ARP Operation             : %s\n", pkt->arp_op == ARP_REQUEST ? "ARP REQUEST" : "ARP REPLY");
    kprintf(" |-Sender MAC Address        : %02x:%02x:%02x:%02x:%02x:%02x\n", pkt->arp_eth_source[0], pkt->arp_eth_source[1], pkt->arp_eth_source[2], pkt->arp_eth_source[3], pkt->arp_eth_source[4], pkt->arp_eth_source[5]);
    kprintf(" |-Sender IP Address         : %d.%d.%d.%d\n", pkt->arp_ip_source[0], pkt->arp_ip_source[1], pkt->arp_ip_source[2], pkt->arp_ip_source[3]);
    kprintf(" |-Target MAC Address        : %02x:%02x:%02x:%02x:%02x:%02x\n", pkt->arp_eth_dest[0], pkt->arp_eth_dest[1], pkt->arp_eth_dest[2], pkt->arp_eth_dest[3], pkt->arp_eth_dest[4], pkt->arp_eth_dest[5]);
    kprintf(" |-Target IP Address         : %d.%d.%d.%d\n", pkt->arp_ip_dest[0], pkt->arp_ip_dest[1], pkt->arp_ip_dest[2], pkt->arp_ip_dest[3]);
}

void print_ethernet_header(ETH ether, u_int_t len)
{
    
  kprintf("\n");
  kprintf("\n###############################################################\n");
  kprintf("Ethernet Header\n");
  kprintf("   |-Destination Address : %02x:%02x:%02x:%02x:%02x:%02x \n", ether->dst[0] , ether->dst[1] , ether->dst[2] , ether->dst[3] , ether->dst[4] , ether->dst[5] );
  kprintf("   |-Source Address      : %02x:%02x:%02x:%02x:%02x:%02x \n", ether->src[0] , ether->src[1] , ether->src[2] , ether->src[3] , ether->src[4] , ether->src[5] );
  kprintf("   |-Protocol            : %#05X \n",ntohs_tos(ether->type));
}

void print_ip_header(IP ip_pkt, u_int_t packet_len)
{
    unsigned short ipheader_len  = ip_pkt->ip_hdr_len*4;
  
    unsigned short checksum = ip_pkt ->ip_chksum;
    ip_pkt ->ip_chksum = 0;
    
    kprintf("\n");
    kprintf("IP Header\n");
    kprintf("   |-IP Version        : %u\n",ip_pkt->ip_version);
    kprintf("   |-IP Header Length  : %u DWORDS or %u Bytes\n",ip_pkt->ip_hdr_len,ipheader_len);
    kprintf("   |-Type Of Service   : %#04X\n",ip_pkt->ip_tos);
    kprintf("   |-IP Total Length   : %u  Bytes(Size of Packet)\n",ip_pkt->ip_len);
    kprintf("   |-Identification    : %#010x\n",ip_pkt->ip_id);
    kprintf("   |-IP Offset         : %#010x\n",ip_pkt->ip_offset);
    kprintf("   |-TTL               : %u\n",ip_pkt->ip_ttl);
    kprintf("   |-Protocol          : %#04X\n",ip_pkt->ip_proto);
    kprintf("   |-Checksum          : %#06X\n",checksum);
    kprintf("   |-Computed checksum : %#06X\n",ntohs_tos(ip_checksum_v2(ip_pkt,ipheader_len)));
    kprintf("   |-Source IP         : %u.%u.%u.%u\n" , ip_pkt->ip_src[0],ip_pkt->ip_src[1],ip_pkt->ip_src[2],ip_pkt->ip_src[3]);
    kprintf("   |-Destination IP    : %u.%u.%u.%u\n" , ip_pkt->ip_dst[0],ip_pkt->ip_dst[1],ip_pkt->ip_dst[2],ip_pkt->ip_dst[3]);
  
  }


void print_udp_header(UDP ud, u_int_t len,PSEUDOIP sip)
  {
	  unsigned short checksum = ud->udp_checksum;
	  ud->udp_checksum = 0;

    kprintf("\n");
    kprintf("UDP Header\n");
    kprintf("   |-Source Port                : %u\n",ntohs_tos(ud->udp_src_port));
    kprintf("   |-Destination Port           : %u\n",ntohs_tos(ud->udp_dst_port));
    kprintf("   |-Length                     : %u\n",ntohs_tos(ud->udp_len));
    kprintf("   |-UDP checksum (optional)    : %#06X\n",ntohs_tos(checksum));
    kprintf("   |-Computed UDP checksum      : %#06X\n",ntohs_tos(udp_checksum(ud,len,sip)));

    kprintf("PSEUDO IP Header\n");
    kprintf("   |-Source IP         : %u.%u.%u.%u\n" , sip->ip_src[0],sip->ip_src[1],sip->ip_src[2],sip->ip_src[3]);
    kprintf("   |-Destination IP    : %u.%u.%u.%u\n" , sip->ip_dst[0],sip->ip_dst[1],sip->ip_dst[2],sip->ip_dst[3]);
  }
#endif