#ifndef _TEST_PRINT_C
#define _TEST_PRINT_C

#include <nll.h>
#include <kernel.h>

void print_arp(ARP *pkt, u_int_t len)
 {
    kprintf("\n###############################################################\n");
    kprintf("\nARP Header\n");
    kprintf(" |-ARP Packet Total Length   : %d  Bytes(Size of Packet)\n", len);
    kprintf(" |-ARP Operation             : %s\n", pkt->op == ARP_REQUEST ? "ARP REQUEST" : "ARP REPLY");
    kprintf(" |-Sender MAC Address        : %02x:%02x:%02x:%02x:%02x:%02x\n", pkt->eth_source[0], pkt->eth_source[1], \
				pkt->eth_source[2], pkt->eth_source[3], pkt->eth_source[4], pkt->eth_source[5]);
    kprintf(" |-Sender IP Address         : %d.%d.%d.%d\n", pkt->ip_source[0], pkt->ip_source[1], \
						pkt->ip_source[2], pkt->ip_source[3]);
    kprintf(" |-Target MAC Address        : %02x:%02x:%02x:%02x:%02x:%02x\n", pkt->eth_dest[0], pkt->eth_dest[1], \
					pkt->eth_dest[2], pkt->eth_dest[3], pkt->eth_dest[4], pkt->eth_dest[5]);
    kprintf(" |-Target IP Address         : %d.%d.%d.%d\n", pkt->ip_dest[0], pkt->ip_dest[1], \
		pkt->ip_dest[2], pkt->ip_dest[3]);
}

void print_ethernet_header(ETH *ether, u_int_t len)
{
    
  kprintf("\n");
  kprintf("\n###############################################################\n");
  kprintf("Ethernet Header\n");
  kprintf("   |-Destination Address : %02x:%02x:%02x:%02x:%02x:%02x\n", ether->dst[0] , ether->dst[1] , ether->dst[2] , ether->dst[3] , ether->dst[4] , ether->dst[5] );
  kprintf("   |-Source Address      : %02x:%02x:%02x:%02x:%02x:%02x\n", ether->src[0] , ether->src[1] , ether->src[2] , ether->src[3] , ether->src[4] , ether->src[5] );
  kprintf("   |-Protocol            : %04x\n",ntohs_tos(ether->type));
}

void print_ip_header(IP *ip_pkt)
{
    unsigned short ipheader_len  = ip_pkt->hdr_len*4;
  
       
    kprintf("\n");
    kprintf("IP Header\n");
    kprintf("   |-IP Version        : %u\n",ip_pkt->version);
    kprintf("   |-IP Header Length  : %u DWORDS or %u Bytes\n",ip_pkt->hdr_len,ipheader_len);
    kprintf("   |-Type Of Service   : %04X\n",ip_pkt->tos);
    kprintf("   |-IP Total Length   : %u  Bytes(Size of Packet)\n",ntohs_tos(ip_pkt->len));
    kprintf("   |-Identification    : %04x\n",ntohs_tos(ip_pkt->id));
    kprintf("   |-IP Offset         : %04x\n",ntohs_tos(ip_pkt->offset));
    kprintf("   |-TTL               : %u\n",ip_pkt->ttl);
    kprintf("   |-Protocol          : %04x\n",ip_pkt->protocol);
    kprintf("   |-Checksum          : %04x\n",ntohs_tos(ip_pkt->checksum));
    kprintf("   |-Computed checksum : %04x\n",ntohs_tos(ip_checksum_v2(ip_pkt)));
    kprintf("   |-Source IP         : %u.%u.%u.%u\n" , ip_pkt->src[0],ip_pkt->src[1],ip_pkt->src[2], \
           							  ip_pkt->src[3]);
    kprintf("   |-Destination IP    : %u.%u.%u.%u\n" , ip_pkt->dst[0],ip_pkt->dst[1],ip_pkt->dst[2], \
           							ip_pkt->dst[3]);
  
  }


void print_udp_header(UDP *ud, u_char_t *src,u_char_t *dst)
  {
	
    kprintf("\n");
    kprintf("UDP Header\n");
    kprintf("   |-Source Port                : %u\n",ntohs_tos(ud->src_port));
    kprintf("   |-Destination Port           : %u\n",ntohs_tos(ud->dst_port));
    kprintf("   |-Length                     : %u\n",ntohs_tos(ud->len));
	kprintf("   |-UDP checksum (optional)    : %04x\n",ntohs_tos(ud->checksum));
   	}

void print_packet(void *packet,u_int_t len)
{
	u_char_t *pkt = (u_char_t *)packet;
	int i;
	for(i=0;i<len;i++)
	{
		kprintf("%02x:", pkt[i]);
		if((i%16 == 0 && i!=0) || i==len-1 )
        	 kprintf("\n");
    }
   kprintf("\n");
}


void print_udp_data(UDP *ud)
 {
	 u_int_t i , j;
 	 u_char_t *buf = (u_char_t *)ud->payload;
	 u_int_t data_length = (ntohs_tos(ud->len)-UDP_HEAD_MIN_LEN);
	 kprintf("%d",data_length);

	 kprintf("\n###################################################################\n");
	 for(i=0 ; i < data_length ; i++)
		  {
			  if( i!=0 && i%16==0)   //if one line of hex printing is complete...
				  {
				  	  kprintf( "         ");
					  for(j=i-16 ; j<i ; j++)
					  {
						  if(buf[j]>=32 && buf[j]<=128)
							  kprintf("%c",(u_char_t)buf[j]); //if its a number or alphabet

						  else kprintf("."); //otherwise print a dot
					  }
					  kprintf( "\n");
				  }

				  if(i%16==0) kprintf("   ");
				  kprintf(" %02X",(u_int_t)buf[i]);

				  if( i==data_length-1)  //print the last spaces
				  {
					  for(j=0;j<15-i%16;j++)
					  {
						  kprintf("   "); //extra spaces
					  }

					  kprintf("         ");

					  for(j=i-i%16 ; j<=i ; j++)
					  {
						  if(buf[j]>=32 && buf[j]<=128)
						  {
							  kprintf("%c",(u_char_t)buf[j]);
						  }
						  else
						  {
							  kprintf(".");
						  }
					  }

					  kprintf("\n" );
				  }
			  }
		  }
#endif
