#include <kernel.h>
#include <assert.h>

BOOL pong_init = 0;
BOOL coin_inserted = 0;
PORT pong_port;
WINDOW pong_wnd = {0, 0, 80, 20, 0, 0, ' '};

void pong_coin_inserted() {
    coin_inserted = 1;
}

void pong_process(PROCESS self, PARAM param) {
    PROCESS sender_proc;
    EM_Message * msg;

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

    int times = 20;
    while (!coin_inserted) {
        times % 2 == 0
                ? wprintf(&pong_wnd, " ******************************** INSERT COIN ******************************\n")
                : wprintf(&pong_wnd, "                                                                            \n");
        pong_wnd.cursor_y -= 1;
        times--;
        sleep(50);
    }

    clear_window(&pong_wnd);

    em_register_kboard_listener();
    em_register_udp_listener(9090);

    while (1) {
        msg = (EM_Message*) receive(&sender_proc);
        switch (msg->type) {
            case EM_GET_NEXT_EVENT:
                break;
            case EM_EVENT_KEY_STROKE:
                wprintf(&pong_wnd, "key=%c (%d)", msg->key, msg->key);
                break;
            case EM_EVENT_UDP_PACKET_RECEIVED:
                wprintf(&pong_wnd, "New packet received");
                break;
            default:
                panic("UNKNOWN MESSAGE RECIEVED");
                break;
        }
    }
}

void init_pong() {
    if (pong_init)
        return;
    pong_port = create_process(pong_process, 5, 0, "Pong process");
    pong_init = 1;
    resign();
}