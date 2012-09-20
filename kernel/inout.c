
#include <kernel.h>

unsigned char inportb (unsigned short port)
{
    unsigned char _v;

    asm ("inb %w1,%0" : "=a" (_v) : "Nd" (port));
    return _v;
}


void outportb (unsigned short port, unsigned char value)
{
    asm ("outb %b0,%w1" : : "a" (value), "Nd" (port));
}


unsigned short inportw (unsigned short port)
{
    unsigned short _v;

    asm ("inw %w1,%w0" : "=a" (_v) : "Nd" (port));
    return _v;
}
