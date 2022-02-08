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

#include <ctype.h>
#include <string.h>
#include <esp_log.h>
#include <sys/param.h>

#include <esp_http_server.h>

#include "../file_system_utils.h"

#include "http_utils.h"
#include "http_server.h"

#define BOUNDARY_NAME "boundary"
#define BOUNDARY_SIZE 8

static const char TAG[] = "http_utils";

/* Set HTTP response content type according to file extension */
esp_err_t HTTP_SetReqTypeByExt(httpd_req_t *req, const char *filename)
{
    if (IS_FILE_EXT(filename, ".pdf")) {
        return httpd_resp_set_type(req, "application/pdf");
    } else if (IS_FILE_EXT(filename, ".html")) {
        return httpd_resp_set_type(req, "text/html");
    } else if (IS_FILE_EXT(filename, ".jpeg")) {
        return httpd_resp_set_type(req, "image/jpeg");
    } else if (IS_FILE_EXT(filename, ".ico")) {
        return httpd_resp_set_type(req, "image/x-icon");
    } else if (IS_FILE_EXT(filename, ".css")) {
        return httpd_resp_set_type(req, "text/css");
    } else if (IS_FILE_EXT(filename, ".js")) {
        return httpd_resp_set_type(req, "application/javascript");
    }
    /* This is a limited set only */
    /* For any other type always set as plain text */
    return httpd_resp_set_type(req, "text/plain");
}

cJSON* HTTP_ReceiveJSON(httpd_req_t *req)
{
    int             aReceived;
    cJSON           *aJson = NULL;
    HTTPServer*     aServer = (HTTPServer*)req->user_ctx;
    while(1){
        if ((aReceived = httpd_req_recv(req, aServer->m_FileBuffer, HTTP_BUFFER_SIZE)) <= 0) {
            if (aReceived == HTTPD_SOCK_ERR_TIMEOUT) {
                    /* Retry if timeout occurred */
                continue;
            }

            ESP_LOGE(TAG, "Can't receive data");
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_500(req);
            return NULL;
        }
        break;
    }
    aServer->m_FileBuffer[aReceived] = 0;
    ESP_LOGI(TAG,"Received=%s", aServer->m_FileBuffer);
    aJson = cJSON_Parse(aServer->m_FileBuffer);
    if( !aJson ){
        ESP_LOGE(TAG, "Can't parse JSON");
        httpd_resp_send_500(req);
    }
    return aJson;
}