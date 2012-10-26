
#ifdef NO_TOS
	//#include <stdarg.h>
	#include <pcap.h>
	#include <stdio.h>
	#include <stdlib.h> // for exit()
	#include <string.h> //for memset
	#include <net/if.h>
	#include <sys/ioctl.h>
	#include <sys/socket.h>
#endif

#include <nll.h>


//#define DEBUG

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
void printPacket(u_int_t len, const u_char_t * packet);

void print_all_arp(ARP pkt);

ARP arp_request_packet;
ARP arp_reply_packet;
IP ip_packet;
ETH ether_header;
UDP udp_header;
PSEUDOIP sip;
u_char_t *srip[4];
u_char_t *dip[4];
u_char_t *smask[4];


u_char_t dest_ip[4] = 	{192,168,1,254};
u_char_t h_ip[4] = 	{192,168,1,79};

u_char_t h_mac[ETH_ADDR_LEN]={0xbc,0xae,0x82,0x69,0xeb,0x28};

int main()
{
		//u_char_t *arp_packet;
		  //printf("what sup dude");
		  //u_int_t x;
		  //x = create_arp_packet(dest_ip,eth_bcast,h_ip,h_mac,ARP_REQUEST,&arp_packet);
		  //printPacket(x,&arp_packet);
  
  pcap_if_t *alldevsp , *device;
  pcap_t *handle; //Handle of the device that shall be sniffed
	 
  char errbuf[100] , *devname , devs[100][100];
  int count = 1 , n,count_t=0;

  memset(errbuf,0,PCAP_ERRBUF_SIZE); 
	     

  //First get the list of available devices
  printf("Finding available devices ... ");
  if( pcap_findalldevs(&alldevsp , errbuf) )
    {
      printf("Error finding devices : %s" , errbuf);
      exit(1);
    }
  printf("Done");
	     
  //Print the available devices
  printf("\nAvailable Devices are :\n");

 
  for(device = alldevsp ; device != NULL ; device = device->next)
    {
      //printf("hello");
      printf("%d. %s - %s\n" , count , device->name , device->description);
      if(device->name != NULL)
	{
	  strcpy(devs[count] , device->name);
	}
      count++;
    }
	     
  //Ask user which device to sniff
  printf("Enter the number of the device you want to sniff : ");
  scanf("%d" , &n);
  devname = devs[n] ;
	     
  //Open the device for sniffing
  printf("Opening device %s for sniffing ... " , devname);


  handle = pcap_open_live(devname , 65536 , 1 , 0 , errbuf);
	     
  if (handle == NULL)
    {
      //fprintf(stderr, "Couldn't open device %s : %s\n" , devname , errbuf);
	  printf("Couldn't open device %s\n" , devname);
      exit(1);
    }

  printf("Done\n");

 
  pcap_loop(handle, -1, got_packet, (u_char *)&count_t);



   /*// Combine the Ethernet header and ARP request into a contiguous block.
  unsigned char frame[ETH_HEAD_LEN + size];
  memcpy_tos(frame,&ethernet,ETH_HEAD_LEN);
  memcpy_tos(frame+ETH_HEAD_LEN,&arp_packet,size);

   int len = ETH_HEAD_LEN + size;

  if(!is_arp_request((void *)frame,(u_int_t)len,&arp_request_packet))
	 print_arp_request(&arp_request_packet,(u_int_t)size);
  	 print_all_arp(&arp_request_packet); */

  // Write the Ethernet frame to the interface.
      //if (pcap_inject(handle,&frame,len)==-1) {
         //exit(1);
      //}
 
  return 0;
}
 
void got_packet(u_char *args, const struct pcap_pkthdr *header, const  u_char *packet)
{

  if(is_arp_request(packet,(u_int_t)header->len,&arp_request_packet))
  {
     print_arp(&arp_request_packet,(u_int_t)header->len);
     print_all_arp(&arp_request_packet);
     //printPacket((u_int_t)header->len,packet);

  }
      
  else if(is_arp_reply(packet,(u_int_t)header->len,&arp_reply_packet))
  {
    print_arp(&arp_reply_packet,(u_int_t)header->len);
    print_all_arp(&arp_reply_packet);
}
  else if(is_ip_packet(packet,(u_int_t)header->len,&ip_packet))
  {
    printf("\n************************************************************************\n");
    if(is_ethernet_header(packet,&ether_header))
    	print_ethernet_header(&ether_header,(u_int_t)header->len);
    print_ip_header(&ip_packet,(u_int_t)header->len);
  if(is_udp_packet(packet,(u_int_t)header->len,&udp_header)){
	  set_pseudo_ip_header(&ip_packet,&sip);
      print_udp_header(&udp_header,(u_int_t)header->len,&sip);
      print_udp_data(&udp_header,&ip_packet,(u_int_t)header->len);
      }
  }
}

