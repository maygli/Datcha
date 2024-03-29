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

#pragma once

#include <stdint.h>
#include <stdbool.h>

#define BOARD_NAME "Datcha WiFiSwitch"
#define BOARD_HW_VERSION "v1.0.0"
#define BOARD_FW_VERSION "v0.0.1"
#define BOARD_DESCRIPTION "Datcha WiFi touch switch"
#define BOARD_MANUFACTURER "Maygli <a href=\\\"mailto:mmaygli@gmail.com\\\">mmaygli@gmail.com</a>"
#define BOARD_COPYRIGHT "&copy; Maygli 2021"
#define BOARD_DEVICES "WiFiSwitch"

#define MAX_PATH_SIZE           256

#define INT_FLASH_BASE_PATH 	"/int"
#define HTML_DIR  		        INT_FLASH_BASE_PATH"/html"
#define UPLOAD_DIR              INT_FLASH_BASE_PATH"/upload"
//#define UPLOAD_DIR              INT_FLASH_BASE_PATH
#define COMPLETE_FILE_NAME      "complete"
#define COMPLETE_FILE_PATH      UPLOAD_DIR"/"COMPLETE_FILE_NAME
#define RESET_FILE_NAME         "reset"
#define RESTART_CFG_FILE_PATH   INT_FLASH_BASE_PATH"/"RESET_FILE_NAME
#define TAR_EXT                 ".tar"
#define CONFIG_FILE_NAME	    "board_config.json"
#define CONFIG_FILE_PATH        INT_FLASH_BASE_PATH"/"CONFIG_FILE_NAME
#define UPLOAD_CONFIG_FILE_NAME "config.json"
#define UPLOADED_CONFIG_PATH    INT_FLASH_BASE_PATH"/"UPLOAD_CONFIG_FILE_NAME
#define FIRMWARE_FILE_NAME      "firmware.bin"
#define FIRMWARE_FILE_PATH      HTML_DIR"/"FIRMWARE_FILE_NAME

#define UPLOAD_COMPLETE_DELAY   1000

//This values should be set according IDF library
#define MAX_SSID_SIZE           31
#define MAX_PASSWORD_SIZE       31

#define IP_SIZE                 4  
#define AP_MAX_STA_CONN         4

#define DEFAULT_ST_SSID         "MaygliHome"
#define DEFAULT_ST_PASSWORD     "firebird"
#define ST_IP_0                 192
#define ST_IP_1                 168
#define ST_IP_2                 0
#define ST_IP_3                 105
#define ST_MASK_0               255
#define ST_MASK_1               255
#define ST_MASK_2               255
#define ST_MASK_3               0
#define ST_GATE_0               192
#define ST_GATE_1               168
#define ST_GATE_2               0
#define ST_GATE_3               1
#define ST_ATTEMPTS_COUNT       7
#define ST_CONNECT_AP_AFTER     true

#define DEFAULT_AP_SSID         "Datcha5"
#define DEFAULT_AP_PASSWORD     "11111111"
#define AP_IP_0                 192
#define AP_IP_1                 168
#define AP_IP_2                 1
#define AP_IP_3                 105
#define AP_MASK_0               255
#define AP_MASK_1               255
#define AP_MASK_2               255
#define AP_MASK_3               0
#define AP_GATE_0               192
#define AP_GATE_1               168
#define AP_GATE_2               1
#define AP_GATE_3               105

#define DEFAULT_ON_BRIGNESS      99
#define DEFAULT_OFF_BRIGNESS     95
#define DEFAULT_SWITCH_SOUND     true
#define DEFAULT_SWITCH_STYLE     0
#define DEFAULT_SWITCH_LIMIT     12
#define LIMIT_PRECISION          100

#define ON_VALUE    "on"
#define OFF_VALUE   "off"

#define MAX_STR_SIZE            256

#define DEFAULT_ENABLE_MQTT         true
#define DEFAULT_MQTT_SERVER         "mqtt://mqtt.by"
#define DEFAULT_MQTT_USER           "Maygli"
#define DEFAULT_MQTT_PASSWORD       "uvrgt8xy"
#define DEFAULT_MQTT_DEVICE_PATH    "/user/Maygli/Datcha/Heater1/"
#define DEFAULT_MQTT_CONTROL_TOPIC  "/user/Maygli/Datcha/Switch1/temperature"