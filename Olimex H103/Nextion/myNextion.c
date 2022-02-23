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
uint8_t page,element,value,waveform=150,valvePower,motorPower=0, bigValveFreq =0, nowMode=0;
int8_t valveDiff=0;
uint32_t errorTick=0;

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
      
      /********************************MASSAGE 1*******************************/
      if(page==3){
        nowMode = 0;
        if(element == 0){                                                       //СТАРТ МАССАЖ 1 
            motorPower=value;  
            if(value!=0)                                                        
              M_ON;                                                             //реле вкл мотора
            else 
              M_OFF;
            
            if(bigValveFreq == 0){
              waveform=0;
              BIGVALVE_OPEN;
            }
            else
              waveform=100/bigValveFreq;                                         //Устанавливаем частоту переключения большого клапана
            
            TIM_Cmd(TIM4, ENABLE);                                            //запускаем алгоритм щелкания клапанами
            sendAck();
        }
        else if(element == 1 || element==3){                                    //СТОП / ПАУЗА МАССАЖ 1
          M_OFF; 
          TIM_Cmd(TIM4, DISABLE);
          sendAck();
        }
        else if(element==5 || element==6){                                      //-/+мощность
          sendAck();
        } 
        else if(element==7 || element==8){                                      //-/+частота
          bigValveFreq = value;
          if(bigValveFreq == 0){
              waveform=0;
              BIGVALVE_OPEN;
            }
            else
              waveform=100/bigValveFreq;                                         //Устанавливаем частоту переключения большого клапана
          sendAck();
        }
        else if(element==127){                                                  //Выход из второго массажа
          sendAckExit();
          bigValveFreq=0;                                                           //Устанавливаем форму массажа в первую
        }    
      }
      
      /********************************MASSAGE 2*******************************/
      else if(page==4){                                                         
        nowMode = 1;
        if(element == 0){                                                        //СТАРТ МАССАЖ 2
         if(value!=0){
             POMP_ON;                                                          //включаем компрессоры
             TIM_Cmd(TIM4, ENABLE);                                             //запускаем алгоритм щелкания клапанами
          }
          else{
            POMP_OFF;                                        
            TIM_Cmd(TIM4, DISABLE);
          }
          sendAck();
        } 
        else if(element == 1){                                                  //ПАУЗА МАССАЖ 2
          POMP_OFF;                                        
          TIM_Cmd(TIM4, DISABLE);
          sendAck();
        }
        else if(element==3){                                                     //СТОП МАССАЖ 2
          POMP_OFF;                                        
          VALVE1_OPEN;                                                                  
          VALVE2_OPEN;
          TIM_Cmd(TIM4, DISABLE);
          sendAck();
        }
        else if(element==2){                                                    ////форма сигнала 150 - меандр, 40-ногодрыг, 10- синус
          waveform=value;
          sendAck();
        } 
        else if(element==5){                                                     //-мощность
          setSharPos();
          sendAck();
        } 
        else if(element==6){                                                     //+мощность
          setSharPos();
          sendAck();
        }
        else if(element==126){                                                  //Вход во второй режим массажа
          tim5_init(((uint16_t)value-7)*5);                                     //Инициализируем калибровку шарового клапана
        }  
        else if(element==127){                                                  //Выход из второго массажа
          sendAckExit();
          VALVE1_OPEN;                                                                  
          VALVE2_OPEN;
          value=255;                                                            //Закрываем шар
          setSharPos();
          waveform=150;                                                           //Устанавливаем форму массажа в первую
        }        
      } 
      
      /********************************MANUAL*********************************/
      else if(page==11){                                                        
        if(element == 0){                                                       //Электромагнитный клапан 1
          if(value==1){
            VALVE1_CLOSE;
            sendAck();
          }
          else if(value==0){
            VALVE1_OPEN;
            sendAck();
          }
        }
        else if(element == 1){                                                  //Электромагнитный клапан 2
          if(value==1){
            VALVE2_CLOSE;
            sendAck();
          }
          else if(value==0){
            VALVE2_OPEN;
            sendAck();
          }
        } 
        else if(element == 2){                                                  //Регулируемый клапан (шар)
          setSharPos();
          sendAck();
        } 
        else if(element == 3){                                                  //Компрессор 1
          if(value!=0)          
            POMP_ON;
          else 
            POMP_OFF;  
          sendAck();
        }
        else if(element == 4){                                                  //Большой клапан
          if(value!=0)          
            BIGVALVE_OPEN;
          else 
            BIGVALVE_CLOSE;  
          sendAck();
        }         
        else if(element == 5){                                                  //Мотор насоса
            motorPower=value;
            if(value!=0)          
              M_ON;
            else 
              M_OFF; 
            sendAck();
        }
        else if(element == 6){                                                  //Калибровка шара
          if(value!=0){
            if(value!=255)
              if(value>10)                                                      //Чтобы при вычитании 10 не получилось отрицательное число
                tim5_init(((uint16_t)value-10)*5);                                 //Устанавливаем откалиброванное время c поправкой 1000 мс на реакцию пользователя
            value=255;                                                          //Возващаем клапан на место
          }
          else{                                                                 //Команда калибровки
            tim5_init(1000);                                                    //Включаем клапан на открытие в течение 10 секунд ( с запасом)
            value=100;  
          }
          setSharPos();
          sendAck();
        }
        else if(element==127){                                                  //Выход со страницы ручного управления
          sendAckExit();
          switchOffAll();
        }
      }
      /********************************EDITION*********************************/
      else if(page==2){                                                         //Запрос комплектации
        if(element==0){
          if(value==1){
            st = IS_VALVE_IN_LPG*2 + IS_VAKUUM;
            Nextion_SetValue_Number("mode.val", st);
          }
        }     
      }
      /*********************************Проверка связи*************************/
      else if(page==1){                                                         
        if(element==0){
          if(value==1){
            errorTick++;
          }
        }     
      }
 clear_RXBuffer();
}

