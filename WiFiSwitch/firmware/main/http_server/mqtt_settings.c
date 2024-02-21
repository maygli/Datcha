#include <esp_log.h>

#include <switch_board.h>

#include "http_server.h"
#include "http_utils.h"
#include "mqtt_settings.h"
#include "../config.h"

#ifdef MQTT_ENABLED

#define MAX_MQTT_JSON_SIZE 768
#define MQTT_SETTINGS_COMPLETE_DELAY 1000

static const char TAG[] = "mqtt_settings";

esp_err_t HTTP_SetMqttSettings(httpd_req_t *req)
{
    esp_err_t aRetVal;
    HTTPServer* aServer = (HTTPServer*)req->user_ctx;
    BoardConfig* aConfig = aServer->m_BoardConfig;
    cJSON* aRootJson = HTTP_ReceiveJSON(req);
    if( aRootJson == NULL ){
        ESP_LOGE(TAG,"HTTP_SetMqttSettings::Can't parse JSON file");
        return ESP_FAIL;
    }
    aRetVal = CFG_ParseMqttSettings(aConfig, aRootJson, true);
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

esp_err_t HTTP_GetMqttSettings(httpd_req_t *req)
{
    char* aRes = malloc(MAX_MQTT_JSON_SIZE);
    if( aRes != NULL ){
        ESP_LOGE(TAG, "Can't allocate memory for MQTT settings");
        return ESP_ERR_NO_MEM;
    }
    HTTPServer* aServer = (HTTPServer*)req->user_ctx;
    BoardConfig* aConfig = aServer->m_BoardConfig;

    CFG_MqttGetSettingsString(&aConfig->m_MqttConfig,aRes);
    ESP_LOGI(TAG, "Read mqtt data=%s", aRes);
    httpd_resp_send_chunk(req, aRes, strlen(aRes));    
    httpd_resp_send_chunk(req, NULL, 0);
    free(aRes);    
    return ESP_OK;
}
#endif