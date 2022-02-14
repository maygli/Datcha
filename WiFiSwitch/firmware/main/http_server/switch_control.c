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

#include "switch_command.h"

#include "http_server.h"
#include "http_utils.h"
#include "switch_control.h"
#include "switch_board.h"
#include "cJSON.h"

static const char TAG[]="switch_control";

#define STATE_PARAMETER_NAME "state"
#define OUT_BUFFER_SIZE 128
#define ON_NAME "on"
#define OFF_NAME "off"

char* http_getJSONParameterValue(cJSON* theJson, char* theParam)
{
  cJSON* aJson = cJSON_GetObjectItem(theJson,theParam);
  if( aJson ){
    char* aVal = cJSON_GetStringValue(aJson);
    return aVal;
  }
  return NULL;
}

esp_err_t HTTP_GetSwitchState(httpd_req_t *req)
{
  char anOutBuffer[OUT_BUFFER_SIZE];
  SwitchState aState = getBoardState();
  if( aState == SS_ON ){
    sprintf(anOutBuffer,"{\"state\":\"%s\"}", ON_NAME);
  }
  else{
    sprintf(anOutBuffer,"{\"state\":\"%s\"}", OFF_NAME);
  }
  httpd_resp_send_chunk(req, anOutBuffer, strlen(anOutBuffer));    
  httpd_resp_send_chunk(req, NULL, 0);    
  return ESP_OK;
}

esp_err_t HTTP_SwitchControl(httpd_req_t *req)
{
    esp_err_t   aRes = ESP_OK;

    HTTPServer* aServer = ((HTTPServer*)req->user_ctx);

    ESP_LOGI(TAG,"Get file: '%s'", req->uri);

    cJSON* aRootJSON = HTTP_ReceiveJSON(req);
    if( aRootJSON ){
      char* aStateVal = http_getJSONParameterValue(aRootJSON, STATE_PARAMETER_NAME);
      if( aStateVal ){
        ESP_LOGI(TAG,"Switch state= '%s'", aStateVal);
        SwitchState aState = SS_OFF;
        if( strcmp(aStateVal, ON_NAME) == 0 ){
          aState = SS_ON;
        }
        setBoardState(aState);
      }
      httpd_resp_send_chunk(req, NULL, 0);    
      aRes = ESP_OK;
    }
    else{
      ESP_LOGE(TAG,"Error parse JSON");
      aRes = ESP_FAIL;
    }
    return aRes;
}
