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

#include <esp_err.h>
#include <esp_log.h>
#include <esp_http_server.h>

#include "../common_def.h"

#include "server_config.h"
#include "server_get_file.h"
#include "http_upload.h"
#include "switch_control.h"
#include "switch_settings.h"
#include "wifi_settings.h"
#include "http_server.h"
#include "board_info.h"
#include "meteo_info.h"
#include "mqtt_settings.h"

//Requests
#define SWITCH_STATE    "/switch_state"
#define SWITCH_CONTROL  "/switch_control"
#define SWITCH_SETTINGS "/switch_settings"
#define WIFI_SETTINGS   "/wifi_settings"
#define MQTT_SETTINGS   "/mqtt_settings"
#define BOARD_INFO      "/board_info"
#define METEO_STATE     "/meteo_state"

static const char SERVER_TAG[]="server";

esp_err_t HTTP_ServerStart(HTTPServer* theServer)
{
    if( theServer == NULL ){
        ESP_LOGE(SERVER_TAG,"HTTP_ServerStart: Can't init server. Pointer to structure is NULL");
        return ESP_ERR_INVALID_ARG;
    }
    theServer->m_HttpServer = NULL;
//    strcpy(theServer->m_Path,INT_FLASH_BASE_PATH"/"HTML_DIR_NAME"/");
    strcpy(theServer->m_Path,HTML_DIR"/");
    theServer->m_BasePathSize = strlen(theServer->m_Path);
    ESP_LOGI(SERVER_TAG,"Server base path=%s", theServer->m_Path);

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    /* Use the URI wildcard matching function in order to
     * allow the same handler to respond to multiple different
     * target URIs which match the wildcard scheme */
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.max_uri_handlers = 14;
    config.stack_size = 3000;
    
    ESP_LOGI(SERVER_TAG, "Starting HTTP Server");
    esp_err_t aRes = httpd_start(&theServer->m_HttpServer, &config);
    if ( aRes != ESP_OK ) {
        ESP_LOGE(SERVER_TAG, "Failed to start file server %d!", aRes);
        return ESP_FAIL;
    }

    httpd_uri_t aGetDefault = {
        .uri       = DEFAULT_PAGE_URI,  // Match all URIs of type /path/to/file
        .method    = HTTP_GET,
        .handler   = HTTP_SendFile,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aGetDefault);

    httpd_uri_t aGetBoardInfo = {
        .uri       = BOARD_INFO,  // Match all URIs of type /path/to/file
        .method    = HTTP_GET,
        .handler   = HTTP_GetBoardInfo,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aGetBoardInfo);

    httpd_uri_t aGetMeteoInfo = {
        .uri       = METEO_STATE,  // Match all URIs of type /path/to/file
        .method    = HTTP_GET,
        .handler   = HTTP_GetMeteoInfo,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aGetMeteoInfo);

    httpd_uri_t aGetSwitchState = {
        .uri       = SWITCH_STATE,  // Match all URIs of type /path/to/file
        .method    = HTTP_GET,
        .handler   = HTTP_GetSwitchState,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aGetSwitchState);

    httpd_uri_t aSwitchSettingsGet = {
        .uri       = SWITCH_SETTINGS,  // Match all URIs of type /path/to/file
        .method    = HTTP_GET,
        .handler   = HTTP_GetSwitchSettings,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aSwitchSettingsGet);

    httpd_uri_t aWiFiSettingsGet = {
        .uri       = WIFI_SETTINGS,  // Match all URIs of type /path/to/file
        .method    = HTTP_GET,
        .handler   = HTTP_GetWiFiSettings,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aWiFiSettingsGet);

#ifdef MQTT_ENABLED
    httpd_uri_t aMqttSettingsGet = {
        .uri       = MQTT_SETTINGS,  // Match all URIs of type /path/to/file
        .method    = HTTP_GET,
        .handler   = HTTP_GetMqttSettings,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aMqttSettingsGet);
#endif

    /* get html */
    httpd_uri_t aGetHtml = {
        .uri       = "*",  // Match all URIs of type /path/to/file
        .method    = HTTP_GET,
        .handler   = HTTP_SendFile,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aGetHtml);

    httpd_uri_t aSwitchControl = {
        .uri       = SWITCH_CONTROL,  // Match all URIs of type /path/to/file
        .method    = HTTP_POST,
        .handler   = HTTP_SwitchControl,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aSwitchControl);

    httpd_uri_t aSwitchSettingsSet = {
        .uri       = SWITCH_SETTINGS,  // Match all URIs of type /path/to/file
        .method    = HTTP_POST,
        .handler   = HTTP_SetSwitchSettings,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aSwitchSettingsSet);

    httpd_uri_t aWiFiSettingsSet = {
        .uri       = WIFI_SETTINGS,  // Match all URIs of type /path/to/file
        .method    = HTTP_POST,
        .handler   = HTTP_SetWiFiSettings,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aWiFiSettingsSet);

#ifdef MQTT_ENABLED
    httpd_uri_t aMqttSettingsSet = {
        .uri       = MQTT_SETTINGS,  // Match all URIs of type /path/to/file
        .method    = HTTP_POST,
        .handler   = HTTP_SetMqttSettings,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aMqttSettingsSet);
#endif

    httpd_uri_t aFileUpload = {
        .uri       = "*",  // Match all URIs of type /path/to/file
        .method    = HTTP_POST,
        .handler   = HTTP_UploadFile,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aFileUpload);

    return ESP_OK;
}

esp_err_t HTTP_ServerStop(HTTPServer* theServer)
{
    if( theServer == NULL ){
        ESP_LOGE(SERVER_TAG,"HTTP_ServerStop: Can't init server. Pointer to structure is NULL");
        return ESP_ERR_INVALID_ARG;
    }
    if( theServer->m_HttpServer == NULL ){
        return ESP_OK;
    }
    httpd_stop(theServer->m_HttpServer);
    return ESP_OK;
}
