#include "user_GPIO.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_exti.h"
void GPIO_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStruct;
  EXTI_InitTypeDef EXTI_InitStruct;
  /* Enable USART1 and GPIOA clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOA, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		                        //Управление семистором
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
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
  
  /* Add IRQ vector to NVIC */
  /* PB0 is connected to EXTI_Line0, which has EXTI0_IRQn vector */
  NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
  /* Set priority */
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
  /* Set sub priority */
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
  /* Enable interrupt */
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  /* Add to NVIC */
  NVIC_Init(&NVIC_InitStruct);

  /* Tell system that you will use PB0 for EXTI_Line0 */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);

  /* PB0 is connected to EXTI_Line0 */
  EXTI_InitStruct.EXTI_Line = EXTI_Line0;
  /* Enable interrupt */
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  /* Interrupt mode */
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  /* Triggers on rising and falling edge */
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  /* Add to EXTI */
  EXTI_Init(&EXTI_InitStruct);  
		       
}



