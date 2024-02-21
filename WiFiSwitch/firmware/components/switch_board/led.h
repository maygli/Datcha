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

#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum _LEDState{ LS_ON, LS_OFF } LEDState;
typedef enum _LEDTransition{ LT_NONE, LT_TRANS} LEDTransition;

/* LED */
typedef struct _LED{
/* Brightness (0-100) */   
    uint8_t         m_Brightness;
/* Previous brightnes */    
    uint8_t         m_PrevBrightness;
/* LED state (ON/OFF) */    
    LEDState        m_State;
/* PWM channel */
    uint8_t         m_Channel;
/* LED transition (internal) */    
    LEDTransition   m_Transition;
/* LED transition counter */   
    uint8_t         m_TransCnt;     
} LED;

#ifdef __cplusplus
extern "C" {
#endif

void initLED(LED* theLED, uint32_t theChannel);
void setBrightness(LED* theLED, uint8_t m_Brightness);
void updateLED(LED* theLED);
void adjustLED(LED* theLED, bool isRestart);

#ifdef __cplusplus
}
#endif
