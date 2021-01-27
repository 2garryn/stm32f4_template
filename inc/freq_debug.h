#include "helpers.h"


void freq_debug_init();

void freq_debug_tick();

void freq_debug_tim6_init(uint32_t arr, void (*clb)());

void freq_debug_down();

void freq_debug_up();

void freq_debug_delay_init();

void freq_debug_delay(uint32_t ms, int n);

void freq_debug_switch();