#include <stdint.h>

void usart_init(void);
void clear_RXBuffer(void);
void setRxi(uint16_t i);
uint8_t toBuf(uint8_t n);
uint8_t toBuf(uint8_t n);
uint8_t fromBuf( uint16_t i);
void USART1_put_char(uint8_t c);
void USART1_put_string(unsigned char *string, uint32_t l);
