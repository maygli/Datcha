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

#include <esp_log.h>

#include <switch_board.h>

#include <board_config.h>
#include "config_switch.h"
#include "common_def.h"

#define ON_BRIGHTNESS_KEYWORD "on_brightness"
#define OFF_BRIGHTNESS_KEYWORD "off_brightness"
#define SOUND_KEYWORD "sound"
#define STYLE_KEYWORD "style"

#define ON_VALUE    "on"
#define OFF_VALUE   "off"

static const char TAG[]="config_switch";

static const char SwitchSettingTemplate[] = "{"\
    "\"on_brightness\": %d,"\
    "\"off_brightness\": %d,"\
    "\"sound\": \"%s\","\
    "\"style\": %d,"\
  "}";

void CFG_SwitchInit(SwitchConfig* theConfig)
{
    theConfig->m_OnBrightness = DEFAUL_ON_BRIGNESS;
    theConfig->m_OnBrightness = DEFAUL_OFF_BRIGNESS;
    theConfig->m_IsSoundOn = DEFAUL_SWITCH_SOUND;
    theConfig->m_Style = DEFAUL_SWITCH_STYLE;
}

esp_err_t CFG_SwitchGetSettingsString(SwitchConfig* theConfig, char* theBuffer)
{
    char* aSoundOnStr = OFF_VALUE;
    if(theConfig->m_IsSoundOn){
        aSoundOnStr = ON_VALUE;
    }
    sprintf(theBuffer, SwitchSettingTemplate, 
                        theConfig->m_OnBrightness,
                        theConfig->m_OnBrightness,
                        aSoundOnStr,
                        theConfig->m_Style);
    return ESP_OK;
}

esp_err_t CFG_SwitchParseSettings(SwitchConfig* theConfig, cJSON* theJSON)
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
                return ESP_FAIL;
            }
            else{
                theConfig->m_OnBrightness = (uint8_t)aNumber;
                ESP_LOGI(TAG, "On_BRIGHTNESS found=%d", theConfig->m_OnBrightness);
                SWB_setOnBrightness(theConfig->m_OnBrightness);
            }
        }
        else if(strcmp( aDataItem->string, OFF_BRIGHTNESS_KEYWORD) == 0 ){
            int aNumber = 100;
            int aCnt = sscanf(aVal, "%d",&aNumber);
            if( aCnt != 1 ){
                ESP_LOGE(TAG, "Switch parameters JSON has wrong format. Off brightness shoud be a number");
                return ESP_FAIL;
            }
            else{
                theConfig->m_OffBrightness = aNumber;
                ESP_LOGI(TAG, "Off_BRIGHTNESS found=%d", theConfig->m_OffBrightness);
                SWB_setOffBrightness(theConfig->m_OffBrightness);
            }
        }
        else if(strcmp( aDataItem->string, SOUND_KEYWORD) == 0 ){
            if( cJSON_IsString(aDataItem) ){
                if( strcmp(aVal, ON_VALUE) == 0 ){
                    theConfig->m_IsSoundOn = true;
                }
                else{
                    theConfig->m_IsSoundOn = false;
                }
                ESP_LOGI(TAG, "Sound found=%d", theConfig->m_IsSoundOn);
                SWB_soundOn(theConfig->m_IsSoundOn);
            }
            else{
                ESP_LOGE(TAG, "Switch parameters JSON has wrong format. Style shoud be a number (0 or 1)");
                return ESP_FAIL;
            }
        }
        else if(strcmp( aDataItem->string, STYLE_KEYWORD) == 0 ){
            int aNumber = 0;
            int aCnt = sscanf(aVal, "%d", &aNumber);
            if( aCnt != 1 ){
                ESP_LOGE(TAG, "Switch parameters JSON has wrong format. Style shoud be a number");
                return ESP_FAIL;
            }
            else{
                theConfig->m_Style = aNumber;
                if( theConfig->m_Style > MAX_STYLE ){
                    ESP_LOGE(TAG, "Switch parameters JSON has wrong format. Style shoud be a number (0 or 1)");
                    theConfig->m_Style = MAX_STYLE;   
                }
                ESP_LOGI(TAG, "Style found=%d", theConfig->m_Style);
            }
        }
        aDataItem = aDataItem->next;
    }
   return ESP_OK;
}