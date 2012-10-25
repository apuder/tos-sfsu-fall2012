/**
 * NE2000 driver for TOS
 * 
 * Bibliography:
 * http://wiki.osdev.org/Ne2000
 * http://www.jbox.dk/sanos/source/sys/dev/ne2000.c.html
 * All resources were accessed during Fall 2012
 * 
 */
#include <kernel.h>
#include <arp.h>

PORT ne2k_driver_port;

struct ne *__ne;

typedef unsigned short uid_t;
typedef unsigned short gid_t;

#define ETHERNET_POLYNOMIAL 0x04c11db7U
#define ETHER_HLEN 14
#define ETHER_ADDR_LEN 6
#define DPC_QUEUED_BIT 0
#define DEVNAMELEN      32

#define NE_BUFFER_START_PAGE 0x40
#define NE_BUFFER_STOP_PAGE 0x4000

// Card config
#define NE_RX_PAGE_START 0x46
#define NE_RX_PAGE_BNDRY 0x46
#define NE_RX_PAGE_STOP  0x80

// Page 0 register offsets
#define NE_P0_CR        0x00           // Command Register
#define NE_P0_CLDA0     0x01           // Current Local DMA Addr low (read)
#define NE_P0_PSTART    0x01           // Page Start register (write)
#define NE_P0_CLDA1     0x02           // Current Local DMA Addr high (read)
#define NE_P0_PSTOP     0x02           // Page Stop register (write)
#define NE_P0_BNRY      0x03           // Boundary Pointer
#define NE_P0_TSR       0x04           // Transmit Status Register (read)
#define NE_P0_TPSR      0x04           // Transmit Page Start (write)
#define NE_P0_NCR       0x05           // Number of Collisions Reg (read)
#define NE_P0_TBCR0     0x05           // Transmit Byte count, low (write)
#define NE_P0_FIFO      0x06           // FIFO register (read)
#define NE_P0_TBCR1     0x06           // Transmit Byte count, high (write)
#define NE_P0_ISR       0x07           // Interrupt Status Register
#define NE_P0_CRDA0     0x08           // Current Remote DMA Addr low (read)
#define NE_P0_RSAR0     0x08           // Remote Start Address low (write)
#define NE_P0_CRDA1     0x09           // Current Remote DMA Addr high (read)
#define NE_P0_RSAR1     0x09           // Remote Start Address high (write)
#define NE_P0_RBCR0     0x0A           // Remote Byte Count low (write)
#define NE_P0_RBCR1     0x0B           // Remote Byte Count high (write)
#define NE_P0_RSR       0x0C           // Receive Status (read)
#define NE_P0_RCR       0x0C           // Receive Configuration Reg (write)
#define NE_P0_CNTR0     0x0D           // Frame alignment error counter (read)
#define NE_P0_TCR       0x0D           // Transmit Configuration Reg (write)
#define NE_P0_CNTR1     0x0E           // CRC error counter (read)
#define NE_P0_DCR       0x0E           // Data Configuration Reg (write)
#define NE_P0_CNTR2     0x0F           // Missed packet counter (read)
#define NE_P0_IMR       0x0F           // Interrupt Mask Register (write)

// Page 1 register offsets
#define NE_P1_CR        0x00           // Command Register
#define NE_P1_PAR0      0x01           // Physical Address Register 0
#define NE_P1_PAR1      0x02           // Physical Address Register 1
#define NE_P1_PAR2      0x03           // Physical Address Register 2
#define NE_P1_PAR3      0x04           // Physical Address Register 3
#define NE_P1_PAR4      0x05           // Physical Address Register 4
#define NE_P1_PAR5      0x06           // Physical Address Register 5
#define NE_P1_CURR      0x07           // Current RX ring-buffer page
#define NE_P1_MAR0      0x08           // Multicast Address Register 0
#define NE_P1_MAR1      0x09           // Multicast Address Register 1
#define NE_P1_MAR2      0x0A           // Multicast Address Register 2
#define NE_P1_MAR3      0x0B           // Multicast Address Register 3
#define NE_P1_MAR4      0x0C           // Multicast Address Register 4
#define NE_P1_MAR5      0x0D           // Multicast Address Register 5
#define NE_P1_MAR6      0x0E           // Multicast Address Register 6
#define NE_P1_MAR7      0x0F           // Multicast Address Register 7

// Page 2 register offsets
#define NE_P2_CR        0x00           // Command Register
#define NE_P2_PSTART    0x01           // Page Start (read)
#define NE_P2_CLDA0     0x01           // Current Local DMA Addr 0 (write)
#define NE_P2_PSTOP     0x02           // Page Stop (read)
#define NE_P2_CLDA1     0x02           // Current Local DMA Addr 1 (write)
#define NE_P2_RNPP      0x03           // Remote Next Packet Pointer
#define NE_P2_TPSR      0x04           // Transmit Page Start (read)
#define NE_P2_LNPP      0x05           // Local Next Packet Pointer
#define NE_P2_ACU       0x06           // Address Counter Upper
#define NE_P2_ACL       0x07           // Address Counter Lower
#define NE_P2_RCR       0x0C           // Receive Configuration Register (read)
#define NE_P2_TCR       0x0D           // Transmit Configuration Register (read)
#define NE_P2_DCR       0x0E           // Data Configuration Register (read)
#define NE_P2_IMR       0x0F           // Interrupt Mask Register (read)

