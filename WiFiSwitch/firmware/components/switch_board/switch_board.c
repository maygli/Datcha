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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include "board_config.h"
#include "led.h"
#include "io_out.h"
#include "io_in.h"
#include "buzzer.h"
#include "switch_command.h"
#include "switch_board.h"

//const char *TAGS = "main";

// PWM period is 125 Hz
#define PWM_PERIOD 1000
#define UPDATE_PERIOD 20

static SwitchBoard  aBoard;

void boardUpdate( TimerHandle_t xTimer )
{
    updateLED(&aBoard.m_OnLED);
    updateLED(&aBoard.m_OffLED);
    pwm_start();
    updateBuzzer(&aBoard.m_Buzzer); 
}

void setBoardState(SwitchBoard* theBoard, SwitchState theState)
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

SwitchState getBoardState()
{
  return getBoardStateInt(&aBoard);
}


void initSwitchBoard(SwitchBoard* theBoard, xQueueHandle theQueue)
{
    theBoard->m_StateMutex=NULL;
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
    initGPIOIn(&theBoard->m_Input,PIN_INPUT, theQueue);

    TimerHandle_t anUpdTimer = xTimerCreate("timer100ms", pdMS_TO_TICKS(UPDATE_PERIOD), pdTRUE,
                     (void*)0, boardUpdate);
    xTimerStart(anUpdTimer, 0);
}

void onSwitch(SwitchBoard* theBoard)
{
    setBoardState(theBoard, SS_ON);
    onLED(&theBoard->m_OnLED);
    offLED(&theBoard->m_OffLED);

    onGPIOOut(&theBoard->m_Relay);
    playBuzzer(&theBoard->m_Buzzer);
}

void offSwitch(SwitchBoard* theBoard)
{
    setBoardState(theBoard, SS_OFF);
    onLED(&theBoard->m_OffLED);
    offLED(&theBoard->m_OnLED);

    offGPIOOut(&theBoard->m_Relay);
    playBuzzer(&theBoard->m_Buzzer);
}

void switchBoardTask(void *arg)
{
    xQueueHandle aQueue = (xQueueHandle)arg;
    initSwitchBoard(&aBoard, aQueue);
    SwitchCommand aCmd;
    offSwitch(&aBoard);

    for(;;){
        if (xQueueReceive(aQueue, &aCmd, portMAX_DELAY)) {
            switch(aCmd.m_Command){
            case CC_SWITCH_OFF:
              offSwitch(&aBoard);
              break;
            case CC_SWITCH_ON:
              onSwitch(&aBoard);
              break;
            case CC_SWITCH_REV:
              if( aBoard.m_State == SS_ON ){
                offSwitch(&aBoard);
              }
              else{
                onSwitch(&aBoard);
              }              
            }
            vTaskDelay(100 / portTICK_RATE_MS);
        }
    }
}
