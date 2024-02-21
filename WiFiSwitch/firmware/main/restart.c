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

#include "restart.h"
#include "common_def.h"
#include "esp_log.h"

#define RESTART_TAG "RESTART"

esp_err_t RESTART_Save( RestartFlag theFlag)
{
  FILE* aRestartFile = fopen(RESTART_CFG_FILE_PATH,"wb");
  if( aRestartFile == NULL ){
    ESP_LOGE(RESTART_TAG,"Can't open 'restart' file");
    return ESP_FAIL;
  }
  int aCnt = fwrite(&theFlag, sizeof(theFlag), 1, aRestartFile);
  if( aCnt != 1 ){
    ESP_LOGE(RESTART_TAG,"Can't write to 'restart' file");
    return ESP_FAIL;
  }
  fclose(aRestartFile);
  return ESP_OK;
}

RestartFlag RESTART_Read()
{
  FILE* aRestartFile = fopen(RESTART_CFG_FILE_PATH,"rb");
  if( aRestartFile == NULL ){
    ESP_LOGE(RESTART_TAG,"Can't open 'restart' file");
    return RF_NONE;
  }
  RestartFlag aFlag;
  int aCnt = fread(&aFlag, sizeof(aFlag), 1, aRestartFile);
  if( aCnt != 1 ){
    ESP_LOGE(RESTART_TAG,"Can't read restart flag from 'restart' file");
    return RF_NONE;
  }
  fclose(aRestartFile);
  return RF_NONE;
}
