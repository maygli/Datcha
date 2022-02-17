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

#include "sdkconfig.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event_loop.h"
#include "tcpip_adapter.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include "config.h"
#include "http_server/http_server.h"

#define GOT_IPV4_BIT BIT(0)

#define CONNECTED_BITS (BIT(0))
#define FAILED_BITS (BIT(1))

static EventGroupHandle_t s_connect_event_group;

static const char *TAG = "wifi";

static void wifi_OnAPStaConnected(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
    ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
}

static void wifi_OnAPStaDisonnected(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
    ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
}

static void wifi_OnStDisconnect(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    system_event_sta_disconnected_t *event = (system_event_sta_disconnected_t *)event_data;
    HTTPServer* aServer = (HTTPServer*)arg;
    if( aServer )
        HTTP_ServerStop(aServer);
    ESP_LOGI(TAG, "Wi-Fi disconnected, trying to reconnect...");
    if (event->reason == WIFI_REASON_BASIC_RATE_NOT_SUPPORT) {
        /*Switch to 802.11 bgn mode */
        esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
    }
    ESP_ERROR_CHECK(esp_wifi_connect());
}

static void wifi_OnStGotIp(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data)
{
    HTTPServer* aServer = (HTTPServer*)arg;
    tcpip_adapter_up(TCPIP_ADAPTER_IF_STA);
/*    if( aServer )
        HTTP_ServerStart(aServer);*/
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    xEventGroupSetBits(s_connect_event_group, GOT_IPV4_BIT);
    ESP_LOGI(TAG, "OnGotIp:" IPSTR, IP2STR(&event->ip_info.ip));
}

static esp_err_t wifi_configureStation(ConnectionInfo* theConn)
{
    esp_err_t aRes;

    wifi_config_t wifi_st_config = { 0 };

    strncpy((char *)&wifi_st_config.sta.ssid, theConn->m_SSID, MAX_SSID_SIZE);
    strncpy((char *)&wifi_st_config.sta.password, theConn->m_Password, MAX_PASSWORD_SIZE);

    ESP_LOGI(TAG, "Connecting to %s...", wifi_st_config.sta.ssid);
    aRes = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_st_config);
    if( aRes != ESP_OK )
        return aRes;
    if( aRes != ESP_OK )
        return aRes;
    if( theConn->m_IsFixedIP ){
        aRes = tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA);
        if( aRes != ESP_OK )
            return aRes;
        tcpip_adapter_ip_info_t aStIpConfig;
        IP4_ADDR(&aStIpConfig.ip, theConn->m_Ip[0], theConn->m_Ip[1], theConn->m_Ip[2], theConn->m_Ip[3]);
        IP4_ADDR(&aStIpConfig.gw, theConn->m_Gateway[0], theConn->m_Gateway[1], theConn->m_Gateway[2], theConn->m_Gateway[3]);
        IP4_ADDR(&aStIpConfig.netmask, theConn->m_NetMask[0], theConn->m_NetMask[1], theConn->m_NetMask[2], theConn->m_NetMask[3]);
        aRes = tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA, &aStIpConfig);
    }
    return aRes;
}

static esp_err_t wifi_configureAP(ConnectionInfo* theConn)
{
    esp_err_t aRes;
    wifi_config_t wifi_ap_config = {0};

    wifi_ap_config.ap.max_connection = AP_MAX_STA_CONN;
    wifi_ap_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    strncpy((char *)&wifi_ap_config.ap.ssid, theConn->m_SSID, MAX_SSID_SIZE);
    wifi_ap_config.ap.ssid_len = 0;
    strncpy((char *)&wifi_ap_config.ap.password, theConn->m_Password, MAX_PASSWORD_SIZE);
    if (strlen(theConn->m_Password) == 0) {
        wifi_ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    aRes = esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_ap_config);
    if( aRes != ESP_OK )
        return aRes;
    if( theConn->m_IsFixedIP ){
        aRes = tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP);
        if( aRes != ESP_OK )
            return aRes;
        tcpip_adapter_ip_info_t anAPIpConfig;
        IP4_ADDR(&anAPIpConfig.ip, theConn->m_Ip[0], theConn->m_Ip[1], theConn->m_Ip[2], theConn->m_Ip[3]);
        IP4_ADDR(&anAPIpConfig.gw, theConn->m_Gateway[0], theConn->m_Gateway[1], theConn->m_Gateway[2], theConn->m_Gateway[3]);
        IP4_ADDR(&anAPIpConfig.netmask, theConn->m_NetMask[0], theConn->m_NetMask[1], theConn->m_NetMask[2], theConn->m_NetMask[3]);
        aRes = tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &anAPIpConfig);
        if(aRes != ESP_OK)
            return aRes;
        aRes = tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP);
        if( aRes != ESP_OK )
            return aRes;
    }
    return aRes;
}

static bool wifi_start(HTTPServer* theServer, BoardConfig* theConfig)
{
//   esp_err_t aRes;
    bool aRes = true;
    ESP_LOGI(TAG,"wifi_Start");
    ConnectionInfo* anAPConn = CFG_GetAPConnection(theConfig);
    ConnectionInfo* aStConn = CFG_GetSTConnection(theConfig);

    wifi_mode_t aMode = WIFI_MODE_NULL;

    aStConn->m_IsEnabled = true;
    anAPConn->m_IsEnabled = true;

    if( aStConn->m_IsEnabled && anAPConn->m_IsEnabled){
        ESP_LOGI(TAG, "Set mode to APST");
        aMode = WIFI_MODE_APSTA;
        aRes = false;
    }
    else if( aStConn->m_IsEnabled ){
        ESP_LOGI(TAG, "Set mode to ST");
        aMode = WIFI_MODE_STA;
    }
    else if( anAPConn->m_IsEnabled ){
        ESP_LOGI(TAG, "Set mode to AP");
        aMode = WIFI_MODE_AP;
        aRes = false;
    }
    if( aMode == WIFI_MODE_NULL ){
        return false;
    }


    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(aMode));

    if( aStConn->m_IsEnabled ){
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &wifi_OnStDisconnect, theServer));
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_OnStGotIp, theServer));

        ESP_ERROR_CHECK(wifi_configureStation(aStConn));
    }

    if( anAPConn->m_IsEnabled ){
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_STACONNECTED, &wifi_OnAPStaConnected, theServer));
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_STADISCONNECTED, &wifi_OnAPStaDisonnected, theServer));
        ESP_ERROR_CHECK(wifi_configureAP(anAPConn));
    }

    ESP_ERROR_CHECK(esp_wifi_start());
    if( aMode != WIFI_MODE_AP ){
        ESP_ERROR_CHECK(esp_wifi_connect());
    }
    else{
        ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s",
             anAPConn->m_SSID, anAPConn->m_Password);
    }
    tcpip_adapter_ip_info_t anIpInfo;
    ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP, &anIpInfo));
    ESP_LOGI(TAG, "AP Ip:" IPSTR, IP2STR(&anIpInfo.ip));
    return aRes;
}

esp_err_t WiFi_Connect(HTTPServer* theServer, BoardConfig* theConfig)
{
    if (s_connect_event_group != NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    s_connect_event_group = xEventGroupCreate();
    if( wifi_start(theServer, theConfig) ){
        xEventGroupWaitBits(s_connect_event_group, CONNECTED_BITS, true, true, portMAX_DELAY);
    }
    if( theServer )
        HTTP_ServerStart(theServer);
    return ESP_OK;
}

