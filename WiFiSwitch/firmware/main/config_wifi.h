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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include <esp_err.h>

#include <cJSON.h>

#include "common_def.h"

typedef struct _ConnectionInfo{
  bool      m_IsEnabled;
  char      m_SSID[MAX_SSID_SIZE];
  char      m_Password[MAX_PASSWORD_SIZE];
  bool      m_IsFixedIP;
  uint8_t   m_Ip[IP_SIZE];
  uint8_t   m_NetMask[IP_SIZE];  
  uint8_t   m_Gateway[IP_SIZE];  
} ConnectionInfo;

typedef struct _StConnectionInfo{
    ConnectionInfo  m_Connection;
    bool            m_IsConnectAPAfter;
    uint8_t         m_StAttemptsCount;
} StConnectionInfo;

typedef struct _WiFiConfig{
  ConnectionInfo    m_APConn;
  StConnectionInfo  m_StConn;
} WiFiConfig;

void CFG_WiFiInit(WiFiConfig* theConfig);
esp_err_t CFG_WiFiParseSettings(WiFiConfig* theConfig, cJSON* theJSON, bool isFullSet);
esp_err_t CFG_WiFiGetSettingsString(WiFiConfig* theConfig, char* theBuffer);

#ifdef __cplusplus
}
#endif
