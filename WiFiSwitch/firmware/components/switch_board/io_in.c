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

#include "driver/gpio.h"
#include "esp_log.h"

#include "board_config.h"
#include "switch_command.h"
#include "io_in.h"

static void inGPIOIsr(void *arg)
{
    GPIOIn* aGPIO = (GPIOIn*)arg;
    SwitchCommand aCmd;
    aCmd.m_Command = CC_SWITCH_REV;
    aCmd.m_Parameter = 0; 
    xQueueSendFromISR(aGPIO->m_Queue, &aCmd, NULL);
}

void initGPIOIn(GPIOIn* theGPIO, uint32_t thePin, xQueueHandle theQueue)
{
    theGPIO->m_Pin = thePin;
    theGPIO->m_Queue = theQueue;
    gpio_config_t aGPIOConf;
    //interrupt of rising edge
    aGPIOConf.intr_type = GPIO_INTR_POSEDGE;
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
}
