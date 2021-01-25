#include "main.h"
#include "convf.h"

void delay1(volatile uint32_t delay) {
    while (delay--);
}

int led1 = 1;
int print = 0;


void printout(uint32_t val) {
    usart_write_uint32(val);
}


int main(void) {
    rcc_init();
    usart_init();

    // LED - PD13

    RCC->AHB1ENR |=RCC_AHB1ENR_GPIODEN;

    // set 0b01 (General output)
    GPIOD->MODER |= GPIO_MODER_MODE12_0;

    GPIOD->OTYPER &= ~GPIO_OTYPER_OT12;

    GPIOD->OSPEEDR |= (GPIO_OSPEEDR_OSPEED12_0 | GPIO_OSPEEDR_OSPEED12_1);

    conf_set_callback(printout);

    adc_init(convf_callback);

/*
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

    TIM6->PSC = 42000 - 1; 
    TIM6->ARR = 1000; 
    TIM6->DIER |= TIM_DIER_UIE; 
    TIM6->CR1 |= TIM_CR1_CEN; 
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
    */

    //uint32_t adcv;

/*
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 42 - 1; 
    TIM2->ARR = 40; 
    TIM2->DIER |= TIM_DIER_UIE; 
    TIM2->CR1 |= TIM_CR1_CEN; 
    NVIC_EnableIRQ(TIM2_IRQn);
*/

    while(1) {
        /*
        if(print) {
            adcv = adc_value();
            usart_write_uint32(adcv);
            print = 0;
        }
        */
       convf_loop();
    }

    return 0;
}




void TIM2_IRQHandler(void)
{
    TIM2->SR &= ~TIM_SR_UIF; 

   

}
