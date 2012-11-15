#include <kernel.h>

void kernel_main() {
    init_process();
    init_dispatcher();
    init_ipc();
    init_interrupts();
    init_null_process();
    init_timer();
    init_com();
    init_keyb();
    clear_kernel_window();
    init_ne_driver();
    init_em();
    init_shell();
    while (1);
}
