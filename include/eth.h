 #ifndef _ETH_H
 #define _ETH_H
 
#include "nll.h"


#define		ETHERTYPE_IP            0x0800		/* IP */
#define		ETHERTYPE_ARP           0x0806		/* Address resolution */

 #define ETH_ADDR_LEN            6
 
 #define IP_LEN 		  		  4

 #define ETH_HEAD_LEN            14

 #define ETH_MIN_LEN             60

 #define ETH_MAX_LEN             1514
 #define ETH_MAX_TRANSFER_UNIT   (ETH_MAX_LEN - ETH_HEAD_LEN)
 

 
struct _ethernet;
typedef struct _ethernet* ETH;
 
typedef struct _ethernet
 {
         //! Destination MAC address.
         u_char_t  dst[ETH_ADDR_LEN];
         //! Source MAC address.
         u_char_t  src[ETH_ADDR_LEN];
         //! The packet type.
        u_int16_t type;
         //! The packet content.
       // u_int8_t data[1];
 }ethernet_t ;

 // --- Prototypes ----------------------------------------------------- //
 
BOOL is_ethernet_header(void *buffer , ETH ether);
void set_host_mac(u_char_t *mac);
u_char_t *get_host_mac();
void print_ethernet_header(ETH ether,u_int_t len);

 
 
#endif
