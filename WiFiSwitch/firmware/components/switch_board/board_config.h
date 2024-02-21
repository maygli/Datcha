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

//#define WIFI_SWITCH_V2 
#define WIFI_SWITCH_V2


#ifdef WIFI_SWITCH_V1
//Board LED count
    #define LED_COUNT 2
//Maximum supported styles number
    #define MAX_STYLE   1
// On LED pin
    #define PIN_LED_1   5
// Off LED pin    
    #define PIN_LED_2   12
// Relay pin    
    #define PIN_RELAY   4
// Buzzer pin    
    #define PIN_BUZER   14
// Input pin    
    #define PIN_INPUT   13
#endif

#ifdef WIFI_SWITCH_V2
    //Board LED count
    #define LED_COUNT 2
    #define AHT_20_INSTALLED
    #define BMP_280_INSTALLED
//Maximum supported styles number
    #define MAX_STYLE   1
// On LED pin
    #define PIN_LED_1   2
// Off LED pin    
    #define PIN_LED_2   15
// Relay pin    
    #define PIN_RELAY   14
// Buzzer pin    
    #define PIN_BUZER   12
// Input pin    
    #define PIN_INPUT   13
// Active input level. 1 - for touch, 0 - for key
    #define ACTIVE_INPUT_LEVEL  1     
#if defined(AHT_20_INSTALLED) || defined(BMP_280_INSTALLED)
    #define PIN_SDA     4
    #define PIN_SCL     5
#endif
#ifdef AHT_20_INSTALLED
    #define TEMP_ENABLED        true
    #define HUMIDITY_ENABLED    true
    #define METEO_ENABLED       true
#endif
#ifdef BMP_280_INSTALLED
    #define TEMP_ENABLED        true
    #define PRESSURE_ENABLED    true
    #define METEO_ENABLED       true
#endif
// Temperature unit index 0 - C, 1 - F, 2 -K
    #define TEMPERATURE_UNIT_INDEX  0
// Pressure unit index 0 - mm Hg, 1 - Pa, 2 - bar, 3 - atm, 4 - psi     
    #define PRESSURE_UNIT_INDEX  1
// Humidity unit index 0 - %
    #define HUMIDITY_UNIT_INDEX  0
// Read meteo data period in secs
    #define DEFAULT_METEO_PERIOD 60    
#endif

#ifdef SOCKET_SWITCH_V2
    //Board LED count
    #define LED_COUNT 1
//    #define AHT_20_INSTALLED
//    #define BMP_280_INSTALLED
//Maximum supported styles number
    #define MAX_STYLE   1
// On LED pin
    #define PIN_LED_1  0
// Off LED pin    
 //   #define PIN_LED_2 15
// Relay pin    
    #define PIN_RELAY   14
// Buzzer pin    
    #define PIN_BUZER   12
// Input pin    
    #define PIN_INPUT   13
// Active input level. 1 - for touch, 0 - for key
    #define ACTIVE_INPUT_LEVEL  0     
#if defined(AHT_20_INSTALLED) || defined(BMP_280_INSTALLED)
    #define PIN_SDA     4
    #define PIN_SCL     5
#endif
#ifdef AHT_20_INSTALLED
    #define TEMP_ENABLED        true
    #define HUMIDITY_ENABLED    true
#endif
#ifdef BMP_280_INSTALLED
    #define TEMP_ENABLED        true
    #define PRESSURE_ENABLED    true
#endif
// Temperature unit index 0 - C, 1 - F, 2 -K
    #define TEMPERATURE_UNIT_INDEX  0
// Pressure unit index 0 - mm Hg, 1 - Pa, 2 - bar, 3 - atm, 4 - psi     
    #define PRESSURE_UNIT_INDEX  1
// Humidity unit index 0 - %
    #define HUMIDITY_UNIT_INDEX  0
// Read meteo data period in secs
    #define DEFAULT_METEO_PERIOD 60    
#endif
