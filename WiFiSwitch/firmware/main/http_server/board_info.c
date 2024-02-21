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

#include "../common_def.h"

#include "http_server.h"
#include "board_info.h"

#define MAX_BOARD_INFO_SIZE 512

static const char TAG[] = "board_info";

static const char BoardInfoTemplate[] = "{"\
    "\"board_name\": \"%s\","\
    "\"board_hw_version\": \"%s\","\
    "\"board_sw_version\": \"%s\","\
    "\"board_desc\": \"%s\","\
    "\"manufacturer\": \"%s\","\
    "\"copyright\": \"%s\","\
    "\"devices\": \"%s\""\
  "}";

esp_err_t HTTP_GetBoardInfo(httpd_req_t *req)
{
    esp_err_t aRetVal;
    char* aBuffer = malloc(MAX_BOARD_INFO_SIZE);
    if( aBuffer == NULL ){
        ESP_LOGE(TAG,"Can't allocate memory for board info");
        return ESP_ERR_NO_MEM;
    }
    sprintf(aBuffer, BoardInfoTemplate, 
        BOARD_NAME,
        BOARD_HW_VERSION,
        BOARD_FW_VERSION,
        BOARD_DESCRIPTION,
        BOARD_MANUFACTURER,
        BOARD_COPYRIGHT,
        BOARD_DEVICES);
    ESP_LOGI(TAG,"Get board info. Len=%d\n", strlen(aBuffer));
    HTTPServer* aServer = (HTTPServer*)req->user_ctx;
    httpd_resp_send_chunk(req, aBuffer, strlen(aBuffer));    
    httpd_resp_send_chunk(req, NULL, 0);    
    free(aBuffer);    
    return ESP_OK;
}
