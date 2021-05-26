#include "myNextion.h"
#include "user_USART.h"
#include "user_TIMER.h"
#include "user_USART.h"
#include "stm32f10x_gpio.h"
#include <stdio.h>
#include <string.h>

uint8_t RX_FLAG_END_LINE = 0;
uint8_t status[21]="t3.txt=";
uint8_t command[11]="sendme";
uint32_t st=0;
uint8_t endMes[3]={0xFF,0xFF,0xFF};
static uint8_t StrBuff[64]; 
uint8_t page,element,value,waveform=1,valvePower,pumpPower=0,nasosPower=0;

void USART_IRQProcessFunc(uint8_t RXc){
    toBuf(RXc);
    TIM_Cmd(TIM3, DISABLE);
    if (RXc == 0xFF)                                                            //Если пришел признак окончания команды (их должно быть 3)
       RX_FLAG_END_LINE++;
    if(RX_FLAG_END_LINE!=3)                                                     //Если команда пришла еще не полностью, запускаем таймер сброса
      TIM_Cmd(TIM3, ENABLE);
}

void nextionEvent(void){
      page = fromBuf(0);                                                        // номер страницы
      element = fromBuf(1);                                                     //номер кнопки (элемента). 
      value = fromBuf(3);                                                       //значение 
      
      RX_FLAG_END_LINE=0;

      if(page==3){                                                              //mode1
        if(element == 0)                                                        //СТАРТ МАССАЖ 1
          GPIO_SetBits(GPIOC,GPIO_Pin_12);
        else if(element == 1)                                                   //ПАУЗА МАССАЖ 1
          GPIO_ResetBits(GPIOC,GPIO_Pin_12);
        else if(element==3)                                                     //СТОП МАССАЖ 1
          GPIO_ResetBits(GPIOC,GPIO_Pin_12);
      }
      else if(page==4){                                                         //mode2
        if(element == 0)                                                        //СТАРТ МАССАЖ 2
          GPIO_SetBits(GPIOC,GPIO_Pin_13);
        else if(element == 1)                                                   //ПАУЗА МАССАЖ 2
          GPIO_ResetBits(GPIOC,GPIO_Pin_13);
        else if(element==3)                                                     //СТОП МАССАЖ 2
          GPIO_ResetBits(GPIOC,GPIO_Pin_13);
        else if(element==2){                                                    ////форма сигнала 1 - меандр, 2-ногодрыг, 3- синус
          waveform=value;
        }         
      } 
      else if(page==11){                                                        //manual
        if(element == 0){                                                       //Электромагнитный клапан 1
          if(value==1)
            GPIO_SetBits(GPIOD,GPIO_Pin_1);
          else if(value==0)
            GPIO_ResetBits(GPIOD,GPIO_Pin_1);
        }
        if(element == 1){                                                       //Электромагнитный клапан 2
          if(value==1)
            GPIO_SetBits(GPIOD,GPIO_Pin_2);
          else if(value==0)
            GPIO_ResetBits(GPIOD,GPIO_Pin_2);
        } 
        if(element == 2){                                                       //Регулируемый клапан (шар)
            valvePower=value;
        } 
        if(element == 3){                                                       //Компрессор
          pumpPower=value;
          if(value!=0)          
            GPIO_SetBits(GPIOD,GPIO_Pin_3);
          else if(value==0)
            GPIO_ResetBits(GPIOD,GPIO_Pin_3);                       
        }
        if(element == 4){                                                       //Компрессор
          pumpPower=value;
          if(value!=0)          
            GPIO_SetBits(GPIOD,GPIO_Pin_4);
          else if(value==0)
            GPIO_ResetBits(GPIOD,GPIO_Pin_4);                       
        }         
        if(element == 5){                                                       //Мотор насоса
            nasosPower=value;
        } 
      }
      else if(page==2){                                                         //Запрос комплектации
        if(element==0){
          if(value==1){
            st = GPIO_ReadInputDataBit (GPIOD, GPIO_Pin_14)*2 + GPIO_ReadInputDataBit (GPIOD, GPIO_Pin_15);
            Nextion_SetValue_Number("mode.val", st);
          }
        }
      }
     clear_RXBuffer();
}

void Nextion_SetValue_Number(char *ValueName, uint32_t Value)
{
  sprintf((char *)StrBuff, "%s=%d", ValueName, Value);
  uint16_t Len = strlen((char *)StrBuff);
  
  USART1_put_string(StrBuff, Len);
  USART1_put_string((uint8_t *)endMes, 3);
}
void Nextion_SetValue_String(char *ValueName, char *Value)
{
  sprintf((char *)StrBuff, "%s%s", ValueName, Value);
  uint16_t Len = strlen((char *)StrBuff);
  
  USART1_put_string(StrBuff, Len);
  USART1_put_string((uint8_t *)endMes, 3);
}

void incFLAG_END_LINE(void){
  RX_FLAG_END_LINE++;
}
void resetFLAG_END_LINE(void){
  RX_FLAG_END_LINE=0;
}
uint8_t getFLAG_END_LINE(void){
  return RX_FLAG_END_LINE;
}