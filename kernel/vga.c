#include <kernel.h>
#include <font.h>
#include <vga.h>
#include <vga_regs.h>

unsigned char* VGA_BASE = (unsigned char*) 0xA0000;
unsigned char clear_color = 0x0;

/*************** Function prototypes *******************/
void v_write_8(tos_color color, unsigned char mask, unsigned offset);
tos_color read_pixel_position4p(unsigned x, unsigned y);
tos_color read_pixel_offset4p(unsigned offset);
void read_scan_segment4p(unsigned offset, unsigned length, tos_color* colors);
void write_segment4p(unsigned offset, unsigned length, tos_color* colors);
void fill_segment4p(tos_color c, unsigned length, unsigned offset);
void write_pixel4p(unsigned x, unsigned y, tos_color c);

/*************** FUNCTIONS FOR vga_modes ***************/

#if 0

static void dump(unsigned char *regs, unsigned count) {
    unsigned i;

    i = 0;
    kprintf(" ");
    for (; count != 0; count--) {
        kprintf("0x%02X,", *regs);
        i++;
        if (i >= 8) {
            i = 0;
            kprintf("\n");
        } else
            kprintf(" ");
        regs++;
    }
    kprintf("\n");
}

void dump_regs(unsigned char *regs) {
    kprintf("unsigned char g_mode[] =\n");
    kprintf("{\n");
    /* dump MISCELLANEOUS reg */
    kprintf("/* MISC */\n");
    kprintf("\t0x%02X,\n", *regs);
    regs++;
    /* dump SEQUENCER regs */
    kprintf("/* SEQ */\n");
    dump(regs, VGA_NUM_SEQ_REGS);
    regs += VGA_NUM_SEQ_REGS;
    /* dump CRTC regs */
    kprintf("/* CRTC */\n");
    dump(regs, VGA_NUM_CRTC_REGS);
    regs += VGA_NUM_CRTC_REGS;
    /* dump GRAPHICS CONTROLLER regs */
    kprintf("/* GC */\n");
    dump(regs, VGA_NUM_GC_REGS);
    regs += VGA_NUM_GC_REGS;
    /* dump ATTRIBUTE CONTROLLER regs */
    kprintf("/* AC */\n");
    dump(regs, VGA_NUM_AC_REGS);
    regs += VGA_NUM_AC_REGS;
    kprintf("};\n");
}

void read_regs(unsigned char *regs) {
    unsigned i;

    /* read MISCELLANEOUS reg */
    *regs = inportb(VGA_MISC_READ);
    regs++;
    /* read SEQUENCER regs */
    for (i = 0; i < VGA_NUM_SEQ_REGS; i++) {
        outportb(VGA_SEQ_INDEX, i);
        *regs = inportb(VGA_SEQ_DATA);
        regs++;
    }
    /* read CRTC regs */
    for (i = 0; i < VGA_NUM_CRTC_REGS; i++) {
        outportb(VGA_CRTC_INDEX, i);
        *regs = inportb(VGA_CRTC_DATA);
        regs++;
    }
    /* read GRAPHICS CONTROLLER regs */
    for (i = 0; i < VGA_NUM_GC_REGS; i++) {
        outportb(VGA_GC_INDEX, i);
        *regs = inportb(VGA_GC_DATA);
        regs++;
    }
    /* read ATTRIBUTE CONTROLLER regs */
    for (i = 0; i < VGA_NUM_AC_REGS; i++) {
        (void) inportb(VGA_INSTAT_READ);
        outportb(VGA_AC_INDEX, i);
        *regs = inportb(VGA_AC_READ);
        regs++;
    }
    /* lock 16-color palette and unblank display */
    (void) inportb(VGA_INSTAT_READ);
    outportb(VGA_AC_INDEX, 0x20);
}
#endif

