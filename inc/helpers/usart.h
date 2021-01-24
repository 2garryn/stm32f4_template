#include "stm32f4xx.h"

void usart_init();
void usart_write(volatile char *s);
void usart_putc(unsigned char c);
void usart_write_uint32(uint32_t v);

#define LOG(s) usart_write(s)
