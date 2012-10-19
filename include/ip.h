#ifndef _IP_H
#define _IP_H

#include "nll.h"


 #define IP_V4                   4
 
 #define IP_V6                   6
 
 
 #define IP_FRAME_LEN            65535
 
 #define IP_HEAD_MIN_LEN         20
 
 #define IP_DEFAULT_TTL          64

 #define IP_PROTO_TCP            6

 #define IP_PROTO_UDP            0x11
 

 #define IP_TOS_MIN_DELAY        0x10

 #define IP_TOS_MAX_THRU         0x08

 #define IP_TOS_MAX_RELY         0x04

 #define IP_TOS_MIN_COST         0x02
 

 #define IP_FLAG_MF              0x2000

 #define IP_FLAG_DF              0x4000

 #define IP_FLAG_SERVED          0x8000

 #define IP_FLAG_MASK            0x1FFF


 #define IP_ADDRESS(a, b, c, d)  ((a) | (b) << 8 | (c) << 16 | (d) << 24)
 
 #define IP_A(ip)                ((u_int8_t) ((ip) >> 24))

 #define IP_B(ip)                ((u_int8_t) ((ip) >> 16))

 #define IP_C(ip)                ((u_int8_t) ((ip) >>  8))
 
 #define IP_D(ip)                ((u_int8_t) ((ip) >>  0))


 #define INADDR_BROADCAST        IP_ADDRESS(255, 255, 255, 255)

 struct _ip;
 typedef struct _ip* IP;
 
 typedef struct _ip
 {
#if __BYTE_ORDER__==__LITTLE_ENDIAN__

         u_int8_t ip_hdr_len:4;   
         u_int8_t ip_version:4;  
 #else
         u_int8_t ip_version:4;   
         u_int8_t ip_hdr_len:4;   
 #endif
         
         u_int8_t  ip_tos;
         u_int16_t ip_len;
         u_int16_t ip_id;
       
         u_int16_t ip_offset;
        
         u_int8_t ip_ttl;
        
         u_int8_t ip_proto;
        
         u_int16_t ip_chksum;

         u_char_t ip_src[4];
         u_char_t ip_dst[4];
 
         //u_int32_t ip_src;
         
         //u_int32_t ip_dst;
 }ip_t;
 

 
 u_int16_t ip_checksum(IP ip , u_int_t hdr_len);
 u_int16_t ip_checksum_v2(IP ip , u_int_t hdr_len);


 BOOL is_ip_packet(void *buffer,u_int_t len , IP ip_pkt);
 int inet_aton_tos(u_char_t *dot_ip, u_char_t *net_ip);
 void print_ip_header(IP ip_pkt, u_int_t packet_len);
 

 

 
 #endif
