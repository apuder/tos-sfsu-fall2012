#ifndef _IP_C
#define _IP_C

//#define INLINE

#include <nll.h>

u_char_t host_ip[4];
u_char_t host_mask[4];

void set_host_ip_net(u_char_t *ip, u_char_t *netmask)
{
  
  memcpy_tos(host_ip,ip,IP_LEN);
  memcpy_tos(host_mask, netmask,IP_LEN);
 }

  u_char_t *get_host_ip()
  {
    return host_ip;
  }
  u_char_t *get_host_netmask()
  {
    return host_mask;
  }

  BOOL is_ip_packet(void *buffer, u_int_t len, IP ip_pkt)
  {

     //u_int_t i;
     char *buf = (char *)(buffer);
     ETH ethheader = (ETH)(buf);

    if(ntohs_tos(ethheader ->type) != ETHERTYPE_IP)
      return FALSE;

     IP ipheader = (IP )(buf + ETH_HEAD_LEN);
     if((ipheader->ip_hdr_len *4) < IP_HEAD_MIN_LEN )
    	 return FALSE;


      ip_pkt->ip_hdr_len  = ipheader->ip_hdr_len;
      ip_pkt->ip_version = ipheader->ip_version;
      ip_pkt->ip_tos = ipheader->ip_tos;
      ip_pkt->ip_len = ipheader->ip_len;
      ip_pkt->ip_id = ipheader->ip_id;
      ip_pkt->ip_offset = ipheader->ip_offset;
      ip_pkt->ip_ttl = ipheader->ip_ttl;
      ip_pkt->ip_proto = ipheader->ip_proto;
      ip_pkt->ip_chksum = ipheader->ip_chksum;

      //for(i=0;i<4;i++){
      memcpy_tos(ip_pkt->ip_src,ipheader->ip_src,IP_LEN);
      memcpy_tos(ip_pkt->ip_dst,ipheader->ip_dst,IP_LEN);
      //}

      return TRUE;

  }

 
 u_int16_t ip_checksum(IP ip, u_int_t hdr_len)
 {
    unsigned long sum = 0;  /* assume 32 bit long, 16 bit short */
    const u_int16_t *ip1 = (u_int16_t *)ip;
    
           while(hdr_len > 1){
             sum += *(ip1)++;
             if(sum & 0x80000000)   /* if high order bit set, fold */
               sum = (sum & 0xFFFF) + (sum >> 16);
             hdr_len -= 2;
           }

           if(hdr_len)      /* take care of left over byte */
             sum += (unsigned short) *(unsigned char *)ip;
          
           while(sum>>16)
             sum = (sum & 0xFFFF) + (sum >> 16);

           return ~sum;
 }
 
 u_int16_t ip_checksum_v2(IP ip , u_int_t hdr_len){
   
    u_int_t sum = 0;
    unsigned short answer = 0;
    unsigned short *w = (unsigned short *)ip;

 
    while(hdr_len > 1){
        sum += *w++;
        hdr_len -= 2;
    }
 
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    answer = ~sum;
    return(answer);
 }
 
 int inet_aton_tos(u_char_t *dot_ip, u_char_t *net_ip)
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
  }


#endif

 