// Command Register (CR)
#define NE_CR_STP       0x01           // Stop
#define NE_CR_STA       0x02           // Start
#define NE_CR_TXP       0x04           // Transmit Packet
#define NE_CR_RD0       0x08           // Remote DMA Command 0
#define NE_CR_RD1       0x10           // Remote DMA Command 1
#define NE_CR_RD2       0x20           // Remote DMA Command 2
#define NE_CR_PS0       0x40           // Page Select 0
#define NE_CR_PS1       0x80           // Page Select 1
#define NE_CR_PAGE_0    0x00           // Select Page 0
#define NE_CR_PAGE_1    0x40           // Select Page 1
#define NE_CR_PAGE_2    0x80           // Select Page 2

// Interrupt Status Register (ISR)
#define NE_ISR_PRX      0x01           // Packet Received
#define NE_ISR_PTX      0x02           // Packet Transmitted
#define NE_ISR_RXE      0x04           // Receive Error
#define NE_ISR_TXE      0x08           // Transmission Error
#define NE_ISR_OVW      0x10           // Overwrite
#define NE_ISR_CNT      0x20           // Counter Overflow
#define NE_ISR_RDC      0x40           // Remote Data Complete
#define NE_ISR_RST      0x80           // Reset status
#define NE_ISR_ALL      0xFF           // All of them

// Interrupt Mask Register (IMR)
#define NE_IMR_PRXE     0x01           // Packet Received Interrupt Enable
#define NE_IMR_PTXE     0x02           // Packet Transmit Interrupt Enable
#define NE_IMR_RXEE     0x04           // Receive Error Interrupt Enable
#define NE_IMR_TXEE     0x08           // Transmit Error Interrupt Enable
#define NE_IMR_OVWE     0x10           // Overwrite Error Interrupt Enable
#define NE_IMR_CNTE     0x20           // Counter Overflow Interrupt Enable
#define NE_IMR_RDCE     0x40           // Remote DMA Complete Interrupt Enable

// Data Configuration Register (DCR)
#define NE_DCR_WTS      0x01           // Word Transfer Select
#define NE_DCR_BOS      0x02           // Byte Order Select
#define NE_DCR_LAS      0x04           // Long Address Select
#define NE_DCR_LS       0x08           // Loopback Select
#define NE_DCR_AR       0x10           // Auto-initialize Remote
#define NE_DCR_FT0      0x20           // FIFO Threshold Select 0
#define NE_DCR_FT1      0x40           // FIFO Threshold Select 1

// Transmit Configuration Register (TCR)
#define NE_TCR_CRC      0x01           // Inhibit CRC
#define NE_TCR_LB0      0x02           // Loopback Control 0
#define NE_TCR_LB1      0x04           // Loopback Control 1
#define NE_TCR_ATD      0x08           // Auto Transmit Disable
#define NE_TCR_OFST     0x10           // Collision Offset Enable

// Transmit Status Register (TSR)
#define NE_TSR_PTX      0x01           // Packet Transmitted
#define NE_TSR_COL      0x04           // Transmit Collided
#define NE_TSR_ABT      0x08           // Transmit Aborted
#define NE_TSR_CRS      0x10           // Carrier Sense Lost
#define NE_TSR_FU       0x20           // FIFO Underrun
#define NE_TSR_CDH      0x40           // CD Heartbeat
#define NE_TSR_OWC      0x80           // Out of Window Collision

// Receiver Configuration Register (RCR)
#define NE_RCR_SEP      0x01           // Save Errored Packets
#define NE_RCR_AR       0x02           // Accept Runt packet
#define NE_RCR_AB       0x04           // Accept Broadcast
#define NE_RCR_AM       0x08           // Accept Multicast
#define NE_RCR_PRO      0x10           // Promiscuous Physical
#define NE_RCR_MON      0x20           // Monitor Mode

// Receiver Status Register (RSR)
#define NE_RSR_PRX      0x01           // Packet Received Intact
#define NE_RSR_CRC      0x02           // CRC Error
#define NE_RSR_FAE      0x04           // Frame Alignment Error
#define NE_RSR_FO       0x08           // FIFO Overrun
#define NE_RSR_MPA      0x10           // Missed Packet
#define NE_RSR_PHY      0x20           // Physical Address
#define NE_RSR_DIS      0x40           // Receiver Disabled
#define NE_RSR_DFR      0x80           // Deferring

// Novell NE2000
#define NE_NOVELL_NIC_OFFSET    0x00
#define NE_NOVELL_ASIC_OFFSET   0x10
#define NE_NOVELL_DATA          0x00
#define NE_NOVELL_RESET         0x0F
#define NE_PAGE_SIZE            256    // Size of RAM pages in bytes
#define NE_TXBUF_SIZE           6      // Size of TX buffer in pages
#define NE_TX_BUFERS            2      // Number of transmit buffers
#define NE_TIMEOUT              10000
#define NE_TXTIMEOUT            30000