void write_regs(unsigned char *regs) {
    unsigned i;

    /* write MISCELLANEOUS reg */
    outportb(VGA_MISC_WRITE, *regs);
    regs++;
    /* write SEQUENCER regs */
    for (i = 0; i < VGA_NUM_SEQ_REGS; i++) {
        outportb(VGA_SEQ_INDEX, i);
        outportb(VGA_SEQ_DATA, *regs);
        regs++;
    }
    /* unlock CRTC registers */
    outportb(VGA_CRTC_INDEX, 0x03);
    outportb(VGA_CRTC_DATA, inportb(VGA_CRTC_DATA) | 0x80);
    outportb(VGA_CRTC_INDEX, 0x11);
    outportb(VGA_CRTC_DATA, inportb(VGA_CRTC_DATA) & ~0x80);
    /* make sure they remain unlocked */
    regs[0x03] |= 0x80;
    regs[0x11] &= ~0x80;
    /* write CRTC regs */
    for (i = 0; i < VGA_NUM_CRTC_REGS; i++) {
        outportb(VGA_CRTC_INDEX, i);
        outportb(VGA_CRTC_DATA, *regs);
        regs++;
    }
    /* write GRAPHICS CONTROLLER regs */
    for (i = 0; i < VGA_NUM_GC_REGS; i++) {
        outportb(VGA_GC_INDEX, i);
        outportb(VGA_GC_DATA, *regs);
        regs++;
    }
    /* write ATTRIBUTE CONTROLLER regs */
    for (i = 0; i < VGA_NUM_AC_REGS; i++) {
        (void) inportb(VGA_INSTAT_READ);
        outportb(VGA_AC_INDEX, i);
        outportb(VGA_AC_WRITE, *regs);
        regs++;
    }
    /* lock 16-color palette and unblank display */
    (void) inportb(VGA_INSTAT_READ);
    outportb(VGA_AC_INDEX, 0x20);
}

#if 0

void set_plane(unsigned p) {
    unsigned char pmask;

    p &= 3;
    pmask = 1 << p;
    /* set read plane */
    outportb(VGA_GC_INDEX, 4);
    outportb(VGA_GC_DATA, p);
    /* set write plane */
    outportb(VGA_SEQ_INDEX, 2);
    outportb(VGA_SEQ_DATA, 0x0f);
}
#endif

/**
 * VGA framebuffer is at A000:0000, B000:0000, or B800:0000
 * depending on bits in GC 6
 */
unsigned get_fb_seg(void) {
    unsigned seg;

    outportb(VGA_GC_INDEX, 6);
    seg = inportb(VGA_GC_DATA);
    seg >>= 2;
    seg &= 3;
    switch (seg) {
        case 0:
        case 1:
            seg = 0xA000;
            break;
        case 2:
            seg = 0xB000;
            break;
        case 3:
            seg = 0xB800;
            break;
    }
    return seg;
}

#if 0

void vmemwr(unsigned dst_off, unsigned char *src, unsigned count) {
    videoMemCpyData((unsigned char *) ((get_fb_seg() * 16) + dst_off), src, count);
}

#endif

/**
 * Poke a byte to the video memory
 */
void vpokeb(unsigned int off, unsigned int val) {
    unsigned char* base = VGA_BASE;
    videoPokeByte((base + off), val);
}

#if 0

void vpokes(unsigned int off, unsigned int val) {
    unsigned char* base = VGA_BASE;
    videoPokeWord((base + off), val);
}

#endif

void vpokew(unsigned int off, unsigned int val) {
    unsigned int* base = (unsigned int*) VGA_BASE;
    videoPokeLong((base + off), val);
}

/**
 * Peek a byte from the video memory
 */
unsigned int vpeekb(unsigned int off) {
    return (videoPeekByte((unsigned char *) (16uL * get_fb_seg() + off)));
}

#if 0

/**
 * write font to plane P4 (assuming planes are named P1, P2, P4, P8)
 */
