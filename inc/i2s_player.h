#include "helpers.h"

void i2s_player_init(); 
void i2s_player_enable();
void i2s_player_disable();
void i2s_send(uint16_t data);
void i2s_play_array(); 

void i2s_player_play(uint32_t length, uint16_t* offset, uint32_t level);
void i2s_player_play2(uint32_t length, volatile uint16_t* offset);