//tosvga.h---------------------------------------------------------------------
//
// This code has been HEAVILY influenced by svgalib-1.4.3
// You may get a copy of svgalib at http://www.svgalib.org
//
// Authors: Berndt Jung, Ilias Konstantinidis
//-----------------------------------------------------------------------------

#ifndef __TOS_VGA__
#define __TOS_VGA__

//#define	peekb(S,O)		*(unsigned char *)(16uL * (S) + (O))
//#define	pokeb(S,O,V)		*(unsigned char *)(16uL * (S) + (O)) = (V)
//#define	pokew(S,O,V)		*(unsigned short *)(16uL * (S) + (O)) = (V)
//#define	_vmemwr(DS,DO,S,N)	memcpy((char *)((DS) * 16 + (DO)), S, N)

#define	VGA_AC_INDEX		0x3C0
#define	VGA_AC_WRITE		0x3C0
#define	VGA_AC_READ		0x3C1
#define	VGA_MISC_WRITE		0x3C2
#define VGA_SEQ_INDEX		0x3C4
#define VGA_SEQ_DATA		0x3C5
#define	VGA_DAC_READ_INDEX	0x3C7
#define	VGA_DAC_WRITE_INDEX	0x3C8
#define	VGA_DAC_DATA		0x3C9
#define	VGA_MISC_READ		0x3CC
#define VGA_GC_INDEX 		0x3CE
#define VGA_GC_DATA 		0x3CF
/*			COLOR emulation		MONO emulation */
#define VGA_CRTC_INDEX		0x3D4		/* 0x3B4 */
#define VGA_CRTC_DATA		0x3D5		/* 0x3B5 */
#define	VGA_INSTAT_READ		0x3DA

#define	VGA_NUM_SEQ_REGS	5
#define	VGA_NUM_CRTC_REGS	25
#define	VGA_NUM_GC_REGS		9
#define	VGA_NUM_AC_REGS		21
#define	VGA_NUM_REGS		(1 + VGA_NUM_SEQ_REGS + VGA_NUM_CRTC_REGS + \
							 VGA_NUM_GC_REGS + VGA_NUM_AC_REGS)


#define TEXT 	     0		/* Compatible with VGAlib v1.2 */
#define G320x200x16  1
#define G640x200x16  2
#define G640x350x16  3
#define G640x480x16  4
#define G320x200x256 5
#define G320x240x256 6
#define G320x400x256 7
#define G360x480x256 8
#define G640x480x2   9

#define BLACK		0x0
#define BLUE		0x1
#define GREEN		0x2
#define CYAN		0x3
#define RED		0x4
#define VIOLET		0x5
#define YELLOW		0x6
#define WHITE		0x7
#define GRAY		0x8
#define INTENSE_BLUE    0x9
#define INTENSE_GREEN	0xa
#define INTENSE_CYAN	0xb
#define INTENSE_RED	0xc
#define INTENSE_VIOLET	0xd
#define INTENSE_YELLOW	0xe
#define INTENSE_WHITE	0xf
#define NONE		0x10

extern unsigned char* VGA_BASE;

typedef unsigned char tos_color;

/** 
 * graphics mode information 
 */
typedef struct {
    unsigned width;
    unsigned height;
    unsigned colors;
    unsigned bytes_per_pixel;
    void (*write_pixel)(unsigned x, unsigned y, tos_color c);
    tos_color(*read_pixel_position)(unsigned x, unsigned y);
    tos_color(*read_pixel_offset)(unsigned offset);
    void (*write_segment)(unsigned offset, unsigned length, tos_color * colors);
    void (*read_segment)(unsigned offset, unsigned length, tos_color * colors);
    void (*fill_segment)(tos_color color, unsigned offset, unsigned length);
    void (*write_char)(unsigned char c, unsigned x, unsigned y, tos_color color);
} graphics;

graphics tos_graphics; //current graphics mode info

extern unsigned char clear_color;

unsigned get_fb_seg(void);

void vmemwr(unsigned dst_off, unsigned char *src, unsigned count);


/**
 * Poke a byte/short/word to the video memory
 */
void vpokeb(unsigned off, unsigned val);
void vpokes(unsigned int off, unsigned int val);
void vpokew(unsigned int off, unsigned int val);


/**
 * Peek a byte from the video memory
 */
unsigned vpeekb(unsigned off);


/**
 * Any initialization goes here (may just do all init in setmode)
 */
void init();

/**
 * Change graphics/text modes.  Mode is changed through writing to a series
 * of VGA registers.
 *
 * see: tosvga_regs.h
 */
void set_mode(unsigned mode);

void set_text_mode();

/**
 * Clear the screen
 */
void clear_graphics(void);

void scroll_graphics(int lines);

void set_clear_color(tos_color c);

/**
 * Draws one pixel to the screen using the current color
 */
void draw_pixel(unsigned x, unsigned y, tos_color c);

/**
 * Returns the color at x, y
 */
tos_color read_pixel(unsigned x, unsigned y);

/**
 * Draws length colors to a segment of the VGA starting at x, y
 */
void draw_scan_segment(unsigned offset, unsigned length,
        tos_color* color);

/**
 * Reads length colors to a segment of the VGA starting at x, y
 */
void read_scan_segment(unsigned offset, unsigned length,
        tos_color* colors);

void draw_fill_segment(unsigned offset, unsigned length,
        tos_color color);

void draw_char(unsigned char c, unsigned x, unsigned y, tos_color color);

/**
 * Get the current xresolution
 */
unsigned get_width();

/**
 * Get the current yresolution
 */
unsigned get_height();

/**
 * Set the color pallette at index
 */
void set_palette(int index, int red, int green, int blue);

/**
 * Get the color pallette at index
 */
void get_palette(int index, int *red, int *green, int *blue);

/**
 * Set num colors of the color pallette
 */
void set_palvec(int start, int num, int *pal);

/**
 * Set num colors of the color pallette
 */
void get_palvec(int start, int num, int *pal);

#endif
