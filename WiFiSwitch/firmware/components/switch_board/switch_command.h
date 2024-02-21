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

#include "board_config.h"

typedef enum _CommandCode{
  CC_SWITCH_OFF=0, CC_SWITCH_ON=1, CC_SWITCH_REV=2, 
  CC_SET_LED_BR=3, CC_STYLE=4, CC_SOUND_ON=5, CC_RESET=6, 
  CC_RESET_REQ=7, CC_TEMPERATURE=8, CC_IN_NEG_EDGE=9, 
  CC_SET_STATE=10, CC_SET_LIMIT=11, CC_SET_VALUE=12
} CommandCode;

typedef struct _LEDsBrightness{
  uint8_t m_State;
  uint8_t m_Brightness[LED_COUNT];
} LEDsBrightness;

typedef struct _SwitchCommand{
    uint8_t  m_Command;
    union{
      uint8_t         m_IntParam;
      double          m_DoubleParam;
      LEDsBrightness  m_Brightness;
    };
} SwitchCommand;


