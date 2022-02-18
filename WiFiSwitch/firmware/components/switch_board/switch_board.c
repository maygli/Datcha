#include "driver/pwm.h"
#include "driver/gpio.h"
#include "esp_log.h"
/*
 * Copyright (c) 2022 Maygli (mmaygli@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/timers.h>
#include <semphr.h>

#include <esp_log.h>
#include <esp_system.h>    

#include "board_config.h"
#include "led.h"
#include "io_out.h"
#include "io_in.h"
#include "buzzer.h"
#include "switch_command.h"
#include "switch_board.h"

//const char *TAGS = "main";
static const char TAG[] = "switch_board";

// PWM period is 125 Hz
#define PWM_PERIOD 1000
#define UPDATE_PERIOD 20
#define RESET_DEALY 1000

typedef struct _SwitchBoard{
    LED         	    m_OnLED;
    LED         	    m_OffLED;
    Buzzer      	    m_Buzzer;
    GPIOOut     	    m_Relay;
    GPIOIn      	    m_Input;
    SwitchState 	    m_State;
    bool                m_isSoundOn;
    xSemaphoreHandle 	m_StateMutex;
    xQueueHandle        m_Queue;
} SwitchBoard;

static SwitchBoard  aBoard;

SwitchState getBoardStateInt(SwitchBoard* theBoard);
void setBoardStateInt(SwitchBoard* theBoard, SwitchState theState);
void initSwitchBoard(SwitchBoard* theBoardx);
void offSwitch(SwitchBoard* theBoard);
void onSwitch(SwitchBoard* theBoard);

void SWB_switchBoardTask(void *arg)
{
    aBoard.m_Queue = xQueueCreate(10, sizeof(SwitchCommand));
    initSwitchBoard(&aBoard);
    SwitchCommand aCmd;
    offSwitch(&aBoard);

    for(;;){
        if (xQueueReceive(aBoard.m_Queue, &aCmd, portMAX_DELAY)) {
            switch(aCmd.m_Command){
            case CC_SWITCH_OFF:
              ESP_LOGI("switch", "Switch. Off called");
              offSwitch(&aBoard);
              break;
            case CC_SWITCH_ON:
              ESP_LOGI("switch", "Switch. On called");
              onSwitch(&aBoard);
              break;
            case CC_SWITCH_REV:
              ESP_LOGI("switch", "Switch. Revert called");
              if( aBoard.m_State == SS_ON ){
                offSwitch(&aBoard);
              }
              else{
                onSwitch(&aBoard);
              }
              break;
            case CC_SOUND_ON:
              aBoard.m_isSoundOn = (bool)aCmd.m_Parameter;
              enableBuzzer(&aBoard.m_Buzzer, aBoard.m_isSoundOn);
              break;
            case CC_OFF_LED_BR:
              setBrightness(&aBoard.m_OffLED, aCmd.m_Parameter);
              break;
            case CC_ON_LED_BR:
              setBrightness(&aBoard.m_OnLED, aCmd.m_Parameter);
              break;
            case CC_STYLE:{
              ESP_LOGI(TAG, "Set style %d",aCmd.m_Parameter);
              if( aCmd.m_Parameter == 0 ){
                  aBoard.m_OnLED.m_Channel = 0;
                  aBoard.m_OffLED.m_Channel = 1;
              }
              else{
                  aBoard.m_OnLED.m_Channel = 1;
                  aBoard.m_OffLED.m_Channel = 0;
              }
              adjustLED(&aBoard.m_OnLED);
              adjustLED(&aBoard.m_OffLED);
              pwm_start();
              break;
            }
            case CC_RESET:
                vTaskDelay(RESET_DEALY/portTICK_PERIOD_MS);
                esp_restart();
                break;
            }
            vTaskDelay(100 / portTICK_RATE_MS);
        }
    }
}

SwitchState SWB_getBoardState()
{
  return getBoardStateInt(&aBoard);
}

void SWB_setBoardState(SwitchState theState)
{
    SwitchCommand aCmd;
    aCmd.m_Command = CC_SWITCH_OFF;
    if( theState == SS_ON ){
        aCmd.m_Command = CC_SWITCH_ON;
    }
    aCmd.m_Parameter = 0; 
    xQueueSend(aBoard.m_Queue, &aCmd, NULL);
}

void SWB_setOnBrightness(uint8_t theVal)
{
    SwitchCommand aCmd;
    aCmd.m_Command = CC_ON_LED_BR;
    aCmd.m_Parameter = theVal; 
    xQueueSend(aBoard.m_Queue, &aCmd, NULL);
}

void SWB_setOffBrightness(uint8_t theVal)
{
    SwitchCommand aCmd;
    aCmd.m_Command = CC_OFF_LED_BR;
    aCmd.m_Parameter = theVal; 
    xQueueSend(aBoard.m_Queue, &aCmd, NULL);
}

void SWB_soundOn(bool theVal)
{
    SwitchCommand aCmd;
    aCmd.m_Command = CC_SOUND_ON;
    aCmd.m_Parameter = theVal;
    xQueueSend(aBoard.m_Queue, &aCmd, NULL);
}

void SWB_setStyle(uint8_t theStyle)
{
    SwitchCommand aCmd;
    aCmd.m_Command = CC_STYLE;
    aCmd.m_Parameter = theStyle;
    xQueueSend(aBoard.m_Queue, &aCmd, NULL);
}

void SWB_reset()
{
    SwitchCommand aCmd;
    aCmd.m_Command = CC_RESET;
    aCmd.m_Parameter = 0;
    xQueueSend(aBoard.m_Queue, &aCmd, NULL);
}

void boardUpdate( TimerHandle_t xTimer )
{
    updateLED(&aBoard.m_OnLED);
    updateLED(&aBoard.m_OffLED);
    pwm_start();
    updateBuzzer(&aBoard.m_Buzzer); 
}

void setBoardStateInt(SwitchBoard* theBoard, SwitchState theState)
{
    if( theBoard->m_StateMutex ){
        if( xSemaphoreTake( theBoard->m_StateMutex, portMAX_DELAY ) == pdTRUE )
        {
            theBoard->m_State = theState;
            xSemaphoreGive( theBoard->m_StateMutex );
        }
    }
}

SwitchState getBoardStateInt(SwitchBoard* theBoard)
{
    SwitchState aRes=SS_OFF;
    if( theBoard->m_StateMutex ){
        if( xSemaphoreTake( theBoard->m_StateMutex, portMAX_DELAY ) == pdTRUE )
        {
            aRes = theBoard->m_State;
            xSemaphoreGive( theBoard->m_StateMutex );
        }
    }
    return aRes;
}

void onSwitch(SwitchBoard* theBoard)
{
    setBoardStateInt(theBoard, SS_ON);
    onLED(&theBoard->m_OnLED);
    offLED(&theBoard->m_OffLED);

    onGPIOOut(&theBoard->m_Relay);
    if( theBoard->m_isSoundOn )
        playBuzzer(&theBoard->m_Buzzer);
}

void offSwitch(SwitchBoard* theBoard)
{
    setBoardStateInt(theBoard, SS_OFF);
    onLED(&theBoard->m_OffLED);
    offLED(&theBoard->m_OnLED);

    offGPIOOut(&theBoard->m_Relay);
    if( theBoard->m_isSoundOn )
        playBuzzer(&theBoard->m_Buzzer);
}

void initSwitchBoard(SwitchBoard* theBoard)
{
    theBoard->m_StateMutex=NULL;
    theBoard->m_isSoundOn = true;
    //PWM pins
    const uint32_t aPins[LED_COUNT] = {
        PIN_LED_ON,
        PIN_LED_OFF
    };
    //Off all LEDs
    uint32_t aDuties[LED_COUNT] = {
        1,
        1
    };    
    //Init PWMs
    pwm_init(PWM_PERIOD, aDuties, LED_COUNT, aPins);
    pwm_set_phase(0,0);
    pwm_set_phase(1,0);
    //Start PWMs
    pwm_start();
    //Create semaphore to access to board state
    theBoard->m_StateMutex = xSemaphoreCreateMutex();
    //Create input queue

    //install gpio isr service
    gpio_install_isr_service(0);

    initLED(&theBoard->m_OnLED,0);
    initLED(&theBoard->m_OffLED,1);
    initGPIOOut(&theBoard->m_Relay,PIN_RELAY);
    initBuzzer(&theBoard->m_Buzzer,PIN_BUZER);
    initGPIOIn(&theBoard->m_Input,PIN_INPUT, aBoard.m_Queue);

    TimerHandle_t anUpdTimer = xTimerCreate("timer100ms", pdMS_TO_TICKS(UPDATE_PERIOD), pdTRUE,
                     (void*)0, boardUpdate);
    xTimerStart(anUpdTimer, 0);
}

