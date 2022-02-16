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
#include "json_utils.h"
#include "common_def.h"

#define ON_BRIGHTNESS_KEYWORD "on_brightness"
#define OFF_BRIGHTNESS_KEYWORD "off_brightness"
#define SOUND_KEYWORD "sound"
#define STYLE_KEYWORD "style"

//static const char TAG[]="config_switch";

static const char SwitchSettingTemplate[] = "{"\
    "\"on_brightness\": %d,"\
    "\"off_brightness\": %d,"\
    "\"sound\": \"%s\","\
    "\"style\": %d"\
  "}";

void CFG_SwitchInit(SwitchConfig* theConfig)
{
    theConfig->m_OnBrightness = DEFAULT_ON_BRIGNESS;
    theConfig->m_OnBrightness = DEFAULT_OFF_BRIGNESS;
    theConfig->m_IsSoundOn = DEFAULT_SWITCH_SOUND;
    theConfig->m_Style = DEFAULT_SWITCH_STYLE;
}

esp_err_t CFG_SwitchGetSettingsString(SwitchConfig* theConfig, char* theBuffer)
{
    char* aSoundOnStr = OFF_VALUE;
    if(theConfig->m_IsSoundOn){
        aSoundOnStr = ON_VALUE;
    }
    sprintf(theBuffer, SwitchSettingTemplate, 
                        theConfig->m_OnBrightness,
                        theConfig->m_OffBrightness,
                        aSoundOnStr,
                        theConfig->m_Style);
    return ESP_OK;
}

void cfg_setBoardParameters(SwitchConfig* theConfig)
{
    SWB_setOnBrightness(theConfig->m_OnBrightness);
    SWB_setOffBrightness(theConfig->m_OffBrightness);
    SWB_soundOn(theConfig->m_IsSoundOn);
    SWB_setStyle(theConfig->m_Style);
}

esp_err_t CFG_SwitchParseSettings(SwitchConfig* theConfig, cJSON* theJSON, bool isFullSet)
{
    SwitchConfig aConfig;
    aConfig = *theConfig;
    cJSON* aDataItem = theJSON->child;
    esp_err_t aRes;
    if( isFullSet ){
        aConfig.m_IsSoundOn = false;
    }
    while( aDataItem != NULL ){
        if(strcmp( aDataItem->string, ON_BRIGHTNESS_KEYWORD) == 0 ){
            int aVal;
            aRes = JSU_ConverInt(aDataItem, &aVal);
            if( aRes == ESP_OK ){
                aConfig.m_OnBrightness = (uint8_t)aVal;
            }
            else{
                return aRes;
            }
        }
        else if(strcmp( aDataItem->string, OFF_BRIGHTNESS_KEYWORD) == 0 ){
            int aVal;
            aRes = JSU_ConverInt(aDataItem, &aVal);
            if( aRes == ESP_OK ){
                aConfig.m_OffBrightness = (uint8_t)aVal;
            }
            else{
                return aRes;
            }
        }
        else if(strcmp( aDataItem->string, SOUND_KEYWORD) == 0 ){
            aRes = JSU_ConverBool(aDataItem, &aConfig.m_IsSoundOn);
            if( aRes != ESP_OK ){
                return aRes;
            }
        }
        else if(strcmp( aDataItem->string, STYLE_KEYWORD) == 0 ){
            int aVal;
            aRes = JSU_ConverInt(aDataItem, &aVal);
            if( aRes == ESP_OK ){
                aConfig.m_Style = (uint8_t)aVal;
            }
        }
        aDataItem = aDataItem->next;
    }
    *theConfig = aConfig;
    cfg_setBoardParameters(theConfig);
    return ESP_OK;
}