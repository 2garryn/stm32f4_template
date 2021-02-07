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

void i2s_player_init() {
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    if(!(RCC->AHB1ENR & RCC_AHB1ENR_GPIOBEN)) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    }
    /*
        RCC->CR  
            PLLI2SON: PLLI2S enable
            PLLI2SRDY: PLLI2S clock ready flag

        RCC->CFGR
            I2SSRC: I2S clock selection

        RCC->PLLI2SCFGR
            PLLI2SR: PLLI2S division factor for I2S clocks = 2
            PLLI2SN: PLLI2S multiplication factor for VCO = 302

        I2SDIV = 53
        I2SODD = 1

    */

    // PLLI2SR = 2 = 0b010
    RCC->PLLI2SCFGR |= (RCC_PLLI2SCFGR_PLLI2SR_1);

    // PLLI2SN = 302 = 0b100101110
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

    SPI2->I2SCFGR |= (
        // i2s selected
        SPI_I2SCFGR_I2SMOD |
        // 10: Master - transmit 
        SPI_I2SCFGR_I2SCFG_1 | 
        // 01: MSB justified standard (left justified)
        SPI_I2SCFGR_I2SSTD_0
    );
    // Data length to be transferred - 16bit
    SPI2->I2SCFGR &= ~(SPI_I2SCFGR_DATLEN_0 | SPI_I2SCFGR_DATLEN_1);
    // Channel length (number of bits per audio channel) - 16bit
    SPI2->I2SCFGR &= ~SPI_I2SCFGR_CHLEN;

    // I2SDIV = 53
    SPI2->I2SPR |= (uint32_t) 53;
    // I2SODD set
    SPI2->I2SPR != SPI_I2SPR_ODD;


    // ENABLED!111
    SPI2->I2SCFGR |= SPI_I2SCFGR_I2SE;

}

void dma_init() {

}

void i2s_player_play(int level) {

}