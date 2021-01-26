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

void freq_debug_tim6_init(uint32_t arr, void (*clb)()) {
    tim6Clb = clb;
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

    TIM6->PSC = 84 - 1; 
    TIM6->ARR = arr - 1; 
    TIM6->DIER |= TIM_DIER_UIE; 
    TIM6->CR1 |= TIM_CR1_CEN; 
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
}


void TIM6_DAC_IRQHandler(void)
{
    TIM6->SR &= ~TIM_SR_UIF; 
    tim6Clb();
}