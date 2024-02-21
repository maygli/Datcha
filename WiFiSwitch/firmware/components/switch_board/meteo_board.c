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

#include <aht.h>
#include <bmp280.h>
#include <esp_log.h>

#include <freertos.h>
#include <task.h>

#include "meteo_board.h"
#include "board_config.h" 

#ifdef METEO_ENABLED

static const char TAG[]="aht_board";

typedef struct _MeteoBoard{
    float               m_Temperature;
    float               m_Humidity;
    float               m_Pressure;
    int                 m_Period;
    bool                m_IsUseBmpForTemperature;
    bool                m_IsBmpInstalled;
    bool                m_IsAHTInstalled;
    xSemaphoreHandle 	m_DataMutex;
    aht_t               m_AhtDev;
    bmp280_t            m_BmpDev;
} MeteoBoard;

static MeteoBoard m_MeteoBoard;

void  Meteo_setData(MeteoBoard* theBoard, float theTemperature, float thePressure, float theHumidity)
{
    if( theBoard->m_DataMutex ){
        if( xSemaphoreTake( theBoard->m_DataMutex, portMAX_DELAY ) == pdTRUE )
        {
            theBoard->m_Temperature = theTemperature;
            theBoard->m_Pressure = thePressure;
            theBoard->m_Humidity = theHumidity;
            xSemaphoreGive( theBoard->m_DataMutex );
        }
    }
}

void Meteo_Init()
{
    m_MeteoBoard.m_IsAHTInstalled = false;
    m_MeteoBoard.m_IsBmpInstalled = false;
    m_MeteoBoard.m_IsUseBmpForTemperature = true;
    m_MeteoBoard.m_IsBmpInstalled = false;
    m_MeteoBoard.m_IsAHTInstalled = false;
    m_MeteoBoard.m_IsUseBmpForTemperature = false;

    m_MeteoBoard.m_DataMutex = xSemaphoreCreateMutex();
    memset(&m_MeteoBoard.m_AhtDev,0, sizeof(m_MeteoBoard.m_AhtDev));
    m_MeteoBoard.m_AhtDev.mode = AHT_MODE_NORMAL;
    m_MeteoBoard.m_AhtDev.type = AHT_TYPE_AHT20;

    if( aht_init_desc(&m_MeteoBoard.m_AhtDev, AHT_I2C_ADDRESS_GND, 0, PIN_SDA, PIN_SCL) == ESP_OK ){
        m_MeteoBoard.m_IsAHTInstalled = true;
    }
    if( m_MeteoBoard.m_IsAHTInstalled && aht_reset(&m_MeteoBoard.m_AhtDev) != ESP_OK ){
        m_MeteoBoard.m_IsAHTInstalled = false;
    }
    if( m_MeteoBoard.m_IsAHTInstalled && aht_init(&m_MeteoBoard.m_AhtDev) != ESP_OK ){
        m_MeteoBoard.m_IsAHTInstalled = false;
    }

    bool calibrated;
    if( m_MeteoBoard.m_IsAHTInstalled && aht_get_status(&m_MeteoBoard.m_AhtDev, NULL, &calibrated)){
        if (calibrated)
            ESP_LOGE(TAG, "Sensor calibrated");
        else
            ESP_LOGE(TAG, "Sensor not calibrated!");
    }


    bmp280_params_t params;
    bmp280_init_default_params(&params);
    memset(&m_MeteoBoard.m_BmpDev, 0, sizeof(bmp280_t));

    if( bmp280_init_desc(&m_MeteoBoard.m_BmpDev, BMP280_I2C_ADDRESS_0, 0, PIN_SDA, PIN_SCL) == ESP_OK ){
        m_MeteoBoard.m_IsBmpInstalled = true;
    }
    if( m_MeteoBoard.m_IsBmpInstalled && bmp280_init(&m_MeteoBoard.m_BmpDev, &params) != ESP_OK ){
        m_MeteoBoard.m_IsBmpInstalled = false;
    }

    bool bme280p = m_MeteoBoard.m_BmpDev.id == BME280_CHIP_ID;
    ESP_LOGI(TAG, "BMP280: found %s\n", bme280p ? "BME280" : "BMP280");
}

void Meteo_Read()
{
    ESP_LOGI(TAG,"Meteo read");
    float pressure=0, aht_temperature=0, bmp_temperature=0, aht_humidity=0, bme_humidity=0;
    if( m_MeteoBoard.m_IsAHTInstalled){
        esp_err_t res = aht_get_data(&m_MeteoBoard.m_AhtDev, &aht_temperature, &aht_humidity);
        if (res == ESP_OK){
            ESP_LOGI(TAG, "AHT Temperature: %d°C, Humidity: %d%%", (int)(aht_temperature*100), (int)(aht_humidity*100));
        }
        else{
            aht_temperature = 0;
            aht_humidity = 0;
            ESP_LOGE(TAG, "AHT Error reading data: %d (%s)", res, esp_err_to_name(res));
        }
    } else{
        ESP_LOGI(TAG, "AHT not installed");
    }
    if( m_MeteoBoard.m_IsBmpInstalled ){
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (bmp280_read_float(&m_MeteoBoard.m_BmpDev, &bmp_temperature, &pressure, &bme_humidity) != ESP_OK)
        {
            bmp_temperature = 0;
            pressure = 0;
            bme_humidity = 0;
            ESP_LOGE(TAG, "BMP Temperature/pressure reading failed\n");
        }
        else{
            ESP_LOGI(TAG, "BMP Temperature: %d°C, Pressure: %d%%", (int)(bmp_temperature*100), (int)(pressure*100));
        }
    } else{
        ESP_LOGI(TAG,"BMP not installed");
    }
    Meteo_setData(&m_MeteoBoard, aht_temperature, pressure, aht_humidity );
}

void  Meteo_GetData(MeteoData* theData)
{
    if( m_MeteoBoard.m_DataMutex ){
        if( xSemaphoreTake( m_MeteoBoard.m_DataMutex, portMAX_DELAY ) == pdTRUE )
        {
            theData->m_Temperature = m_MeteoBoard.m_Temperature;
            theData->m_Pressure = m_MeteoBoard.m_Pressure;
            theData->m_Humidity = m_MeteoBoard.m_Humidity;
#ifdef TEMP_ENABLED
            theData->m_IsTemperature = true;
#else
            theData->m_IsTemperature = false;
#endif             
#ifdef PRESSURE_ENABLED
            theData->m_IsPressure = true;
#else
            theData->m_IsPressure = false;
#endif             
#ifdef HUMIDITY_ENABLED
            theData->m_IsHumidity = true;
#else
            theData->m_IsHumidity = false;
#endif        
            theData->m_TemperatureIndex = TEMPERATURE_UNIT_INDEX;
            theData->m_PressureIndex = PRESSURE_UNIT_INDEX;
            theData->m_HumidityIndex = HUMIDITY_UNIT_INDEX;
            xSemaphoreGive( m_MeteoBoard.m_DataMutex );
        }
    }
}

void Meteo_SetPeriod(MeteoBoard* theBoard)
{

}

void Meteo_SetUseBmpForTemp(MeteoBoard* theBoard)
{
    
}

#else

void  Meteo_GetData(MeteoData* theData)
{

}

#endif