// IO port addresses
#define PIC_MSTR_CTRL           0x20
#define PIC_MSTR_MASK           0x21
#define PIC_SLV_CTRL            0xA0
#define PIC_SLV_MASK            0xA1

// End of interrupt commands
#define PIC_EOI_BASE            0x60
#define PIC_EOI_CAS             0x62
#define PIC_EOI_FD              0x66

#define IRQBASE       0x20
#define IRQ2INTR(irq) (IRQBASE + (irq))
#define INTRS 64

unsigned int irq_mask = 0xFFFB;
unsigned int initialized = 0;

// Receive ring descriptor

struct recv_ring_desc {
    unsigned char rsr; // Receiver status
    unsigned char next_pkt; // Pointer to next packet
    unsigned short count; // Bytes in packet (length + 4)
};

typedef struct interrupt {
    struct interrupt *next;
    int flags;
    // intrproc_t handler;
    void *arg;
};

struct interrupt *intrhndlr[INTRS];

typedef struct dpc {
    // dpcproc_t proc;
    void *arg;
    struct dpc *next;
    int flags;
};

typedef struct event {
};

struct eth_addr {
    unsigned char addr[ETHER_ADDR_LEN];
};

struct eth_hdr {
    struct eth_addr dest;
    struct eth_addr src;
    unsigned short type;
};

typedef struct pbuf {
    struct pbuf *next;
    unsigned short flags;
    unsigned short ref;
    void *payload;
    int tot_len; // Total length of buffer + additionally chained buffers.
    int len; // Length of this buffer.
    int size; // Allocated size of buffer
} pbuf;

typedef unsigned int dev_t;
struct dpc *dpc_queue_tail;
struct dpc *dpc_queue_head;

// NE2000 NIC status

struct ne {
    dev_t devno; // Device number
    struct eth_addr hwaddr; // MAC address
    unsigned short iobase; // Configured I/O base
    unsigned short irq; // Configured IRQ
    unsigned short membase; // Configured memory base
    unsigned short memsize; // Configured memory size
    unsigned short asic_addr; // ASIC I/O bus address
    unsigned short nic_addr; // NIC (DP8390) I/O bus address
    struct interrupt intr; // Interrupt object for driver
    struct dpc dpc; // DPC for driver
    unsigned short rx_ring_start; // Start address of receive ring
    unsigned short rx_ring_end; // End address of receive ring
    unsigned short rx_page_start; // Start of receive ring
    unsigned short rx_page_stop; // End of receive ring
    unsigned char next_pkt; // Next unread received packet
    struct event rdc; // Remote DMA completed event
    struct event ptx; // Packet transmitted event
    // struct mutex txlock;               // Transmit lock
};

struct dev {
    char name[DEVNAMELEN];
    struct driver *driver;
    struct unit *unit;
    void *privdata;
    int refcnt;
    uid_t uid;
    gid_t gid;
    int mode;
    struct devfile *files;
    int reads;
    int writes;
    int input;
    int output;
    struct netif *netif;
    int (*receive)(struct netif *netif, pbuf * p);
};

struct netstats *netstats;

static void ne_readmem(struct ne *ne, unsigned short src, char *dst, unsigned short len) {
    // Word align length
    // i.e., if it's odd make it even, because we are sending words not bytes
    if (len & 1) len++;

    // Abort any remote DMA already in progress
    outportb(ne->nic_addr + NE_P0_CR, NE_CR_RD2 | NE_CR_STA);

    // Setup DMA byte count (set remote byte count)
    outportb(ne->nic_addr + NE_P0_RBCR0, (unsigned char) len);
    outportb(ne->nic_addr + NE_P0_RBCR1, (unsigned char) (len >> 8));

    // Setup NIC memory source address (set remote address)
    outportb(ne->nic_addr + NE_P0_RSAR0, (unsigned char) src);
    outportb(ne->nic_addr + NE_P0_RSAR1, (unsigned char) (src >> 8));

    // Select remote DMA read
    outportb(ne->nic_addr + NE_P0_CR, NE_CR_RD0 | NE_CR_STA);

    // Read NIC memory
    while (len > 0) {
        unsigned short d;
        d = inportw(ne->asic_addr + NE_NOVELL_DATA);
        *dst++ = d;
        *dst++ = d >> 8;
        len -= 2;
    }
}

/**
 * Reset the card
 */
static void ne_reset(struct ne *ne) {
    unsigned char byte;
    byte = inportb(ne->asic_addr + NE_NOVELL_RESET);
    outportb(ne->asic_addr + NE_NOVELL_RESET, byte);
    outportb(ne->nic_addr + NE_P0_CR, NE_CR_RD2 | NE_CR_STP);
}

