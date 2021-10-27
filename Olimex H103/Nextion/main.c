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
  tim5_init(300);//default valve full open 3 se
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency /1000);
  while(1){
    //GPIOD->ODR ^= GPIO_Pin_0;
    if(getFLAG_END_LINE() == 3)
      nextionEvent();
  }
}
