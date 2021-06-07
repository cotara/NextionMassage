#include "stm32f10x_it.h"
#include "stm32f10x_usart.h"
#include "user_TIMER.h"
#include "user_USART.h"
#include "myNextion.h"
#include <stdlib.h>
#define TX_BUFFER_SIZE 100

extern volatile uint8_t   tx_buffer[TX_BUFFER_SIZE];
extern volatile unsigned long  tx_wr_index,tx_rd_index,tx_counter;
uint32_t counter=0, ms_counter=0;
uint8_t waveformCounter=0, valvePowerCounter=0;
void HardFault_Handler(void){
  while (1)
  {}
}

void MemManage_Handler(void){
  while (1)
  {}
}

void BusFault_Handler(void){
   while (1)
  {}
}


void SysTick_Handler(void){
  TimingDelay_Decrement();
  ms_counter++;
  if(ms_counter==5000){
    ms_counter=0;
    if(getErrorTick()==0){
      GPIO_ResetBits(GPIOE,GPIO_Pin_5);                                          //Зажигаем светодиод
      switchOffAll();
    }
    else{
      GPIO_SetBits(GPIOE,GPIO_Pin_5);
      setErrorTick(0);
    }
  }
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/
//USART bads
void TIM3_IRQHandler(void){
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    TIM_Cmd(TIM3, DISABLE);
    setRxi(0);
}
//Control tiristor
void TIM2_IRQHandler(void){
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    //counter++;
    //GPIOD->ODR ^= GPIO_Pin_4;
    GPIO_ResetBits(GPIOA,GPIO_Pin_6);
    TIM_Cmd(TIM2, DISABLE);
}
//Control 1,2 valves
void TIM4_IRQHandler(void){
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    if(waveformCounter < getWaveform()*8/10){
      GPIO_SetBits(GPIOD,GPIO_Pin_1); 
      GPIO_SetBits(GPIOD,GPIO_Pin_2); 
    }
    else{
      GPIO_ResetBits(GPIOD,GPIO_Pin_1);
      GPIO_ResetBits(GPIOD,GPIO_Pin_2); 
    }
    waveformCounter++;
    if(waveformCounter >= getWaveform())
      waveformCounter=0;
}
//X ms for 5% change valve position
void TIM5_IRQHandler(void){
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
                              //reach position                                
    int8_t leftValvePower =  abs(getValvePower());
    if(leftValvePower>50)
      setValvePower(leftValvePower-1);                                          //slow close
    else if(leftValvePower<=0){//reach position 
        TIM_Cmd(TIM5, DISABLE);
        GPIO_SetBits(GPIOE,GPIO_Pin_6);                                         //Stops valve
        GPIO_SetBits(GPIOE,GPIO_Pin_0);
    }
    else{
      setValvePower(leftValvePower-5);
    }
    

}
void USART1_IRQHandler(void){
  if ((USART1->SR & USART_FLAG_RXNE))
   USART_IRQProcessFunc(USART_ReceiveData(USART1));    
  
  if(USART_GetITStatus(USART1, USART_IT_TC) == SET) {                          //прерывание по передаче
      if (tx_counter) {                                                           //если есть что передать
        --tx_counter;                                                             // уменьшаем количество не переданных данных
        USART_SendData(USART1,tx_buffer[tx_rd_index++]);                          //передаем данные инкрементируя хвост буфера
        if (tx_rd_index == TX_BUFFER_SIZE) tx_rd_index=0;                         //идем по кругу
      }
      else {
        USART_ITConfig(USART1, USART_IT_TC, DISABLE);                            //если нечего передать, запрещаем прерывание по передаче
      }
   } 
}


void EXTI0_IRQHandler(void) {
	/* Make sure that interrupt flag is set */
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
                GPIO_SetBits(GPIOA,GPIO_Pin_6);
                TIM_SetCounter(TIM2,(uint16_t)getMotorPower());
                TIM_Cmd(TIM2, ENABLE);
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}
/*******************************************************************************/

