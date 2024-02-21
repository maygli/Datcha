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
#include "board_config.h"

typedef enum _SwitchState{ SS_ON, SS_OFF } SwitchState;
typedef enum _SwitchMode{ SM_NORMAL, SM_FLASh} SwitchMode;

typedef enum _NotifyCommand{NC_RESET=0,NC_NONE} NotifyCommand;

typedef enum _StateType{ ST_OFF=0, ST_ON, ST_CONTROL} StateType;
typedef enum _ControlSource{ CS_EXTERNAL=0, CS_INTERNAL} ControlSource;
typedef enum _InternalControlSource{ IS_TEMPERATURE, IS_PRESSURE, IS_HUMIDITY} InternalControlSource;
typedef enum _ControlType{ CT_LESS_LIMIT=0, CT_MORE_LIMIT } ControlType;

typedef struct _Notification{
    uint8_t     m_Command;
} Notification;

#ifdef __cplusplus
extern "C" {
#endif

void        SWB_switchBoardTask(void *arg);
int         SWB_getBoardState();
void        SWB_setBoardState(int theState);
#if LED_COUNT == 1
    void        SWB_setBrightness(uint8_t theState, uint8_t theBrightness);
#elif LED_COUNT == 2
    void        SWB_setBrightness(uint8_t theState, uint8_t theBrightness1, uint8_t theBrightness2);
#else
    #error Support only LED_COUNT = 1 or 2
#endif
void        SWB_soundOn(bool theVal);
void        SWB_reset();
void        SWB_MeasureTemperature();
void        SWB_SetLimit(double theLimit);
void        SWB_SetValue(double theValue);

#ifdef __cplusplus
}
#endif

