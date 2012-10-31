#ifndef _IP_C
#define _IP_C

#include <nll.h>

  BOOL is_ip_packet(void *buffer, u_int_t len, IP *ip_pkt)
  {
	  ETH ethheader;

	  //u_int_t payload_len = (len - (ETH_HEAD_LEN + IP_HEAD_MIN_LEN));
	  if (is_ethernet_header(buffer, len,&ethheader)) {

		  if (ntohs_tos(ethheader.type) != ETHERTYPE_IP)
			  return FALSE;

		  u_char_t *ipheader = (u_char_t *)(buffer + ETH_HEAD_LEN);
		  if(((*(ipheader) & 0x0F)*4) < IP_HEAD_MIN_LEN )
		      	 return FALSE;
		  ip_pkt->version = ((*(ipheader) >> 4 ) & 0x0F);
		  ip_pkt->hdr_len  = (*(ipheader) & 0x0F);
		  ip_pkt->tos = *(ipheader + 1) ;
		  ip_pkt->len = *((u_int16_t *)(ipheader + 2));
		  ip_pkt->id = *((u_int16_t *)(ipheader + 4));
		  ip_pkt->offset = *((u_int16_t *)(ipheader + 6));
		  ip_pkt->ttl = *(ipheader + 8);
		  ip_pkt->protocol = *(ipheader + 9);
		  ip_pkt->checksum = *((u_int16_t *)(ipheader + 10));
		  memcpy_tos(ip_pkt->src,(u_char_t *)(ipheader + 12),IP_LEN);
		  memcpy_tos(ip_pkt->dst,(u_char_t *)(ipheader + 16),IP_LEN);
		  //ip_pkt->payload = (void *)&ethheader.payload[IP_HEAD_MIN_LEN];
		  #ifdef NO_TOS
				//printPacket(payload_len,(u_char_t *)ip_pkt->payload);
		  #endif

      return TRUE;
	  }
	  else
		  return FALSE;
	}

 
 u_int16_t ip_checksum(IP *ip)
  {
	
	ip->checksum = 0;
   
	int len = (ip->hdr_len)*4;
	unsigned long sum = 0;  /* assume 32 bit long, 16 bit short */
    const u_int16_t *ip1 = (u_int16_t *)ip;
    
           while(len > 1){
             sum += *(ip1)++;
             if(sum & 0x80000000)   /* if high order bit set, fold */
               sum = (sum & 0xFFFF) + (sum >> 16);
             len -= 2;
           }

           if(len)      /* take care of left over byte */
             sum += (unsigned short) *(unsigned char *)ip;
          
           while(sum>>16)
             sum = (sum & 0xFFFF) + (sum >> 16);
		   return ~sum;
 }
 
 u_int16_t ip_checksum_v2(IP *ip){

	 //unsigned short temp = ip->checksum;
	 ip->checksum = 0;
	 //ip->payload =  0;
	 int len = (ip->hdr_len)*4;
	 unsigned long  sum = 0;
	 unsigned short answer = 0;
	 unsigned short *w = (unsigned short *)ip;

 
    while(len > 1){
        sum += *w++;
        len -= 2;
    }
 
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    answer = ~sum;
	//ip->checksum = temp;
    return(answer);
 }
 
 /*int inet_aton_tos(u_char_t *dot_ip, u_char_t *net_ip)
  {
          //static const in_addr_t max[4] = { 0xFFFFFFFF, 0xFFFFFF, 0xFFFF, 0xFF };
          //u_int_t ;
          u_char_t c;
          //union iaddr {
            //uint8_t bytes[4];
            //uint32_t word;
          //} res;
          //uint8_t *pp = res.bytes;
          u_int_t digit,base,val;

         //res.word = 0;

          c = *(dot_ip);
          u_char_t *p = net_ip;
         // printf("%s",c);
          for (;;) {
                  // Collect number up to '.'                             //
                  // Values are specified as for C:                       //
                  // 0x=hex, 0=octal, isdigit=decimal.                    //
                  if (!is_digit(c))
                	  return -1;
                  val = 0; base = 10; digit = 0;
                  for (;;) {
                          if (is_digit(c)) {
                                  val = (val * base) + (c - '0');
                                  c = *++dot_ip;
                                  digit = 1;
                          } else {
                                  break;
                          }
                  }
                  if (c == '.') {
                          // Internet format:                             //
                          //      a.b.c.d                                 //
                          //      a.b.c   (with c treated as 16 bits)     //
                          //      a.b     (with b treated as 24 bits)     //
                          //if (net_ip > p + 2 || val > 0xff) {
                            //      return -1;
                         // }
                          //printf("%d",val);
                          *net_ip++ = val;
                          c = *++dot_ip;
                          //printf("%s",c);
                  } else
                          break;
          }

          // Check for trailing characters                                //
          if (c != '\0' && (!is_ascii(c) || !is_space(c))) {
                  return -1;
          }
 return 0;
  }*/


#endif

 
