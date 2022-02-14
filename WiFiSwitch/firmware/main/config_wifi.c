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
#include "config_wifi.h"
#include "common_def.h"

#define ON_VALUE    "on"
#define OFF_VALUE   "off"

#define IS_AP_KEYWORD           "is_ap"
#define AP_SSID_KEYWORD         "ap_ssid"
#define AP_PWD_KEYWORD          "ap_pwd"
#define AP_FIXED_IP_KEYWORD     "ap_fixed_ip"
#define AP_IP_KEYWORD           "ap_ip"
#define AP_NETMASK_KEYWOR       "ap_netmask"
#define AP_GATEWAY_KEYWORD      "ap_gateway"

#define IS_ST_KEYWORD           "is_st"
#define ST_SSID_KEYWORD         "st_ssid"
#define ST_PWD_KEYWORD          "st_pwd"
#define ST_FIXED_IP_KEYWORD     "st_fixed_ip"
#define ST_IP_KEYWORD           "st_ip"
#define ST_NETMASK_KEYWOR       "st_netmask"
#define ST_GATEWAY_KEYWORD      "st_gateway"
#define ST_ATTEMPTS_KEYWORD     "st_attempts"

static const char WiFiSettingTemplate[] = "{"\
    "\"is_ap\" : \"%s\","\
    "\"ap_ssid\":\"%s\","\
    "\"ap_pwd\":\"%s\","\
    "\"ap_fixed_ip\":\"%s\","\
    "\"ap_ip\":\"%d.%d.%d.%d\","\
    "\"ap_netmask\":\"%d.%d.%d.%d\","\
    "\"ap_gateway\":\"%d.%d.%d.%d\","\
    "\"is_st\" : \"%s\","\
    "\"st_ssid\":\"%s\","\
    "\"st_pwd\":\"%s\","\
    "\"st_fixed_ip\":\"%s\","\
    "\"st_ip\": \"%d.%d.%d.%d\","\
    "\"st_netmask\":\"%d.%d.%d.%d\","\
    "\"st_gateway\":\"%d.%d.%d.%d\","\
    "\"st_attempts\": %d"\
    "}";

static const char TAG[]="config_wifi";

void CFG_WiFiInit(WiFiConfig* theConfig)
{
    theConfig->m_StConn.m_IsEnabled = true;
    strcpy(theConfig->m_StConn.m_SSID, DEFAULT_ST_SSID);
    strcpy(theConfig->m_StConn.m_Password, DEFAULT_ST_PASSWORD);
    theConfig->m_StConn.m_IsFixedIP = true;
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
    theConfig->m_StAttemptsCount = ST_ATTEMPTS_COUNT;

    theConfig->m_APConn.m_IsEnabled = false;
    strcpy(theConfig->m_APConn.m_SSID, DEFAULT_AP_SSID);
    strcpy(theConfig->m_APConn.m_Password, DEFAULT_AP_PASSWORD);
    theConfig->m_APConn.m_IsFixedIP = true;
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
}

#define IS_ST_KEYWORD           "is_st"
#define ST_SSID_KEYWORD         "st_ssid"
#define ST_PWD_KEYWORD          "st_pwd"
#define ST_FIXED_IP_KEYWORD     "st_fixed_ip"
#define ST_IP_KEYWORD           "st_ip"
#define ST_NETMASK_KEYWOR       "st_netmask"
#define ST_GATEWAY_KEYWORD      "st_gateway"
#define ST_ATTEMPTS_KEYWORD     "st_attempts"

esp_err_t cfg_convertIP( char* theString, uint8_t* theIP)
{
    int anIP1;
    int anIP2;
    int anIP3;
    int anIP4;
    int aConvNumber = sscanf(theString, "%d.%d.%d.%d", &anIP1, &anIP2, &anIP3, & anIP4);
    if( aConvNumber != 4 ){
        ESP_LOGE(TAG,"Can't convert IP address. Wrong format");
        return ESP_FAIL;
    }
    theIP[0] = (uint8_t)anIP1;
    theIP[1] = (uint8_t)anIP2;
    theIP[2] = (uint8_t)anIP3;
    theIP[3] = (uint8_t)anIP4;
    return ESP_OK;
}

