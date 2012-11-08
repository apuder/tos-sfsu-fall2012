
#ifdef NO_TOS
	#include <pcap.h>
	#include <stdio.h>
	#include <stdlib.h> // for exit()
	#include <string.h> //for memset
	#include <net/if.h>
	#include <sys/ioctl.h>
	#include <sys/socket.h>

#include <nll.h>

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
void printPacket(u_int_t len, const u_char_t * packet);
void print_all_arp(ARP *arp);

ARP arp_request_packet;
ARP arp_reply_packet;
IP ip_header;
ETH ether_header;
UDP udp;

u_char_t srip[4]={0,0,0,0};
u_char_t dip[4]={0,0,0,0};
//u_char_t smask[4];


u_char_t *dest_ip = "192.168.1.254";
u_char_t *h_ip = 	"192.168.1.79";

u_char_t host_mac[ETH_ADDR_LEN]={0xBC,0xAE,0x82,0x69,0xEB,0x28};

u_char_t eth_bcast[ETH_ADDR_LEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

int main()
{
		u_char_t *data ="This is test message";
  		udp_packet p;
  if((inet_aton_tos(dest_ip,&dip)!= -1) && (inet_aton_tos(h_ip,&srip) != -1)){
  	  	int x = create_udp_packet(45,26,&srip, &dip,20,(void *)data,&p);
 		printPacket(x, (u_char_t *) &p);}
	
  /*pcap_if_t *alldevsp , *device;
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



   // Combine the Ethernet header and ARP request into a contiguous block.
  unsigned char frame[ETH_HEAD_LEN + size];
  memcpy_tos(frame,&ethernet,ETH_HEAD_LEN);
  memcpy_tos(frame+ETH_HEAD_LEN,&arp_packet,size);

   int len = ETH_HEAD_LEN + size;

  if(!is_arp_request((void *)frame,(u_int_t)len,&arp_request_packet))
	 print_arp_request(&arp_request_packet,(u_int_t)size);
  	 print_all_arp(&arp_request_packet); 

  // Write the Ethernet frame to the interface.
      //if (pcap_inject(handle,&frame,len)==-1) {
         //exit(1);
      //}
 */
  return 0;
}
 
void got_packet(u_char *args, const struct pcap_pkthdr *header, const  u_char *packet)
{

  if(is_arp_request((void *)packet,(u_int_t)header->len,&arp_request_packet))
  {
     print_arp(&arp_request_packet,(u_int_t)header->len);
     print_all_arp(&arp_request_packet);
     //printPacket((u_int_t)header->len,packet);

  }
      
 else if(is_arp_reply((void *)packet,(u_int_t)header->len,&arp_reply_packet))
  {
    print_arp(&arp_reply_packet,(u_int_t)header->len);
    print_all_arp(&arp_reply_packet);
}
  else if(is_ip_packet((void *)packet,(u_int_t)header->len,&ip_header))
  {
    printf("\n************************************************************************\n");
    if(is_ethernet_header((void *)packet,(u_int_t)header->len,&ether_header))
    	print_ethernet_header(&ether_header,(u_int_t)header->len);
    print_ip_header(&ip_header);
  	if(is_udp_packet((void *)packet,(u_int_t)header->len,&udp)){
	  print_udp_header(&udp);
      //printPacket((u_int_t)((udp.len)-UDP_HEAD_MIN_LEN),udp.payload);
		  print_udp_data(&udp);
      }
  }
}

void print_ethernet_header(ETH *ether, u_int_t len)
{
    
  printf("\n");
  printf("Ethernet Header\n");
  printf("   |-Destination Address : %02x:%02x:%02x:%02x:%02x:%02x \n", ether->dst[0] , ether->dst[1] , ether->dst[2] , ether->dst[3] , ether->dst[4] , ether->dst[5] );
  printf("   |-Source Address      : %02x:%02x:%02x:%02x:%02x:%02x \n", ether->src[0] , ether->src[1] , ether->src[2] , ether->src[3] , ether->src[4] , ether->src[5] );
  printf("   |-Protocol            : %#05X \n",ntohs_tos(ether->type));
}


void print_arp(ARP *pkt,u_int_t len)
{
  	
    printf("\n###############################################################\n");
 
    printf("\nARP Header\n");
    printf("   |-ARP Packet Total Length   : %u  Bytes(Size of Packet)\n",len);
    printf("   |-ARP Operation             : %s\n",pkt->op == ARP_REQUEST ? "ARP REQUEST" : "ARP REPLY");
    printf("   |-Sender MAC Address        : %02x:%02x:%02x:%02x:%02x:%02x\n",pkt->eth_source[0],pkt->eth_source[1],\
    										pkt->eth_source[2],pkt->eth_source[3],pkt->eth_source[4],pkt->eth_source[5]);
    printf("   |-Sender IP Address         : %d.%d.%d.%d\n",pkt->ip_source[0],pkt->ip_source[1], \
    										pkt->ip_source[2],pkt->ip_source[3]);
    printf("   |-Target MAC Address        : %02x:%02x:%02x:%02x:%02x:%02x\n",pkt->eth_dest[0],pkt->eth_dest[1],\
    										pkt->eth_dest[2],pkt->eth_dest[3],pkt->eth_dest[4],pkt->eth_dest[5]);
    printf("   |-Target IP Address         : %d.%d.%d.%d\n",pkt->ip_dest[0],pkt->ip_dest[1],pkt->ip_dest[2],pkt->ip_dest[3]);
   
  }
void print_ip_header(IP *ip_pkt)
{	
    unsigned short ipheader_len  = ip_pkt->hdr_len*4;
  
    
    printf("\n");
    printf("IP Header\n");
    printf("   |-IP Version        : %u\n",ip_pkt->version);
    printf("   |-IP Header Length  : %u DWORDS or %u Bytes\n",ip_pkt->hdr_len,ipheader_len);
    printf("   |-Type Of Service   : %#04X\n",ip_pkt->tos);
    printf("   |-IP Total Length   : %u  Bytes(Size of Packet)\n",ntohs_tos(ip_pkt->len));
    printf("   |-Identification    : %#04x\n",ntohs_tos(ip_pkt->id));
    printf("   |-IP Offset         : %#04x\n",ntohs_tos(ip_pkt->offset));
    printf("   |-TTL               : %u\n",ip_pkt->ttl);
    printf("   |-Protocol          : %#04X\n",ip_pkt->protocol);
    printf("   |-Checksum          : %#04X\n",ntohs_tos(ip_pkt->checksum));
    printf("   |-Computed checksum : %#04X\n",ntohs_tos(ip_checksum_v2(ip_pkt)));
    printf("   |-Source IP         : %u.%u.%u.%u\n" , ip_pkt->src[0],ip_pkt->src[1],ip_pkt->src[2], \
           							  ip_pkt->src[3]);
    printf("   |-Destination IP    : %u.%u.%u.%u\n" , ip_pkt->dst[0],ip_pkt->dst[1],ip_pkt->dst[2], \
           							ip_pkt->dst[3]);
  
  }		

 void print_udp_header (UDP *ud)
  {

    printf("\n");
    printf("UDP Header\n");
    printf("   |-Source Port                : %u\n",ntohs_tos(ud->src_port));
    printf("   |-Destination Port           : %u\n",ntohs_tos(ud->dst_port));
    printf("   |-Length                     : %u\n",ntohs_tos(ud->len));
    printf("   |-UDP checksum (optional)    : %#04X\n",ntohs_tos(ud->checksum));
 
  }
  
 void print_udp_data(UDP *ud)
 {


	 u_int_t i , j;

 	 u_char_t *buf = (u_char_t *)ud->payload;
	 u_int_t data_length = (ntohs_tos(ud->len)-UDP_HEAD_MIN_LEN);
	 printf("%d",data_length);

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


	
  void print_all_arp(ARP *arp)
    {
	

	printf("\n###############################################################\n");
 
    printf("\nARP Header\n");
    printf("   |-Hardware type             : %x\n",arp->hard_type);
    printf("   |-Protocol type             : %x\n",arp->proto_type);
    printf("   |-Hardware size             : %x\n",arp->hard_size);
    printf("   |-Protocol size             : %x\n",arp->proto_size);
    printf("   |-Operation                 : %x\n",arp->op);
    printf("   |-Source MAC Address        : %x%x%x%x%x%x\n",arp->eth_source[0],arp->eth_source[1], \
	   					arp->eth_source[2],arp->eth_source[3],arp->eth_source[4],arp->eth_source[5]);
    printf("   |-Source IP Address         : %x%x%x%x\n",arp->ip_source[0],arp->ip_source[1], \
           												arp->ip_source[2],arp->ip_source[3]);
    printf("   |-Destination MAC Address   : %x%x%x%x%x%x\n",arp->eth_dest[0],arp->eth_dest[1],\
	   							arp->eth_dest[2],arp->eth_dest[3],arp->eth_dest[4],arp->eth_dest[5]);
    printf("   |-Destination IP Address    : %x%x%x%x\n",arp->ip_dest[0],arp->ip_dest[1],\
           						arp->ip_dest[2],arp->ip_dest[3]);
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
         //printf(". ");

       if((i%16 == 0 && i!=0)|| i==len-1)
         printf("\n");
    }
   printf("\n");
   printf("%d\n",len);
   return;
  }
#endif
