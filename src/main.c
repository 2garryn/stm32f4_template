#include "main.h"
#include "convf.h"
#include "freq_debug.h"
#include "spi.h"
#include "i2s_player.h"

void printout(uint32_t value) {
    LOG("Print");
    usart_write_uint32(value);
}


int main(void) {
    rcc_init();
//    usart_init();
    freq_debug_init();
    freq_debug_delay_init();
//    spi_init();

//    freq_debug_tim6_init(10000, printout);

//    convf_set_callback(printout);

//    adc_init(convf_callback);
    i2s_player_init();
    i2s_player_enable();
    while(1) {
        
    //    freq_debug_delay(10, 0);
    //    freq_debug_switch();
        i2s_send(0b1110110111101101);
        //spi_send_data(54);
        
        
    }

    return 0;
}


