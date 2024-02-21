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

#include "mqtt_client.h"
#include "mqtt.h"
#include "esp_log.h"
#include "config_restart.h"

#include <switch_board.h>
#include <meteo_board.h>

#if MQTT_ENABLED
#define TAG "mqtt"

#define MAX_METEO_INFO_SZIE 64
#define MAX_TOPIC_SIZE      MAX_STR_SIZE

#define TEMPERATURE_TOPIC_NAME  "temperature"
#define PRESSURE_TOPIC_NAME     "pressure"
#define HUMIDATE_TOPIC_NAME     "humidity"
#define SWITCH_TOPIC_NAME       "switch"
#define RESTART_TOPIC_NAME      "restart"

#define AP_SYMBOL           'A'
#define ST_SYMBOL           'S'
#define MQTT_SYMBOL         'M'
#define HTTP_SYMBOL         'H'
#define RESTART_DATA_SIZE    2

static char aBuffer[MAX_METEO_INFO_SZIE];
static char aTopicBuffer[MAX_TOPIC_SIZE];

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event, BoardConfig* theCfg)
{
    esp_mqtt_client_handle_t client = event->client;
//    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            sprintf(aTopicBuffer,"%s%s", theCfg->m_MqttConfig.m_DevicePath, SWITCH_TOPIC_NAME);
            esp_mqtt_client_subscribe(client, aTopicBuffer, 1);
            sprintf(aTopicBuffer,"%s%s", theCfg->m_MqttConfig.m_DevicePath, RESTART_TOPIC_NAME);
            esp_mqtt_client_subscribe(client, aTopicBuffer, 1);
            if( theCfg->m_MqttConfig.m_ControlTopic != NULL ){
                ESP_LOGI(TAG, "Subscribe to control topic '%s'", theCfg->m_MqttConfig.m_ControlTopic);
                esp_mqtt_client_subscribe(client, theCfg->m_MqttConfig.m_ControlTopic, 1);
            }

//            msg_id = esp_mqtt_client_publish(client, "/user/Maygli/test", "data_3", 0, 1, 0);
//            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

//            msg_id = esp_mqtt_client_subscribe(client, "/user/Maygli/test", 0);
  //          ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

//            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
//            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

//            msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
//            ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED: {
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            }
            break;
        case MQTT_EVENT_SUBSCRIBED:
//            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
//            msg_id = esp_mqtt_client_publish(client, "/user/Maygli/test", "data", 0, 0, 0);
//            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            sprintf(aTopicBuffer,"%s%s", theCfg->m_MqttConfig.m_DevicePath, SWITCH_TOPIC_NAME);
            if( strncmp(aTopicBuffer, event->topic, event->topic_len) == 0){
                if( event->data[0] == '1'){
                    SWB_setBoardState(SS_ON);
                }
                else if( event->data[0] == '0' ){
                    SWB_setBoardState(SS_OFF);
                }
            }
            sprintf(aTopicBuffer,"%s%s", theCfg->m_MqttConfig.m_DevicePath, RESTART_TOPIC_NAME);
            if( strncmp(aTopicBuffer, event->topic, event->topic_len) == 0 ){
                uint8_t aRestartFlag = 0;
                if( event->data_len >= RESTART_DATA_SIZE){
                    uint8_t aRestartFlag = 0;
                    if( event->data[0] == AP_SYMBOL ){
                        aRestartFlag |= RESTART_AP_FLAG;
                    }
                    if( event->data[1] == HTTP_SYMBOL ){
                        aRestartFlag |= RESTART_HTTP_FLAG;
                    }
                    CFG_RestartSaveToFile(&theCfg->m_RestartMode, aRestartFlag);
                    SWB_reset();

                }
            }
            if( strncmp(theCfg->m_MqttConfig.m_ControlTopic, event->topic, event->topic_len) == 0 ){
                ESP_LOGI(TAG, "Get value from control topic");
                strncpy(aTopicBuffer, event->data, event->data_len);
                aTopicBuffer[event->data_len] = 0;
                double aVal = atof(aTopicBuffer);
                ESP_LOGI(TAG, "  Value=%d", (int)aVal);
                SWB_SetValue(aVal);
            }
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    BoardConfig* aCfg = (BoardConfig*)handler_args;
    mqtt_event_handler_cb(event_data, aCfg);
}

static void mqtt_app(BoardConfig* theCfg)
{
    ESP_LOGI(TAG, "============== MQTT  task started ================");
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = theCfg->m_MqttConfig.m_Server,
        .username = theCfg->m_MqttConfig.m_User,
        .password = theCfg->m_MqttConfig.m_Password,
        .task_stack = 4096,
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, theCfg);
    esp_mqtt_client_start(client);
    ESP_LOGI(TAG, "MQTT has been started");
    while(1){
	    const TickType_t xDelay = 60000 / portTICK_PERIOD_MS;
	    vTaskDelay(xDelay);
        SWB_MeasureTemperature();
	    const TickType_t xDelay1 = 200 / portTICK_PERIOD_MS;
	    vTaskDelay(xDelay1);
        MeteoData aData;
        Meteo_GetData(&aData);
        int aTemperature = (int)(aData.m_Temperature*100.+0.5);
        int aPressure = (int)(aData.m_Pressure/10.+0.5);
        int aHumidity = (int)(aData.m_Humidity*100.+0.5);
        sprintf(aTopicBuffer, "%s%s", theCfg->m_MqttConfig.m_DevicePath, TEMPERATURE_TOPIC_NAME);
        ESP_LOGI(TAG, "Topic=%s", aTopicBuffer);
        sprintf(aBuffer,"%d.%d C", aTemperature/100, aTemperature%100);
        esp_mqtt_client_publish(client, aTopicBuffer, aBuffer, 0, 1, 0);
        sprintf(aTopicBuffer, "%s%s", theCfg->m_MqttConfig.m_DevicePath, HUMIDATE_TOPIC_NAME);
        sprintf(aBuffer,"%d.%d %%", aHumidity/100, aHumidity%100);
        esp_mqtt_client_publish(client, aTopicBuffer, aBuffer, 0, 1, 0);
        sprintf(aTopicBuffer, "%s%s", theCfg->m_MqttConfig.m_DevicePath, PRESSURE_TOPIC_NAME);
        sprintf(aBuffer,"%d.%d mm Hg", (aPressure*15)/200, aHumidity%100);
        esp_mqtt_client_publish(client, aTopicBuffer, aBuffer, 0, 1, 0);
        uint32_t size = esp_get_free_heap_size();
        ESP_LOGE("HEAP", "size=%d", size);
    }
}

void MQTT_AppStart( BoardConfig* theCfg )
{
    ESP_LOGI(TAG, "============== MQTT  mode ================");
    mqtt_app(theCfg);
/*   	xTaskCreate(mqtt_app, "mqtt_task", 16384, NULL, 10, NULL);
    while(1){
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }*/
}
#endif