void write_font(unsigned char *buf, unsigned font_height) {
    unsigned char seq2, seq4, gc4, gc5, gc6;
    unsigned i;

    /* save registers
            set_plane() modifies GC 4 and SEQ 2, so save them as well */
    outportb(VGA_SEQ_INDEX, 2);
    seq2 = inportb(VGA_SEQ_DATA);

    outportb(VGA_SEQ_INDEX, 4);
    seq4 = inportb(VGA_SEQ_DATA);
    /* turn off even-odd addressing (set flat addressing)
    assume: chain-4 addressing already off */
    outportb(VGA_SEQ_DATA, seq4 | 0x04);

    outportb(VGA_GC_INDEX, 4);
    gc4 = inportb(VGA_GC_DATA);

    outportb(VGA_GC_INDEX, 5);
    gc5 = inportb(VGA_GC_DATA);
    /* turn off even-odd addressing */
    outportb(VGA_GC_DATA, gc5 & ~0x10);

    outportb(VGA_GC_INDEX, 6);
    gc6 = inportb(VGA_GC_DATA);
    /* turn off even-odd addressing */
    outportb(VGA_GC_DATA, gc6 & ~0x02);
    /* write font to plane P4 */
    set_plane(2);
    /* write font 0 */
    for (i = 0; i < 256; i++) {
        vmemwr(16384u * 0 + i * 32, buf, font_height);
        buf += font_height;
    }
#if 0
    /* write font 1 */
    for (i = 0; i < 256; i++) {
        vmemwr(16384u * 1 + i * 32, buf, font_height);
        buf += font_height;
    }
#endif
    /* restore registers */
    outportb(VGA_SEQ_INDEX, 2);
    outportb(VGA_SEQ_DATA, seq2);
    outportb(VGA_SEQ_INDEX, 4);
    outportb(VGA_SEQ_DATA, seq4);
    outportb(VGA_GC_INDEX, 4);
    outportb(VGA_GC_DATA, gc4);
    outportb(VGA_GC_INDEX, 5);
    outportb(VGA_GC_DATA, gc5);
    outportb(VGA_GC_INDEX, 6);
    outportb(VGA_GC_DATA, gc6);
}

/**
 * Returns the color at x, y
 */
tos_color read_pixel_position4p(unsigned x, unsigned y) {
    return read_pixel_offset4p((y * tos_graphics.width) + x);
}

/**
 * Returns the color at offset
 */
tos_color read_pixel_offset4p(unsigned offset) {
    unsigned off, i;
    unsigned char state;
    tos_color c, color = 0;

    off = offset >> 3;

    outportb(VGA_GC_INDEX, 5); //change read mode
    state = inportb(VGA_GC_DATA);
    outportb(VGA_GC_DATA, state & 0xf7);
    for (i = 0; i < 4; i++) {
        c = 0;
        outportb(VGA_GC_INDEX, 4);
        outportb(VGA_GC_DATA, i);
        c = vpeekb(off) << (3 - i);
        color |= c;
    }
    outportb(VGA_GC_INDEX, 5); //change read mode back
    outportb(VGA_GC_DATA, state);
    return color;
}

void read_scan_segment4p(
        unsigned offset, unsigned length, tos_color* colors) {
    int i;
    for (i = 0; i < length; i++) {
        *(colors++) = read_pixel_offset4p(offset);
    }
}

/**
 * Draws length colors to a segment of the VGA starting at x, y
 */
void write_segment4p(unsigned offset, unsigned length, tos_color* colors) {
    unsigned int off, i, j;

    length >>= 3;
    off = offset >> 3;

    for (i = off; i < length + off; i++) {
        for (j = 0; j < 8; j++) {
            v_write_8(*(colors++), 0x80 >> j, i);
        }
    }
}
#endif 

void v_write_8(tos_color color, unsigned char mask, unsigned int offset) {
    outportb(VGA_GC_INDEX, 0);
    outportb(VGA_GC_DATA, color);

    //bit mask
    outportb(VGA_GC_INDEX, 8);
    outportb(VGA_GC_DATA, mask);

    tos_color temp = vpeekb(offset);
    vpokeb(offset, 0x1 | temp);

}

void write_pixel4p(unsigned int x, unsigned int y, tos_color c) {
    unsigned wd_in_bytes, off;
    tos_color color = c & 0xff;
    if (x < 0 || x >= tos_graphics.width ||
            y < 0 || y >= tos_graphics.height) {
        return;
    }
    //calculate the offset
    wd_in_bytes = tos_graphics.width >> 3;
    off = wd_in_bytes * y + (x >> 3);
    v_write_8(color, 0x80 >> (x % 8), off);
}

#if 0