esp_err_t CFG_WiFiParseSettings(WiFiConfig* theConfig, cJSON* theJSON, bool isFullSet)
{
    if( isFullSet ){
        theConfig->m_APConn.m_IsEnabled = false;
        theConfig->m_APConn.m_IsFixedIP = false;
        theConfig->m_StConn.m_IsEnabled = false;
        theConfig->m_StConn.m_IsFixedIP = false;
    }
    cJSON* aDataItem = theJSON->child;
    while( aDataItem != NULL ){
        char* aVal = cJSON_GetStringValue(aDataItem);
        ESP_LOGI(TAG, "Item=%s, value=%s\n", aDataItem->string, aVal);
        if(strcmp( aDataItem->string, IS_AP_KEYWORD) == 0 ){
            if( strcmp(aVal, ON_VALUE) ==0 ){
                theConfig->m_APConn.m_IsEnabled = true;
            }
            else{
                theConfig->m_APConn.m_IsEnabled = false;
            }
        }
        else if(strcmp( aDataItem->string, AP_SSID_KEYWORD) == 0 ){
            if( strlen(aVal) > MAX_SSID_SIZE ){
                ESP_LOGE(TAG, "Len of access point SSID more than maximum");
                return ESP_FAIL;
            }
            strcpy(theConfig->m_APConn.m_SSID, aVal );
        }     
        else if(strcmp( aDataItem->string, AP_PWD_KEYWORD) == 0 ){
            if( strlen(aVal) > MAX_PASSWORD_SIZE ){
                ESP_LOGE(TAG, "Len of access point password more than maximum");
                return ESP_FAIL;
            }
            strcpy(theConfig->m_APConn.m_Password, aVal );
        }     
        else if(strcmp( aDataItem->string, AP_FIXED_IP_KEYWORD) == 0 ){
            if( strcmp(aVal, ON_VALUE) ==0 ){
                theConfig->m_APConn.m_IsFixedIP = true;
            }
            else{
                theConfig->m_StConn.m_IsFixedIP = false;
            }
        }     
        else if(strcmp( aDataItem->string, AP_IP_KEYWORD) == 0 ){
            if( cfg_convertIP(aVal, theConfig->m_APConn.m_Ip) != ESP_OK )
                return ESP_FAIL;
        }     
        else if(strcmp( aDataItem->string, AP_NETMASK_KEYWOR) == 0 ){
            if( cfg_convertIP(aVal, theConfig->m_APConn.m_NetMask) != ESP_OK )
                return ESP_FAIL;
        }
        else if(strcmp( aDataItem->string, AP_GATEWAY_KEYWORD) == 0 ){
            if( cfg_convertIP(aVal, theConfig->m_APConn.m_Gateway) != ESP_OK )
                return ESP_FAIL;
        }
        else if(strcmp( aDataItem->string, IS_ST_KEYWORD) == 0 ){
            if( strcmp(aVal, ON_VALUE) ==0 ){
                theConfig->m_StConn.m_IsEnabled = true;
            }
            else{
                theConfig->m_StConn.m_IsEnabled = false;
            }
        }
        else if(strcmp( aDataItem->string, ST_SSID_KEYWORD) == 0 ){
            if( strlen(aVal) > MAX_SSID_SIZE ){
                ESP_LOGE(TAG, "Len of station SSID more than maximum");
                return ESP_FAIL;
            }
            strcpy(theConfig->m_StConn.m_SSID, aVal );
        }     
        else if(strcmp( aDataItem->string, ST_PWD_KEYWORD) == 0 ){
            if( strlen(aVal) > MAX_PASSWORD_SIZE ){
                ESP_LOGE(TAG, "Len of station password more than maximum");
                return ESP_FAIL;
            }
            strcpy(theConfig->m_StConn.m_Password, aVal );
        }     
        else if(strcmp( aDataItem->string, ST_FIXED_IP_KEYWORD) == 0 ){
            if( strcmp(aVal, ON_VALUE) ==0 ){
                theConfig->m_StConn.m_IsFixedIP = true;
            }
            else{
                theConfig->m_StConn.m_IsFixedIP = false;
            }
        }     
        else if(strcmp( aDataItem->string, ST_IP_KEYWORD) == 0 ){
             if( cfg_convertIP(aVal, theConfig->m_StConn.m_Ip) != ESP_OK )
                return ESP_FAIL;
        }     
        else if(strcmp( aDataItem->string, ST_NETMASK_KEYWOR) == 0 ){
            if( cfg_convertIP(aVal, theConfig->m_StConn.m_NetMask) != ESP_OK )
                return ESP_FAIL;
        }
        else if(strcmp( aDataItem->string, ST_GATEWAY_KEYWORD) == 0 ){
            if( cfg_convertIP(aVal, theConfig->m_StConn.m_Gateway) != ESP_OK )
                return ESP_FAIL;
        }     
        else if(strcmp( aDataItem->string, ST_ATTEMPTS_KEYWORD) == 0 ){
            int aNumber=100;
            int aCnt = sscanf(aVal, "%d", &aNumber);
            if( aCnt != 1){
                ESP_LOGE(TAG, "Wrong wifi config format. Connection attemts count should be number.");
                return ESP_FAIL;
            }
            else{
                theConfig->m_StAttemptsCount = (uint8_t)aNumber;
            }
        }     
        aDataItem = aDataItem->next;
    }
    return ESP_OK;
}

