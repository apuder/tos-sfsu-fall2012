#ifndef __KERNELINIT__
#define __KERNELINIT__

typedef struct regs {

    union {

        struct {
            unsigned long edi; //This are in order for PUSHAD
            unsigned long esi;
            unsigned long ebp;
            unsigned long esp;
            unsigned long ebx;
            unsigned long edx;
            unsigned long ecx;
            unsigned long eax;
        };

        struct {
            unsigned short di;
            unsigned short di_highword;
            unsigned short si;
            unsigned short si_highword;
            unsigned short bp;
            unsigned short bp_highword;
            unsigned short sp;
            unsigned short sp_highword;
            unsigned short bx;
            unsigned short bx_highword;
            unsigned short dx;
            unsigned short dx_highword;
            unsigned short cx;
            unsigned short cx_highword;
            unsigned short ax;
            unsigned short ax_highword;
        };

        struct {
            unsigned char di_lowbyte;
            unsigned char di_highbyte;
            unsigned char di_e_lowbyte;
            unsigned char di_e_highbyte;

            unsigned char si_lowbyte;
            unsigned char si_highbyte;
            unsigned char si_e_lowbyte;
            unsigned char si_e_highbyte;

            unsigned char bp_lowbyte;
            unsigned char bp_highbyte;
            unsigned char bp_e_lowbyte;
            unsigned char bp_e_highbyte;

            unsigned char sp_lowbyte;
            unsigned char sp_highbyte;
            unsigned char sp_e_lowbyte;
            unsigned char sp_e_highbyte;

            unsigned char bl;
            unsigned char bh;
            unsigned char bx_e_lowbyte;
            unsigned char bx_e_highbyte;

            unsigned char dl;
            unsigned char dh;
            unsigned char dx_e_lowbyte;
            unsigned char dx_e_highbyte;

            unsigned char cl;
            unsigned char ch;
            unsigned char cx_e_lowbyte;
            unsigned char cx_e_highbyte;

            unsigned char al;
            unsigned char ah;
            unsigned char ax_e_lowbyte;
            unsigned char ax_e_highbyte;
        };
    };
} regs, * reg_pointer;


extern unsigned int stack_base;

void printSMAP(); //Print the System Memory Map

// guaranteed PEEK/POKE functions to write into the video memory space
void videoPokeByte(void * p, unsigned char val);
void videoPokeWord(void * p, unsigned short val);
void videoPokeLong(void * p, unsigned long val);
unsigned char videoPeekByte(unsigned char * p);
unsigned short videoPeekWord(unsigned short * p);
unsigned long videoPeekLong(unsigned long * p);

// copy from data source to video memory
void videoMemCpyData(unsigned char * dst_off, unsigned char *src, unsigned long count);

// selector Functions
unsigned long getCSselector();
unsigned long getDSselector();
unsigned long getESselector();
unsigned long getFSselector();
unsigned long getGSselector();
unsigned long getBase0selector();

// void * getPhysicalAddress(unsigned long selector, void * offset);
// void clrPageBitMap(unsigned long startaddress, unsigned long addresslen);

// void kernel_main();

#endif
