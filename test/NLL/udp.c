#ifndef _UDP_C
#define _UDP_C


#include <nll.h>
#ifdef NO_TOS
	#include <stdio.h>
#endif

BOOL is_udp_packet(void *buffer,u_int_t len,UDP *packet)
{
	IP ip_header;
	P_UDP p_udp_header;
	
	u_int_t payload_len = (len - (ETH_HEAD_LEN + IP_HEAD_MIN_LEN + UDP_HEAD_MIN_LEN));
	if(is_ip_packet(buffer,len,&ip_header)){
		if(ip_header.protocol != IP_PROTO_UDP)
			return FALSE;
		u_char_t *udpheader = (u_char_t *)(buffer + ETH_HEAD_LEN + IP_HEAD_MIN_LEN);
		
		packet->src_port = *((u_int16_t *)(udpheader));
		packet->dst_port = *((u_int16_t *)(udpheader + 2));
		packet->len = *((u_int16_t *)(udpheader + 4));
		packet->checksum = *((u_int16_t *)(udpheader + 6));
		packet->payload = (u_char_t *)(buffer + ETH_HEAD_LEN + IP_HEAD_MIN_LEN + UDP_HEAD_MIN_LEN);

		#ifdef NO_TOS
				//printPacket(payload_len,packet->payload);
		#endif

		return TRUE;
	}
	else
		return FALSE;
}

 u_int16_t udp_checksum(UDP *udp,u_char_t *src_ip,u_char_t *dst_ip) // udp length header + payload
 {
 	#ifdef NO_TOS
		// printf("I am her");
	#endif
	 //if(udp->checksum != 0)
		// if(udp->checksum == 0xFFFF)
		 //	return 0x0000;
	 //unsigned short temp = udp->checksum;
	 udp->checksum = 0;
	 int length = ntohs_tos(udp->len);
	 int header_len = UDP_HEAD_MIN_LEN;
	 int payload_len = (ntohs_tos(udp->len) - header_len);

     u_int16_t *w = (u_int16_t *)udp;
	 u_int16_t *src = (u_int16_t *)src_ip;
	 u_int16_t *dst = (u_int16_t *)dst_ip;
	 u_int16_t *data = (u_int16_t *)udp->payload;
     u_int32_t sum = 0;
     unsigned short answer = 0;
	 #ifdef NO_TOS
		 //printf("I am her");
	#endif

	 while (header_len > 1)
         {
                 sum += *w++;
                 //if (sum & 0x80000000)
                     //sum = (sum & 0xFFFF) + (sum >> 16);
                 header_len -= 2;
         }
	 while(payload_len > 1)
	 {
		 sum += *data++;
          if (sum & 0x80000000)
             sum = (sum & 0xFFFF) + (sum >> 16);
          payload_len -= 2;
	 }

      if ( length & 1 )
                 // Add the padding if the packet length is odd         
                 sum += *((u_char_t *)w);
	 			 //sum += 0;
		
		 sum += *(src++);
       	 sum += *src;

	     sum += *(dst++);
         sum += *dst;
	 
 		 sum +=0;
         sum += IP_PROTO_UDP;
	     sum += udp->len;
     // Add the carries                                              
         while (sum >> 16)
                 sum = (sum & 0xFFFF) + (sum >> 16);

         answer = ~sum;
	 	 //udp->checksum = temp;
	 	   #ifdef NO_TOS
				//printPacket(payload_len,(u_char_t *)ip_pkt->payload);
	 			//printf("%#04X",answer);
		  #endif
         return (answer);
 }
/*u_int16_t udp_checksum_v2(UDP *udp)
{
	 udp->checksum = 0;
	 P_UDP *temp = udp->pseudo_header;
	 udp->pseudo_header = 0;
	 
	 int Pseudo_len = sizeof(P_UDP);
	 int length = udp->len;

     u_int16_t *w = (u_int16_t *)udp;
	 u_int16_t *src = temp->src_ip;
	 u_int16_t *dst = temp->dst_ip;
     u_int32_t sum = 0;
     unsigned short answer = 0;
	 int payload_len = udp->len -UDP_HEAD_MIN_LEN;
	BOOL padding;
	u_int16_t padd =0;
	int i;
	if(udp->len%2 ==0)
		padding=FALSE;
	else{
		padding=TRUE;
		padd =1;}
	if(padding)
		*((u_char_t *)(udp + udp->len)) = 0;
	for (i=0;i<udp->len+padd;i=i+2){
		*w =((buff[i]<<8)&0xFF00)+(buff[i+1]&0xFF);
typedef unsigned short u16;
typedef unsigned long u32;

u16 udp_sum_calc(u16 len_udp, u16 src_addr[],u16 dest_addr[], BOOL padding, u16 buff[])
{
u16 prot_udp=17;
u16 padd=0;
u16 word16;
u32 sum;	
	
	// Find out if the length of data is even or odd number. If odd,
	// add a padding byte = 0 at the end of packet
	if (padding&1==1){
		padd=1;
		buff[len_udp]=0;
	}
	
	//initialize sum to zero
	sum=0;
	
	// make 16 bit words out of every two adjacent 8 bit words and 
	// calculate the sum of all 16 vit words
	for (i=0;i<len_udp+padd;i=i+2){
		word16 =((buff[i]<<8)&0xFF00)+(buff[i+1]&0xFF);
		sum = sum + (unsigned long)word16;
	}	
	// add the UDP pseudo header which contains the IP source and destinationn addresses
	for (i=0;i<4;i=i+2){
		word16 =((src_addr[i]<<8)&0xFF00)+(src_addr[i+1]&0xFF);
		sum=sum+word16;	
	}
	for (i=0;i<4;i=i+2){
		word16 =((dest_addr[i]<<8)&0xFF00)+(dest_addr[i+1]&0xFF);
		sum=sum+word16; 	
	}
	// the protocol number and the length of the UDP packet
	sum = sum + prot_udp + len_udp;

	// keep only the last 16 bits of the 32 bit calculated sum and add the carries
    	while (sum>>16)
		sum = (sum & 0xFFFF)+(sum >> 16);
		
	// Take the one's complement of sum
	sum = ~sum;

return ((u16) sum);
}	
}*/
#endif

