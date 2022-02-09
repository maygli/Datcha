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

#include <sys/stat.h>
#include <sys/unistd.h>
#include <dirent.h>
#include <string.h>

#include "FreeRTOS.h"
#include <task.h>

#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>

#include <esp_ota_ops.h>
#include <esp_partition.h>

#include "common_def.h"
#include "file_system_utils.h"
#include "tarextractor.h"
#include "updater.h"
#include "ota.h"

#define OTA_COPY_BUFFER_SIZE 4096

static const char UPDATER_TAG[]="updater";

esp_err_t upd_createDefaultFolders()
{
  esp_err_t aRes=ESP_OK;
  aRes = FSU_CheckOrCreateDir(HTML_DIR);
  if( aRes != ESP_OK ){
    return aRes;
  }
  aRes = FSU_CheckOrCreateDir(UPLOAD_DIR);
  if( aRes != ESP_OK ){
    return aRes;
  }
  return aRes;
}

esp_err_t upd_CopyUploaded()
{
    esp_err_t aRes;
    DIR* aDir = opendir(UPLOAD_DIR);
    if( !aDir ){
        ESP_LOGE(UPDATER_TAG,"Can't open dir %s", UPLOAD_DIR);
        return ESP_FAIL;
    }
    char aSrcPath[MAX_PATH_SIZE];
    strcpy(aSrcPath, UPLOAD_DIR);
    int aSrcPathBaseLen = strlen(aSrcPath);
    char aDstPath[MAX_PATH_SIZE];
    while(true){
        struct dirent* de = readdir(aDir);
        if( !de ){
            break;
        }
        ESP_LOGI(UPDATER_TAG," Found entry %s type=%d", de->d_name, de->d_type);
        if( strcmp(de->d_name, COMPLETE_FILE_NAME) == 0)
          continue;
        FSU_AppendPath(aSrcPath, aSrcPathBaseLen, de->d_name);
        ESP_LOGI(UPDATER_TAG,"Process entry path=%s", aSrcPath);
        if( strcmp(de->d_name, CONFIG_FILE_NAME) ==0 ){
/* Config found */
          strcpy(aDstPath, INT_FLASH_BASE_PATH);
          FSU_AppendPath(aDstPath,strlen(aDstPath),de->d_name);
          ESP_LOGI(UPDATER_TAG,"Board config file found");
          aRes = FSU_FCopy(aSrcPath, aDstPath);
          if( aRes != ESP_OK ){
            ESP_LOGI(UPDATER_TAG,"Error copy board config to %s", aDstPath);
            return aRes;
          }
          continue;
        }
        if( strcmp(de->d_name, FIRMWARE_FILE_NAME) == 0 ){
/* Firmware file found. Do nothing. Process OTA at the end of copy process */
          ESP_LOGI(UPDATER_TAG,"Firmware file found");
          continue;
        }
        if( IS_FILE_EXT(de->d_name, TAR_EXT) ){
/* Tar file - need to extarct */
          aRes = TE_Extract(aSrcPath);
          if( aRes != ESP_OK )
            return aRes;
          ESP_LOGI(UPDATER_TAG,"Tar file found");
          continue;
        }
        else{
/* Just copy file to html dir */          
          strcpy(aDstPath, HTML_DIR);
          FSU_AppendPath(aDstPath,strlen(aDstPath),de->d_name);
          ESP_LOGI(UPDATER_TAG,"Copy uploaded file from '%s' to '%s'", aSrcPath, aDstPath);
          aRes = FSU_FCopy(aSrcPath, aDstPath);
          if( aRes != ESP_OK ){
            ESP_LOGI(UPDATER_TAG,"Error copy uploaded file");
            return aRes;
          }
        }  
    }
    closedir(aDir);
    return ESP_OK;
}

