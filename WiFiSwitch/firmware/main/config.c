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
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#include <esp_err.h>
#include <esp_log.h>

#include <cJSON.h>

#include "switch_board.h"

#include "board_config.h"
#include "common_def.h"
#include "config.h"
#include "config_wifi.h"
#include "config_switch.h"

#define WIFI_SECTION_NAME "wifi"
#define SWITCH_SECTION_NAME "switch"
#define MAX_BUFFER_SIZE 768
#define MAX_CONFIG_FILE_BUFFER_SIZE 512

static const char TAG[] = "config";

cJSON* cfg_getJSON(char* theFileName)
{
    char aFileBuffer[MAX_CONFIG_FILE_BUFFER_SIZE];
    FILE* aCfgFile = fopen(theFileName, "r");
    if( aCfgFile == NULL ){
        ESP_LOGE(TAG, "Can't open config file %s", theFileName);
        return NULL;
    }
    size_t aRdCount = fread(aFileBuffer, sizeof(char), MAX_CONFIG_FILE_BUFFER_SIZE, aCfgFile);
    aFileBuffer[aRdCount] = 0;
    fclose(aCfgFile);
    cJSON* aCfgJson = cJSON_Parse((const char*)aFileBuffer);
    ESP_LOGI(TAG, "Converted JSON %d",aRdCount);
    ESP_LOGI(TAG, "%s",aFileBuffer);
    return aCfgJson;
}

esp_err_t cfg_loadFromFile(BoardConfig* theConfig, char* theFileName)
{
    esp_err_t aRes = ESP_OK;
    cJSON* aCfgJson = cfg_getJSON(theFileName);
    if( !aCfgJson ){
        ESP_LOGE(TAG, "Can't parse config file");
        return ESP_FAIL;
    }
    cJSON* aDataItem = aCfgJson->child;
    while( aDataItem != NULL ){
        if( strcmp(aDataItem->string, WIFI_SECTION_NAME) == 0 ){
            aRes = CFG_WiFiParseSettings(&theConfig->m_WiFiConfig, aDataItem, false);
            if( aRes != ESP_OK ){
                cJSON_Delete(aCfgJson); 
                return aRes;
            }
        }
        if( strcmp(aDataItem->string, SWITCH_SECTION_NAME) == 0 ){
            aRes = CFG_SwitchParseSettings(&theConfig->m_SwitchConfig, aDataItem, false);
            if( aRes != ESP_OK ){
                cJSON_Delete(aCfgJson); 
                return aRes;
            }
        }
        aDataItem = aDataItem->next;
    }
    ESP_LOGI(TAG, "Config loaded!");
    cJSON_Delete(aCfgJson); 
    return ESP_OK;
}

esp_err_t CFG_Init(BoardConfig* theConfig)
{
    esp_err_t aRes;
    CFG_SwitchInit(&theConfig->m_SwitchConfig);
    CFG_WiFiInit(&theConfig->m_WiFiConfig);

    struct stat aStat;
    aRes = stat(CONFIG_FILE_PATH,&aStat);
    if( aRes == ESP_OK ){
        if( aStat.st_size >= MAX_CONFIG_FILE_BUFFER_SIZE ){
            ESP_LOGE(TAG, "Can't read config file. File too big");
            return ESP_FAIL;
        }
        aRes = cfg_loadFromFile(theConfig, CONFIG_FILE_PATH);
        if( aRes != ESP_OK ){
            return aRes;
        }

    }
    aRes = stat(UPLOADED_CONFIG_PATH, &aStat);
    if( aRes == ESP_OK ){
        if( aStat.st_size >= MAX_CONFIG_FILE_BUFFER_SIZE ){
            ESP_LOGE(TAG, "Can't read uploaded config file. File too big");
            unlink(UPLOADED_CONFIG_PATH);
            return ESP_FAIL;
        }
        aRes = cfg_loadFromFile(theConfig, UPLOADED_CONFIG_PATH);
        unlink(UPLOADED_CONFIG_PATH);
        if( aRes != ESP_OK ){
            return aRes;
        }
    }
    return ESP_OK;
}

ConnectionInfo* CFG_GetSTConnection(BoardConfig* theConfig)
{
    return &theConfig->m_WiFiConfig.m_StConn;
}

ConnectionInfo* CFG_GetAPConnection(BoardConfig* theConfig)
{
    return &theConfig->m_WiFiConfig.m_APConn;
}

esp_err_t CFG_ParseWiFiSettings(BoardConfig* theConfig, cJSON* theJSON, bool isFullSet)
{
    esp_err_t aRes;
    aRes = CFG_WiFiParseSettings(&theConfig->m_WiFiConfig, theJSON, isFullSet);
    if( aRes != ESP_OK ){
        return aRes;
    }
    return CFG_SaveToFile(theConfig, CONFIG_FILE_PATH);
}

esp_err_t CFG_ParseSwitchSettings(BoardConfig* theConfig, cJSON* theJSON, bool isFullSet)
{
    esp_err_t aRes;
    aRes = CFG_SwitchParseSettings(&theConfig->m_SwitchConfig, theJSON, isFullSet);
    if( aRes != ESP_OK ){
        return aRes;
    }
    return CFG_SaveToFile(theConfig, CONFIG_FILE_PATH);
}

esp_err_t CFG_SaveToFile(BoardConfig* theConfig, const char* theFileName)
{
    esp_err_t aRes;
    char aBuffer[MAX_BUFFER_SIZE];
    FILE* aFile = fopen(theFileName, "w");
    if( aFile == NULL ){
        ESP_LOGE(TAG, "Can't open config file %s for write", theFileName);
        return ESP_FAIL;
    }
    fputs("{\"",aFile);
    fputs(WIFI_SECTION_NAME, aFile);
    fputs("\":", aFile);
    aRes = CFG_WiFiGetSettingsString(&theConfig->m_WiFiConfig, aBuffer);
    ESP_LOGI(TAG, "=====CFG_SaveToFile: WiFi config====");
    ESP_LOGI(TAG, aBuffer);    
    if( aRes != ESP_OK ){
        ESP_LOGE(TAG, "Error generates wifi settings string");
        fclose(aFile);
        unlink(theFileName);
        return aRes;
    }
    fputs(aBuffer, aFile);
    fputs(", \"", aFile);
    fputs(SWITCH_SECTION_NAME, aFile);
    fputs("\":", aFile);
    aRes = CFG_SwitchGetSettingsString(&theConfig->m_SwitchConfig, aBuffer);
    if( aRes != ESP_OK ){
        ESP_LOGE(TAG, "Error generates switch settings string");
        fclose(aFile);
        unlink(theFileName);
        return aRes;
    }
    fputs(aBuffer, aFile);
    fputs(" }", aFile);
    fclose(aFile);
    return ESP_OK;
}

