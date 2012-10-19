#ifndef _NLL_H
#define _NLL_H

#ifndef INLINE
	# define INLINE extern inline
#endif


//! Little endian architecture.
#define __LITTLE_ENDIAN__       1234
//! Big endian architecture.
#define __BIG_ENDIAN__          4321
//! The current architecture.
#define __BYTE_ORDER__          __LITTLE_ENDIAN__




/*unsigned short n;
#if __BYTE_ORDER__==__LITTLE_ENDIAN__
  #define ntohs_tos(n) ( (((n) & 0xFF00) >> 8) | (((n) & 0x00FF) << 8) )
  #define htons_tos(n) ( (((n) & 0xFF00) >> 8) | (((n) & 0x00FF) << 8) )
  #define ntohl_tos(n) ( (((n) & 0xFF000000) >> 24) | (((n) & 0x00FF0000) >> 8) \
          | (((n) & 0x0000FF00) << 8) | (((n) & 0x000000FF) << 24) )
  #define htonl_tos(n) ( (((n) & 0xFF000000) >> 24) | (((n) & 0x00FF0000) >> 8) \
                | (((n) & 0x0000FF00) << 8) | (((n) & 0x000000FF) << 24) )
#else
  #define ntohs_tos(n) (n)
  #define htons_tos(n) (n)
  #define ntohl_tos(n) (n)
  #define htonl_tos(n) (n)

#endif */

INLINE unsigned int ntohs_tos(unsigned short n){
	#if __BYTE_ORDER__==__LITTLE_ENDIAN__
		return (((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8));
	#else
		return n;
	#endif
}

INLINE unsigned int ntohl_tos(unsigned int n){
	#if __BYTE_ORDER__==__LITTLE_ENDIAN__
		return ((n & 0xFF000000) >> 24) | ((n & 0x00FF0000) >> 8) | ((n & 0x0000FF00) << 8) |((n & 0x000000FF) << 24) ;
	#else
		return n;
	#endif

}

INLINE unsigned short htons_tos(unsigned short n){
#if __BYTE_ORDER__==__LITTLE_ENDIAN__
		return (((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8));
	#else
		return n;
	#endif
}

INLINE unsigned int htonl_tos(unsigned int n){
	#if __BYTE_ORDER__==__LITTLE_ENDIAN__
		return ((n & 0xFF000000) >> 24) | ((n & 0x00FF0000) >> 8) | ((n & 0x0000FF00) << 8) |((n & 0x000000FF) << 24) ;
	#else
		return n;
	#endif

}



#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#define MAX(a, b)       ((a) > (b) ? (a) : (b))


#define TRUE	1
#define FALSE	0		


#ifndef NULL
#define NULL	((void *) 0)
#endif


typedef int BOOL;

typedef unsigned char           	byte;
typedef unsigned short          	word;
typedef signed char             	char_t;
typedef unsigned char           	u_char_t;
typedef unsigned char 		   	u_int8_t;
typedef signed short int        	int16_t;
typedef unsigned short int      	u_int16_t;
typedef signed int              	int_t;
typedef unsigned int            	u_int_t;
typedef unsigned int 		 		u_int32_t;
//typedef unsigned long long      u_int64_t;
//typedef long long               int64_t;
//typedef int8_t                  int8;
//typedef u_int8_t                u_int8;
//typedef int16_t                 int16;
//typedef u_int16_t               u_int16;
//typedef int32_t                 int32;
//typedef u_int32_t               u_int32;
//typedef int64_t                 int64;
//typedef u_int64_t               u_int64;
//typedef unsigned char           u_char_t;
//typedef u_int32_t               w_char_t;
//typedef u_int32_t               int_t;
//typedef u_int32_t               addr_t;


extern u_char_t host_mac[6];

extern u_char_t host_ip[4];
extern u_char_t host_mask[4];

static u_char_t eth_bcast[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };



INLINE void memcpy_tos(void *dst,void *src,u_int_t len)
{
  if(dst == NULL || src==NULL)
    return;
   while(len--)
     *((u_char_t *)(dst++))= *((u_char_t *)(src++));
}

INLINE BOOL memcmp_tos(void *pt1, void *pt2,u_int_t size)
{
  u_char_t *p1 = (u_char_t *)pt1;
  u_char_t *p2 = (u_char_t *)pt2;

  u_int_t i;
  for(i=0;i<size;i++)
    if(*(p1 + i) != *(p2 +i))
      return FALSE;
  return TRUE;

}

INLINE BOOL is_digit(u_char_t c)
{
	return (c >= 0x30 && c <= 0x39);
}


INLINE BOOL is_ascii(u_char_t c)
{
	return (c <= 0x7f);
}

INLINE BOOL is_space(u_char_t c)
{
	return (c == 0x20);
}

INLINE void set_host_ip_net(u_char_t *,u_char_t *);

INLINE u_char_t *get_host_ip();

INLINE u_char_t *get_host_netmask();

INLINE void set_host_mac(u_char_t *mac);

INLINE u_char_t *get_host_mac();



 


#endif