static int ne_probe(struct ne *ne) {
    unsigned char byte;

    // Reset the ethernet card
    byte = inportb(ne->asic_addr + NE_NOVELL_RESET);
    outportb(ne->asic_addr + NE_NOVELL_RESET, byte);
    outportb(ne->nic_addr + NE_P0_CR, NE_CR_RD2 | NE_CR_STP);

    // msleep(100);
    // sleep(100);

    // Test for a generic DP8390 NIC
    byte = inportb(ne->nic_addr + NE_P0_CR);
    byte &= NE_CR_RD2 | NE_CR_TXP | NE_CR_STA | NE_CR_STP;
    if (byte != (NE_CR_RD2 | NE_CR_STP))
        return 0;

    byte = inportb(ne->nic_addr + NE_P0_ISR);
    byte &= NE_ISR_RST;
    if (byte != NE_ISR_RST)
        return 0;

    return 1;
}

void ne_get_packet(struct ne *ne, unsigned short src, char *dst, unsigned short len) {
    if (src + len > ne->rx_ring_end) {
        unsigned short split = ne->rx_ring_end - src;

        ne_readmem(ne, src, dst, split);
        len -= split;
        src = ne->rx_ring_start;
        dst += split;
    }
    ne_readmem(ne, src, dst, len);
}

void display_packet(void *payload, int size) {
    int i;
    kprintf("DST=%02x:%02x:%02x:%02x:%02x:%02x",
            *((unsigned char *) payload + 0),
            *((unsigned char *) payload + 1),
            *((unsigned char *) payload + 2),
            *((unsigned char *) payload + 3),
            *((unsigned char *) payload + 4),
            *((unsigned char *) payload + 5));
    kprintf(" SRC=%02x:%02x:%02x:%02x:%02x:%02x",
            *((unsigned char *) payload + 6),
            *((unsigned char *) payload + 7),
            *((unsigned char *) payload + 8),
            *((unsigned char *) payload + 9),
            *((unsigned char *) payload + 10),
            *((unsigned char *) payload + 11));
    kprintf(" TYPE=%02x:%02x\n",
            *((unsigned char *) payload + 12),
            *((unsigned char *) payload + 13));
    kprintf("DATA=");
    for (i = 14; i < size; i++)
        kprintf("%02x:", *((unsigned char *) payload + i));
    kprintf("\n");
}

void ne_receive(struct ne *ne) {
    struct recv_ring_desc packet_hdr;
    unsigned short packet_ptr;
    unsigned short len;
    unsigned char bndry;
    pbuf *p, *q;
    pbuf data_buffer;
    int rc;
    int i;
    unsigned short packet_header_length = (unsigned short) sizeof (struct recv_ring_desc);
    ARP __arReq;

    // Set page 1 registers
    outportb(ne->nic_addr + NE_P0_CR, NE_CR_PAGE_1 | NE_CR_RD2 | NE_CR_STA);

    kprintf("Receiving packet:0x%X ... 0x%X\n", inportb(ne->nic_addr + NE_P1_CURR), ne->next_pkt);

    if (ne->next_pkt != inportb(ne->nic_addr + NE_P1_CURR)) {
        // Get pointer to buffer header structure
        packet_ptr = ne->next_pkt * NE_PAGE_SIZE;
        // packet_ptr = inportb(ne->nic_addr + NE_P0_BNRY) * NE_PAGE_SIZE;

        // Read receive ring descriptor
        ne_readmem(ne, packet_ptr, &packet_hdr, packet_header_length);

        // Allocate packet buffer
        p = &data_buffer;
        p->next = NULL;
        p->len = packet_hdr.count - packet_header_length;
        p->tot_len = p->len;

        kprintf("header:0x%02X next:0x%02X len:%d\n", packet_hdr.rsr, packet_hdr.next_pkt, p->tot_len);

        if (is_arp_request(p->payload, p->len, &__arReq)) {
            kprintf("YES!!!!!!!!!!");
        } else {
            kprintf("NOOOO!!!!!!!!!!");
        }

        // Get packet from nic and send to upper layer
        packet_ptr += 4; // sizeof (struct recv_ring_desc);
        ne_readmem(ne, packet_ptr, p->payload, (unsigned short) p->len);
        // ne_get_packet(ne, packet_ptr, p->payload, (unsigned short) p->len);

        /*
        for (q = p; q != NULL; q = q->next) {
            ne_get_packet(ne, packet_ptr, q->payload, (unsigned short) q->len);
            packet_ptr += q->len;
        }
         */

        // show what we got
        display_packet(p->payload, p->len);

        kprintf(", %d bytes\n", packet_hdr.count);
        // rc = dev_receive(ne->devno, p);
        rc = 0;
        if (rc < 0) {
            kprintf("ne_receive: error %d processing packet\n", rc);
            // pbuf_free(p);
        }

        // Set page 0 registers
        outportb(ne->nic_addr + NE_P0_CR, NE_CR_PAGE_0 | NE_CR_RD2 | NE_CR_STA);

        // Update next packet pointer
        ne->next_pkt = packet_hdr.next_pkt;
        // Update boundary pointer
        bndry = ne->next_pkt - 1;
        if (bndry < ne->rx_page_start) {
            bndry = ne->rx_page_stop - 1;
        }

        // Release the buffer by increasing the boundary pointer.
        outportb(ne->nic_addr + NE_P0_BNRY, bndry);
        //outportb(ne->nic_addr + NE_P0_BNRY, packet_hdr.next_pkt);

        kprintf("start:0x%02x stop:0x%02x next:0x%02x bndry:0x%02x\n", ne->rx_page_start, ne->rx_page_stop, ne->next_pkt, bndry);

        // Set page 1 registers
        outportb(ne->nic_addr + NE_P0_CR, NE_CR_PAGE_1 | NE_CR_RD2 | NE_CR_STA);
    }
    ne_setup(__ne);
}

