#include "flash.h"

void flash_init() {}
 
uint32_t flash_pointer(uint32_t offset) {
    return INTERNAL_DATA_OFFSET + offset;
}