void print_ethernet_header(ETH ether, u_int_t len)
{
    
  printf("\n");
  printf("Ethernet Header\n");
  printf("   |-Destination Address : %02x:%02x:%02x:%02x:%02x:%02x \n", ether->dst[0] , ether->dst[1] , ether->dst[2] , ether->dst[3] , ether->dst[4] , ether->dst[5] );
  printf("   |-Source Address      : %02x:%02x:%02x:%02x:%02x:%02x \n", ether->src[0] , ether->src[1] , ether->src[2] , ether->src[3] , ether->src[4] , ether->src[5] );
  printf("   |-Protocol            : %#05X \n",ntohs_tos(ether->type));
}


void print_arp(ARP pkt, u_int_t len)
{

    printf("\n###############################################################\n");
 
    printf("\nARP Header\n");
    printf("   |-ARP Packet Total Length   : %u  Bytes(Size of Packet)\n",len);
    printf("   |-ARP Operation             : %s\n",pkt->arp_op == ARP_REQUEST ? "ARP REQUEST" : "ARP REPLY");
    printf("   |-Sender MAC Address        : %02x:%02x:%02x:%02x:%02x:%02x\n",pkt->arp_eth_source[0],pkt->arp_eth_source[1],pkt->arp_eth_source[2],pkt->arp_eth_source[3],pkt->arp_eth_source[4],pkt->arp_eth_source[5]);
    printf("   |-Sender IP Address         : %d.%d.%d.%d\n",pkt->arp_ip_source[0],pkt->arp_ip_source[1], pkt->arp_ip_source[2],pkt->arp_ip_source[3]);
    printf("   |-Target MAC Address        : %02x:%02x:%02x:%02x:%02x:%02x\n",pkt->arp_eth_dest[0],pkt->arp_eth_dest[1],pkt->arp_eth_dest[2],pkt->arp_eth_dest[3],pkt->arp_eth_dest[4],pkt->arp_eth_dest[5]);
    printf("   |-Target IP Address         : %d.%d.%d.%d\n",pkt->arp_ip_dest[0],pkt->arp_ip_dest[1],pkt->arp_ip_dest[2],pkt->arp_ip_dest[3]);
   
  }
