#include <kernel.h>
#include <assert.h>
#include <keycodes.h>

#define DISP_PORT (u_int16_t) 9886
#define LISTEN_PORT (u_int16_t) 9866

BOOL chat_init = 0;
PORT chat_port;
WINDOW disp_chat_wnd = {40, 20, 40, 4, 0, 0, 0xDC};
WINDOW in_mess_wnd = {40, 24, 40, 1, 0, 0, 0xDC};

void chat_process(PROCESS self, PARAM param) {
    PROCESS sender_proc;
    EM_Message * msg;

    clear_window(&in_mess_wnd);
    clear_window(&disp_chat_wnd);
    em_register_kboard_listener();
    em_register_udp_listener(DISP_PORT);

    unsigned char * message;
    unsigned char mess_buffer[100];
    int i = 0;
    UDP * packet;
    u_char_t dip[4] = {192, 168, 1, 1};

    while (1) {
        msg = (EM_Message*) receive(&sender_proc);
        switch (msg->type) {
            case EM_GET_NEXT_EVENT:
                break;
            case EM_EVENT_KEY_STROKE:
                if (msg->key == KEY_RETURN) {
                    clear_window(&in_mess_wnd);
                    mess_buffer[i] = '\0';
                    wprintf(&disp_chat_wnd, "User: %s\n", mess_buffer);
                    ne_do_send_udp(LISTEN_PORT, DISP_PORT, dip, i, (void *) mess_buffer);
                    i = 0;

                } else {
                    wprintf(&in_mess_wnd, "%c", msg->key);
                    mess_buffer[i] = msg->key;
                    i++;
                }
                break;
            case EM_EVENT_UDP_PACKET_RECEIVED:
                packet = (UDP *) msg->data;
                message = (unsigned char *) packet->payload;
                wprintf(&disp_chat_wnd, "Opponent: %s\n", message);
                break;
            default:
                panic("UNKNOWN MESSAGE RECIEVED");
                break;
        }
    }
}

void init_chat() {
    if (chat_init)
        return;
    chat_port = create_process(chat_process, 5, 0, "Chat process");
    chat_init = 1;
    resign();
}
