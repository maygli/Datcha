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

#include "esp_log.h"

#include "board_config.h"
#include "buzzer.h"

#define PLAY_TIME 3

void initBuzzer(Buzzer* theBuzzer, uint32_t thePin)
{
    initGPIOOut(&theBuzzer->m_GPIO, thePin);
    theBuzzer->m_SoundCounter = PLAY_TIME;
}

void playBuzzer(Buzzer* theBuzzer)
{
    theBuzzer->m_SoundCounter = 0;
}

void updateBuzzer(Buzzer* theBuzzer )
{
    if( theBuzzer->m_SoundCounter >= PLAY_TIME ){
        offGPIOOut(&theBuzzer->m_GPIO);
        return;
    }
    if( theBuzzer->m_SoundCounter < PLAY_TIME ){
        onGPIOOut(&theBuzzer->m_GPIO);
    }
    theBuzzer->m_SoundCounter++;
}

void enableBuzzer(Buzzer* theBuzzer, bool isEnable)
{
    if( !isEnable ){
        theBuzzer->m_SoundCounter = PLAY_TIME;
        offGPIOOut(&theBuzzer->m_GPIO);
    }
}
