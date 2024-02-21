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

#include "json_utils.h"
#include "common_def.h"
#include "stdlib.h"

static const char TAG[] = "json_utils";

esp_err_t JSU_ConverBool(cJSON* theJSON, bool* theRes)
{
    if( cJSON_IsString(theJSON) ){
        char* aVal = cJSON_GetStringValue(theJSON);
        if( aVal != NULL ){
            *theRes = false;
            if( strcmp(aVal, ON_VALUE) == 0 ){
                *theRes = true;
            }
            return ESP_OK;
        }
    }
    else if(cJSON_IsBool(theJSON)){
        *theRes = false;
        if( cJSON_IsTrue(theJSON) ){
            *theRes = true;
        }
        return ESP_OK;
    }
    ESP_LOGE(TAG, "Invalid format for JSON item %s. Expected boolean value or string with values 'on' ro 'off'.", theJSON->string);
    return ESP_OK;
}

esp_err_t JSU_ConverInt(cJSON* theJSON, int* theRes)
{
    if( cJSON_IsNumber(theJSON) ){
        *theRes = (int)theJSON->valuedouble;
        return ESP_OK;
    }
    else if(cJSON_IsString(theJSON)) {
        char* aVal = cJSON_GetStringValue(theJSON);
        if( aVal != NULL ){
            int aCnt = sscanf(aVal, "%d", theRes);
            if( aCnt != 1){
                ESP_LOGE(TAG, "Invalid format for JSON item %s. Expected integer value.", theJSON->string);
                return ESP_FAIL;
            }
            return ESP_OK;
        }
    }
    ESP_LOGE(TAG, "Invalid format for JSON item %s. Expected integer value.", theJSON->string);
    return ESP_FAIL;
}

esp_err_t JSU_ConverDouble(cJSON* theJSON, double* theRes)
{
    if( cJSON_IsNumber(theJSON) ){
        *theRes = theJSON->valuedouble;
        return ESP_OK;
    }
    else if(cJSON_IsString(theJSON)) {
        char* aVal = cJSON_GetStringValue(theJSON);
        if( aVal != NULL ){
            *theRes = atof(aVal);
            return ESP_OK;
        }
    }
    ESP_LOGE(TAG, "Invalid format for JSON item %s. Expected double value.", theJSON->string);
    return ESP_FAIL;
}

esp_err_t JSU_ConverString(cJSON* theJSON, char* theBuffer, size_t theStringMaxSize)
{
    if( cJSON_IsString(theJSON) ){
        char* aVal = cJSON_GetStringValue(theJSON);
        if( aVal != NULL ){
            if( strlen(aVal) >= theStringMaxSize ){
                ESP_LOGE(TAG, "Invalid format for JSON item %s. String value is too long.", theJSON->string);
                return ESP_FAIL;
            }
            strcpy(theBuffer, aVal);
            return ESP_OK;
        }
    }
    ESP_LOGE(TAG, "Invalid format for JSON item %s. Expected string.", theJSON->string);
    return ESP_FAIL;
}

esp_err_t JSU_ConvertIP(cJSON* theJSON, uint8_t* theRes)
{
    if( !cJSON_IsString(theJSON) ){
        ESP_LOGE(TAG, "Invalid format for JSON item %s. Expected string with ip address.", theJSON->string);
        return ESP_FAIL;
    }
    char* aVal = cJSON_GetStringValue(theJSON);
    if( aVal == NULL ){
        ESP_LOGE(TAG, "Invalid format for JSON item %s. Expected string with ip address.", theJSON->string);
        return ESP_FAIL;
    }
    int anIP1;
    int anIP2;
    int anIP3;
    int anIP4;
    int aConvNumber = sscanf(aVal, "%d.%d.%d.%d", &anIP1, &anIP2, &anIP3, & anIP4);
    if( aConvNumber != 4 ){
        ESP_LOGE(TAG, "Invalid format for JSON item %s. Expected string with ip address.", theJSON->string);
        return ESP_FAIL;
    }
    theRes[0] = (uint8_t)anIP1;
    theRes[1] = (uint8_t)anIP2;
    theRes[2] = (uint8_t)anIP3;
    theRes[3] = (uint8_t)anIP4;
    return ESP_OK;
   
}
