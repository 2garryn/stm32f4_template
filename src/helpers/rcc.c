#include "rcc.h"


/*

multiplier = 168000000 / 4000000;
 mills = multiplier * mills * 1000 - 10;
 mills = 42 * 1000 * 1000 - 10

*/

void rcc_init() {
  // HSE is enabled
  RCC->CR |= RCC_CR_HSEON;
  while( !(RCC->CR & RCC_CR_HSERDY) ) {};

  // HSE is PLL source
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;
  RCC->CR &= ~(RCC_CR_PLLON); 

  // set PLL P prescaler (0b00 = 2)
  RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP); 

  // clear PLL N 
  RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN); 	
  // set PLL N (0b101010000 = 336)
  RCC->PLLCFGR |= (RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_8);

  // clear PLL M
  RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM); 
  // set PLL M prescaler (0b001000 = 8)
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_3;

  // AHB Prescaler (not divided)
  RCC->CFGR &= ~(RCC_CFGR_HPRE); 				
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1; 

  //APB1 Prescaler (divide by 4)
  RCC->CFGR &= ~(RCC_CFGR_PPRE1);
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

  //APB2 Prescaler (divide by 2)
  RCC->CFGR &= ~(RCC_CFGR_PPRE2);
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

  //PLL enable
  RCC->CR |= RCC_CR_PLLON; 					
  // Wait PLL is ready			
  while((RCC->CR & RCC_CR_PLLRDY) == 0) {} ;

  FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;
  //PLL System
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL; 					
  while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {};
}
