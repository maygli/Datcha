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

#include <esp_err.h>

#include <cJSON.h>

#include "common_def.h"
#include "config_switch.h"
#include "config_wifi.h"

typedef struct _BoardConfig{
  WiFiConfig      m_WiFiConfig;
  SwitchConfig    m_SwitchConfig;
} BoardConfig;

esp_err_t CFG_Init(BoardConfig* theConfig);
StConnectionInfo* CFG_GetSTConnection(BoardConfig* theConfig);
ConnectionInfo* CFG_GetAPConnection(BoardConfig* theConfig);
esp_err_t CFG_ParseWiFiSettings(BoardConfig* theConfig, cJSON* theJSON, bool isFullSet);
esp_err_t CFG_ParseSwitchSettings(BoardConfig* theConfig, cJSON* theJSON, bool isFullSet);
esp_err_t CFG_SaveToFile(BoardConfig* theConfig, const char* theFileName);

#ifdef __cplusplus
}
#endif
