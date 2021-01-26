#include "main.h"
#include "convf.h"
#include "freq_debug.h"

void printout(uint32_t value) {
    LOG("Print");
    usart_write_uint32(value);
}


int main(void) {
    rcc_init();
    usart_init();
//    freq_debug_init();
//    freq_debug_tim6_init(10000, printout);

    convf_set_callback(printout);

    adc_init(convf_callback);

    while(1) {
        convf_loop();
    }

    return 0;
}


