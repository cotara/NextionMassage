#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "user_USART.h"
#include "user_GPIO.h"
#include "user_TIMER.h"
#include "myNextion.h"

int main()
{    
  GPIO_init();     
  usart_init();
  tim3_init();//USART ERROR
  tim2_init();
  tim4_init();
  while(1){
    //GPIOD->ODR ^= GPIO_Pin_0;
    if(getFLAG_END_LINE() == 3)
      nextionEvent();
  }
}
