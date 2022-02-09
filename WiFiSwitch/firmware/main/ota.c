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

#include "ota.h"

static const char TAG[]="ota";

esp_err_t OTA_Start(esp_ota_handle_t* theOTAHandle, esp_partition_t** thePartition)
{
    *theOTAHandle = 0;
    *thePartition = NULL;
    *thePartition = esp_ota_get_next_update_partition(NULL);
    if( (*thePartition) == NULL ){
        ESP_LOGE(TAG, "Can't find OTA partition for update");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Writing to partition %s subtype %d at offset 0x%x",
             (*thePartition)->label, (*thePartition)->subtype, (*thePartition)->address);
    esp_err_t anErr = esp_ota_begin(*thePartition, OTA_SIZE_UNKNOWN, theOTAHandle);
    if (anErr != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin failed, error=%d", anErr);
        return anErr;
    }
    ESP_LOGI(TAG, "esp_ota_begin succeeded");
    return ESP_OK;
}

esp_err_t OTA_Write(esp_ota_handle_t theOTAHandle, char* theBuffer, int theLen)
{
    esp_err_t anErr = esp_ota_write( theOTAHandle, (const void *)theBuffer, theLen);
    if( anErr != ESP_OK ){
        ESP_LOGE(TAG,"Error OTA wrire");
    }
    ESP_LOGI(TAG,"Written firmware data");
    return anErr;
}

esp_err_t OTA_WriteOTAComplete(esp_ota_handle_t theOTAHandle, esp_partition_t* theOTAPartition)
{
    esp_err_t anErr;
    anErr = esp_ota_end(theOTAHandle);
    if ( anErr != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_end failed!");
        return -1;
    }
    anErr = esp_ota_set_boot_partition(theOTAPartition);
    if (anErr != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed! err=0x%x", anErr);
        return -1;
    }
    ESP_LOGI(TAG,"Update firmware success");
    return ESP_OK;
}