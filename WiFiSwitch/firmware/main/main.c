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

#include <stdlib.h>

#include <sys/param.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <esp_system.h>
#include <esp_log.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <esp_vfs.h>
#include <esp_vfs_fat.h>
#include <esp_event.h>
#include <esp_netif.h>

#include <switch_board.h>
#include <meteo_board.h>
#include <switch_command.h>

#include "wifi.h"
#include "http_server/http_server.h"
#include "common_def.h"
#include "updater.h"
#include "switch_command.h"
#include "restart.h"
#include "control.h"
#include "http_client.h"

static const char *TAG="APP";

static Device       s_Device;
static HTTPServer   s_Server;

#define CONTROL_UPDATE_PERIOD 5000

void initInternalFlash()
{
    static wl_handle_t aWLHandle = WL_INVALID_HANDLE;
    ESP_LOGI(TAG, "Mounting internal flash FAT filesystem");
    // To mount device we need name of device partition, define base_path
    // and allow format partition in case if it is new one and was not formated before
    const esp_vfs_fat_mount_config_t mount_config = {
            .max_files = 5,
            .format_if_mount_failed = true,
            .allocation_unit_size = 512
    };
    esp_err_t err = esp_vfs_fat_spiflash_mount(INT_FLASH_BASE_PATH, "storage", &mount_config, &aWLHandle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(TAG, "Mount complete");
}

void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    s_Server.m_BoardConfig = &s_Device.m_Config;
    xTaskCreate(SWB_switchBoardTask, "switch_task", 3000, (void*)(&s_Device.m_Queue), 1, NULL);
    initInternalFlash();
    UPD_Process();
    ESP_ERROR_CHECK(CFG_Init(&s_Device.m_Config));

    ESP_ERROR_CHECK(CTRL_ControlTaskInit(&s_Device));
    xTaskCreate(CTRL_ControlTask, "control_task", 2048, (void*)(&s_Device),1, NULL);
    ESP_ERROR_CHECK(WiFi_Connect(&s_Server, &s_Device.m_Config));
}
