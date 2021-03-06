#include "board_config.h"
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

#include "led.h"

#include "driver/pwm.h"
#include "esp_log.h"

// PWM period is 125 Hz
#define PWM_PERIOD 1000
#define TRANSITION_MAX_COUNT 5

void initLED(LED* theLED, uint32_t theChannel)
{
    theLED->m_Brightness = 100;
    theLED->m_State = LS_OFF;
    theLED->m_Channel = theChannel;
    theLED->m_Transition = LT_NONE;
    theLED->m_TransCnt = 0;
}

void offLED(LED* theLED)
{
    if( theLED->m_State == LS_OFF ){
        return;
    }
    theLED->m_State = LS_OFF;
    theLED->m_Transition = LT_OFF;
    theLED->m_TransCnt = 0;
}

void onLED(LED* theLED)
{
    if( theLED->m_State == LS_ON ){
        return;
    }
    theLED->m_State = LS_ON;
    theLED->m_Transition = LT_ON;
    theLED->m_TransCnt = 0;
}

void setBrightness(LED* theLED, uint8_t m_Brightness)
{
    theLED->m_Brightness = m_Brightness;
    if( theLED->m_Transition == LT_NONE ){
        if( theLED->m_State == LS_ON ){
            uint32_t aDuty = (theLED->m_Brightness * PWM_PERIOD)/100;
            pwm_set_duty(theLED->m_Channel,aDuty);
            pwm_start();
        }
    }
}

void adjustLED(LED* theLED)
{
    if( theLED->m_Transition == LT_NONE ){
        uint32_t aDuty = 1;
        if( theLED->m_State == LS_ON ){
            aDuty = (theLED->m_Brightness * PWM_PERIOD)/100;
        }
        pwm_set_duty(theLED->m_Channel,aDuty);
    }
}

void updateLED(LED* theLED)
{
    uint32_t aDuty = 0;
    if( theLED->m_Transition == LT_ON ){
        aDuty = theLED->m_Brightness;
        aDuty = (theLED->m_Brightness * theLED->m_TransCnt)/TRANSITION_MAX_COUNT;
        aDuty = (aDuty * PWM_PERIOD)/100;
        if( aDuty >= PWM_PERIOD ){
            aDuty = PWM_PERIOD - 1;
        }
        theLED->m_TransCnt++;
        if( theLED->m_TransCnt > TRANSITION_MAX_COUNT ){
            theLED->m_Transition = LT_NONE;
        }
    }
    if( theLED->m_Transition == LT_OFF){
        aDuty = theLED->m_Brightness - (theLED->m_Brightness * theLED->m_TransCnt)/TRANSITION_MAX_COUNT;
//        ESP_LOGI("LED","duty=%d, channel=%d",aDuty,theLED->m_Channel);
        aDuty = (aDuty * PWM_PERIOD)/100;
        if( aDuty < 1 ){
            aDuty = 1;
        }
        theLED->m_TransCnt++;
        if( theLED->m_TransCnt > TRANSITION_MAX_COUNT ){
            theLED->m_Transition = LT_NONE;
        }
    }
    if( aDuty != 0 ){
        if( aDuty == 1 )
          aDuty = 0;
        pwm_set_duty(theLED->m_Channel,aDuty);
//        pwm_set_phase(theLED->m_Channel,0);
//        pwm_start();
    }
}