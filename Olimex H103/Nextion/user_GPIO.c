#include "user_GPIO.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

void GPIO_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable USART1 and GPIOA clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOA, ENABLE);

                                //ШАР           //ЭМ1     //ЭМ2        //компрессор1//компрессор2 /реле включения цепи мотора
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;        		
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
  GPIO_Init(GPIOD, &GPIO_InitStructure);
                                //Комплектация1/Комплектация 2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15 ;        	        //Джамперы комплектаций	
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	                                //ловим 0 для семистора	
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		                        //Управление семистором
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
  GPIO_Init(GPIOA, &GPIO_InitStructure);		       
}



