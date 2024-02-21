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

#include <stdint.h>
#include <stdio.h>

#include "driver/gpio.h"
#include "esp_log.h"

#include "board_config.h"
#include "switch_command.h"
#include "switch_board.h"

#include "io_in.h"

#define TAG "GPIO_IN"

#define RESET_TIME 10

static void IRAM_ATTR inGPIOIsr(void *arg)
{
    GPIOIn* aGPIO = (GPIOIn*)arg;
    if( aGPIO->m_isReal ){
        if( aGPIO->m_isEnable ){
            SwitchCommand aCmd;
            aCmd.m_Command = CC_SWITCH_REV;
            aCmd.m_IntParam = 0; 
            xQueueSendFromISR(aGPIO->m_Queue, &aCmd, 0);
            aGPIO->m_isEnable = false;
        }
    }
    else{
        aGPIO->m_isReal = true;
    }
}

void initGPIOIn(GPIOIn* theGPIO, uint32_t thePin, xQueueHandle theQueue)
{
    theGPIO->m_Pin = thePin;
    theGPIO->m_Queue = theQueue;
    theGPIO->m_isReal = true;
    theGPIO->m_isResetSent = false;
    gpio_config_t aGPIOConf;
    //interrupt of rising edge
    aGPIOConf.intr_type = GPIO_INTR_ANYEDGE;
    //bit mask of the pins
    aGPIOConf.pin_bit_mask = 1ULL<<thePin;
    //set as input mode
    aGPIOConf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    aGPIOConf.pull_up_en = 1;
    //disable pull-down mode
    aGPIOConf.pull_down_en = 0;
    gpio_config(&aGPIOConf);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(thePin, inGPIOIsr, (void *)theGPIO);
    theGPIO->m_CurrLvl = gpio_get_level(theGPIO->m_Pin);
    gettimeofday(&theGPIO->m_TimeBtnStart, NULL);
}

int getGPIOState(GPIOIn* theGPIO)
{
    int aLvl = gpio_get_level(theGPIO->m_Pin);
    if( aLvl != theGPIO->m_CurrLvl){
        theGPIO->m_CurrLvl = aLvl;
        gettimeofday(&theGPIO->m_TimeBtnStart, NULL);
    }
    return aLvl;
}

void updateGPIOIn(GPIOIn* theGPIO)
{
   int aLvl = gpio_get_level(theGPIO->m_Pin);
   if( theGPIO->m_CurrLvl == ACTIVE_INPUT_LEVEL){
       if( theGPIO->m_isEnable ){
            if( aLvl != ACTIVE_INPUT_LEVEL ){
// Addtional protect               
                SwitchCommand aCmd;
                aCmd.m_Command = CC_SWITCH_REV;
                aCmd.m_IntParam = 0; 
                xQueueSend(theGPIO->m_Queue, &aCmd, 0);
                theGPIO->m_isEnable = false;
                theGPIO->m_isResetSent = false;
                return;
            }
       }
        struct timeval aCurrTime;
        gettimeofday(&aCurrTime, NULL);
        if( (aCurrTime.tv_sec - theGPIO->m_TimeBtnStart.tv_sec) > RESET_TIME){
            if( !theGPIO->m_isResetSent ){
                ESP_LOGI(TAG, "============ Reset called ============");
                SwitchCommand aCmd;
                aCmd.m_Command = CC_RESET_REQ;
                aCmd.m_IntParam = 0; 
                xQueueSend(theGPIO->m_Queue, &aCmd, 0);
                theGPIO->m_isEnable = false;
                theGPIO->m_isResetSent = true;
                return;
            }
        }
//        int aTimeInterval = timeval_durationBeforeNow(&theGPIO->m_TimeBtnStart);
        ESP_LOGI(TAG, "TimeInterval=%ld", aCurrTime.tv_sec);
    }
    
}