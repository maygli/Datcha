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

#include "board_config.h"
#include "io_out.h"

#include "driver/gpio.h"
#include "esp_log.h"

void initGPIOOut(GPIOOut* theGPIO, uint32_t thePin)
{
    theGPIO->m_Pin = thePin;
    gpio_config_t aGPIOConf;
    //disable interrupt
    aGPIOConf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    aGPIOConf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO15/16
    aGPIOConf.pin_bit_mask = 1ULL<<thePin;
    //disable pull-down mode
    aGPIOConf.pull_down_en = 0;
    //disable pull-up mode
    aGPIOConf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&aGPIOConf);
}

void onGPIOOut(GPIOOut* theGPIO)
{
    gpio_set_level(theGPIO->m_Pin,1);
}

void offGPIOOut(GPIOOut* theGPIO)
{
    gpio_set_level(theGPIO->m_Pin,0);
}