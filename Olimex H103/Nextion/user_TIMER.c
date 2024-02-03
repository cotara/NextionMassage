#include "user_TIMER.h"
#include "stm32f10x_tim.h"

volatile uint32_t TimingDelay;

void tim3_init(void){
    TIM_TimeBaseInitTypeDef TIMER_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseStructInit(&TIMER_InitStructure);
    TIMER_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIMER_InitStructure.TIM_Prescaler = 720-1;                                  //10 мкс
    TIMER_InitStructure.TIM_Period = 100;                                       //10 мс при 72 МГц
    TIM_TimeBaseInit(TIM3, &TIMER_InitStructure);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    //TIM_Cmd(TIM3, ENABLE);
  
    NVIC_SetPriority (TIM3_IRQn, 3);
    NVIC_EnableIRQ (TIM3_IRQn);
}

void tim2_init(void){
   
    TIM_TimeBaseInitTypeDef TIMER_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructInit(&TIMER_InitStructure);
    TIMER_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIMER_InitStructure.TIM_Prescaler = 7200-1;                                 //100мкс
    TIMER_InitStructure.TIM_Period = 100;                                       //10мс
    TIM_TimeBaseInit(TIM2, &TIMER_InitStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    //TIM_Cmd(TIM2, ENABLE);
  
    NVIC_SetPriority (TIM2_IRQn, 3);
    NVIC_EnableIRQ (TIM2_IRQn);
}

void tim4_init(void){
   
    TIM_TimeBaseInitTypeDef TIMER_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_TimeBaseStructInit(&TIMER_InitStructure);
    TIMER_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIMER_InitStructure.TIM_Prescaler = 36000-1;                                //500мкс
    TIMER_InitStructure.TIM_Period = 20-1;                                      //10мс
    TIM_TimeBaseInit(TIM4, &TIMER_InitStructure);
    TIM_ClearFlag(TIM4,TIM_IT_Update);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    //TIM_Cmd(TIM4, ENABLE);
  
    NVIC_SetPriority (TIM4_IRQn, 3);
    NVIC_EnableIRQ (TIM4_IRQn);
}
void tim5_init(uint16_t period){
   
    TIM_DeInit(TIM5);
    TIM_TimeBaseInitTypeDef TIMER_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    TIM_TimeBaseStructInit(&TIMER_InitStructure);
    TIMER_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIMER_InitStructure.TIM_Prescaler = 7200-1;                                 //100мкс
    TIMER_InitStructure.TIM_Period = 10*period-1;                                    
    TIM_TimeBaseInit(TIM5, &TIMER_InitStructure);
    TIM_ClearFlag(TIM5,TIM_IT_Update);
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
    //TIM_Cmd(TIM5, ENABLE);
  
    NVIC_SetPriority (TIM5_IRQn, 3);
    NVIC_EnableIRQ (TIM5_IRQn);
}
/*
void tim4_init(void){
    
	GPIO_InitTypeDef port;
	TIM_TimeBaseInitTypeDef timer;
	TIM_OCInitTypeDef timerPWM;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	GPIO_StructInit(&port);
	port.GPIO_Mode = GPIO_Mode_AF_PP;
	port.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &port);

	TIM_TimeBaseStructInit(&timer);
	timer.TIM_Prescaler = 240-1;
	timer.TIM_Period = 100;
	timer.TIM_ClockDivision = 0;
	timer.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &timer);

	TIM_OCStructInit(&timerPWM);
	timerPWM.TIM_Pulse = 0;
	timerPWM.TIM_OCMode = TIM_OCMode_PWM1;
	timerPWM.TIM_OutputState = TIM_OutputState_Enable;
	timerPWM.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM4, &timerPWM);
	TIM_OC2Init(TIM4, &timerPWM);
	TIM_OC3Init(TIM4, &timerPWM);

      
    TIM_Cmd(TIM4, ENABLE);
}
*/

/*Задержка в nTime 10мс*/
void Delay(volatile uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}
/**/
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

