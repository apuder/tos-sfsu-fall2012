#include <kernel.h>
//#include <paging.h>
//#include <string.h>

// We define paging enabled to 0 or 1, if 0 we have to use FS overrides
// on the video calls, otherwise if paging is on, we map the video

#define	PAGING_ENABLED	0

unsigned int stack_base;
unsigned long pmode;
unsigned long rmode;
unsigned long memSize;
unsigned long base0selector;

//Start of temporary directory code

/*
struct biosParameterBlockPacked {
    unsigned char jumpInstruction[3] __attribute__((__packed__));
    char oemName[8] __attribute__((__packed__));
    short bytesPerSector __attribute__((__packed__));
    unsigned char sectorsPerCluster __attribute__((__packed__));
    short reservedSectors __attribute__((__packed__));
    unsigned char numberOfFats __attribute__((__packed__));
    short rootEntries __attribute__((__packed__));
    short smallSectors __attribute__((__packed__));
    unsigned char mediaDescriptor __attribute__((__packed__));
    short sectorsPerFAT __attribute__((__packed__));
    short sectorsPerTrack __attribute__((__packed__));
    short heads __attribute__((__packed__));
    long hiddenSectors __attribute__((__packed__));
    long largeSectors __attribute__((__packed__));
    //start FAT 16 elements
    unsigned char physicalDriveNumber __attribute__((__packed__));
    unsigned char currentHead __attribute__((__packed__));
    unsigned char signature __attribute__((__packed__));
    unsigned long volumeID __attribute__((__packed__));
    char volumeLabel[11] __attribute__((__packed__));
    char fileSystemType[8] __attribute__((__packed__));
} __attribute__((__packed__));

typedef struct biosParameterBlockPacked biosParameterBlock;

typedef struct dirTime {
    unsigned short twoSecs : 5;
    unsigned short minutes : 6;
    unsigned short hours : 5;
} dirTime;

typedef struct dirDate {
    unsigned short day : 5;
    unsigned short month : 4;
    unsigned short year : 7;
} dirDate;

typedef struct dirEntry {
    char name[8] __attribute__((__packed__));
    char ext[3] __attribute__((__packed__));
    unsigned char attr __attribute__((__packed__));
    char reserved[10] __attribute__((__packed__));
    dirTime time __attribute__((__packed__));
    dirDate date __attribute__((__packed__));
    short firstCluster __attribute__((__packed__));
    long size __attribute__((__packed__));
} dirEntry;

#define DIRATTR_READONLY	0x01
#define DIRATTR_HIDDEN		0x02
#define DIRATTR_SYSTEM		0x04
#define DIRATTR_VOLUME		0x08
#define DIRATTR_SUBDIR		0x10
#define DIRATTR_ARCHIVE		0x20

unsigned long getCSselector() {
    unsigned long ret;
    asm("movl	%cs, %eax");
    asm("movl %%eax,%0" : "=m" (ret) :);
    return (ret);
}

unsigned long getDSselector() {
    unsigned long ret;
    asm("movl	%ds, %eax");
    asm("movl %%eax,%0" : "=m" (ret) :);
    return (ret);
}

unsigned long getESselector() {
    unsigned long ret;
    asm("movl	%es, %eax");
    asm("movl %%eax,%0" : "=m" (ret) :);
    return (ret);
}

unsigned long getFSselector() {
    unsigned long ret;
    asm("movl	%fs, %eax");
    asm("movl %%eax,%0" : "=m" (ret) :);
    return (ret);
}

unsigned long getGSselector() {
    unsigned long ret;
    asm("movl	%gs, %eax");
    asm("movl %%eax,%0" : "=m" (ret) :);
    return (ret);
}

unsigned long getBase0selector() {
    return (base0selector);
}

// This code converts the address specified for the current process into 
// An actual physical address.

void * getPhysicalAddress(unsigned long selector, void * offset) {
#if (PAGING_ENABLED == 1)
    return (offset + 0x100000); //temporary
#else
    return (offset + 0x100000); //temporary
#endif
}

/* 
 * Guaranteed video peek/poke functions.  Given a "physical" pointer to 
 * video memory, these functions will correctly map to actual video memory.
 */
