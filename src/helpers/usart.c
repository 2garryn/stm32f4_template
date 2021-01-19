#include "usart.h"

/*
RM0090 Rev 18 is used for references

*/

/*
    PB6 is set as USART1_TX
*/
void usart_init() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // Alternate function mode 0b10 (8.4.1 p281)
    GPIOB->MODER |= GPIO_MODER_MODE6_1;

    // Push pull (8.4.2 p281)
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT6;

    // High speed (8.4.3 p282)
    GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED6;
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED6_1;

    // Pull up (8.4.4 p282)
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD6_0;

    // Alternate function AF7 USART1_TX 0b0111 (8.4.9 p285, Datasheet table 9, p63)
    GPIOB->AFR[0] &=~ GPIO_AFRL_AFRL6;
    GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_0 | GPIO_AFRL_AFSEL6_1 | GPIO_AFRL_AFSEL6_2; //(GPIO_AFRL_AFRL6_0 | GPIO_AFRL_AFRL6_1 | GPIO_AFRL_AFRL6_2);

    USART1->CR1 &=~ USART_CR1_UE;
    USART1->CR1 &=~ USART_CR1_M;			//8 bits
	USART1->CR1 &=~ USART_CR1_OVER8;  //versamping by 16
	USART1->CR2 &=~ USART_CR2_STOP;   //stp bit 1
    // Baudrate = 115.226 KBps. (table 142 p986). 
    // Value=45.5625
    // 45 = 0x2D
    // 16 * 0.5625 = 0x9
    // Value = 0x2D9
    USART1->BRR = 0x2D9;

    // Enable usart and tx
    USART1->CR1 |= (USART_CR1_UE | USART_CR1_TE);
}

void usart_write() {
    USART1->DR = 10;
    while( !(USART1->SR & 0x00000040) ); 
    USART1->DR = 97;
    while( !(USART1->SR & 0x00000040) ); 
    USART1->DR = 98;
    while( !(USART1->SR & 0x00000040) ); 
    USART1->DR = 99;
    while( !(USART1->SR & 0x00000040) ); 
}