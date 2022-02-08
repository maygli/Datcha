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

#include <semphr.h>

#include "led.h"
#include "buzzer.h"
#include "io_out.h"
#include "io_in.h"

typedef enum _SwitchState{ SS_ON, SS_OFF } SwitchState;

typedef struct _SwitchBoard{
    LED         	m_OnLED;
    LED         	m_OffLED;
    Buzzer      	m_Buzzer;
    GPIOOut     	m_Relay;
    GPIOIn      	m_Input;
    SwitchState 	m_State;
    xSemaphoreHandle 	m_StateMutex;
} SwitchBoard;

#ifdef __cplusplus
extern "C" {
#endif

void switchBoardTask(void *arg);
SwitchState getBoardState();

#ifdef __cplusplus
}
#endif

