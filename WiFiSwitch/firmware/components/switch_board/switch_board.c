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
#include "meteo_board.h"

//const char *TAGS = "main";
static const char TAG[] = "switch_board";

// PWM period is 125 Hz
#define PWM_PERIOD 1000
#define UPDATE_PERIOD 20
#define RESET_DEALY 1000

typedef struct _StateDesc{
    StateType               m_StateType;
    ControlSource           m_ControlSource;
    InternalControlSource   m_InternalSource;
    ControlType             m_ControlType;
} StateDesc;

typedef struct _SwitchBoard{
    LED         	    m_LED[LED_COUNT];
    Buzzer      	    m_Buzzer;
    GPIOOut     	    m_Relay;
    GPIOIn      	    m_Input;
    int 	            m_CurrState;
    int                 m_StateCout;
    StateDesc*          m_States;
    double              m_Limit;
    double              m_Value;
    bool                m_isSoundOn;    
    uint8_t*            m_Brightness;
    xSemaphoreHandle 	m_StateMutex;
    xQueueHandle        m_Queue;
    xQueueHandle*       m_NotifyQueue;
} SwitchBoard;

static SwitchBoard  aBoard;

int  getBoardStateInt(SwitchBoard* theBoard);
void setBoardStateInt(SwitchBoard* theBoard, int theState);
void initSwitchBoard(SwitchBoard* theBoardx);
void offSwitch(SwitchBoard* theBoard);
void onSwitch(SwitchBoard* theBoard);
void controlSwitch(SwitchBoard* theBoard);
void initState(StateDesc* theStateDesc);
void updateBoardState(SwitchBoard* theBoard);
void updateControlledOut(SwitchBoard* theBoard);

void SWB_switchBoardTask(void *arg)
{
    aBoard.m_NotifyQueue = (xQueueHandle*)arg;
    aBoard.m_Queue = xQueueCreate(10, sizeof(SwitchCommand));
/*************** Touch switch *****************/    
    aBoard.m_StateCout = 2;
    aBoard.m_CurrState = 0;
/*************** Touch switch *****************/    
    aBoard.m_States = malloc(aBoard.m_StateCout * sizeof(StateDesc));
    aBoard.m_Brightness = malloc(aBoard.m_StateCout * LED_COUNT * sizeof(uint8_t));
    for( int i = 0 ; i < aBoard.m_StateCout * LED_COUNT ; i++ ){
        initState(&aBoard.m_States[i]);
        aBoard.m_Brightness[i] = 0;
    }
/*************** Touch switch with one led *****************/    
    aBoard.m_States[0].m_StateType = ST_OFF;
    aBoard.m_States[1].m_StateType = ST_ON;
    aBoard.m_Brightness[0] = 0;
    aBoard.m_Brightness[1] = 100;
    aBoard.m_Brightness[2] = 100;
    aBoard.m_Brightness[3] = 0;
/*************** Touch switch *****************/    
    aBoard.m_CurrState = 0;
    initSwitchBoard(&aBoard);
    SwitchCommand aCmd;
    Notification aNotify;
    updateBoardState(&aBoard);
    for(;;){
        aBoard.m_Input.m_isEnable = true;
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
              vTaskDelay(20/portTICK_PERIOD_MS);
              int aLvl = getGPIOState(&aBoard.m_Input);
              ESP_LOGI("switch", "Switch. Revert called %d", aLvl);
              if( aLvl == ACTIVE_INPUT_LEVEL ){
                aBoard.m_CurrState++;
                if( aBoard.m_CurrState >= aBoard.m_StateCout ){
                    aBoard.m_CurrState = 0;
                }
                updateBoardState(&aBoard);
              }
              break;
            case CC_SOUND_ON:
              aBoard.m_isSoundOn = (bool)aCmd.m_IntParam;
              enableBuzzer(&aBoard.m_Buzzer, aBoard.m_isSoundOn);
              break;
            case CC_SET_LED_BR:{
              int anIndx = aCmd.m_Brightness.m_State * LED_COUNT;
              for( int aCnt = 0; aCnt < LED_COUNT ; aCnt++ ){
                aBoard.m_Brightness[anIndx] = aCmd.m_Brightness.m_Brightness[aCnt];
                anIndx++;  
              }
              break;
            }
            case CC_RESET:
                ESP_LOGI(TAG, "*********************** Reset");
                vTaskDelay(RESET_DEALY/portTICK_PERIOD_MS);
                esp_restart();
                break;
            case CC_RESET_REQ:
                ESP_LOGI(TAG, "*********************** Reset request");
                aNotify.m_Command = NC_RESET;
                xQueueSend(*aBoard.m_NotifyQueue, &aNotify, 0);
                break;
            case CC_TEMPERATURE:
#ifdef METEO_ENABLED            
                Meteo_Read();
#endif                
                break;
            case CC_IN_NEG_EDGE:
                ESP_LOGI(TAG, "NEG EDGY GET %d",aCmd.m_IntParam);
                break;
            case CC_SET_LIMIT:
                aBoard.m_Limit = aCmd.m_DoubleParam;
                ESP_LOGI(TAG, "Set limit=%d.%d", (int)aBoard.m_Limit, ((int)(aBoard.m_Limit*100))%100);
                updateControlledOut(&aBoard);
                break;
            case CC_SET_VALUE:
                aBoard.m_Value = aCmd.m_DoubleParam;
                ESP_LOGI(TAG, "Set value=%d.%d", (int)aBoard.m_Value, ((int)(aBoard.m_Value*100))%100);
                updateControlledOut(&aBoard);
                break;
            }
            vTaskDelay(100 / portTICK_RATE_MS);
        }
    }
}

