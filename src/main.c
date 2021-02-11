#include "main.h"
#include "convf.h"
#include "freq_debug.h"
#include "spi.h"
#include "i2s_player.h"
#include "sample_storage.h"

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
    sample_storage_init();

    FileData sample = sample_storage_get();
  //  i2s_play_array();
    while(1) {
        
        i2s_player_play(sample.file_size, sample.start_pointer);
        freq_debug_delay(500, 0);
        freq_debug_switch();
        //i2s_send(0b1110110111101101);
        //spi_send_data(54);
        
        
    }

    return 0;
}


