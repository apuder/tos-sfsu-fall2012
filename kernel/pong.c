#include <kernel.h>
#include <assert.h>

#define PONG_OUTPUT_BUFFER_SIZE 25
#define PONG_MAX_NAME_SIZE 20
#define PONG_EMPTY_CHAR 32
#define PONG_PADDLE     219
#define PONG_BALL       79
#define PONG_BACKGROUND 0x0f00
#define PONG_HEIGHT     20
#define PONG_WIDTH      80
#define PONG_MOVE_UP    113
#define PONG_MOVE_DOWN  97
#define OUR_PORT (u_int16_t)    9876
#define THEIR_PORT (u_int16_t)  9875

BOOL pong_init = 0;
BOOL coin_inserted = 0;
PORT pong_port;
WINDOW pong_wnd = {0, 0, PONG_WIDTH, PONG_HEIGHT, 0, 0, ' '};
WINDOW pong_score_wnd = {10, 0, PONG_WIDTH - 20, 2, 0, 0, ' '};

char * pong_user_name;
char * pong_opponent_name;

unsigned char our_paddle = 0;
unsigned char our_paddle_prev = 0;

unsigned char their_paddle = 0;
unsigned char their_paddle_prev = 0;

unsigned char ball_x = 40;
unsigned char ball_x_prev = 40;

unsigned char ball_y = 10;
unsigned char ball_y_prev = 10;

unsigned char our_score = 0;
unsigned char our_score_prev = 0;

unsigned char their_score = 0;
unsigned char their_score_prev = 0;

void pong_coin_inserted() {
    coin_inserted = 1;
}

void pong_blank_position(unsigned char x, unsigned char y) {
    poke_screen(x, y, (unsigned short) PONG_EMPTY_CHAR | PONG_BACKGROUND);
}

void pong_draw_paddle(unsigned char x, unsigned char y) {
    poke_screen(x, y, (unsigned short) PONG_PADDLE | PONG_BACKGROUND);
}

void pong_update_ui() {
    // player's paddle
    if ((our_paddle != our_paddle_prev && our_paddle < PONG_HEIGHT) || ball_x_prev == 0) {
        pong_blank_position(0, our_paddle_prev);
        pong_draw_paddle(0, our_paddle);
        our_paddle_prev = our_paddle;
    }

    // opponent's paddle
    if ((their_paddle != their_paddle_prev && their_paddle < PONG_HEIGHT) || ball_x_prev == PONG_WIDTH - 1) {
        pong_blank_position(PONG_WIDTH - 1, their_paddle_prev);
        pong_draw_paddle(PONG_WIDTH - 1, their_paddle);
        their_paddle_prev = their_paddle;
    }

    // update ball's position
    if (ball_x < PONG_WIDTH && ball_y < PONG_HEIGHT) {
        pong_blank_position(ball_x_prev, ball_y_prev);
        poke_screen(ball_x, ball_y, (unsigned short) PONG_BALL | PONG_BACKGROUND);
        ball_x_prev = ball_x;
        ball_y_prev = ball_y;
    }

    if (our_score != our_score_prev || their_score != their_score_prev || ball_y_prev == 0) {
        clear_window(&pong_score_wnd);
        wprintf(&pong_score_wnd, "%s=%d vs. %s=%d\n",
                pong_user_name, our_score,
                pong_opponent_name, their_score);
        our_score_prev = our_score;
        their_score_prev = their_score;
    }
}

void pong_process(PROCESS self, PARAM param) {
    PROCESS sender_proc;
    EM_Message * msg;

    assert(window_is_valid(&pong_wnd));
    assert(window_is_valid(&pong_score_wnd));

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

    wprintf(&pong_wnd, "Pong is starting...");
    sleep(50);

    clear_window(&pong_wnd);

    em_register_kboard_listener();
    em_register_udp_listener(OUR_PORT);

    // option is used to differentiate packet's content and protocol
    unsigned char option;

    // names
    unsigned char * player_name = pong_user_name;
    unsigned char opponent_name[PONG_MAX_NAME_SIZE];
    opponent_name[0] = '?';
    opponent_name[1] = '\0';
    pong_opponent_name = opponent_name;
    unsigned char output_data[PONG_OUTPUT_BUFFER_SIZE];

    // to handle UDP packets
    unsigned char * byte;
    UDP * packet;
    u_char_t dip[4] = {192, 168, 1, 1};

    // send our name to the opponent
    int length = 1;
    output_data[0] = 1;
    while (length < PONG_OUTPUT_BUFFER_SIZE && *pong_user_name != '\0') {
        output_data[length++] = *pong_user_name++;
    }
    ne_do_send_udp(OUR_PORT, THEIR_PORT, dip, length, (void *) output_data);
    pong_user_name = player_name;

    // draw elements
    pong_update_ui();

    while (1) {

        // receive new event from event manager
        msg = (EM_Message*) receive(&sender_proc);

        switch (msg->type) {

            case EM_GET_NEXT_EVENT:
                break;

            case EM_EVENT_KEY_STROKE:
                if (msg->key == PONG_MOVE_UP && our_paddle > 0) {
                    our_paddle--;
                    pong_update_ui();
                } else if (msg->key == PONG_MOVE_DOWN && our_paddle < 19) {
                    our_paddle++;
                    pong_update_ui();
                }
                output_data[0] = 0;
                output_data[1] = our_paddle;
                ne_do_send_udp(OUR_PORT, THEIR_PORT, dip, 2, (void *) output_data);
                break;

            case EM_EVENT_UDP_PACKET_RECEIVED:
                packet = (UDP *) msg->data;
                byte = (unsigned char *) packet->payload;
                option = *byte;
                byte++;
                switch (option) {
                    case 0:
                        // get new positions and update game state
                        ball_x = (unsigned char) *(byte);
                        ball_y = (unsigned char) *(++byte);
                        their_paddle = (unsigned char) *(++byte);
                        their_score = (unsigned char) *(++byte);
                        our_score = (unsigned char) *(++byte);
                        our_score = (unsigned char) 0;
                        // update game ui
                        pong_update_ui();
                        break;
                    case 1:
                        length = 0;
                        while (length < PONG_MAX_NAME_SIZE - 1 && (unsigned char) *byte != '\0') {
                            opponent_name[length++] = (unsigned char *) byte++;
                        }
                        opponent_name[PONG_MAX_NAME_SIZE - 1] = '\0';
                        pong_update_ui();
                        break;
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