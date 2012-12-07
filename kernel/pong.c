#include <kernel.h>
#include <assert.h>

#define PONG_EMPTY_CHAR 32
#define PONG_PADDLE 219
#define PONG_BALL 79
#define PONG_BACKGROUND 0x0f00

#define PONG_MOVE_UP 113
#define PONG_MOVE_DOWN 97

#define OUR_PORT (u_int16_t) 9876
#define THEIR_PORT (u_int16_t) 9875

char * pong_user_name;
BOOL pong_init = 0;
BOOL coin_inserted = 0;
PORT pong_port;
WINDOW pong_wnd = {0, 0, 80, 20, 0, 0, ' '};

void pong_coin_inserted() {
    coin_inserted = 1;
}

void blank_position(unsigned char x, unsigned char y) {
    poke_screen(x, y, (unsigned short) PONG_EMPTY_CHAR | PONG_BACKGROUND);
}

void pong_process(PROCESS self, PARAM param) {
    PROCESS sender_proc;
    EM_Message * msg;

    clear_window(&pong_wnd);

    wprintf(&pong_wnd, " .----------------. .----------------. .-----------------..----------------. \n");
    wprintf(&pong_wnd, "| .--------------. | .--------------. | .--------------. | .--------------. |\n");
    wprintf(&pong_wnd, "| |   ______     | | |     ____     | | | ____  _____  | | |    ______    | |\n");
    wprintf(&pong_wnd, "| |  |_   __ \\   | | |   .'    `.   | | ||_   \\|_   _| | | |  .' ___  |   | |\n");
    wprintf(&pong_wnd, "| |    | |__) |  | | |  /  .--.  \   | | |  |   \\ | |   | | | / .'   \\_|   | |\n");
    wprintf(&pong_wnd, "| |    |  ___/   | | |  | |    | |  | | |  | |\\ \\| |   | | | | |    ____  | |\n");
    wprintf(&pong_wnd, "| |   _| |_      | | |  \  `--'  /   | | | _| |_\\   |_  | | | \\ `.___]  _| | |\n");
    wprintf(&pong_wnd, "| |  |_____|     | | |   `.____.'   | | ||_____|\\____| | | |  `._____.'   | |\n");
    wprintf(&pong_wnd, "| |              | | |              | | |              | | |              | |\n");
    wprintf(&pong_wnd, "| '--------------' | '--------------' | '--------------' | '--------------' |\n");
    wprintf(&pong_wnd, " '----------------' '----------------' '----------------' '----------------' \n");
    wprintf(&pong_wnd, "\n");

    unsigned char times = 10;
    while (!coin_inserted) {
        times % 2 == 0
                ? wprintf(&pong_wnd, "********************************* INSERT COIN *********************************\n")
                : wprintf(&pong_wnd, "                                                                               \n");
        pong_wnd.cursor_y -= 1;
        times++;
        sleep(10);
    }

    clear_window(&pong_wnd);

    em_register_kboard_listener();
    em_register_udp_listener(OUR_PORT);

    unsigned char our_paddle = 0;
    unsigned char their_paddle = 0;
    unsigned char ball_x = 40;
    unsigned char ball_y = 10;
    unsigned char our_score = 0;
    unsigned char their_score = 0;

    unsigned char output_data[2];

    unsigned char * byte;
    UDP * packet;
    u_char_t dip[4] = {192, 168, 1, 1};

    poke_screen(0, our_paddle, (unsigned short) PONG_PADDLE | PONG_BACKGROUND);
    poke_screen(79, their_paddle, (unsigned short) PONG_PADDLE | PONG_BACKGROUND);
    poke_screen(ball_x, ball_y, (unsigned short) PONG_BALL | PONG_BACKGROUND);

    while (1) {
        msg = (EM_Message*) receive(&sender_proc);
        switch (msg->type) {
            case EM_GET_NEXT_EVENT:
                break;
            case EM_EVENT_KEY_STROKE:
                if (msg->key == PONG_MOVE_UP && our_paddle > 0) {
                    blank_position(0, our_paddle);
                    our_paddle--;
                    poke_screen(0, our_paddle, (unsigned short) PONG_PADDLE | PONG_BACKGROUND);
                }
                if (msg->key == PONG_MOVE_DOWN && our_paddle < 19) {
                    blank_position(0, our_paddle);
                    our_paddle++;
                    poke_screen(0, our_paddle, (unsigned short) PONG_PADDLE | PONG_BACKGROUND);
                }

                output_data[0] = 0;
                output_data[1] = our_paddle;
                ne_do_send_udp(OUR_PORT, THEIR_PORT, dip, 2, (void *) output_data);
                break;

            case EM_EVENT_UDP_PACKET_RECEIVED:
                packet = (UDP *) msg->data;
                byte = (unsigned char *) packet->payload;

                if (*byte == 0) {
                    byte++;

                    // clear the current position of objects
                    blank_position(79, their_paddle);
                    blank_position(ball_x, ball_y);
                    blank_position(0, 20);
                    blank_position(0, 60);

                    // get new positions
                    ball_x = (unsigned char) *(byte);
                    ball_y = (unsigned char) *(++byte);
                    their_paddle = (unsigned char) *(++byte);
                    their_score = (unsigned char) *(++byte);
                    our_score = (unsigned char) *(++byte);

                    //kprintf("Ball %d,%d Paddle %d", ball_x, ball_y, their_paddle);
                    poke_screen(79, their_paddle, (unsigned short) PONG_PADDLE | PONG_BACKGROUND);
                    poke_screen(ball_x, ball_y, (unsigned short) PONG_BALL | PONG_BACKGROUND);

                    poke_screen(0, 20, (unsigned short) our_score | PONG_BACKGROUND);
                    poke_screen(0, 60, (unsigned short) their_score | PONG_BACKGROUND);
                }
                break;
            case EM_EVENT_FOCUS_IN:
                break;
            case EM_EVENT_FOCUS_OUT:
                break;
            default:
                panic("UNKNOWN MESSAGE RECIEVED");
                break;
        }
    }
}

void init_pong(char * name) {
    if (pong_init)
        return;
    pong_port = create_process(pong_process, 5, 0, "Pong process");
    pong_init = 1;
    pong_user_name = name;
    resign();
}