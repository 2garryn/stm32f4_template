#include "main.h"

void delay1(volatile uint32_t delay) {
    while (delay--);
}

int led = 1;

int main(void) {
    rcc_init();
    usart_init();
    // LED - PD13

    RCC->AHB1ENR |=RCC_AHB1ENR_GPIODEN;

    // set 0b01 (General output)
    GPIOD->MODER |= GPIO_MODER_MODE13_0;

    GPIOD->OTYPER &= ~GPIO_OTYPER_OT13;

    GPIOD->OSPEEDR |= (GPIO_OSPEEDR_OSPEED13_0 | GPIO_OSPEEDR_OSPEED13_1);


    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

    TIM6->PSC = 42000 - 1; 
    TIM6->ARR = 1000; 
    TIM6->DIER |= TIM_DIER_UIE; 
    TIM6->CR1 |= TIM_CR1_CEN; 
    NVIC_EnableIRQ(TIM6_DAC_IRQn);


    while(1) {}

    return 0;
}


void TIM6_DAC_IRQHandler(void)
{
    TIM6->SR &= ~TIM_SR_UIF; 
    if(led) {
        GPIOD->ODR &= ~GPIO_ODR_ODR_13;
        led = 0;
    } else {
        GPIOD->ODR |= GPIO_ODR_ODR_13;
        led = 1;
    }
}