/*
 * Deferred Procedure Call?
 *
 */
void ne_dpc(void *arg) {

    struct ne *ne = arg;
    unsigned char isr;

    // kprintf("ne_dpc called (interrupt!)\n");

    // Select page 0
    outportb(ne->nic_addr + NE_P0_CR, NE_CR_RD2 | NE_CR_STA);

    // Loop until there are no pending interrupts
    while ((isr = inportb(ne->nic_addr + NE_P0_ISR)) != 0) {
        // isr = inportb(ne->nic_addr + NE_P0_ISR);

        kprintf("ne_dpc: interrupt! isr=%d.\n", isr);

        // Reset bits for interrupts being acknowledged
        outportb(ne->nic_addr + NE_P0_ISR, isr);

        // Packet received
        if (isr & NE_ISR_PRX) {
            kprintf(" New packet arrived.\n");
            ne_receive(ne);
        }

        // Packet transmitted
        if (isr & NE_ISR_PTX) {
            kprintf(" Packet transmitted.\n");
            // set_event(&ne->ptx);
        }

        // Remote DMA complete
        if (isr & NE_ISR_RDC) {
            kprintf(" Remote DMA complete.\n");
            // set_event(&ne->rdc);
        }

        // Select page 0
        outportb(ne->nic_addr + NE_P0_CR, NE_CR_RD2 | NE_CR_STA);
    }

    // kprintf("IRQ is %x; %d", ne->irq, ne->irq);

    // Signal end of interrupt to PIC
    outportb(ne->nic_addr + NE_P0_ISR, 0xFF);
}