esp_err_t CFG_WiFiGetSettingsString(WiFiConfig* theConfig, char* theBuffer)
{
    char* isAPStr = ON_VALUE;
    if( !theConfig->m_APConn.m_IsEnabled ){
        isAPStr = OFF_VALUE;
    }
    char* isSTStr = ON_VALUE;
    if( !theConfig->m_StConn.m_IsEnabled ){
        isSTStr = OFF_VALUE;
    }
    char* isSTFixedIPStr = ON_VALUE;
    if( !theConfig->m_StConn.m_IsFixedIP ){
        isSTFixedIPStr = OFF_VALUE;
    }
    char* isAPFixedIPStr = ON_VALUE;
    if( !theConfig->m_APConn.m_IsFixedIP ){
        isAPFixedIPStr = OFF_VALUE;
    }
    sprintf(theBuffer, WiFiSettingTemplate,
        isAPStr,
        theConfig->m_APConn.m_SSID,
        theConfig->m_APConn.m_Password,
        isAPFixedIPStr,
        theConfig->m_APConn.m_Ip[0], theConfig->m_APConn.m_Ip[1], theConfig->m_APConn.m_Ip[2], theConfig->m_APConn.m_Ip[3],
        theConfig->m_APConn.m_NetMask[0], theConfig->m_APConn.m_NetMask[1], theConfig->m_APConn.m_NetMask[2], theConfig->m_APConn.m_NetMask[3],
        theConfig->m_APConn.m_Gateway[0], theConfig->m_APConn.m_Gateway[1], theConfig->m_APConn.m_Gateway[2], theConfig->m_APConn.m_Gateway[3],
        isSTStr,
        theConfig->m_StConn.m_SSID,
        theConfig->m_StConn.m_Password,
        isSTFixedIPStr,
        theConfig->m_StConn.m_Ip[0], theConfig->m_StConn.m_Ip[1], theConfig->m_StConn.m_Ip[2], theConfig->m_StConn.m_Ip[3],
        theConfig->m_StConn.m_NetMask[0], theConfig->m_StConn.m_NetMask[1], theConfig->m_StConn.m_NetMask[2], theConfig->m_StConn.m_NetMask[3],
        theConfig->m_StConn.m_Gateway[0], theConfig->m_StConn.m_Gateway[1], theConfig->m_StConn.m_Gateway[2], theConfig->m_StConn.m_Gateway[3],
        theConfig->m_StAttemptsCount
        );
    return ESP_OK;
}