uint32_t getErrorTick(){
  return errorTick;
}
void setErrorTick(uint32_t tick){
  errorTick=tick;
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
uint8_t getJP(){
  return st;
}
uint8_t getMode(){
  return nowMode;
}
uint8_t getMotorPower(){
  return motorPower;
}
uint8_t getWaveform(){
  return waveform;
}
int8_t getValvePower(){
  return valveDiff;
}
void setValvePower(int8_t diff){
   valveDiff = diff;
}
//подтверждение получния управляющего сообщения
void sendAck(){
  Nextion_SetValue_Number("transpState.val", 0);
}
void sendAckExit(){
  Nextion_SetValue_Number("transpState.val", 2);
}
void setSharPos(){
    if(value==0)
      valveDiff=-50;                                                           //С запасом, чтобы точно закрылся
    else if(value==100)
      valveDiff=50;                                                            //С запасом, чтобы точно открылся
    else if(value==255)                                                         //Полное закрытие не с +5%
      valveDiff=-127;
    else
      valveDiff=value-valvePower;                                               //На сколько повернуть клапан
    if(valveDiff!=0){                                                           //Если надо хоть на сколько повернуть
      if(value==255)
        valvePower=0;
      else
        valvePower=value;                                                       //Сохраняем новое положение клапана
      if(valveDiff>0)
        SHAR_CLOSE;                                                             //Закрываем
      else
        SHAR_OPEN;                                                              //Открываем
      SHAR_START;
      LED1_ON;
      TIM5->CNT=0;
      TIM_Cmd(TIM5, ENABLE);                                                    //Включаем клапан повотора 
    }   
}
void switchOffAll(){
  
   GPIO_SetBits(GPIOD,GPIO_Pin_1);                                              //Открыть клапана
   GPIO_SetBits(GPIOD,GPIO_Pin_2);
   value=255;                                                                   //Закрываем шар
   setSharPos();
   GPIO_ResetBits(GPIOD,GPIO_Pin_3);                                            //Выключаем компрессоры
   GPIO_ResetBits(GPIOD,GPIO_Pin_4); 
   GPIO_ResetBits(GPIOD,GPIO_Pin_5);                                            //Выключаем мотор
   TIM_Cmd(TIM5, DISABLE);                                                      //Таймер поворота шара
   TIM_Cmd(TIM4, DISABLE);                                                      //Таймер дрыгания клапанов
}
  