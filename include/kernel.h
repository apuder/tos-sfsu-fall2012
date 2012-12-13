
#ifndef __KERNEL__
#define __KERNEL__

#include <assert.h>
#include <tos_util.h>
#include <nll.h>
#include <keycodes.h>
#include <kernelinit.h>
#include <vga.h>
#include <vga_draw.h>

#define TRUE	1
#define FALSE	0


#ifndef NULL
#define NULL	((void *) 0)
#endif

#define SANITY_SHORT (unsigned short) 0xDEAD
#define SANITY_INT (unsigned int) 0xDEADBEEF

typedef int BOOL;

/*=====>>> stdlib.c <<<=====================================================*/

int k_strlen(const char* str);
void* k_memcpy(void* dst, const void* src, int len);
int k_memcmp(const void* b1, const void* b2, int len);


/*=====>>> mem.c <<<========================================================*/

typedef unsigned MEM_ADDR;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned LONG;

void poke_b(MEM_ADDR addr, BYTE value);
void poke_w(MEM_ADDR addr, WORD value);
void poke_l(MEM_ADDR addr, LONG value);
BYTE peek_b(MEM_ADDR addr);
WORD peek_w(MEM_ADDR addr);
LONG peek_l(MEM_ADDR addr);

/*=====>>> window.c <<<=====================================================*/

typedef struct {
    int x, y;
    int width, height;
    int cursor_x, cursor_y;
    char cursor_char, show_cursor;
} WINDOW;

extern WINDOW* kernel_window;


void move_cursor(WINDOW* wnd, int x, int y);
void remove_cursor(WINDOW* wnd);
void show_cursor(WINDOW* wnd);
void clear_window(WINDOW* wnd);
void clear_kernel_window();
void output_char(WINDOW* wnd, unsigned char ch);
void output_string(WINDOW* wnd, const char *str);
void wprintf(WINDOW* wnd, const char* fmt, ...);
void kprintf(const char* fmt, ...);
void poke_screen(int x, int y, WORD ch);
void cursor_active(WINDOW* wnd);
void cursor_inactive(WINDOW* wnd);

/*=====>>> basicio.c <<<=====================================================*/

void cls();
void set_char_attr(unsigned char attr);
//void output_char(unsigned char ch);
//void output_string(const char *str);
void kprintf(const char* fmt, ...);


/*=====>>> process.c <<<====================================================*/

/*
 * Max. number of processes
 */
#define MAX_PROCS		20


/*
 * Max. number of ready queues
 */
#define MAX_READY_QUEUES	8


#define STATE_READY 		0
#define STATE_SEND_BLOCKED	1
#define STATE_REPLY_BLOCKED 	2
#define STATE_RECEIVE_BLOCKED	3
#define STATE_MESSAGE_BLOCKED	4
#define STATE_INTR_BLOCKED 	5


#define MAGIC_PCB 0x4321dcba


struct _PCB;
typedef struct _PCB* PROCESS;

struct _PORT_DEF;
typedef struct _PORT_DEF* PORT;

typedef struct _PCB {
    unsigned magic;
    unsigned used;
    unsigned short priority;
    unsigned short state;
    MEM_ADDR esp;
    PROCESS param_proc;
    void* param_data;
    PORT first_port;
    PROCESS next_blocked;
    PROCESS next;
    PROCESS prev;
    char* name;
} PCB;


extern PCB pcb[];

typedef unsigned PARAM;

/*
 * The name of the boot process.
 */
#define boot_name "Boot process"
PORT create_process(void (*new_proc) (PROCESS, PARAM),
        int prio,
        PARAM param,
        char *proc_name);
#ifdef XXX
PROCESS fork();
#endif
void print_process(WINDOW* wnd, PROCESS p);
void print_all_processes(WINDOW* wnd);
void init_process();



/*=====>>> dispatch.c <<<===================================================*/

extern PROCESS active_proc;
extern PCB* ready_queue[];


PROCESS dispatcher();
void add_ready_queue(PROCESS proc);
void remove_ready_queue(PROCESS proc);
void resign();
void init_dispatcher();


/*=====>>> null.c <<<=======================================================*/

void init_null_process();


/*=====>>> ipc.c <<<========================================================*/

#define MAX_PORTS	(MAX_PROCS * 2)

#define MAGIC_PORT  0x1234abcd

typedef struct _PORT_DEF {
    unsigned magic;
    unsigned used; /* Port slot used? */
    unsigned open; /* Port open? */
    PROCESS owner; /* Owner of this port */
    PROCESS blocked_list_head; /* First local blocked process */
    PROCESS blocked_list_tail; /* Last local blocked process */
    struct _PORT_DEF *next; /* Next port */
} PORT_DEF;


PORT create_port();
PORT create_new_port(PROCESS proc);
void open_port(PORT port);
void close_port(PORT port);
void send(PORT dest_port, void* data);
void message(PORT dest_port, void* data);
void* receive(PROCESS* sender);
void reply(PROCESS sender);
void init_ipc();


/*=====>>> intr.c <<<=======================================================*/

#define DISABLE_INTR(save)	asm ("pushfl");                   \
                                asm ("popl %0" : "=r" (save) : ); \
				asm ("cli");

#define ENABLE_INTR(save) 	asm ("pushl %0" : : "m" (save)); \
				asm ("popfl");

