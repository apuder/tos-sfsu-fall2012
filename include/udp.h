 #ifndef _UDP_H
 #define _UDP_H
 
#include "nll.h"
#include "ip.h"


 
struct _udp;
typedef struct _udp* UDP;
 
typedef struct _udp
 {
        
        u_int16_t udp_src_port;
        u_int16_t udp_dst_port;
        u_int16_t udp_len;
        u_int16_t udp_checksum;
        u_char_t *payload;
 }udp_t ;

 struct _pseudo_ip;
 typedef struct _pseudo_ip* PSEUDOIP;

 typedef struct _pseudo_ip
  {
        u_char_t ip_src[4];
	    u_char_t ip_dst[4];
  }pseudo_ip_t ;

 // --- Prototypes ----------------------------------------------------- //
 
u_int16_t udp_checksum(UDP ud, u_int_t len, PSEUDOIP sip);
void set_pseudo_ip_header(IP ip,PSEUDOIP sip);
BOOL is_udp_packet(void *buffer,u_int_t len, UDP packet);
void print_udp_header(UDP ud,u_int_t len,PSEUDOIP sip);
void print_udp_data(UDP ud,IP ip,u_int_t len);


#endif
  
