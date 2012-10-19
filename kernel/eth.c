#ifndef _ETH_C
#define _ETH_C

//#define INLINE

#include "eth.h"


u_char_t host_mac[6];

void set_host_mac(u_char_t *mac)
{
  memcpy_tos(host_mac, mac,ETH_ADDR_LEN);
}
u_char_t *get_host_mac()
{
  return host_mac;
}

BOOL is_ethernet_header(void *buffer,ETH ether)
{

	   char *buf = (char *)(buffer);
	   ETH ethheader = (ETH)(buf);

	   memcpy_tos(ether->dst , ethheader->dst,ETH_ADDR_LEN);
	   //printf("I am here\n");
	   memcpy_tos(ether->src , ethheader->src + ETH_ADDR_LEN,ETH_ADDR_LEN);

	   //ether->type = *((u_int16_t *)(buf + 2*ETH_ADDR_LEN));
	   ether->type = ethheader->type;
	   //printf("I am here\n");
	   //printf("   |-Operation                 : %x\n",ntohs_tos(ether->type));

	   return TRUE;
 }

int send_eth_packet(u_char_t *to, const void *data, u_int_t len, u_int16_t type,u_char_t *packet)
{
         //u_char_t *packet;
         u_char_t *mac_addr;


         len = MIN(len, ETH_MAX_TRANSFER_UNIT);


         // Get the local mac address
         //if ( (mac_addr = get_host_mac()) == NULL )
                 // No such device or address!
                 //return -1;
         mac_addr = get_host_mac();
         // Add the ethernet header to the packet

         memcpy_tos(packet, to, ETH_ADDR_LEN);
         memcpy_tos(packet + ETH_ADDR_LEN, mac_addr, ETH_ADDR_LEN);
         memcpy_tos(packet + 2 * ETH_ADDR_LEN, type, sizeof(u_int16_t));

         // Copy the data into the packet
         memcpy_tos(packet + ETH_HEAD_LEN, data, len);

         // Adjust the packet length including the size of the header
         len += ETH_HEAD_LEN;


         while (len < ETH_MIN_LEN)
                 packet[len++] = '\0';

         return len;
}

#endif