typedef struct {
    unsigned short offset_0_15;
    unsigned short selector;
    unsigned short dword_count : 5;
    unsigned short unused : 3;
    unsigned short type : 4;
    unsigned short dt : 1;
    unsigned short dpl : 2;
    unsigned short p : 1;
    unsigned short offset_16_31;
} IDT;

#define CODE_SELECTOR 0x8
#define DATA_SELECTOR 0x10
#define MAX_INTERRUPTS 256
#define IDT_ENTRY_SIZE 8


extern BOOL interrupts_initialized;

void init_idt_entry(int intr_no, void (*isr) (void));
void wait_for_interrupt(int intr_no);
void init_interrupts();


/*=====>>> timer.c <<<===================================================*/

#define TIMER_IRQ   0x60

extern PORT timer_port;

typedef struct _Timer_Message {
    int num_of_ticks;
} Timer_Message;

void sleep(int num_of_ticks);
void init_timer();


/*=====>>> inout.c <<<======================================================*/

unsigned char inportb(unsigned short port);
unsigned short inportw(unsigned short port);
void outportb(unsigned short port, unsigned char value);


/*=====>>> com.c <<<=====================================================*/

#define COM1_IRQ    0x64
#define COM1_PORT   0x3f8

#define COM2_IRQ    0x63
#define COM2_PORT   0x2f8


extern PORT com_port;

typedef struct _COM_Message {
    char* output_buffer;
    char* input_buffer;
    int len_input_buffer;
} COM_Message;

void init_com();


/*=====>>> em.c <<<====================================================*/

#define EM_GET_NEXT_EVENT (unsigned int) 1
#define EM_REG_EVENT_KEY_STROKE (unsigned int) 101
#define EM_REG_EVENT_UDP_PACKET_RECEIVED (unsigned int)102
#define EM_EVENT_KEY_STROKE (unsigned int) 201
#define EM_EVENT_UDP_PACKET_RECEIVED (unsigned int) 202
#define EM_EVENT_FOCUS_IN (unsigned int) 203
#define EM_EVENT_FOCUS_OUT (unsigned int) 204

extern PORT em_port;

typedef struct _EM_Message {
    unsigned short sanity;
    unsigned int type;
    unsigned int port;
    unsigned char key;
    void * data;
} EM_Message;

BOOL em_register_udp_listener(unsigned int port);
BOOL em_register_kboard_listener();
void em_new_event(EM_Message * msg);
void em_new_udp_packet(UDP * udp);
void em_new_keystroke(unsigned char key);
void init_em();

/*=====>>> keyb.c <<<====================================================*/

#define KEYB_IRQ	0x61

extern PORT keyb_port;

typedef struct _Keyb_Message {
    char* key_buffer;
} Keyb_Message;

void init_keyb();

/*=====>>> nek2_driver.c <<<====================================================*/

#define NE2K_IRQ	0x6A
#define NE2K_IOBASE 0x300
#define NE2K_PORT	0x300

extern PORT ne2k_driver_port;

typedef struct _NE2K_Driver_Message {
} NE2K_Driver_Message;

void init_ne_driver();
void ne_test_transmit();
void ne_handle_interrupt();
void ne_config(char * params);
void process_incoming_packet(void * data, int len);
void send_udp(u_char_t * ip, u_int16_t port, u_int_t len, void * payload);
void ne_do_send_udp(u_int16_t sp, u_int16_t dp, u_char_t * dip, u_int_t len, void * payload);

/*=====>>> floppy.c <<<===================================================*/

/* FSLL API */
#define DISK_READ    0    /* Disk read */
#define DISK_WRITE   1    /* Disk write */
#define DRIVE_A      0    /* A Drive */
#define DRIVE_B      1    /* B Drive */
#define DRIVE_C      2    /* C Drive */
#define DRIVE_D      3    /* D Drive */

/* Return error codes */
#define ERR_COMMAND  -1   /* fsdd io command error */
#define ERR_DMA      -2   /* DMA setup error */
#define ERR_DRIVE    -3   /* Drive error */
#define ERR_SEEK     -4   /* Seek error */
#define ERR_TRANSFER -5   /* Transfer error */
#define ERR_PROTECT  -6   /* Write protected */
#define ERR_RAM      -7   /* RAM disk error */

#define FLOPPY_IRQ   0x66 /* IRQ6 */

PORT floppy_port;

typedef struct _Floppy_Message {
    int rw_flag;
    int drive;
    int count; // Multiples of 512
    int position; // 0 ~ 2879
    MEM_ADDR address;
} Floppy_Message;

int fsdd_io(int rw_flag, int drive, int count, int position, MEM_ADDR address);
int flag_isreal;

/*=====>>> shell.c <<<===================================================*/

extern WINDOW* shell_wnd_ptr;
void init_shell();
int is_command(char* s1, char* s2);

/*=====>>> pong.c <<<===================================================*/

extern PORT pong_port;
void init_pong();
void pong_coin_inserted();

/*=====>>> chat.c <<<===================================================*/

extern PORT chat_port;
void init_chat();

/*=====>>> train.c <<<===================================================*/

void init_train(WINDOW* wnd);

/*=====>>> pacman.c <<<==================================================*/

void init_pacman(WINDOW* wnd, int num_ghosts);

/*=====>>> vga.c <<<==================================================*/

void test_vga();

#endif
