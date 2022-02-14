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

#include <string.h>

#include <esp_err.h>
#include <esp_log.h>

#include <cJSON.h>

#include "switch_board.h"

#include "board_config.h"
#include "common_def.h"
#include "config.h"

#define ON_BRIGHTNESS_KEYWORD "on_brightness"
#define OFF_BRIGHTNESS_KEYWORD "off_brightness"
#define SOUND_KEYWORD "sound"
#define STYLE_KEYWORD "style"

#define ON_VALUE    "on"

static const char TAG[]="config";

esp_err_t CFG_Init(BoardConfig* theConfig)
{
    theConfig->m_StConn.m_IsEnabled = true;
    strcpy(theConfig->m_StConn.m_SSID, DEFAULT_ST_SSID);
    strcpy(theConfig->m_StConn.m_Password, DEFAULT_ST_PASSWORD);
    theConfig->m_StConn.m_IsFixedAddress = true;
    theConfig->m_StConn.m_Ip[0] = ST_IP_0;
    theConfig->m_StConn.m_Ip[1] = ST_IP_1;
    theConfig->m_StConn.m_Ip[2] = ST_IP_2;
    theConfig->m_StConn.m_Ip[3] = ST_IP_3;
    theConfig->m_StConn.m_NetMask[0] = ST_MASK_0;
    theConfig->m_StConn.m_NetMask[1] = ST_MASK_1;
    theConfig->m_StConn.m_NetMask[2] = ST_MASK_2;
    theConfig->m_StConn.m_NetMask[3] = ST_MASK_3;
    theConfig->m_StConn.m_Gateway[0] = ST_GATE_0;
    theConfig->m_StConn.m_Gateway[1] = ST_GATE_1;
    theConfig->m_StConn.m_Gateway[2] = ST_GATE_2;
    theConfig->m_StConn.m_Gateway[3] = ST_GATE_3;

    theConfig->m_APConn.m_IsEnabled = false;
    strcpy(theConfig->m_APConn.m_SSID, DEFAULT_AP_SSID);
    strcpy(theConfig->m_APConn.m_Password, DEFAULT_AP_PASSWORD);
    theConfig->m_APConn.m_IsFixedAddress = true;
    theConfig->m_APConn.m_Ip[0] = AP_IP_0;
    theConfig->m_APConn.m_Ip[1] = AP_IP_1;
    theConfig->m_APConn.m_Ip[2] = AP_IP_2;
    theConfig->m_APConn.m_Ip[3] = AP_IP_3;
    theConfig->m_APConn.m_NetMask[0] = AP_MASK_0;
    theConfig->m_APConn.m_NetMask[1] = AP_MASK_1;
    theConfig->m_APConn.m_NetMask[2] = AP_MASK_2;
    theConfig->m_APConn.m_NetMask[3] = AP_MASK_3;
    theConfig->m_APConn.m_Gateway[0] = AP_GATE_0;
    theConfig->m_APConn.m_Gateway[1] = AP_GATE_1;
    theConfig->m_APConn.m_Gateway[2] = AP_GATE_2;
    theConfig->m_APConn.m_Gateway[3] = AP_GATE_3;

    return ESP_OK;
}

ConnectionInfo* CFG_GetSTConnection(BoardConfig* theConfig)
{
    return &theConfig->m_StConn;
}

ConnectionInfo* CFG_GetAPConnection(BoardConfig* theConfig)
{
    return &theConfig->m_APConn;
}

void setBoardUpdateFunc( BoardConfig* theConfig, esp_err_t (*theBoardUpdateFunc)(void* arg), void* theBoardUpdateFuncArg )
{
    theConfig->m_BoardUpdate = theBoardUpdateFunc;
    theConfig->m_BoardUpdateArg = theBoardUpdateFuncArg;
}

esp_err_t CFG_ParseSwitchSettings(BoardConfig* theConfig, cJSON* theJSON)
{
    cJSON* aDataItem = theJSON->child;
    while( aDataItem != NULL ){
        char* aVal = cJSON_GetStringValue(aDataItem);
        ESP_LOGI(TAG, "Item=%s, value=%s\n", aDataItem->string, aVal);
        if(strcmp( aDataItem->string, ON_BRIGHTNESS_KEYWORD) == 0 ){
            int aNumber=100;
            int aCnt = sscanf(aVal, "%d", &aNumber);
            if( aCnt != 1){
                ESP_LOGE(TAG, "Switch parameters JSON has wrong format. On brightness shoud be a number");
            }
            else{
                theConfig->m_SwitchConfig.m_OnBrightness = (uint8_t)aNumber;
                ESP_LOGI(TAG, "On_BRIGHTNESS found=%d", theConfig->m_SwitchConfig.m_OnBrightness);
                setOnBrightness(theConfig->m_SwitchConfig.m_OnBrightness);
            }
        }
        else if(strcmp( aDataItem->string, OFF_BRIGHTNESS_KEYWORD) == 0 ){
            int aNumber = 100;
            int aCnt = sscanf(aVal, "%d",&aNumber);
            if( aCnt != 1 ){
                ESP_LOGE(TAG, "Switch parameters JSON has wrong format. Off brightness shoud be a number");
            }
            else{
                theConfig->m_SwitchConfig.m_OffBrightness = aNumber;
                ESP_LOGI(TAG, "Off_BRIGHTNESS found=%d", theConfig->m_SwitchConfig.m_OffBrightness);
                setOffBrightness(theConfig->m_SwitchConfig.m_OffBrightness);
            }
        }
        else if(strcmp( aDataItem->string, SOUND_KEYWORD) == 0 ){
            if( cJSON_IsString(aDataItem) ){
                if( strcmp(aVal, ON_VALUE) == 0 ){
                    theConfig->m_SwitchConfig.m_IsSoundOn = true;
                }
                else{
                    theConfig->m_SwitchConfig.m_IsSoundOn = false;
                }
                ESP_LOGI(TAG, "Sound found=%d", theConfig->m_SwitchConfig.m_IsSoundOn);
                soundOn(theConfig->m_SwitchConfig.m_IsSoundOn);
            }
            else{
                ESP_LOGE(TAG, "Switch parameters JSON has wrong format. Style shoud be a number (0 or 1)");
            }
        }
        else if(strcmp( aDataItem->string, STYLE_KEYWORD) == 0 ){
            int aNumber = 0;
            int aCnt = sscanf(aVal, "%d", &aNumber);
            if( aCnt != 1 ){
                ESP_LOGE(TAG, "Switch parameters JSON has wrong format. Style shoud be a number");
            }
            else{
                theConfig->m_SwitchConfig.m_Style = aNumber;
                if( theConfig->m_SwitchConfig.m_Style > MAX_STYLE ){
                    ESP_LOGE(TAG, "Switch parameters JSON has wrong format. Style shoud be a number (0 or 1)");
                    theConfig->m_SwitchConfig.m_Style = MAX_STYLE;   
                }
                ESP_LOGI(TAG, "Style found=%d", theConfig->m_SwitchConfig.m_Style);
            }
        }
        aDataItem = aDataItem->next;
    }
    if( theConfig->m_BoardUpdate ){
        theConfig->m_BoardUpdate(theConfig->m_BoardUpdateArg);
    }
    return ESP_OK;
}