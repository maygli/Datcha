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

#include "control.h"
#include "config_restart.h"
#include "http_client.h"
#include <freertos/task.h>
#include <switch_board.h>
#include <meteo_board.h>

#define TAG "CONTROL_TASK"
#define WAIT_TIME 5000
#define READ_DELAY 1000
#define MAX_NOTIFY_SIZE 512

static const char SwitchSettingTemplate[] = "{"\
    "\"project\": \"%s\","\
    "\"device\": \"%s\","\
    "\"temperature\": %d.%d,"\
    "\"temperature_unit\": \"C\","\
    "\"pressure\": %d.%d,"\
    "\"pressure_unit\": \"Pa\","\
    "\"humidity\": %d.%d,"\
    "\"humidity_unit\": \"%%\","\
    "\"lock\": \"off\","\
    "\"state\": \"on\""\
  "}";

esp_err_t CTRL_ControlTaskInit(Device* theDevice)
{
    ESP_LOGI(TAG, "Control task init");

    theDevice->m_Queue = xQueueCreate(10, sizeof(Notification));
    return ESP_OK;
}

static int counter = 0;

void CTRL_sendNotification(){
    MeteoData aMeteoData;
    Meteo_Read();
    vTaskDelay(READ_DELAY/portTICK_PERIOD_MS);
    Meteo_GetData(&aMeteoData);
    int aTemperature = (int)(aMeteoData.m_Temperature*100.+0.5);
    int aPressure = (int)(aMeteoData.m_Pressure/10.+0.5);
    int aHumidity = (int)(aMeteoData.m_Humidity*100.+0.5);
    char aData[MAX_NOTIFY_SIZE] = {0};
    sprintf(aData,SwitchSettingTemplate,"datcha","Кухня термометр",
    aTemperature/100, abs(aTemperature)%100, aPressure/100, abs(aPressure)%100,
    aHumidity/100, abs(aHumidity)%100);
    ESP_LOGI(TAG, "Send data=%s",aData);
//    CHTTP_SendNotify("195.133.144.16", 6080, "/state", aData);   
    CHTTP_SendNotify("192.168.0.80", 6080, "/state", aData);   
}

void CTRL_ControlTask(void *arg)
{
    Device* aDevicePtr = (Device*)arg; 
    Notification aCmd;
    while(1){
        if (xQueueReceive(aDevicePtr->m_Queue, &aCmd, WAIT_TIME/portTICK_PERIOD_MS)) {
            switch( aCmd.m_Command){
                case NC_RESET:
                    CFG_RestartSaveToFile(&aDevicePtr->m_Config.m_RestartMode, 
                    RESTART_AP_FLAG|RESTART_HTTP_FLAG);
                    SWB_reset();
                    break;
            }
            
            ESP_LOGI(TAG, "Get board notification %d", aCmd.m_Command);
        }
        CTRL_sendNotification();
        vTaskDelay(100/portTICK_PERIOD_MS);
    }    
}