void write_char_8x8(
        unsigned char c, unsigned int x, unsigned int y, tos_color color) {
    unsigned wd_in_bytes, off, i, over;
    tos_color temp;

    //calculate the offset
    wd_in_bytes = tos_graphics.width >> 3;
    off = wd_in_bytes * y + (x >> 3);
    over = x % 8;

    for (i = 0; i < 8; i++) {
        temp = g_8x8_font[(c << 3) + i] >> over;
        v_write_8(color, temp, off + (wd_in_bytes * i));
    }
    if (over) {
        off++;
        for (i = 0; i < 8; i++) {
            temp = g_8x8_font[(c << 3) + i] << (8 - over);
            v_write_8(color, temp, off + (wd_in_bytes * i));
        }
    }
}

void fill_segment4p(tos_color color, unsigned length, unsigned offset) {
    unsigned off, len, mask, p;

    //tos_color color = 0x0f & c;
    unsigned under, over = 0;

    under = offset % 8;

    //set/reset register
    outportb(VGA_GC_INDEX, 0);
    outportb(VGA_GC_DATA, color);

    //all in one segment
    if (under) {
        over = (offset + length) % 8;
        mask = 0xff >> under;
        if (under + length <= 8) {
            p = 0xff << (8 - (under + length));
            //p = ~ p;
            mask &= p;
        }

        //bit mask
        outportb(VGA_GC_INDEX, 8);
        outportb(VGA_GC_DATA, mask);

        off = offset >> 3;
        vpeekb(off);
        vpokeb(off, 0xff);

        length = (under + length > 8) ? length - (8 - under) : 0;
        offset += (8 - under);
    }

    off = offset >> 3;
    len = (length + offset) >> 3;

    over = (length + offset) - (len << 3);

    //bit mask
    outportb(VGA_GC_INDEX, 8);
    outportb(VGA_GC_DATA, 0xff);

    int i = 0;
    for (i = off; i < len; i++) {
        vpokeb(i, 0xff);
    }

    if (over) {

        mask = 0xff << (8 - over);

        //bit mask
        outportb(VGA_GC_INDEX, 8);
        outportb(VGA_GC_DATA, mask);

        off = (offset + length) >> 3;
        vpeekb(off);
        vpokeb(off, 0xff);
    }
}

/**
 * READ AND DUMP VGA REGISTER VALUES FOR CURRENT VIDEO MODE
 * This is where g_40x25_text[], g_80x50_text[], etc. came from
 */
void dump_state(void) {
    unsigned char state[VGA_NUM_REGS];
    read_regs(state);
    dump_regs(state);
}

/**
 * Any initialization goes here (may just do all init in setmode)
 */
void init() {
}

/**
 * SET TEXT MODES
 */
void set_text_mode() {
    unsigned rows, cols, ht, i;
    {
        write_regs(g_80x25_text);
        cols = 80;
        rows = 25;
        ht = 16;
    }
    write_font(g_8x16_font, 16);
    /* tell the BIOS what we've done, so BIOS text output works OK */
    vpokes(0x40 + 0x4A, cols); /* columns on screen */
    vpokes(0x40 + 0x4C, cols * rows * 2); /* framebuffer size */
    vpokes(0x40 + 0x50, 0); /* cursor pos'n */
    vpokeb(0x40 + 0x60, ht - 1); /* cursor shape */
    vpokeb(0x40 + 0x61, ht - 2);
    vpokeb(0x40 + 0x84, rows - 1); /* rows on screen - 1 */
    vpokeb(0x40 + 0x85, ht); /* char height */
    /* set white-on-black attributes for all text */
    for (i = 0; i < cols * rows; i++)
        vpokeb(0xB800 + (i * 2 + 1), 7);
    cls();
}
#endif

void set_vga_mode() {
    cls();
    write_regs(g_640x480x16);
    tos_graphics.width = 640;
    tos_graphics.height = 480;
    tos_graphics.colors = 16;
    tos_graphics.bytes_per_pixel = 1;
    tos_graphics.write_pixel = write_pixel4p;
    // tos_graphics.read_pixel_position = read_pixel_position4p;
    // tos_graphics.read_pixel_offset = read_pixel_offset4p;
    // tos_graphics.write_segment = write_segment4p;
    // tos_graphics.read_segment = read_scan_segment4p;
    // tos_graphics.fill_segment = fill_segment4p;
    // tos_graphics.write_char = write_char_8x8;
    clear_graphics();
}

#if 0

