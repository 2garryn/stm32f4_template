#include "adc.h"
#include "stm32f407xx.h"

void (*adcEndCallback)(uint32_t);

/*

p12 https://www.st.com/resource/en/application_note/dm00315319-stm32-gpio-configuration-for-hardware-settings-and-lowpower-consumption-stmicroelectronics.pdf
*/

void adc_init(void (*clb)(uint32_t)) {
    // ADC1 - ADC123_IN1 - PA0
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    ADC123_COMMON->CCR |= ADC_CCR_ADCPRE_0;

    // Analog mode
    GPIOA->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER0_1 );

    ADC1->CR2 &= ~ADC_CR2_ADON;
    ADC1->SQR1 = 0; // one conversion
    ADC1->SQR3 = 0; // channel 0

    ADC1->CR2 &= ~ADC_CR2_CONT; // Disable continous
    ADC1->CR1 &= ~ADC_CR1_SCAN; // Disable scan

    ADC1->CR1 |= ADC_CR1_RES_1; // 12-bit resolution
    ADC1->CR2 &= ~ADC_CR2_ALIGN;

    // TIM2_TRGO event
    ADC1->CR2 &= ~(ADC_CR2_EXTSEL);
    ADC1->CR2 |= (ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2);

    // Detection on the rising edge
    ADC1->CR2 |= ADC_CR2_EXTEN_0;

    // Enable interruption on end or conversion
    ADC1->CR1 |= ADC_CR1_EOCIE;
    // Enable ADC interruption
    NVIC_EnableIRQ(ADC_IRQn); 



    // 50khz
    TIM2->PSC = 42 - 1; 
    TIM2->ARR = 20; 
    // The update event is selected as trigger output (TRGO).
    TIM2->CR2 &= ~(TIM_CR2_MMS_0 | TIM_CR2_MMS_2);
    TIM2->CR2 |= TIM_CR2_MMS_1;

    adcEndCallback = clb;

    ADC1->CR2 |= ADC_CR2_ADON;		// Turn on ADC
    TIM2->CR1 |= TIM_CR1_CEN; 

}

uint32_t adc_value() {
    return ADC1->DR;
}

void ADC_IRQHandler(void) {
    if(ADC1->SR & ADC_SR_EOC) {
        (*adcEndCallback)(adc_value());
    }
    ADC1->SR = 0; 
}