esp_err_t upd_UpdateFirmware(char* theFileName)
{
  FILE* aFirmwareFile = fopen(theFileName,"r");
  if( aFirmwareFile == NULL ){
    ESP_LOGE(UPDATER_TAG,"Can't open firmware file %s", theFileName);
    return ESP_FAIL;
  }
  esp_ota_handle_t    anOTAHandle;
  esp_partition_t*    aPartition;
  esp_err_t aRes = ESP_OK;
  aRes = OTA_Start(&anOTAHandle, &aPartition);
  if( aRes != ESP_OK )
    return aRes;
  int aReadSize;
  char aBuffer[OTA_COPY_BUFFER_SIZE];
  do{
      aReadSize = fread(aBuffer,1,OTA_COPY_BUFFER_SIZE, aFirmwareFile);
      if( aReadSize > 0 ){
        aRes = OTA_Write(anOTAHandle, aBuffer, OTA_COPY_BUFFER_SIZE);
        if( aRes != ESP_OK )
          break;
      }
  } while(aReadSize > 0 );
  fclose(aFirmwareFile);
  if( aRes == ESP_OK ){
    aRes = OTA_WriteOTAComplete(anOTAHandle, aPartition);
  }
  unlink(theFileName);
  return aRes;
}

esp_err_t upd_ProcessUploaded()
{
  bool isClear;
  esp_err_t aRes;
  struct stat aStat;
  FILE* aComplFile = fopen(COMPLETE_FILE_PATH,"r");
  if( aComplFile == NULL ){
    ESP_LOGE(UPDATER_TAG,"Can't open 'complete' file");
    return ESP_FAIL;
  }
  int aCnt = fread(&isClear, sizeof(isClear), 1, aComplFile);
  if( aCnt != 1 ){
    ESP_LOGE(UPDATER_TAG,"Can't read 'clear' flag from 'complete' file");
    return ESP_FAIL;
  }
  fclose(aComplFile);
  if( isClear ){
    FSU_RmTree(HTML_DIR);
  }
  ESP_LOGI(UPDATER_TAG,"'complete' file read. Clear=%d", isClear);
  aRes = FSU_CheckOrCreateDir(HTML_DIR);
  if( aRes != ESP_OK ){
    ESP_LOGE(UPDATER_TAG,"Can't check or create 'html' dir");
    return aRes;
  }
//Scan upload dir to find files
  aRes = upd_CopyUploaded();
  if( aRes != ESP_OK ){
    ESP_LOGE(UPDATER_TAG,"Can't copy files from upload dir");
    return aRes;
  }
  bool isNeedRestart = false;
  if( stat(FIRMWARE_FILE_PATH, &aStat) == ESP_OK ){
//Firmware file found - update firmware
    aRes = upd_UpdateFirmware(FIRMWARE_FILE_PATH);
    isNeedRestart = true;    
  }
  aRes = FSU_RmTree(UPLOAD_DIR);
  if( aRes != ESP_OK ){
    ESP_LOGE(UPDATER_TAG,"Can't remove upload dir");
    return aRes;
  }
  aRes = mkdir(UPLOAD_DIR,  0777);
  if( aRes != ESP_OK ){
    ESP_LOGE(UPDATER_TAG, "Can't create folder %s", UPLOAD_DIR);
  }
  if( isNeedRestart ){
    vTaskDelay(UPLOAD_COMPLETE_DELAY/portTICK_PERIOD_MS);
    esp_restart();
  }
  return ESP_OK;
}

esp_err_t UPD_Process()
{
  struct stat aStat;
  esp_err_t aRes;
  if( stat(COMPLETE_FILE_PATH, &aStat) == ESP_OK ){
//Complete file is created. Should process upload folder
    ESP_LOGI(UPDATER_TAG,"'complete' file found. Process upload");
    aRes = upd_ProcessUploaded();
    if( aRes != ESP_OK ){
      ESP_LOGE(UPDATER_TAG,"Can't process upload");
    }
  }
  else{
//Just clear upload folder and create default structure
    ESP_LOGI(UPDATER_TAG,"'complete' file not found. Create default tree");
  }
  FSU_RmTree(UPLOAD_DIR);
  return upd_createDefaultFolders();
}