int SWB_getBoardState()
{
  return getBoardStateInt(&aBoard);
}

void SWB_setBoardState(int theState)
{
    SwitchCommand aCmd;
    aCmd.m_Command = CC_SWITCH_OFF;
    if( theState == SS_ON ){
        aCmd.m_Command = CC_SWITCH_ON;
    }
    aCmd.m_IntParam = 0; 
    xQueueSend(aBoard.m_Queue, &aCmd, 0);
}

#if LED_COUNT == 1
    void SWB_setBrightness(uint8_t theState, uint8_t theBrightness)
    {
        SwitchCommand aCmd;
        aCmd.m_Command = CC_SET_LED_BR;
        aCmd.m_Brightness.m_State = theState; 
        aCmd.m_Brightness.m_Brightness[0] = theBrightness; 
        xQueueSend(aBoard.m_Queue, &aCmd, 0);
    }
#elif LED_COUNT == 2
    void SWB_setBrightness(uint8_t theState, uint8_t theBrightness1, uint8_t theBrightness2)
    {
	ESP_LOGI(TAG, "SetBrightness state=%d, %d, %d", theState, theBrightness1, theBrightness2);
        SwitchCommand aCmd;
        aCmd.m_Command = CC_SET_LED_BR;
        aCmd.m_Brightness.m_State = theState; 
        aCmd.m_Brightness.m_Brightness[0] = theBrightness1; 
        aCmd.m_Brightness.m_Brightness[1] = theBrightness2; 
        xQueueSend(aBoard.m_Queue, &aCmd, 0);
    }
#else
    #error Only LED_COUNT = 1 or 2 supported
#endif

void SWB_soundOn(bool theVal)
{
    SwitchCommand aCmd;
    aCmd.m_Command = CC_SOUND_ON;
    aCmd.m_IntParam = theVal;
    xQueueSend(aBoard.m_Queue, &aCmd, 0);
}

void SWB_reset()
{
    SwitchCommand aCmd;
    aCmd.m_Command = CC_RESET;
    aCmd.m_IntParam = 0;
    xQueueSend(aBoard.m_Queue, &aCmd, 0);
}

void SWB_MeasureTemperature()
{
    SwitchCommand aCmd;
    aCmd.m_Command = CC_TEMPERATURE;
    aCmd.m_IntParam = 0;
    xQueueSend(aBoard.m_Queue, &aCmd, 0);
}

void SWB_SetLimit(double theLimit)
{
    SwitchCommand aCmd;
    aCmd.m_Command = CC_SET_LIMIT;
    aCmd.m_DoubleParam = theLimit;
    xQueueSend(aBoard.m_Queue, &aCmd, 0);
}

void SWB_SetValue(double theValue)
{
    SwitchCommand aCmd;
    aCmd.m_Command = CC_SET_VALUE;
    aCmd.m_DoubleParam = theValue;
    xQueueSend(aBoard.m_Queue, &aCmd, 0);
}

void boardUpdate( TimerHandle_t xTimer )
{
    for( int i = 0 ; i < LED_COUNT ; i++ ){
        updateLED(&aBoard.m_LED[i]);
    }
    pwm_start();
    updateBuzzer(&aBoard.m_Buzzer);
    updateGPIOIn(&aBoard.m_Input); 
}

void setBoardStateInt(SwitchBoard* theBoard, int theState)
{
    if( theBoard->m_StateMutex ){
        if( xSemaphoreTake( theBoard->m_StateMutex, portMAX_DELAY ) == pdTRUE )
        {
            theBoard->m_CurrState = theState;
            xSemaphoreGive( theBoard->m_StateMutex );
        }
        updateBoardState(theBoard);
    }
}

int getBoardStateInt(SwitchBoard* theBoard)
{
    int aRes=0;
    if( theBoard->m_StateMutex ){
        if( xSemaphoreTake( theBoard->m_StateMutex, portMAX_DELAY ) == pdTRUE )
        {
            aRes = theBoard->m_CurrState;
            xSemaphoreGive( theBoard->m_StateMutex );
        }
    }
    return aRes;
}