int ne_transmit(pbuf * p) {
    // int ne_transmit(struct dev *dev, struct pbuf *p) {
    // struct ne *ne = dev->privdata;
    struct ne *ne = __ne;
    unsigned short dma_len;
    unsigned short dst;
    unsigned char *data;
    int len;
    int wrap;
    unsigned char save_byte[2];
    pbuf *q;
    int i;

    //    kprintf("ne_transmit: len=%d tot_len=%d\n", p->len, p->tot_len);
    // kprintf("ne_transmit: payload=%X\n", *((unsigned int *) p->payload));

    // We need to transfer a whole number of words (2-byte), so dma_len has to be even
    dma_len = p->tot_len;
    if (dma_len & 1) dma_len++;

    // Set page 0 registers
    // COMMAND register set to "start" and "nodma" (0x22)
    outportb(ne->nic_addr + NE_P0_CR, NE_CR_RD2 | NE_CR_STA);


    //clear pending interupts
    //    outportb(ne->nic_addr + NE_P0_ISR, 0xFF); // ISR


    // Reset remote DMA complete flag
    // "Remote DMA complete?" bit is cleared by writing a 1 in bit 6 of ISR (that's odd, but that's the way it works)
    outportb(ne->nic_addr + NE_P0_ISR, NE_ISR_RDC);

    // Set up DMA byte count
    // RBCRx are loaded with the packet size
    outportb(ne->nic_addr + NE_P0_RBCR0, (unsigned char) dma_len); // send low byte counter
    outportb(ne->nic_addr + NE_P0_RBCR1, (unsigned char) (dma_len >> 8)); // send high byte counter

    // RSARx are loaded with 0x00 (low) and target page number (high) respectively.
    // At this stage, the chip is ready receiving packet data and storing it in the ring buffer for emission.
    // Set up destination address in NIC memory
    dst = ne->rx_page_stop; // for now we only use one tx buffer
    // dst = NE_BUFFER_STOP_PAGE;
    // outportb(ne->nic_addr + NE_P0_RSAR0, (dst * NE_PAGE_SIZE));
    //  outportb(ne->nic_addr + NE_P0_RSAR1, (dst * NE_PAGE_SIZE) >> 8);
    outportb(ne->nic_addr + NE_P0_RSAR0, dst);
    outportb(ne->nic_addr + NE_P0_RSAR1, dst >> 8);

    // Set remote DMA write
    // COMMAND register set to "start" and "remote write DMA" (0x12)
    outportb(ne->nic_addr + NE_P0_CR, NE_CR_RD1 | NE_CR_STA);

    wrap = 0;

    // Packets data is now written to the "data port" (that is register 0x10)
    // of the NIC in a loop (or using an "outsx" if available).
    // The NIC will then update its remote DMA logic after each written word/dword
    // and places bytes in the transmit ring buffer.
    for (q = p; q != NULL; q = q->next) {
        len = q->len;
        if (len > 0) {
            data = q->payload;

            // Finish the last word
            if (wrap) {
                save_byte[1] = *data;
                // kprintf("last byte data=%x\n", *(unsigned short *) save_byte);
                outportw((unsigned short) (ne->asic_addr + NE_NOVELL_DATA), *(unsigned short *) save_byte);
                data++;
                len--;
                wrap = 0;
            }

            // Output contiguous words
            if (len > 1) {
                // I'm replacing this one:
                // outsw(ne->asic_addr + NE_NOVELL_DATA, data, len >> 1);

                // With this one:
                int len2 = len; //len >> 1;
                while (len2 > 0) {
                    /*
                    unsigned short d;
                    d = inportw(ne->asic_addr + NE_NOVELL_DATA);
                     *dst++ = d;
                     *dst++ = d >> 8;
                    // sleep(1);
                    len -= 2;
                     unsigned short d;
                     *dst++ = d;
                     *dst++ = d >> 8;
                     */
                    outportw((unsigned short) (ne->asic_addr + NE_NOVELL_DATA), *(unsigned short *) data);
                    sleep(1);
                    data++;
                    data++;
                    len2 -= 2;
                }

                // this was in the original code
                data += len & ~1;
                len &= 1; // will be 1 if the length is odd
            }

            // Save last byte if necessary
            if (len == 1) {
                save_byte[0] = *data;
                wrap = 1;
            }
        }
    }

    // Output last byte
    if (wrap) {
        outportw((unsigned short) (ne->asic_addr + NE_NOVELL_DATA), *(unsigned short *) save_byte);
    }

    // Wait for remote DMA complete
    // Poll ISR register until bit 6 (Remote DMA completed) is set.
    sleep(10);
    // unsigned char wait;
    // while (((wait = inportb(ne->nic_addr + NE_P0_ISR)) & NE_ISR_RDC) == 0);
    // outportb(ne->nic_addr + NE_P0_ISR, wait);


    // Set TX buffer start page
    // outportb(ne->nic_addr + NE_P0_TPSR, (unsigned char) dst);
    outportb(ne->nic_addr + NE_P0_TPSR, (unsigned char) NE_BUFFER_START_PAGE);

    // Set TX length (packets smaller than 64 bytes must be padded)
    if (p->tot_len > 64) {
        outportb(ne->nic_addr + NE_P0_TBCR0, p->tot_len);
        outportb(ne->nic_addr + NE_P0_TBCR1, p->tot_len >> 8);
    } else {
        outportb(ne->nic_addr + NE_P0_TBCR0, 64);
        outportb(ne->nic_addr + NE_P0_TBCR1, 0);
    }

    // Set page 0 registers, transmit packet, and start
    outportb(ne->nic_addr + NE_P0_CR, NE_CR_RD2 | NE_CR_TXP | NE_CR_STA);

    // Wait for packet transmitted
    sleep(1);
    /*
    if (wait_for_object(&ne->ptx, NE_TIMEOUT) < 0) {
        kprintf(KERN_WARNING "ne2000: timeout waiting for packet transmit\n");
        release_mutex(&ne->txlock);
        return -EIO;
    }
     */

    //    kprintf("ne_transmit: packet transmitted\n");
    // pbuf_free(p);
    // release_mutex(&ne->txlock);

    //Reset registers
    ne_setup(__ne);
    return 0;
}

