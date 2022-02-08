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

#include <stdio.h>
#include <sys/param.h>
#include <esp_log.h>
#include "esp_system.h"    

#include "mpp_file_uploader.h"

#include "../common_def.h"

#include "http_upload.h"
#include "http_server.h"
#include "http_utils.h"

#define CLEAR_PARAMETER_NAME "clear"
#define TRUE_VALUE "true"

typedef struct _UploadContext{
    HTTPServer*    m_Server;
    FILE*          m_SavedFile;
    bool           m_isClear;
} UploadContext;

static const char TAG[]="upload_file";

void http_InitUploadContext(UploadContext* theContext, HTTPServer* theServer)
{
    theContext->m_Server = theServer;
    theContext->m_SavedFile = NULL;
    theContext->m_isClear = false;
}

int http_OpenFile(char* theFileName, int theNameSize, void* theContext)
{
    char aFilePath[MAX_PATH_SIZE];
    int  aPathLen;
    UploadContext* aContext = (UploadContext*)theContext;
/*Copy file to upload folder*/
    strcpy(aFilePath,UPLOAD_DIR);
    aPathLen = strlen(aFilePath);
    aFilePath[aPathLen] = '/';
    aPathLen++;
    strncpy(aFilePath + aPathLen, theFileName, theNameSize);
    aFilePath[aPathLen+theNameSize] = 0;
    ESP_LOGI(TAG,"Open file %s", aFilePath);
    aContext->m_SavedFile = fopen(aFilePath,"w");
    return 0;
}

int http_GetParameter(char* theName, int theNameSize, char* theData, int theDataSize,  void* theContext)
{
    char aNameBuffer[256];
    char aDataBuffer[256];
    strncpy(aNameBuffer, theName, theNameSize);
    aNameBuffer[theNameSize] = 0;
    strncpy(aDataBuffer, theData, theDataSize);
    aDataBuffer[theNameSize] = 0;
    UploadContext* aContext = (UploadContext*)theContext;
    ESP_LOGI(TAG,"Found parameter %s=%s", aNameBuffer, aDataBuffer);
    if( strncmp(theName, CLEAR_PARAMETER_NAME, theNameSize) == 0 ){
        if(strncmp(theData, TRUE_VALUE, theDataSize) == 0 ){
            ESP_LOGI(TAG,"Clear is set");
            aContext->m_isClear = true;
        }
    }
    return 0;
}

int http_WriteFileData(char* theDataPtr, int theLen, void* theContext)
{
    UploadContext* aContext = (UploadContext*)theContext;
    if( aContext->m_SavedFile != NULL ){
        ESP_LOGI(TAG,"Write data to file %d\n", theLen);
        int aWriteCnt = fwrite(theDataPtr, 1, theLen, aContext->m_SavedFile);
        if( aWriteCnt != theLen ){
            ESP_LOGE(TAG,"Error write to uplaoded file. Try to write %d bytes, but actually %d\n", theLen, aWriteCnt);
            return -1;
        }
        ESP_LOGI(TAG,"Data is written");
        return 0;
    }
    ESP_LOGE(TAG,"Error write to uplaoded file. File is not open");
    return -1;
}

int http_WriteComplete(bool isClear)
{
    FILE* aCompleteFile = fopen(COMPLETE_FILE_PATH,"w");
    if( aCompleteFile == NULL ){
      ESP_LOGE(TAG,"Can't open 'complete' file for write.");
      return -1;
    }
    int aCnt = fwrite(&isClear, sizeof(isClear), 1, aCompleteFile);
    if( aCnt != 1 ){
      ESP_LOGE(TAG,"Error write to 'complete' file.");
      return -1;
    }
    fclose(aCompleteFile);
    return 0; 
}

