#include "convf.h"
#include "helpers.h"


#define BUFFER_LENGTH 50
#define MAX_VALUE 255
#define ZERO_VALUE 128
#define START_VALUE 130
#define TAIL_LEGNTH 75

uint32_t buffer_position = 0;
uint32_t max_value = ZERO_VALUE;

uint32_t run_flag = 0;

uint32_t tail_buffer = 0;


void (*maxValueCallback)(uint32_t);

void convf_set_callback(void (*clb)(uint32_t)) {
    maxValueCallback = clb;
}

void convf_loop() {
    if(!run_flag) return;

    maxValueCallback(max_value - 127);
    max_value = 0;
    run_flag = 0;
}




void convf_callback(uint32_t value) {
    if(value < ZERO_VALUE) {
        value = MAX_VALUE - value;
    }

    if (buffer_position < BUFFER_LENGTH && value > START_VALUE) {
        if(value > max_value) {
            max_value = value;
        }
        buffer_position++;
        if(buffer_position == BUFFER_LENGTH) {
            run_flag = 1;
        }
        return;
    }

    if(value > START_VALUE) {
        tail_buffer = 0;
        return;
    }
    tail_buffer++;
    if (tail_buffer > TAIL_LEGNTH) {
        buffer_position = 0;
        tail_buffer = 0;
       
    }



}