int ne_setup(struct ne *ne) {
    // original signature:
    // int ne_setup(unsigned short iobase, int irq, unsigned short membase, unsigned short memsize, struct unit *unit) {

    // struct unit *unit;
    unsigned char romdata[16];
    int i;
    char str[20];

    // Setup NIC configuration
    ne->iobase = NE2K_IOBASE;
    ne->irq = NE2K_IRQ;
    ne->membase = 0;
    ne->memsize = NE_PAGE_SIZE;
    ne->nic_addr = ne->iobase + NE_NOVELL_NIC_OFFSET;
    ne->asic_addr = ne->iobase + NE_NOVELL_ASIC_OFFSET;
    // ne->rx_page_start = ne->membase / NE_PAGE_SIZE;
    ne->rx_page_start = NE_BUFFER_START_PAGE;
    // ne->rx_page_stop = ne->rx_page_start + (ne->memsize / NE_PAGE_SIZE) - NE_TXBUF_SIZE * NE_TX_BUFERS;
    ne->rx_page_stop = NE_BUFFER_STOP_PAGE;
    ne->next_pkt = NE_RX_PAGE_START; // 0x46 
    ne->rx_ring_start = ne->rx_page_start * NE_PAGE_SIZE;
    ne->rx_ring_end = ne->rx_page_stop * NE_PAGE_SIZE;

    // Probe for NE2000 card
    if (!ne_probe(ne)) {
        return 0;
    }

    // READ MAC !
    // Set page 0 registers, abort remote DMA, stop NIC
    outportb(ne->nic_addr + NE_P0_CR, NE_CR_RD2 | NE_CR_STP);
    // Set FIFO threshold to 8, auto-init remote DMA, byte order=80x86, word-wide DMA transfers
    outportb(ne->nic_addr + NE_P0_DCR, NE_DCR_FT1 | NE_DCR_LS | NE_DCR_AR | NE_DCR_WTS);
    ne_readmem(ne, 0, romdata, 16);
    for (i = 0; i < ETHER_ADDR_LEN; i++) {
        ne->hwaddr.addr[i] = romdata[i * 2];
    }

    // Set page 0 registers, abort remote DMA, stop NIC
    //    outportb(ne->nic_addr + NE_P0_CR, NE_CR_RD2 | NE_CR_STP);

    // Set FIFO threshold to 8, no auto-init remote DMA, byte order=80x86, word-wide DMA transfers
    //outportb(ne->nic_addr + NE_P0_DCR, NE_DCR_FT1 | NE_DCR_WTS | NE_DCR_LS);
    // Set FIFO threshold to 8, auto-init remote DMA, byte order=80x86, word-wide DMA transfers
    outportb(ne->nic_addr + NE_P0_DCR, NE_DCR_FT1 | NE_DCR_LS | NE_DCR_AR | NE_DCR_WTS);

    // Clear remote byte count registers
    outportb(ne->nic_addr + NE_P0_RBCR0, 0);
    outportb(ne->nic_addr + NE_P0_RBCR1, 0);

    // Accept broadcast packets
    outportb(ne->nic_addr + NE_P0_RCR, NE_RCR_AB);

    // transmit data register
    outportb(ne->nic_addr + 0x4, 0x20);

    // Take NIC out of loopback
    //outportb(ne->nic_addr + NE_P0_TCR, 0);
    outportb(ne->nic_addr + NE_P0_TCR, 2);

    // Initialize receiver (ring-buffer) page stop and boundary
    // outportb(ne->nic_addr + NE_P0_PSTART, ne->rx_page_start);
    outportb(ne->nic_addr + NE_P0_PSTART, NE_RX_PAGE_START); // 0x46
    // outportb(ne->nic_addr + NE_P0_BNRY, ne->rx_page_start);
    outportb(ne->nic_addr + NE_P0_BNRY, NE_RX_PAGE_BNDRY); // 0x46
    // outportb(ne->nic_addr + NE_P0_PSTOP, ne->rx_page_stop);
    outportb(ne->nic_addr + NE_P0_PSTOP, NE_RX_PAGE_STOP); // 0x80

    // Set page 1 registers
    outportb(ne->nic_addr + NE_P1_CR, NE_CR_PAGE_1 | NE_CR_RD2 | NE_CR_STP);

    // Set current page pointer
    outportb(ne->nic_addr + NE_P1_CURR, ne->next_pkt); // 0x46

    // Copy out our station address (PAR0..PAR5)
    for (i = 0; i < ETHER_ADDR_LEN; i++) {
        outportb(ne->nic_addr + NE_P1_PAR0 + i, ne->hwaddr.addr[i]);
    }

    // Start NIC
    outportb(ne->nic_addr + NE_P0_CR, NE_CR_RD2 | NE_CR_STA);

    // Set page 0 registers, abort remote DMA, stop NIC
    // outportb(ne->nic_addr + NE_P0_CR, NE_CR_RD2 | NE_CR_STP);

    // Enable the following interrupts: receive/transmit complete, receive/transmit error,
    // receiver overwrite and remote dma complete.
    // outportb(ne->nic_addr + NE_P0_IMR, NE_IMR_PRXE | NE_IMR_PTXE | NE_IMR_RXEE | NE_IMR_TXEE | NE_IMR_RDCE | NE_IMR_OVWE);

    /**
     * Programming registers of the NE2000 are collected in pages.
     * Page 0 contains most of the control and status registers while
     * page 1 contains physical (PAR0..PAR5)
     * and multicast addresses (MAR0..MAR7) to be checked by the card.
     * 
     */


    /*
    // Initialize multicast address hashing registers to not accept multicasts
    for (i = 0; i < 8; i++) {
        outportb(ne->nic_addr + NE_P1_MAR0 + i, 0);
        // outportb(ne->nic_addr + NE_P1_MAR0 + i, 0xFF);
    }
     * */

    // Set page 0 registers
    // outportb(ne->nic_addr + NE_P0_CR, NE_CR_RD2 | NE_CR_STP);

    // Clear any pending interrupts
    /**
     * ISR: Interrupt Status Register (07H; Type=R/W in Page0)
     * This register reflects the NIC status.
     * The host reads it to determine the cause of an interrupt.
     * Individual bits are cleared by writing a "1" into the corresponding bit. 
     * It must be cleared after power up. 
     */
    outportb(ne->nic_addr + NE_P0_ISR, NE_ISR_ALL);

    // #define BOB_IMR 0x1b (enable overflow interrupt)
    // Enable the following interrupts: receive/transmit complete, transmit error
    outportb(ne->nic_addr + NE_P0_IMR, NE_IMR_PRXE | NE_IMR_PTXE | NE_IMR_TXEE | NE_IMR_OVWE); // 0x1B

    outportb(ne->nic_addr + 0x0D, 0x00); // TMR

    // Create packet device
    // ne->devno = dev_make("eth#", &ne_driver, unit, ne);

    // kprintf(KERN_INFO "%s: NE2000 iobase 0x%x irq %d mac %s\n", device(ne->devno)->name, ne->iobase, ne->irq, ether2str(&ne->hwaddr, str));

    return 1;
}

