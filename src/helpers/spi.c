#include "spi.h"

/*
SPI1 
SPI1_NSS - PA4 (AF5)
SPI1_SCK - PA5 (AF5)
SPI1_MISO - PA6 (AF5)
SPI1_MOSI - PA7 (AF5)
*/

void spi_init() {
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    GPIOA->MODER |= GPIO_MODER_MODE4_0;
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT4;
    GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED4_0 | GPIO_OSPEEDR_OSPEED4_1);
    GPIOA->ODR |= GPIO_ODR_ODR_4;
    GPIOA->MODER |= (
    //    GPIO_MODER_MODE4_1 |
        GPIO_MODER_MODE5_1 |
        GPIO_MODER_MODE6_1 |
        GPIO_MODER_MODE7_1);

    // High speed (8.4.3 p282)
    GPIOA->OSPEEDR &= ~(
    //    GPIO_OSPEEDR_OSPEED4 | 
        GPIO_OSPEEDR_OSPEED5 | 
        GPIO_OSPEEDR_OSPEED6 | 
        GPIO_OSPEEDR_OSPEED7);
    GPIOA->OSPEEDR |= (
    //    GPIO_OSPEEDR_OSPEED4_1 | 
        GPIO_OSPEEDR_OSPEED5_1 | 
        GPIO_OSPEEDR_OSPEED6_1 |
        GPIO_OSPEEDR_OSPEED7_1);
    
    // Alternate function AF5 (8.4.9 p285, Datasheet table 9, p63)
    GPIOA->AFR[0] &= ~ (
    //    GPIO_AFRL_AFRL4 | 
        GPIO_AFRL_AFRL5 | 
        GPIO_AFRL_AFRL6 | 
        GPIO_AFRL_AFRL7);
    GPIOA->AFR[0] |= (
    //    GPIO_AFRL_AFSEL4_0 | GPIO_AFRL_AFSEL4_2 | 
        GPIO_AFRL_AFSEL5_0 | GPIO_AFRL_AFSEL5_2 | 
        GPIO_AFRL_AFSEL6_0 | GPIO_AFRL_AFSEL6_2 | 
        GPIO_AFRL_AFSEL7_0 | GPIO_AFRL_AFSEL7_2);



    SPI1->CR1 |= (
     //   SPI_CR1_CRCEN | //Hardware CRC calculation enable
        SPI_CR1_MSTR 
        | SPI_CR1_SSM 
        | SPI_CR1_SSI
        ); // Master

    // clk/8
    SPI1->CR1 &= ~(SPI_CR1_BR );
    SPI1->CR1 |= SPI_CR1_BR_1;
    // Mode 0
    SPI1->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA );

    //8-bit
    SPI1->CR1 &= ~(SPI_CR1_DFF);
    SPI1->CR1 &= ~(SPI_CR1_LSBFIRST);

    // enable
    SPI1->CR1 |= SPI_CR1_SPE;

}

void spi_send_data(uint16_t data) {
    GPIOA->ODR &= ~GPIO_ODR_ODR_4;
    SPI1->DR = data;
    while(!(SPI1->SR & SPI_SR_TXE)){};
    GPIOA->ODR |= GPIO_ODR_ODR_4;
}