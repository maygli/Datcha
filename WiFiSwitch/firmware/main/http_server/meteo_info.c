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

#include <esp_log.h>
#include <meteo_board.h>

#include "../common_def.h"

#include "http_server.h"
#include "board_info.h"

#define MAX_METEO_INFO_SZIE 512

static const char TAG[] = "board_info";

static const char TRUE_VALUE[] ="true";
static const char FALSE_VALUE[] ="false";

static const char MeteoInfoTemplate[] = "{"\
    "\"is_temp\" : %s,"\
    "\"temp\" : %d.%d,"\
    "\"temp_unit_index\":%d,"\
    "\"is_pressure\": %s,"\
    "\"pressure\": %d.%d,"\
    "\"pressure_unit_index\" : %d,"\
    "\"is_humidity\" : %s,"\
    "\"humidity\": %d.%d,"\
    "\"humidity_unit_index\" : %d"\
  "}";

esp_err_t HTTP_GetMeteoInfo(httpd_req_t *req)
{
    esp_err_t aRetVal;
    char aBuffer[MAX_METEO_INFO_SZIE];
    MeteoData aData;
    Meteo_GetData(&aData);
    int aTemperature = (int)(aData.m_Temperature*100.+0.5);
    int aPressure = (int)(aData.m_Pressure/10.+0.5);
    int aHumidity = (int)(aData.m_Humidity*100.+0.5);
    char* isTempVal = TRUE_VALUE;
    if( !aData.m_IsTemperature ){
        isTempVal = FALSE_VALUE;
    }
    char* isPressureVal = TRUE_VALUE;
    if( !aData.m_IsPressure ){
        isPressureVal = FALSE_VALUE;
    }
    char* isHumidity = TRUE_VALUE;
    if( !aData.m_IsHumidity ){
        isHumidity = FALSE_VALUE;
    }
    sprintf(aBuffer, MeteoInfoTemplate, 
        isTempVal, aTemperature/100, aTemperature % 100, aData.m_TemperatureIndex,      
        isPressureVal, aPressure/100, aPressure % 100, aData.m_PressureIndex,
        isHumidity, aHumidity/100, aHumidity % 100, aData.m_HumidityIndex);
    ESP_LOGI(TAG,"Get meteo info. Len=%d\n", strlen(aBuffer));
    HTTPServer* aServer = (HTTPServer*)req->user_ctx;
    httpd_resp_send_chunk(req, aBuffer, strlen(aBuffer));    
    httpd_resp_send_chunk(req, NULL, 0);    

    return ESP_OK;
}
