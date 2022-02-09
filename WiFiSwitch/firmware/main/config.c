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

#include <esp_err.h>
#include <esp_log.h>

#include "common_def.h"
#include "config.h"

//static const char TAG[]="config";

esp_err_t CFG_Init(BoardConfig* theConfig)
{
    theConfig->m_StConn.m_IsEnabled = false;
    strcpy(theConfig->m_StConn.m_SSID, DEFAULT_ST_SSID);
    strcpy(theConfig->m_StConn.m_Password, DEFAULT_ST_PASSWORD);
    theConfig->m_StConn.m_IsFixedAddress = true;
    theConfig->m_StConn.m_Ip[0] = ST_IP_0;
    theConfig->m_StConn.m_Ip[1] = ST_IP_1;
    theConfig->m_StConn.m_Ip[2] = ST_IP_2;
    theConfig->m_StConn.m_Ip[3] = ST_IP_3;
    theConfig->m_StConn.m_NetMask[0] = ST_MASK_0;
    theConfig->m_StConn.m_NetMask[1] = ST_MASK_1;
    theConfig->m_StConn.m_NetMask[2] = ST_MASK_2;
    theConfig->m_StConn.m_NetMask[3] = ST_MASK_3;
    theConfig->m_StConn.m_Gateway[0] = ST_GATE_0;
    theConfig->m_StConn.m_Gateway[1] = ST_GATE_1;
    theConfig->m_StConn.m_Gateway[2] = ST_GATE_2;
    theConfig->m_StConn.m_Gateway[3] = ST_GATE_3;

    theConfig->m_APConn.m_IsEnabled = true;
    strcpy(theConfig->m_APConn.m_SSID, DEFAULT_AP_SSID);
    strcpy(theConfig->m_APConn.m_Password, DEFAULT_AP_PASSWORD);
    theConfig->m_APConn.m_IsFixedAddress = true;
    theConfig->m_APConn.m_Ip[0] = AP_IP_0;
    theConfig->m_APConn.m_Ip[1] = AP_IP_1;
    theConfig->m_APConn.m_Ip[2] = AP_IP_2;
    theConfig->m_APConn.m_Ip[3] = AP_IP_3;
    theConfig->m_APConn.m_NetMask[0] = AP_MASK_0;
    theConfig->m_APConn.m_NetMask[1] = AP_MASK_1;
    theConfig->m_APConn.m_NetMask[2] = AP_MASK_2;
    theConfig->m_APConn.m_NetMask[3] = AP_MASK_3;
    theConfig->m_APConn.m_Gateway[0] = AP_GATE_0;
    theConfig->m_APConn.m_Gateway[1] = AP_GATE_1;
    theConfig->m_APConn.m_Gateway[2] = AP_GATE_2;
    theConfig->m_APConn.m_Gateway[3] = AP_GATE_3;

    return ESP_OK;
}

ConnectionInfo* CFG_GetSTConnection(BoardConfig* theConfig)
{
    return &theConfig->m_StConn;
}

ConnectionInfo* CFG_GetAPConnection(BoardConfig* theConfig)
{
    return &theConfig->m_APConn;
}
