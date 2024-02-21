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

#include <board_config.h>
#include "config_mqtt.h"
#include "json_utils.h"
#include "common_def.h"

#ifdef MQTT_ENABLED
#define IS_MQTT_KEYWORD             "is_mqtt"
#define MQTT_SERVER_KEYWORD         "mqtt_server"
#define MQTT_USER_KEYWORD           "mqtt_user"
#define MQTT_PASSWORD_KEYWORD       "mqtt_password"
#define MQTT_DEVICE_PATH_KEYWORD    "mqtt_device_path"
#define MQTT_CONTROL_TOPIC_KEYWORD  "mqtt_control_topic"

//static const char TAG[]="config_switch";

static const char MqttSettingTemplate[] = "{"\
    "\"is_mqtt\": \"%s\","\
    "\"mqtt_server\": \"%s\","\
    "\"mqtt_user\": \"%s\","\
    "\"mqtt_password\": \"%s\","\
    "\"mqtt_device_path\": \"%s\","\
    "\"mqtt_control_topic\": \"%s\""\
  "}";

void CFG_MqttInit(MqttConfig* theConfig)
{
  theConfig->m_IsEnabled = DEFAULT_ENABLE_MQTT;
  strcpy(theConfig->m_Server, DEFAULT_MQTT_SERVER);
  strcpy(theConfig->m_User, DEFAULT_MQTT_USER);
  strcpy(theConfig->m_Password, DEFAULT_MQTT_PASSWORD);
  strcpy(theConfig->m_DevicePath, DEFAULT_MQTT_DEVICE_PATH);
  strcpy(theConfig->m_ControlTopic, DEFAULT_MQTT_CONTROL_TOPIC);
}

esp_err_t CFG_MqttGetSettingsString(MqttConfig* theConfig, char* theBuffer)
{
    char* isMqtt = ON_VALUE;
    if( !theConfig->m_IsEnabled ){
        isMqtt = OFF_VALUE;
    }
    sprintf(theBuffer, MqttSettingTemplate,
        isMqtt,
        theConfig->m_Server,
        theConfig->m_User,
        theConfig->m_Password,
        theConfig->m_DevicePath,
        theConfig->m_ControlTopic);

    return ESP_OK;
}

esp_err_t CFG_MqttParseSettings(MqttConfig* theConfig, cJSON* theJSON, bool isFullSet)
{
    MqttConfig aConfig = *theConfig;
    esp_err_t aRes = ESP_OK;
    if( isFullSet ){
      aConfig.m_IsEnabled = false;
      aConfig.m_Server[0] = 0;
      aConfig.m_User[0] = 0;
      aConfig.m_Password[0] = 0;
    }
    cJSON* aDataItem = theJSON->child;
    while( aDataItem != NULL ){
        if(strcmp( aDataItem->string, IS_MQTT_KEYWORD) == 0 ){
            aRes = JSU_ConverBool(aDataItem, &aConfig.m_IsEnabled);
            if( aRes != ESP_OK ){
                return aRes;
            }
        }
        else if(strcmp( aDataItem->string, MQTT_SERVER_KEYWORD) == 0 ){
            aRes = JSU_ConverString(aDataItem, aConfig.m_Server, MAX_STR_SIZE);
            if( aRes != ESP_OK ){
                return aRes;
            }
        }     
        else if(strcmp( aDataItem->string, MQTT_USER_KEYWORD) == 0 ){
            aRes = JSU_ConverString(aDataItem, aConfig.m_User, MAX_STR_SIZE);
            if( aRes != ESP_OK ){
                return aRes;
            }
        }     
        else if(strcmp( aDataItem->string, MQTT_PASSWORD_KEYWORD) == 0 ){
            aRes = JSU_ConverString(aDataItem, aConfig.m_Password, MAX_PASSWORD_SIZE);
            if( aRes != ESP_OK ){
                return aRes;
            }
        }     
        else if(strcmp( aDataItem->string, MQTT_DEVICE_PATH_KEYWORD) == 0 ){
            aRes = JSU_ConverString(aDataItem, aConfig.m_DevicePath, MAX_STR_SIZE);
            if( aRes != ESP_OK ){
                return aRes;
            }
        }     
        else if(strcmp( aDataItem->string, MQTT_CONTROL_TOPIC_KEYWORD) == 0 ){
            aRes = JSU_ConverString(aDataItem, aConfig.m_ControlTopic, MAX_PASSWORD_SIZE);
            if( aRes != ESP_OK ){
                return aRes;
            }
        }     
        aDataItem = aDataItem->next;
    }
    *theConfig = aConfig;
    return ESP_OK;
}
#endif