unsigned char videoPeekByte(unsigned char * p) {
#if (PAGING_ENABLED == 1)
    return (*p);
#else
    asm("pushl %edi");
    asm("movl 8(%esp), %edi");
    asm("xorl %eax, %eax");
    asm("movb %fs:(%edi),%al");
    asm("popl %edi");
#endif
}

unsigned short videoPeekWord(unsigned short * p) {
#if (PAGING_ENABLED == 1)
    return (*p);
#else
    asm("pushl %edi");
    asm("movl 8(%esp), %edi");
    asm("xorl %eax, %eax");
    asm("movw %fs:(%edi),%ax");
    asm("popl %edi");
#endif
}

unsigned long videoPeekLong(unsigned long * p) {
#if (PAGING_ENABLED == 1)
    return (*p);
#else
    asm("pushl %edi");
    asm("movl 8(%esp), %edi");
    asm("xorl %eax, %eax");
    asm("movl %fs:(%edi),%eax");
    asm("popl %edi");
#endif
}

void videoPokeByte(void * p, unsigned char val) {
#if (PAGING_ENABLED == 1)
    *((unsigned char *) p) = val;
#else
    asm("pushl %edi");
    asm("movl 8(%esp), %edi");
    asm("movb 12(%esp), %al");
    asm("movb %al,%fs:(%edi)");
    asm("popl %edi");
#endif
}

void videoPokeWord(void * p, unsigned short val) {
#if (PAGING_ENABLED == 1)
    *((unsigned short *) p) = val;
#else
    asm("pushl %edi");
    asm("movl 8(%esp), %edi");
    asm("movw 12(%esp), %ax");
    asm("movw %ax,%fs:(%edi)");
    asm("popl %edi");
#endif
}

void videoPokeLong(void * p, unsigned long val) {
#if (PAGING_ENABLED == 1)
    *((unsigned long *) p) = val;
#else
    asm("pushl %edi");
    asm("movl 8(%esp), %edi");
    asm("movl 12(%esp), %eax");
    asm("movl %eax,%fs:(%edi)");
    asm("popl %edi");
#endif
}

void videoMemCpyData(unsigned char * dst_off, unsigned char *src, unsigned long count) {
#if (PAGING_ENABLED == 1)
    memcpy(dst_off, src, count);
#else
    asm("pushl %edi");
    asm("pushl %esi");
    asm("pushl %ecx");
    asm("movl 24(%esp), %ecx"); //get length
    asm("movl 20(%esp), %esi"); //get source
    asm("movl 16(%esp), %edi"); //get destination
    asm("pushl %es");
    asm("pushl %fs");
    asm("popl %es");
    asm("cld");
    asm("rep movsb");
    asm("popl %es");
    asm("popl %ecx");
    asm("popl %esi");
    asm("popl %edi");
#endif
}

