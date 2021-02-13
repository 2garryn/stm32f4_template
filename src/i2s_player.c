#include "i2s_player.h"
#include "freq_debug.h"
#include <string.h>

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

/*
uint16_t data_arr0[16] = {
    0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0   //, 0xFFFF, 0, 0xFFFF, 0, 0xFFFF, 0, 0xFFFF, 0, 0xFFFF, 0, 0xFFFF, 0
};

uint16_t data_arr1[16] = {
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF //, 0xFF, 0, 0xFF, 0, 0xFF, 0, 0xFF, 0, 0xFF, 0, 0xFF, 0
};
*/


#define BUFFER_SIZE 1024

uint16_t data_arr0[BUFFER_SIZE];

volatile uint32_t play_length; 

volatile uint16_t* sample;

volatile uint32_t velocity_level;


void copy_data(uint16_t* dest, int start, int stop) {
    volatile int16_t* signed_value;
    volatile float leveled;
    volatile int16_t rounded;
    for(int i = start; i < stop; i++) {
        if(play_length) {
            if(velocity_level > 126) {
                dest[i] = *sample;
            } else {
                signed_value = (int16_t*) sample;
                leveled = ((float) velocity_level / 127) * (*signed_value);
                rounded = (int16_t) leveled;
                dest[i] = *((uint16_t*) &rounded);
            }
            sample++;
            play_length--;
        } else {
            dest[i] = 0;
        }
    }
}



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

    SPI2->I2SCFGR &= ~(SPI_I2SCFGR_I2SSTD_0 | SPI_I2SCFGR_I2SSTD_1);

    SPI2->I2SCFGR |= (
        // i2s selected
        SPI_I2SCFGR_I2SMOD |
        // 10: Master - transmit 
        SPI_I2SCFGR_I2SCFG_1 //| 
        // 01: MSB justified standard (left justified)
        //SPI_I2SCFGR_I2SSTD_0
    );
    // Data length to be transferred - 16bit
    SPI2->I2SCFGR &= ~SPI_I2SCFGR_DATLEN;
    // Channel length (number of bits per audio channel) - 16bit
    SPI2->I2SCFGR &= ~SPI_I2SCFGR_CHLEN;
    SPI2->CR2 |= SPI_CR2_TXDMAEN;

    //Stream 4, Channel 0
    DMA1_Stream4->CR &= ~(DMA_SxCR_CHSEL);
    DMA1_Stream4->CR |= (
        // half-word (16-bit)
        DMA_SxCR_MSIZE_0 | 
        // half-word (16-bit)
        DMA_SxCR_PSIZE_0 | 
        // Circular mode (automatically enabled on double buffered mode. Left as reminer)
        DMA_SxCR_CIRC |
        // Memory incremental mode
        DMA_SxCR_MINC |
        // Memory to peripheral
        DMA_SxCR_DIR_0 |
        // Double buffered mode
        //DMA_SxCR_DBM | 
        // Transfer complete interrupt enable
        DMA_SxCR_TCIE |
        // Half transfer interrupt enable
        DMA_SxCR_HTIE 
    );

    // Data items (size of array)
    DMA1_Stream4->NDTR = (uint16_t) BUFFER_SIZE;
    // Periphery address
    DMA1_Stream4->PAR = (uint32_t) &(SPI2->DR);
    // Memory address (first buffer)
    DMA1_Stream4->M0AR = (uint32_t) &data_arr0[0];
    // Memory address (second buffer)
    //DMA1_Stream4->M1AR = (uint32_t) &data_arr1[0];

    NVIC_EnableIRQ(DMA1_Stream4_IRQn);
}

void DMA1_Stream4_IRQHandler() {

    //Stream4 transfer complete
    if((DMA1->HISR & DMA_HISR_TCIF4) != 0) {
        // Stream4 clear transfer complete interrupt flag
        DMA1->HIFCR |= DMA_HIFCR_CTCIF4;
        //freq_debug_switch();
        copy_data(data_arr0, 512, 1024);
        return;
    }

    //Stream4 half tranfer 
    if((DMA1->HISR & DMA_HISR_HTIF4) != 0) {
        // Stream4 clear half transfer interrupt flag
        DMA1->HIFCR |= DMA_HIFCR_CHTIF4;
        copy_data(data_arr0, 0, 512);
        return;
    }
}


void i2s_send(uint16_t data) {
    SPI2->DR =  data;
    while (!(SPI2->SR & SPI_SR_TXE));
}



void i2s_player_enable() {
    // ENABLE
    DMA1_Stream4->CR |= DMA_SxCR_EN;
    SPI2->I2SCFGR |= SPI_I2SCFGR_I2SE; 
}

void i2s_player_disable() {
    SPI2->I2SCFGR &= ~SPI_I2SCFGR_I2SE;
    DMA1_Stream4->CR &= ~DMA_SxCR_EN;
}

uint16_t sine[64] = {32768,35979,39160,42279,45307,48214,50972,53555,55938,58097,60013,61666,63041,64124,64905,65377,
    65535,65377,64905,64124,63041,61666,60013,58097,55938,53555,50972,48214,45307,42279,39160,35979,
    32768,29556,26375,23256,20228,17321,14563,11980,9597,7438,5522,3869,2494,1411,630,158,
    0,158,630,1411,2494,3869,5522,7438,9597,11980,14563,17321,20228,23256,26375,29556};

volatile uint16_t sine2[29] = {29556,26375,23256,20228,17321,14563,11980,9597,7438,5522,3869,2494,1411,630,158,630,1411,2494,3869,5522,7438,9597,11980,14563,17321,20228,23256,26375,29556};

volatile uint16_t sine3[29] = {158,630,1411,2494,3869,5522,7438,9597,11980,14563,17321,20228,23256,26375,29556,26375,23256,20228,17321,14563,11980,9597,7438,5522,3869,2494,1411,630,158};
void i2s_play_array() {
    while(1) {
        for(volatile int i = 0; i < 29; i++) {
            i2s_send(sine3[i]);
            i2s_send(sine3[i]);
        }
    }
}

void i2s_player_play2(uint32_t length, volatile uint16_t* offset) {
    for(volatile int i = 0; i < length; i++) {
        i2s_send(*offset);
        offset++;
    }
}


void i2s_player_play(uint32_t length, uint16_t* offset, uint32_t level) {
    i2s_player_disable();
    velocity_level = level;
    play_length = length;
    sample = offset;
    copy_data(data_arr0, 0, 1024);
    i2s_player_enable();
}
