#include "convf.h"

uint32_t buffer[100];



const uint32_t MAX_VALUE = 4096;
const uint32_t ZERO_VALUE = 2048;
const uint32_t START_VALUE = 2300;

void convf_callback(uint32_t value) {
    if(value < ZERO_VALUE) {
        value = MAX_VALUE - value;
    }

}