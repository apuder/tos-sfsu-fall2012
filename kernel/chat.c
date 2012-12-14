#include <kernel.h>
#include <assert.h>
#include <keycodes.h>

#define DISP_PORT (u_int16_t) 10002
#define LISTEN_PORT (u_int16_t) 10001

BOOL chat_init = 0;
PORT chat_port;
WINDOW divider_chat_wnd = {41, 44, 39, 1, 0, 0, CURSOR_EMPTY};
WINDOW disp_chat_wnd = {41, 45, 39, 14, 0, 0, CURSOR_EMPTY};
WINDOW in_mess_wnd = {41, CONSOLE_LINES - 2, 39, 1, 0, 0, CURSOR_INACTIVE};
unsigned char user_name[10] = "User";
unsigned char opp_name[10] = "Opponent";

void chat_process(PROCESS self, PARAM param) {
    PROCESS sender_proc;
    EM_Message * msg;

    assert(window_is_valid(&disp_chat_wnd));
    assert(window_is_valid(&in_mess_wnd));
    show_cursor(&in_mess_wnd);

    clear_window(&in_mess_wnd);
    clear_window(&disp_chat_wnd);
    em_register_kboard_listener();
    em_register_udp_listener(LISTEN_PORT);

    unsigned char mess_buffer[100];
    unsigned char message[100];
    int i = 0;
    int j = 0;
    UDP * packet;
    u_char_t dip[4] = {192, 168, 1, 1};

    wprintf(&divider_chat_wnd, "_______________________________________");

    while (1) {
        msg = (EM_Message*) receive(&sender_proc);
        switch (msg->type) {
            case EM_GET_NEXT_EVENT:
                break;
            case EM_EVENT_KEY_STROKE:
                if (msg->key == KEY_RETURN) {
                    clear_window(&in_mess_wnd);
                    mess_buffer[i] = '\0';
                    char name[5] = {mess_buffer[0], mess_buffer[1], mess_buffer[2], mess_buffer[3], mess_buffer[4]};
                    if (is_command(name, "name ")) {
                        for (j = 0; j < 10; j++) {
                            user_name[j] = mess_buffer[j + 5];
                        }
                    }
                    wprintf(&disp_chat_wnd, "%s: %s\n", user_name, mess_buffer);
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
                int length = (int *) packet->len;
                for (j = 0; j < length; j++) {
                    message[j] = packet->payload[j];
                }

                char name[5] = {message[0], message[1], message[2], message[3], message[4]};
                if (is_command(name, "name ")) {
                    for (j = 0; j < 10; j++) {
                        opp_name[j] = message[j + 5];
                    }
                }
                wprintf(&disp_chat_wnd, "%s: %s\n", opp_name, message);
                break;
            case EM_EVENT_FOCUS_IN:
                cursor_active(&in_mess_wnd);
                break;
            case EM_EVENT_FOCUS_OUT:
                cursor_inactive(&in_mess_wnd);
                break;
            default:
                kprintf("EVENT_TYPE %d", msg->type);
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