void ne2k_driver_notifier(PROCESS self, PARAM param) {
    // NE2K_Message msg;
    while (1) {
        //        kprintf("Waiting for IRQ...\n");
        outportb(__ne->nic_addr + NE_P0_IMR, 0x1B);
        wait_for_interrupt(NE2K_IRQ);
        outportb(__ne->nic_addr + NE_P0_IMR, 0);
        //        kprintf("Got an IRQ!\n");
        ne_dpc(__ne);

    }

}

void ne2k_driver_process(PROCESS self, PARAM param) {
    NE2K_Driver_Message* msg;
    PROCESS sender_proc;

    PORT ne2k_driver_notifier_port;
    PROCESS ne2k_driver_notifier_proc;

    /*
    char          key_buffer;
    int           key_waiting;
    PROCESS       client_proc;
    Keyb_Message* client_msg;
     */

    ne2k_driver_notifier_port = create_process(ne2k_driver_notifier, 7, 0, "NE2K Driver Notifier");
    ne2k_driver_notifier_proc = ne2k_driver_notifier_port->owner;

    /*
    client_proc = NULL;
    client_msg = NULL;
    key_buffer = '\0';
    key_waiting = FALSE;
     */
    while (1) {
        msg = (NE2K_Driver_Message*) receive(&sender_proc);
        if (msg == NULL);
        reply(sender_proc);
    }
}

void ne_show_info(struct ne *ne) {
    kprintf("MAC_ADDR: %02x:%02x:%02x:%02x:%02x:%02x\n",
            ne->hwaddr.addr[0],
            ne->hwaddr.addr[1],
            ne->hwaddr.addr[2],
            ne->hwaddr.addr[3],
            ne->hwaddr.addr[4],
            ne->hwaddr.addr[5]);
}

void ne_send_data(struct eth_addr * dst_addr, void * data, int length) {
    pbuf p;
    p.next = NULL;
    p.payload = data;
    p.len = length + 12; // Length of this buffer.
    p.tot_len = length + 12; // Total length of buffer + additionally chained buffers.
    p.size = length + 12; // Allocated size of buffer
    ne_transmit(&p);
}

void ne_test_transmit() {
    if (initialized != 1) {
        kprintf("test failed: ne2k has not been initialized\n");
        return;
    }

    char __data__[] = {
        // destination
        // 0xD1, 0xE6, 0xAA, 0xDE, 0xAD, 0xBE,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

        // source
        __ne->hwaddr.addr[0], __ne->hwaddr.addr[1], __ne->hwaddr.addr[2],
        __ne->hwaddr.addr[3], __ne->hwaddr.addr[4], __ne->hwaddr.addr[5],

        // type (ARP)
        0x08, 0x06,

        // padding
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    pbuf p;
    p.next = NULL;
    p.payload = &__data__;
    p.len = 42; // Length of this buffer.
    p.tot_len = 42; //64;  // Total length of buffer + additionally chained buffers.
    p.size = (sizeof (unsigned int)); // Allocated size of buffer
    kprintf("ne_test: Trying to send a packet...\n");
    ne_transmit(&p);
}

/*-------------------------------------------------------------------*\
  init_ne2k_driver() - creates the ne2k_driver_process
\*-------------------------------------------------------------------*/
void init_ne2k_driver() {
    ne2k_driver_port = create_process(ne2k_driver_process, 6, 0, "NE2K Driver process");
    resign();
}

void init_ne2k_test() {
    int probe_result = 0;

    kprintf("ne_test: Initializing NE2000...\n");

    initialized = ne_setup(__ne);

    sleep(1);

    ne_show_info(__ne);

    probe_result = ne_probe(__ne);
    kprintf("ne_test: Probe result was: %d.", probe_result);
    if (probe_result == 1) {
        kprintf(" Successful!\n");
    } else {
        kprintf("Error :(\n");
    }

    unsigned char data[64] = {
        // (1) destination
        0x7A, 0xA4, 0x40, 0x64, 0xFC, 0x92,

        // (2) source MAC address
        __ne->hwaddr.addr[0], __ne->hwaddr.addr[1], __ne->hwaddr.addr[2],
        __ne->hwaddr.addr[3], __ne->hwaddr.addr[4], __ne->hwaddr.addr[5],

        // (3) packet type
        // ip packet: 0x08, 0x00,
        // arp packet: 0x08, 0x06,

        // (4) ???

        // frame length: 0x00, 0x00,

        // data
        // 0x00,

        // padding
        0xAA, 2, 3, 4, 5, 6, 7, 8, 9, 10,
        11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
        31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46,
        // crc
        0, 1, 2, 3
    };
    // Start NIC
    outportb(__ne->nic_addr + NE_P0_CR, NE_CR_RD2 | NE_CR_STA);
}