/*
        parsemem, getmemcount are access procedures for getting the memory map
        They are used in printSMAP and will also in setPageMap.
        The address specified in getmemount is a physical address set in the boot
        loader and should not be changed unles changed there also.
 * /

// This is a specific structure for reading the memory SMAP, we only are
// concerned with the lowbase and lowlen becuase we are a 32 bit system
// with a max of 4GB of ram.

typedef struct mementry {
    long lowbase;
    long highbase;
    long lowlen;
    long highlen;
    long type2;
} mementry, *pmementry;

#define BITMAPADDRESS	0xE0000

void parsemem(pmementry memout, pmementry memin) {
    asm("pushl	%edi");
    asm("pushl	%esi");
    asm("movl	16(%esp),%edi");
    asm("movl	12(%esp),%esi");

    asm("movl	%fs:(%edi),%eax");
    asm("movl	%eax,(%esi)");
    asm("movl	%fs:4(%edi),%eax");
    asm("movl	%eax,4(%esi)");
    asm("movl	%fs:8(%edi),%eax");
    asm("movl	%eax,8(%esi)");
    asm("movl	%fs:12(%edi),%eax");
    asm("movl	%eax,12(%esi)");
    asm("movl	%fs:16(%edi),%eax");
    asm("movl	%eax,16(%esi)");

    asm("movl	%edi,%eax");
    asm("addl	$20,%eax");

    asm("popl	%esi");
    asm("popl	%edi");
}

short getmemcount() {
    asm("pushl	%edi");
    asm("movl	$0x00002000,%edi");
    asm("movw	%fs:(%edi), %ax");
    asm("popl	%edi");
}

// Bits are stored byte 0 - bit 0 = page 0, byte 0 bit 1 = page 1
// byte 1 - bit 0 = page 8

void setPageBitMap(unsigned long startaddress, unsigned long addresslen) {
    unsigned char * ptr;
    int i, bitToSet, idx, cnt;

    ptr = (unsigned char *) BITMAPADDRESS;

    startaddress = startaddress / 4096; //starting bit number
    addresslen = addresslen / 4096; //number of bits to set

    // optimized code - set the bits not on a 8 bit boundry at the start
    // then set full bytes as can be by the length
    // then set the trailing (remaining bits)

    idx = (startaddress / 8) - 1;
    ptr = ptr + idx;
    bitToSet = startaddress & 0x00000007;

    // There are more optimal ways of setting this, but this makes it easier to
    // read - we want to set bits starting from 1 --> 7 and limited by len if
    // that is neessary
    // this loop is complex from the point that both the starting bit position
    // and the count of bits are variable.

    cnt = 8 - bitToSet; // this would be how many bits to set

    if (cnt > addresslen)
        cnt = addresslen; //reduce the count to limit by addresslen 

    for (i = 0; i < cnt; i++) {
        switch (bitToSet) {
            case 1:
 *ptr = *ptr | 0x02;
                break;

            case 2:
 *ptr = *ptr | 0x04;
                break;

            case 3:
 *ptr = *ptr | 0x08;
                break;

            case 4:
 *ptr = *ptr | 0x10;
                break;

            case 5:
 *ptr = *ptr | 0x20;
                break;

            case 6:
 *ptr = *ptr | 0x40;
                break;

            case 7:
 *ptr = *ptr | 0x80;
                break;

            default:
                break;
        }
        ++bitToSet; //move to the next bit
    }

    addresslen = addresslen - cnt;

    ++ptr;

    for (i = 0; i < (addresslen / 8); i++) {
 *ptr = 0xFF;
        ++ptr;
    }

    // setting reamining pages	
    // There are more optimal ways of setting this, but this makes it easier to 
    // read - this loop is easier becuase the starting bit position is always 0
    switch (addresslen % 8) {
        case 1:
 *ptr = *ptr | 0x01;
            break;

        case 2:
 *ptr = *ptr | 0x03;
            break;

        case 3:
 *ptr = *ptr | 0x07;
            break;

        case 4:
 *ptr = *ptr | 0x0F;
            break;

        case 5:
 *ptr = *ptr | 0x1F;
            break;

        case 6:
 *ptr = *ptr | 0x3F;
            break;

        case 7:
 *ptr = *ptr | 0x7F;
            break;

        default:
            break;
    }
}

// Bits are stored byte 0 - bit 0 = page 0, byte 0 bit 1 = page 1
// byte 1 - bit 0 = page 8

void clrPageBitMap(unsigned long startaddress, unsigned long addresslen) {
    unsigned char * ptr;
    int i, bitToSet, idx, cnt;

    ptr = (unsigned char *) BITMAPADDRESS;

    startaddress = startaddress / 4096; //starting bit number
    addresslen = addresslen / 4096; //number of bits to clr

    // optimized code - set the bits not on a 8 bit boundry at the start
    // then set full bytes as can be by the length
    // then set the trailing (remaining bits)

    idx = (startaddress / 8);
    ptr = ptr + idx;
    bitToSet = startaddress & 0x00000007;

    // There are more optimal ways of setting this, but this makes it easier to
    // read - we want to set bits starting from 1 --> 7 and limited by len if
    // that is neessary
    // this loop is complex from the point that both the starting bit position
    // and the count of bits are variable.

    cnt = 8 - bitToSet; // this would be how many bits to clr

    if (cnt > addresslen)
        cnt = addresslen; //reduce the count to limit by addresslen 

    for (i = 0; i < cnt; i++) {
        switch (bitToSet) {
            case 0:
 *ptr = *ptr & 0xFE; // 1111 1110
                break;

            case 1:
 *ptr = *ptr & 0xFD; // 1111 1101
                break;

            case 2:
 *ptr = *ptr & 0xFB; //  1111 1011
                break;

            case 3:
 *ptr = *ptr & 0xF7; //  1111 0111
                break;

            case 4:
 *ptr = *ptr & 0xEF; //   1110 1111
                break;

            case 5:
 *ptr = *ptr & 0xDF; //   1101 1111
                break;

            case 6:
 *ptr = *ptr & 0xBF; //   1011 1111
                break;

            case 7:
 *ptr = *ptr & 0x7F; //   0111 1111
                break;

            default:
                break;
        }
        ++bitToSet; //move to the next bit
    }

    addresslen = addresslen - cnt;

    ++ptr;

    for (i = 0; i < (addresslen / 8); i++) {
 *ptr = 0x00;
        ++ptr;
    }

    // setting reamining pages	
    // There are more optimal ways of setting this, but this makes it easier to 
    // read - this loop is easier becuase the starting bit position is always 0
    switch (addresslen % 8) {
        case 1:
 *ptr = *ptr & 0xFE;
            break;

        case 2:
 *ptr = *ptr & 0xFC;
            break;

        case 3:
 *ptr = *ptr & 0xF8;
            break;

        case 4:
 *ptr = *ptr & 0xF0;
            break;

        case 5:
 *ptr = *ptr & 0xE0;
            break;

        case 6:
 *ptr = *ptr & 0xC0;
            break;

        case 7:
 *ptr = *ptr & 0x80;
            break;

        default:
            break;
    }
}

/*
        setPageMap sets a 128K bitmap of what 4K pages are available
        We use memory at logical E0000 (physical 1E0000) 
 * /

void setPageMap() {
    mementry me;
    pmementry pme;
    int i, cnt;
    unsigned long startaddress, addresslen;

    // First set map to physical memory layout

    // Clear the entire map to unavailable
    memset((char *) BITMAPADDRESS, 0, 0x20000);

    cnt = getmemcount();
    me.lowbase = 0;
    pme = (pmementry) 0x00002002;

    for (i = 1; i <= cnt; i++) //loop through SMAP blocks
    {
        parsemem(&me, pme);
        ++pme;

        //only type of 1 is available, all others are reserved.
        //AAAAAAAA is a special safety marker in the boot loader

        if ((me.type2 == 1) && (me.lowbase != 0xAAAAAAAA)) {
            //If base address is not a 4K block, round up and decrement the count
            //by that amount, then check the length and round down
            //i.e. all fractional pages are mapped out (used/reserved)

            startaddress = (unsigned long) me.lowbase;
            addresslen = (unsigned long) me.lowlen;

            if ((startaddress & 0xFFFFF000) != startaddress) {
                addresslen = addresslen - (startaddress & 0x00000FFF);
                startaddress = (startaddress & 0xFFFFF000) + 0x1000;
            }

            addresslen = addresslen & 0xFFFFF000; //ignore fractional pages

            setPageBitMap(startaddress, addresslen);
        }
    }

    // Now mapout used or reserved pages (map out video & bios even if already 
    // mapped out
    clrPageBitMap(0, 0x200000); //Map out entire first 2MB (TOS memory)
    clrPageBitMap(0x1E00000, 0x168000); //Map out RAM DISK

}

void printSMAP() {
    mementry me;
    pmementry pme;
    int i, cnt;

    cnt = getmemcount();
    me.lowbase = 0;
    pme = (pmementry) 0x00002002;
    for (i = 1; i <= cnt; i++) {
        parsemem(&me, pme);
        ++pme;
        if (me.lowbase != 0xAAAAAAAA)
            kprintf("%d:  base: %x   len: %x   type: %d\n", i, me.lowbase, me.lowlen, me.type2);
        else
            kprintf("\n\n");
    }
}
 * 
 */