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
#include <esp_err.h>
#include <sys/stat.h>

#include "../file_system_utils.h"

#include "http_server.h"
#include "http_utils.h"
#include "server_config.h"
#include "server_get_file.h"
#include "server_default_page.h"

static const char SEND_FILE_TAG[]="send_file";

esp_err_t HTTP_SendDefaultPage(httpd_req_t *req)
{
    httpd_resp_send(req, HTTP_DEFAULT_PAGE, strlen(HTTP_DEFAULT_PAGE));
    return ESP_OK;     
}

esp_err_t HTTP_SendFile(httpd_req_t *req)
{
    esp_err_t   aRes;
    int         aSize;
    struct stat aStat;
    HTTPServer* aServer = ((HTTPServer*)req->user_ctx);

    ESP_LOGI(SEND_FILE_TAG,"Get file: '%s'", req->uri);
    if( ( strcmp(req->uri,DEFAULT_PAGE_URI) == 0) ){
        aRes = HTTP_SendDefaultPage(req);
        return aRes;
    }
    if( ( req->uri == NULL ) ||
        ( strcmp(req->uri,"/") == 0 ) ){
        aSize = FSU_AppendNPath(aServer->m_Path, aServer->m_BasePathSize, DEFAULT_PAGE_FILE_NAME, strlen(DEFAULT_PAGE_FILE_NAME));
        if( aSize < 0 ){
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        aRes = stat(aServer->m_Path,&aStat);
        if( aRes != ESP_OK ){
            aRes = HTTP_SendDefaultPage(req);
            return aRes;
        }
    }
    else{
        aSize = FSU_AppendNPath(aServer->m_Path, aServer->m_BasePathSize, req->uri, strlen(req->uri));
        if( aSize < 0 ){
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
    }
    ESP_LOGI(SEND_FILE_TAG,"Request file '%s'", aServer->m_Path);
    aRes = stat(aServer->m_Path,&aStat);
    if( aRes != ESP_OK ){
        ESP_LOGE(SEND_FILE_TAG,"Requested file '%s' not found", aServer->m_Path);
        httpd_resp_send_404(req);
        return aRes;
    }
    FILE* aFile = fopen(aServer->m_Path,"r");
    if( !aFile ){
        ESP_LOGE(SEND_FILE_TAG, "Failed to read existing file : %s", aServer->m_Path);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_500(req);
    }

    HTTP_SetReqTypeByExt(req,aServer->m_Path);

    do {
        /* Read file in chunks into the scratch buffer */
        aSize = fread(aServer->m_FileBuffer, 1, HTTP_BUFFER_SIZE, aFile);

        if (aSize > 0) {
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, aServer->m_FileBuffer, aSize) != ESP_OK) {
                fclose(aFile);
                ESP_LOGE(SEND_FILE_TAG, "File sending failed!");
                /* Abort sending file */
                httpd_resp_send_chunk(req, NULL, 0);
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_500(req);
                return ESP_FAIL;
           }
        }

        /* Keep looping till the whole file is sent */
    } while (aSize != 0);

    /* Close file after sending complete */
    fclose(aFile);
    ESP_LOGI(SEND_FILE_TAG, "File sending complete");

    /* Respond with an empty chunk to signal HTTP response completion */
    httpd_resp_send_chunk(req, NULL, 0);    
    return ESP_OK;
}
