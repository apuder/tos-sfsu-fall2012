#ifndef _ARP_C
#define _ARP_C

#define INLINE

#include "arp.h"
#include "eth.h"
#include "ip.h"


/*static u_char_t eth_bcast[ETH_ADDR_LEN] =
{
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
 };*/
 
 //! The ARP cache maximum entries.
 #define ARP_TABLE_SIZE   10
 
 //! ARP cache (here are stored all addresses resolutions IP<->MAC).
 static struct
 {
         //! IP address.
         u_char_t ip[IP_LEN];
         //! Ethernet address.
         u_char_t mac[ETH_ADDR_LEN];

} arp_table[ARP_TABLE_SIZE];




BOOL is_arp_request(void *buffer , u_int_t len, ARP arp_pkt)
{
   u_int_t i;
   char *buf = (char *)(buffer);
   ETH ethheader = (ETH)(buf);
     
  if(ntohs_tos(ethheader->type) != ETHERTYPE_ARP)
    return FALSE;
  
   ARP arpheader = (ARP )(buf + ETH_HEAD_LEN);
       
   if(ntohs_tos(arpheader->arp_op) != ARP_REQUEST)
     return FALSE;
   
    arp_pkt->arp_hard_type = ntohs_tos(arpheader->arp_hard_type);
  
    arp_pkt->arp_proto_type = ntohs_tos(arpheader->arp_proto_type);
   
    arp_pkt->arp_hard_size=arpheader->arp_hard_size;
    
    arp_pkt->arp_proto_size=arpheader->arp_proto_size;
    arp_pkt->arp_op = ntohs_tos(arpheader->arp_op);
    memcpy_tos(arp_pkt->arp_eth_source,arpheader->arp_eth_source,ETH_ADDR_LEN);
   	memcpy_tos(arp_pkt->arp_ip_source,arpheader->arp_ip_source,IP_LEN);
    memcpy_tos(arp_pkt->arp_eth_dest, arpheader->arp_eth_dest,ETH_ADDR_LEN);
   	memcpy_tos(arp_pkt->arp_ip_dest,arpheader->arp_ip_dest,IP_LEN);
    return TRUE;
 }


BOOL is_arp_reply(void *buffer , u_int_t len, ARP arp_pkt)
{
   u_int_t i;
   char *buf = (char *)(buffer);
   ETH ethheader = (ETH)(buf);

  if(ntohs_tos(ethheader ->type) != ETHERTYPE_ARP)
    return FALSE;
  
   ARP arpheader = (ARP )(buf + ETH_HEAD_LEN);
  
   if(ntohs_tos(arpheader->arp_op) != ARP_REPLY)
     return FALSE;
    
    arp_pkt->arp_hard_type = ntohs_tos(arpheader->arp_hard_type);
    arp_pkt->arp_proto_type = ntohs_tos(arpheader->arp_proto_type);
    arp_pkt->arp_hard_size = arpheader->arp_hard_size;
    arp_pkt->arp_proto_size=arpheader->arp_proto_size;
    arp_pkt->arp_op = ntohs_tos(arpheader->arp_op);
    memcpy_tos(arp_pkt->arp_eth_source,arpheader->arp_eth_source,ETH_ADDR_LEN);
    memcpy_tos(arp_pkt->arp_ip_source,arpheader->arp_ip_source,IP_LEN);
    memcpy_tos(arp_pkt->arp_eth_dest, arpheader->arp_eth_dest,ETH_ADDR_LEN);
    memcpy_tos(arp_pkt->arp_ip_dest,arpheader->arp_ip_dest,IP_LEN);
  
    arp_add_cache(arp_pkt->arp_ip_source,arp_pkt->arp_eth_source);
    return TRUE; 
}


 void arp_add_cache(u_char_t *ip, u_char_t *mac)
 {
         
         u_int_t i;
	 
        for(i=0; i<ARP_TABLE_SIZE; i++)
         {
                 if ( memcmp_tos(arp_table[i].ip,ip,IP_LEN) )
               {
                         // Address already present => refresh cache     //
                         memcpy_tos(arp_table[i].mac, mac, ETH_ADDR_LEN);
                         return;
                 }
	 }
        for(i=0; i<ARP_TABLE_SIZE; i++)
	{
                 if ( arp_table[i].ip == NULL )
                 {
                         // Store the address into the ARP table  
                         for(i=0;i< IP_LEN;i++)
                        	 	arp_table[i].ip[i] = ip[i];
                         memcpy_tos(arp_table[i].mac, mac, ETH_ADDR_LEN);
                         return;
                 }
         }
 }
 
 BOOL arp_ip_to_mac(u_char_t *eth_addr, u_char_t *ip)
 {
         u_int_t i;
                                    //
         if (memcmp_tos(ip,get_host_ip(),IP_LEN))
         {
                 // Maybe we're asking our MAC address (???)             //
                 memcpy_tos(eth_addr, get_host_mac(), ETH_ADDR_LEN);
                 return TRUE;
         }
 
         // Check for a broadcast request                                //
        /* if ( memcmp_tos(ip,get_host_bcast_addr(),IP_LEN) || memcmp_tos(ip, INADDR_BROADCAST,IP_LEN) )
         {
                 // Broadcast in the current LAN                         //
                 memcpy_tos(eth_addr, eth_bcast, ETH_ADDR_LEN);
                 return TRUE;
         }*/
         // --- End of special addresses ---                             //
 
         // Search the address into the ARP cache                //

         for(i=0; i<ARP_TABLE_SIZE; i++)
        	 if ( memcmp_tos(arp_table[i].ip,ip,IP_LEN))
                 {
                         // Resolution is found in the cache     //
                         memcpy_tos(eth_addr, arp_table[i].mac, ETH_ADDR_LEN);
                        return TRUE;
                 }
	           // Ask in broadcast who has the ip              //
         //send_arp_request(ip, eth_bcast, ARP_REQUEST);
          return FALSE;
 }
 
 int send_arp_packet(u_char_t *ip_to, u_char_t *eth_to, u_int16_t arp_op , ARP arp_packet)
 {
         

	 	 u_char_t *mac_addr;
	 	 u_char_t *host_ip;

 
         // Create the ARP header                                        
         arp_packet->arp_hard_type = htons_tos(ARPHRD_ETHER);

         arp_packet->arp_proto_type = htons_tos(ETHERTYPE_IP);
         arp_packet->arp_hard_size = ETH_ADDR_LEN;
         arp_packet->arp_proto_size = IP_LEN;
         arp_packet->arp_op = htons_tos(arp_op);
                           
         mac_addr = get_host_mac();
         host_ip = get_host_ip();

         memcpy_tos(arp_packet->arp_eth_source, mac_addr, ETH_ADDR_LEN);
         memcpy_tos(arp_packet->arp_ip_source,host_ip,IP_LEN);
 
                            
         memcpy_tos(arp_packet->arp_eth_dest, eth_to, ETH_ADDR_LEN);
                          
         memcpy_tos(arp_packet->arp_ip_dest, ip_to,IP_LEN);

         return 28;
 }
 

#endif

 
