#include "stm32f4xx.h"

void usart_init();
void usart_write(volatile char *s);
void usart_putc(unsigned char c);

#define LOG(s) usart_write(s)
