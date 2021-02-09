#include "i2s_player.h"

/*
PCM5102A
16-bit audio data,
i2s, MSB-justified standard (left-justified)
44100khz
16-bit
Stereo

half-duplex (3 wires, I2S2ext_SD not used)

I2S2_WS - PB12 - AF5 (LR CLK)
I2S2_CK - PB13 - AF5
i2S2_SD - PB15 - AF5

I
I2S bitrate = number of bits per channel × number of channels × sampling audio frequency 
= 16 × 2 × 44100

*/

uint16_t data_arr[16] = {
    0xFFFF, 0, 0xFFFF, 0, 0xFFFF, 0, 0xFFFF, 0, 0xFFFF, 0, 0xFFFF, 0, 0xFFFF, 0, 0xFFFF, 0};


void i2s_player_init() {
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    /*
        CLOCKING
    */
    // PLLI2SR = 2 = 0b010
    RCC->PLLI2SCFGR |= (RCC_PLLI2SCFGR_PLLI2SR_1);

    // PLLI2SN = 302 = 0b100101110
    RCC->PLLI2SCFGR &= ~RCC_PLLI2SCFGR_PLLI2SN;
    RCC->PLLI2SCFGR |= (
        RCC_PLLI2SCFGR_PLLI2SN_1 | 
        RCC_PLLI2SCFGR_PLLI2SN_2 | 
        RCC_PLLI2SCFGR_PLLI2SN_3 | 
        RCC_PLLI2SCFGR_PLLI2SN_5 | 
        RCC_PLLI2SCFGR_PLLI2SN_8 
    );

    // Enable PLL I2S and wait it ready
    RCC->CR |= RCC_CR_PLLI2SON; 
    while((RCC->CR & RCC_CR_PLLI2SRDY) == 0) {};	

    RCC->CFGR &= ~RCC_CFGR_I2SSRC;

    /*
        GPIO
    */
    GPIOB->MODER |= (GPIO_MODER_MODER12_1 | GPIO_MODER_MODER13_1 | GPIO_MODER_MODER15_1);
    //Pull down
    GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR12_1 | GPIO_PUPDR_PUPDR13_1 | GPIO_PUPDR_PUPDR15_1) ;
    // High speed
	GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR12_1 | GPIO_OSPEEDER_OSPEEDR13_1 | GPIO_OSPEEDER_OSPEEDR15_1);
    // Alternate function AF5 (8.4.10 p286, Datasheet table 9, p63)
    GPIOB->AFR[1] &= ~(
        GPIO_AFRH_AFSEL12 | 
        GPIO_AFRH_AFSEL13 | 
        GPIO_AFRH_AFSEL15);
    GPIOB->AFR[1] |= (
        GPIO_AFRH_AFSEL12_0 | GPIO_AFRH_AFSEL12_2 | 
        GPIO_AFRH_AFSEL13_0 | GPIO_AFRH_AFSEL13_2 | 
        GPIO_AFRH_AFSEL15_0 | GPIO_AFRH_AFSEL15_2);

    /*
        I2S
    */
    // I2SDIV = 53
    SPI2->I2SPR &= (uint8_t) 0;
    SPI2->I2SPR |= (uint8_t) 53;

    // I2SODD set
    SPI2->I2SPR |= SPI_I2SPR_ODD;

    SPI2->I2SPR &= ~SPI_I2SPR_MCKOE;

    SPI2->I2SCFGR |= (
        // i2s selected
        SPI_I2SCFGR_I2SMOD |
        // 10: Master - transmit 
        SPI_I2SCFGR_I2SCFG_1 | 
        // 01: MSB justified standard (left justified)
        SPI_I2SCFGR_I2SSTD_0
    );
    // Data length to be transferred - 16bit
    SPI2->I2SCFGR &= ~SPI_I2SCFGR_DATLEN;
    // Channel length (number of bits per audio channel) - 16bit
    SPI2->I2SCFGR &= ~SPI_I2SCFGR_CHLEN;
    SPI2->CR2 |= SPI_CR2_TXDMAEN;

    /*
        DMA
    */
    //Stream 4, Channel 0
    DMA1_Stream4->CR &= ~(DMA_SxCR_CHSEL);
    DMA1_Stream4->CR |= (
        // half-word (16-bit)
        DMA_SxCR_MSIZE_0 | 
        // half-word (16-bit)
        DMA_SxCR_PSIZE_0 | 
        // Circular mode
        DMA_SxCR_CIRC |
        // Memory incremental mode
        DMA_SxCR_MINC |
        // Memory to peripheral
        DMA_SxCR_DIR_0
        );

    // Data items (size of array)
    DMA1_Stream4->NDTR = (uint16_t) 16;
    // Periphery address
    DMA1_Stream4->PAR = (uint32_t) &(SPI2->DR);
    // Memory address
    DMA1_Stream4->M0AR = (uint32_t) &data_arr[0];
}

void i2s_send(uint16_t data) {
    while((SPI2->SR & SPI_SR_TXE) == 0) {};
    SPI2->DR = data;
}


void i2s_player_enable() {
    // ENABLE
    DMA1_Stream4->CR |= DMA_SxCR_EN;
    SPI2->I2SCFGR |= SPI_I2SCFGR_I2SE; 
}

void i2s_player_disable() {
    SPI2->I2SCFGR &= ~SPI_I2SCFGR_I2SE;
}


void i2s_player_play(int level) {

}