char* HTTP_GetBound(httpd_req_t *req, char* theBuffer, int* theBoundSize)
{
    char*       aBound = NULL;
    esp_err_t   aRes;
    size_t aLen = httpd_req_get_hdr_value_len(req,"Content-Type");
    if( aLen > (MAX_BOUND_SIZE-1) ){
        ESP_LOGE(TAG,"Content-Type header is too long");
        httpd_resp_send_500(req);
        return NULL;
    }
    ESP_LOGI(TAG,"POST Content-Type_len=%d", aLen);
    aRes = httpd_req_get_hdr_value_str(req,"Content-Type", theBuffer, MAX_BOUND_SIZE-1);
    if( aRes != ESP_OK ){
        ESP_LOGE(TAG,"Can't get header string");
        httpd_resp_send_500(req);
        return NULL;
    }
    theBuffer[aLen] = 0;
    ESP_LOGI(TAG,"POST Content-Type_val=%s", theBuffer);
    aBound = MPP_GetBoundary(theBuffer,theBoundSize);
    aBound[*theBoundSize] = 0;
    ESP_LOGI(TAG,"POST Boundary=%s", aBound);
    return aBound;
}

esp_err_t HTTP_ProcessFileUpload(httpd_req_t *req, FileUploader* theFileUploader, char* theBuff, int theBuffSize)
{
    int aRemaining = req->content_len;
    int aReceived = 0;
    int aRes;
    ESP_LOGI(TAG,"HTTP_ProcessFileUpload=%d", req->content_len);

    while (aRemaining > 0) {

 //       ESP_LOGI(TAG, "Remaining size : %d", aRemaining);
        /* Receive the file part by part into a buffer */
        if ((aReceived = httpd_req_recv(req, theBuff, MIN(aRemaining, theBuffSize-1))) <= 0) {
            if (aReceived == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry if timeout occurred */
                continue;
            }
            /* In case of unrecoverable error,
             * close and delete the unfinished file*/
//            fclose(fd);
//            unlink(filepath);

            ESP_LOGE(TAG, "File upload failed!");
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
//        theBuff[aReceived] = 0;
//        printf(theBuff);
//        printf("\n===================\n");
        aRes = MPP_FileUploaderProcess(theFileUploader,theBuff,aReceived);
        if( aRes != 0 ){
            ESP_LOGE(TAG, "Error process aReceived data");
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        aRemaining -= aReceived;
    }
    return ESP_OK;
}

esp_err_t HTTP_UploadFile(httpd_req_t *req)
{
    ESP_LOGI(TAG,"POST content_len=%d uri='%s'", req->content_len, req->uri);
    int     aRes;
    char    aBoundStr[MAX_BOUND_SIZE];
    int     aBoundSize;
    FileUploader  aFileUploader;
    UploadContext anUploadContext;
    
    HTTPServer* aServer = (HTTPServer*)req->user_ctx;
    char *aBuff = aServer->m_FileBuffer;

    http_InitUploadContext(&anUploadContext,aServer);

    char* aBound = HTTP_GetBound(req,aBoundStr,&aBoundSize);

    MPP_FileUploaderInit(&aFileUploader,aBound,aBoundSize,&anUploadContext);
    aFileUploader.cb_OpenFile = &http_OpenFile;
    aFileUploader.cb_GetParameter = &http_GetParameter;
    aFileUploader.cb_WriteData = &http_WriteFileData;

    aRes = HTTP_ProcessFileUpload(req,&aFileUploader,aBuff,HTTP_BUFFER_SIZE);
    if( anUploadContext.m_SavedFile != NULL ){
        fclose(anUploadContext.m_SavedFile);
    }
    if( aRes != ESP_OK ){
        return aRes;
    }
    const char aMsg[] = "File successfully uploaded";
    httpd_resp_send(req, aMsg, strlen(aMsg));

    http_WriteComplete(anUploadContext.m_isClear);

    ESP_LOGI(TAG, "File reception complete. Restart");
    vTaskDelay(UPLOAD_COMPLETE_DELAY/portTICK_PERIOD_MS);
    esp_restart();
    return ESP_OK;
}