#include "freq_debug.h"

void (*tim6Clb)();

// PD13
void freq_debug_init() {
    RCC->AHB1ENR |=RCC_AHB1ENR_GPIODEN;
    GPIOD->MODER |= GPIO_MODER_MODE12_0;
    GPIOD->OTYPER &= ~GPIO_OTYPER_OT12;
    GPIOD->OSPEEDR |= (GPIO_OSPEEDR_OSPEED12_0 | GPIO_OSPEEDR_OSPEED12_1);
}

void freq_debug_tick() {
    GPIOD->ODR &= ~GPIO_ODR_ODR_12; 
    GPIOD->ODR |= GPIO_ODR_ODR_12;
}


void freq_debug_down() {
    GPIOD->ODR &= ~GPIO_ODR_ODR_12; 
}

void freq_debug_up() {
    GPIOD->ODR |= GPIO_ODR_ODR_12;
}

int sw = 0;
void freq_debug_switch() {
    if(sw) {
        freq_debug_down();
        sw = 0;
    } else {
        freq_debug_up();
        sw = 1;
    }

}

void freq_debug_tim6_init(uint32_t arr, void (*clb)()) {
    tim6Clb = clb;
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = 84 - 1; 
    TIM6->ARR = arr - 1; 
    TIM6->DIER |= TIM_DIER_UIE; 
    TIM6->CR1 |= TIM_CR1_CEN; 
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

void TIM6_DAC_IRQHandler(void) {
    TIM6->SR &= ~TIM_SR_UIF; 
    tim6Clb();
}

#define TIMERS_N 10

volatile uint32_t timers[TIMERS_N];

void freq_debug_delay_init() {
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->PSC = 84 - 1; 
    TIM7->ARR = 1000 - 1; 
    TIM7->DIER |= TIM_DIER_UIE; 
    TIM7->CR1 |= TIM_CR1_CEN; 
    NVIC_EnableIRQ(TIM7_IRQn);
}



void freq_debug_delay(uint32_t ms, int n) {
    timers[n] = ms;
    while(timers[n]) {};
}


void TIM7_IRQHandler(void) {
    TIM7->SR &= ~TIM_SR_UIF; 
    for(volatile int i = 0; i < TIMERS_N; ++i) {
        if (timers[i]) {
            timers[i] -= 1;
        }
    }
}



