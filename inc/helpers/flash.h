#include "stm32f4xx.h"

// we start from SECTOR 6 (RM0090 page 75)
#define INTERNAL_DATA_OFFSET  0x08040000


void flash_init();
uint32_t flash_pointer(uint32_t offset);