#include "drum.h"


uint8_t play = 0;
uint32_t velocity = 0;
FileData _sample_data;

void hit_calback(uint32_t level) {
    velocity = level;
    play = 1;
}


void drum_init() {
    convf_set_callback(hit_calback);
    adc_init(convf_callback);
    i2s_player_init();
    sample_storage_init();
    _sample_data = sample_storage_get();
}


void drum_loop() {
    convf_loop();
    if(play) {
       i2s_player_play(_sample_data.file_size, _sample_data.start_pointer, velocity);
       play = 0;
    }
}