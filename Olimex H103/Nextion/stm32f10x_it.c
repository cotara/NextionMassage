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
  //Если за 5 секунд ни разу не связались, то отрубаем все.
  if(ms_counter==5000){
    ms_counter=0;
    if(getErrorTick()==0){
      LED2_ON;                                          //Зажигаем светодиод
      switchOffAll();
      Nextion_SetValue_Number("mode.val", 4);
    }
    else{
      LED2_OFF;
      setErrorTick(0);
    }
  }
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/
//USART fail control
void TIM3_IRQHandler(void){
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    TIM_Cmd(TIM3, DISABLE);
    setRxi(0);
}
//Control tiristor
void TIM2_IRQHandler(void){
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    //GPIOD->ODR ^= GPIO_Pin_4;
    M_PWM_ON;
    TIM_Cmd(TIM2, DISABLE);
}
//Control 1,2 valves
void TIM4_IRQHandler(void){
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    counter+=10;
    
    if(waveformCounter <= getWaveform()*5/10){
      if(getMode()==0 &&(getJP()==0 || getJP()==1))                              //Проверяем, что мы на странице LPG и комплектация с большим клапаном                                                             //Клапан в связке с LPG
        BIGVALVE_OPEN;    
      else{
        VALVE1_OPEN;                                                            //вакуумные клапаны на 24 в
        VALVE2_OPEN;
      }
    }
    else{
      if(getMode()==0 &&(getJP()==0 || getJP()==1))
        BIGVALVE_CLOSE;
      else{
        VALVE1_CLOSE;
        VALVE2_CLOSE;
      }
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
        SHAR_STOP;                                                              //Stops shar
        LED1_OFF;
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
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
                M_PWM_OFF;
                TIM_SetCounter(TIM2,(uint16_t)getMotorPower());
                TIM_Cmd(TIM2, ENABLE);
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}
/*******************************************************************************/