void updateBoardState(SwitchBoard* theBoard)
{
    if( theBoard->m_CurrState < 0 || theBoard->m_CurrState >= theBoard->m_StateCout ){
        return;
    }
    int anIndx = theBoard->m_CurrState * LED_COUNT;
    for( int aCnt = 0 ; aCnt < LED_COUNT ; aCnt++ ){
        ESP_LOGI(TAG,"update led index=%d, br=%d", anIndx, theBoard->m_Brightness[anIndx]);
        setBrightness(&theBoard->m_LED[aCnt], theBoard->m_Brightness[anIndx]);
        anIndx++;
    }
    switch( theBoard->m_States[theBoard->m_CurrState].m_StateType ){
        case ST_ON:
            onSwitch(theBoard);
            break;
        case ST_OFF:
            offSwitch(theBoard);
            break;
        case ST_CONTROL:
            controlSwitch(theBoard);
            break;
    }
}

void controlSwitch(SwitchBoard* theBoard)
{
    onGPIOOut(&theBoard->m_Relay);
    if( theBoard->m_isSoundOn )
        playBuzzer(&theBoard->m_Buzzer);
}

void onSwitch(SwitchBoard* theBoard)
{
    onGPIOOut(&theBoard->m_Relay);
    if( theBoard->m_isSoundOn )
        playBuzzer(&theBoard->m_Buzzer);
}

void offSwitch(SwitchBoard* theBoard)
{
    offGPIOOut(&theBoard->m_Relay);
    if( theBoard->m_isSoundOn )
        playBuzzer(&theBoard->m_Buzzer);
}

void updateControlledOut(SwitchBoard* theBoard)
{
    ESP_LOGI(TAG, "======== UpdateControlled out state=%d", theBoard->m_CurrState);
    if( theBoard->m_States[theBoard->m_CurrState].m_StateType == ST_CONTROL ){
        if( theBoard->m_States[theBoard->m_CurrState].m_ControlType == CT_MORE_LIMIT ){
            if( theBoard->m_Value > theBoard->m_Limit ){
                onGPIOOut(&theBoard->m_Relay);
            }
            else{
                offGPIOOut(&theBoard->m_Relay);
            }
        } 
        else{
            if( theBoard->m_Value < theBoard->m_Limit ){
                ESP_LOGI(TAG, " Value less than limit");
                onGPIOOut(&theBoard->m_Relay);
            }
            else{
                ESP_LOGI(TAG, " Value more than limit");
                offGPIOOut(&theBoard->m_Relay);
            }
        }
    }
}

void initSwitchBoard(SwitchBoard* theBoard)
{
    theBoard->m_StateMutex=NULL;
    theBoard->m_isSoundOn = true;
    //PWM pins
    const uint32_t aPins[LED_COUNT] = {
#if LED_COUNT > 0        
        PIN_LED_1,
#endif
#if LED_COUNT > 1        
        PIN_LED_2,
#endif
#if LED_COUNT > 2                
        PIN_LED_3
#endif
    };
    ESP_LOGE(TAG, "pin[0]=%d, pin[1]=%d", aPins[0], aPins[1]);
    //Off all LEDs
    uint32_t aDuties[LED_COUNT] = {
#if LED_COUNT > 0        
        0,
#endif
#if LED_COUNT > 1        
        0,
#endif
#if LED_COUNT > 2                
        0
#endif
    };    
    //Init PWMs
    pwm_init(PWM_PERIOD, aDuties, LED_COUNT, aPins);
    for( int i = 0 ; i < LED_COUNT ; i++ ){
        pwm_set_phase(i,0);
    }
    //Start PWMs
    pwm_start();
    //Create semaphore to access to board state
    theBoard->m_StateMutex = xSemaphoreCreateMutex();
    //Create input queue

    //install gpio isr service
    gpio_install_isr_service(0);

    for( int i = 0 ; i < LED_COUNT ; i++ ){
        initLED(&theBoard->m_LED[i],i);
    }
    initGPIOOut(&theBoard->m_Relay,PIN_RELAY);
    initBuzzer(&theBoard->m_Buzzer,PIN_BUZER);
    initGPIOIn(&theBoard->m_Input,PIN_INPUT, aBoard.m_Queue);

    aBoard.m_Limit = 0;
    aBoard.m_Value = 0;

    TimerHandle_t anUpdTimer = xTimerCreate("timer100ms", pdMS_TO_TICKS(UPDATE_PERIOD), pdTRUE,
                     (void*)0, boardUpdate);
    xTimerStart(anUpdTimer, 0);
#ifdef METEO_ENABLED    
    Meteo_Init();
#endif    
}

void initState(StateDesc* theStateDesc)
{
    theStateDesc->m_StateType = ST_OFF;
    theStateDesc->m_ControlSource = CS_EXTERNAL;
    theStateDesc->m_InternalSource = IS_TEMPERATURE;
    theStateDesc->m_ControlType = CT_LESS_LIMIT;
}