void print_ip_header(IP ip_pkt, u_int_t packet_len)
{
    unsigned short ipheader_len  = ip_pkt->ip_hdr_len*4;
  
    unsigned short checksum = ip_pkt ->ip_chksum;
    ip_pkt ->ip_chksum = 0;
    
    printf("\n");
    printf("IP Header\n");
    printf("   |-IP Version        : %u\n",ip_pkt->ip_version);
    printf("   |-IP Header Length  : %u DWORDS or %u Bytes\n",ip_pkt->ip_hdr_len,ipheader_len);
    printf("   |-Type Of Service   : %#04X\n",ip_pkt->ip_tos);
    printf("   |-IP Total Length   : %u  Bytes(Size of Packet)\n",ip_pkt->ip_len);
    printf("   |-Identification    : %#010x\n",ip_pkt->ip_id);
    printf("   |-IP Offset         : %#010x\n",ip_pkt->ip_offset);
    printf("   |-TTL               : %u\n",ip_pkt->ip_ttl);
    printf("   |-Protocol          : %#04X\n",ip_pkt->ip_proto);
    printf("   |-Checksum          : %#06X\n",checksum);
    printf("   |-Computed checksum : %#06X\n",ntohs_tos(ip_checksum_v2(ip_pkt,ipheader_len)));
    printf("   |-Source IP         : %u.%u.%u.%u\n" , ip_pkt->ip_src[0],ip_pkt->ip_src[1],ip_pkt->ip_src[2],ip_pkt->ip_src[3]);
    printf("   |-Destination IP    : %u.%u.%u.%u\n" , ip_pkt->ip_dst[0],ip_pkt->ip_dst[1],ip_pkt->ip_dst[2],ip_pkt->ip_dst[3]);
  
  }
  void print_udp_header(UDP ud,u_int len,PSEUDOIP sip)
  {
	  unsigned short checksum = ud->udp_checksum;
	  ud->udp_checksum = 0;

    printf("\n");
    printf("UDP Header\n");
    printf("   |-Source Port                : %u\n",ntohs_tos(ud->udp_src_port));
    printf("   |-Destination Port           : %u\n",ntohs_tos(ud->udp_dst_port));
    printf("   |-Length                     : %u\n",ntohs_tos(ud->udp_len));
    printf("   |-UDP checksum (optional)    : %#06X\n",ntohs_tos(checksum));
    printf("   |-Computed UDP checksum      : %#06X\n",ntohs_tos(udp_checksum(ud,len,sip)));

    printf("PSEUDO IP Header\n");
    printf("   |-Source IP         : %u.%u.%u.%u\n" , sip->ip_src[0],sip->ip_src[1],sip->ip_src[2],sip->ip_src[3]);
    printf("   |-Destination IP    : %u.%u.%u.%u\n" , sip->ip_dst[0],sip->ip_dst[1],sip->ip_dst[2],sip->ip_dst[3]);
  }
  
 void print_udp_data(UDP ud,IP ip,u_int_t len)
 {


	 u_int_t i , j;

 	 u_int_t ipheaderlen = ip->ip_hdr_len*4;
	 u_int_t header_length = ETH_HEAD_LEN + sizeof(ud) + ipheaderlen ;

	 u_char_t *buf = ud->payload;
	 u_int_t data_length = len - header_length;

	 printf("\n###################################################################\n");
	 for(i=0 ; i < data_length ; i++)
		  {
			  if( i!=0 && i%16==0)   //if one line of hex printing is complete...
				  {
				  	  printf( "         ");
					  for(j=i-16 ; j<i ; j++)
					  {
						  if(buf[j]>=32 && buf[j]<=128)
							  printf("%c",(u_char_t)buf[j]); //if its a number or alphabet

						  else printf("."); //otherwise print a dot
					  }
					  printf( "\n");
				  }

				  if(i%16==0) printf("   ");
				  printf(" %02X",(u_int_t)buf[i]);

				  if( i==data_length-1)  //print the last spaces
				  {
					  for(j=0;j<15-i%16;j++)
					  {
						  printf("   "); //extra spaces
					  }

					  printf("         ");

					  for(j=i-i%16 ; j<=i ; j++)
					  {
						  if(buf[j]>=32 && buf[j]<=128)
						  {
							  printf("%c",(u_char_t)buf[j]);
						  }
						  else
						  {
							  printf(".");
						  }
					  }

					  printf("\n" );
				  }
			  }
		  }


	
  void print_all_arp(ARP arp)
    {
      printf("\n###############################################################\n");
 
    printf("\nARP Header\n");
    printf("   |-Hardware type             : %x\n",arp->arp_hard_type);
    printf("   |-Protocol type             : %x\n",arp->arp_proto_type);
    printf("   |-Hardware size             : %x\n",arp->arp_hard_size);
    printf("   |-Protocol size             : %x\n",arp->arp_proto_size);
    printf("   |-Operation                 : %x\n",arp->arp_op);
    printf("   |-Source MAC Address        : %x%x%x%x%x%x\n",arp->arp_eth_source[0],arp->arp_eth_source[1],
	   arp->arp_eth_source[2],arp->arp_eth_source[3],arp->arp_eth_source[4],arp->arp_eth_source[5]);
    printf("   |-Source IP Address         : %x%x%x%x\n",arp->arp_ip_source[0],arp->arp_ip_source[1], arp->arp_ip_source[2],arp->arp_ip_source[3]);
    printf("   |-Destination MAC Address   : %x%x%x%x%x%x\n",arp->arp_eth_dest[0],arp->arp_eth_dest[1],
	   arp->arp_eth_dest[2],arp->arp_eth_dest[3],arp->arp_eth_dest[4],arp->arp_eth_dest[5]);
    printf("   |-Destination IP Address    : %x%x%x%x\n",arp->arp_ip_dest[0],arp->arp_ip_dest[1],arp->arp_ip_dest[2],arp->arp_ip_dest[3]);
    }

  void printPacket(u_int_t len, const u_char_t *packet){

   int i=0;
		   //*counter = (int *)t;

   //printf("Packet Count: %d\n", ++(*counter));
   //printf("Received Packet Size: %d\n", len);
   //printf("Payload:\n");
   for (i=0; i<len; i++){

      //if ( isprint(packet[i]) ) /* If it is a printable character, print it */
          printf("%02x:", packet[i]);
      //else
         // printf(". ");

       //if( (i%16 == 0 && i!=0) || i==len-1 )
         // printf("\n");
    }
   printf("\n");
   printf("%d\n",len);
   return;
  }