/**
 * Change graphics/text modes.  Mode is changed through writing to a series
 * of VGA registers.
 *
 * see: tosvga_regs.h
 */
void set_mode(unsigned mode) {
    set_text_mode();
    cls();
    switch (mode) {
        case G640x480x16:
            write_regs(g_640x480x16);
            tos_graphics.width = 640;
            tos_graphics.height = 480;
            tos_graphics.colors = 16;
            tos_graphics.bytes_per_pixel = 1;
            tos_graphics.write_pixel = write_pixel4p;
            tos_graphics.read_pixel_position = read_pixel_position4p;
            tos_graphics.read_pixel_offset = read_pixel_offset4p;
            tos_graphics.write_segment = write_segment4p;
            tos_graphics.read_segment = read_scan_segment4p;
            tos_graphics.fill_segment = fill_segment4p;
            tos_graphics.write_char = write_char_8x8;

            clear_graphics();

            break;
        default:
            set_text_mode();
            break;
    }
}
#endif 

void set_clear_color(unsigned char c) {
    clear_color = c;
}

/**
 * Clear the screen
 */
void clear_graphics(void) {
    unsigned int i, temp;

    outportb(VGA_GC_INDEX, 0);
    outportb(VGA_GC_DATA, clear_color);
    outportb(VGA_GC_INDEX, 8);
    outportb(VGA_GC_DATA, 0xff);

    for (i = 0; i < tos_graphics.height * tos_graphics.width >> 5; i++) {
        vpokew(i, 0xffffffff | temp);
    }

}

#if 0

void scroll_graphics(int lines) {
    unsigned char* base;
    unsigned char temp;
    unsigned int i, j, k;
    for (k = 0; k < lines; k++) {
        outportb(VGA_GC_INDEX, 5); //change to write mode 1
        outportb(VGA_GC_DATA, 0x09);
        outportb(VGA_GC_INDEX, 8);
        outportb(VGA_GC_DATA, 0xff);
        base = 0;
        for (j = 1; j < 480; j++) {
            for (i = 0; i < 80; i++) {
                temp = vpeekb((unsigned long) (base + i + (tos_graphics.width >> 3)));
                vpokeb((unsigned int) (base + i), temp);
            }
            base += (tos_graphics.width >> 3);
        }

        outportb(VGA_GC_INDEX, 5); //change to write mode 0
        outportb(VGA_GC_DATA, 0x08);

        outportb(VGA_GC_INDEX, 0);
        outportb(VGA_GC_DATA, clear_color);
        outportb(VGA_GC_INDEX, 8);
        outportb(VGA_GC_DATA, 0xff);

        for (i = 0; i < tos_graphics.width >> 3; i++) {
            vpokeb((unsigned int) (base + i), 0xffff | temp);
        }
    }
}

/**
 * Draws one pixel to the screen using the current color
 */
void draw_pixel(unsigned x, unsigned y, tos_color c) {
    tos_graphics.write_pixel(x, y, c);
}

/**
 * Draws length colors to a segment of the VGA starting at x, y
 */
void draw_scan_segment(unsigned offset, unsigned length, tos_color* colors) {
    write_segment4p(offset, length, colors);
}
#endif

void draw_fill_segment(unsigned offset, unsigned length, tos_color color) {
    tos_graphics.fill_segment(color, length, offset);
}

void draw_char(unsigned char c, unsigned x, unsigned y, tos_color color) {
    tos_graphics.write_char(c, x, y, color);
}

/**
 * Reads length colors to a segment of the VGA starting at x, y
 */
void read_scan_segment(unsigned offset, unsigned length, tos_color* colors) {
    tos_graphics.read_segment(offset, length, colors);
}

unsigned get_width() {
    return tos_graphics.width;
}

unsigned get_height() {
    return tos_graphics.height;
}

/**
 * Set the color pallette at index
 */
void set_palette(int index, int red, int green, int blue) {

}

/**
 * Get the color pallette at index
 */
void get_palette(int index, int *red, int *green, int *blue) {

}

/**
 * Set num colors of the color pallette
 */
void set_palvec(int start, int num, int *pal) {

}

/**
 * Set num colors of the color pallette
 */
void get_palvec(int start, int num, int *pal) {

}

void test_vga() {
    set_vga_mode();
    demo_graphics();
}

