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

#include <switch_board.h>

#include "http_server.h"
#include "http_utils.h"
#include "wifi_settings.h"
#include "../config.h"

#define MAX_WIFI_JSON_SIZE 2048
#define WIFI_SETTINGS_COMPLETE_DELAY 1000

static const char TAG[] = "wifi_settings";

esp_err_t HTTP_SetWiFiSettings(httpd_req_t *req)
{
    esp_err_t aRetVal;
    HTTPServer* aServer = (HTTPServer*)req->user_ctx;
    BoardConfig* aConfig = aServer->m_BoardConfig;
    cJSON* aRootJson = HTTP_ReceiveJSON(req);
    if( aRootJson == NULL ){
        ESP_LOGE(TAG,"HTTP_SetWiFiSettings::Can't parse JSON file");
        return ESP_FAIL;
    }
    aRetVal = CFG_ParseWiFiSettings(aConfig, aRootJson, true);
    if( aRetVal != ESP_OK ){
        httpd_resp_send_500(req);
    }
    else{
        httpd_resp_send_chunk(req, NULL, 0);    
    }
    cJSON_Delete(aRootJson);
    SWB_reset();
    return aRetVal;
}

esp_err_t HTTP_GetWiFiSettings(httpd_req_t *req)
{
    char aRes = malloc(MAX_WIFI_JSON_SIZE);
    if( aRes == NULL ){
        ESP_LOGE(TAG,"Can't allocate memory for wifi settings");
        return ESP_ERR_NO_MEM;
    }
    HTTPServer* aServer = (HTTPServer*)req->user_ctx;
    BoardConfig* aConfig = aServer->m_BoardConfig;

    CFG_WiFiGetSettingsString(&aConfig->m_WiFiConfig,aRes);
    httpd_resp_send_chunk(req, aRes, strlen(aRes));    
    httpd_resp_send_chunk(req, NULL, 0);
    free(aRes);    
    return ESP_OK;
}
