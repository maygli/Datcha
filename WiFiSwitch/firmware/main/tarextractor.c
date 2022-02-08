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
#include <string.h>

#include <sys/unistd.h>
#include <errno.h>

#include "esp_log.h"

#include "microtar.h"

#include "common_def.h"
#include "file_system_utils.h"
#include "tarextractor.h"

static const char TAG[] = "tar_extractor";
#define TE_BUFFER_SIZE 512

esp_err_t te_CreateDir(char* theName)
{
   esp_err_t aRes=ESP_OK;
   char aPath[MAX_PATH_SIZE];
   strcpy(aPath, HTML_DIR);
   FSU_AppendPath(aPath, strlen(aPath), theName);
   int aLen = strlen(aPath);
   if( aPath[aLen-1] == '/' ){
      aPath[aLen-1] = 0;
   }
   ESP_LOGI(TAG,"Create dir %s", aPath);
   aRes = FSU_CheckOrCreateDir(aPath);
   if( aRes != ESP_OK ){
      ESP_LOGE(TAG, "Error create or check folder");
      return aRes;
   }
   return aRes;
}

esp_err_t te_SaveFile(mtar_t* theTar, char* theFilePath, int theSize)
{
   esp_err_t aRes = ESP_OK;
   char* aFileName = FSU_GetFileNameByPath(theFilePath);
   char aDstFileName[MAX_PATH_SIZE];
   if( strcmp(aDstFileName, FIRMWARE_FILE_NAME) == 0 ){
      return ESP_OK;
   }
   if( strcmp(aFileName,CONFIG_FILE_NAME) == 0 ){
      strcpy(aDstFileName, INT_FLASH_BASE_PATH);
   }
   else{
      strcpy(aDstFileName, HTML_DIR);
   }
   FSU_AppendPath(aDstFileName, strlen(aDstFileName), theFilePath);
   ESP_LOGI(TAG,"Save file path=%s, size=%d", aDstFileName, theSize);
   FILE* aFile = fopen(aDstFileName, "w");
   if( aFile == NULL ){
      ESP_LOGE(TAG,"Can't open file %s", aDstFileName);
      return ESP_FAIL;
   }
   int aReminder = theSize;
   char aBuffer[TE_BUFFER_SIZE];
   while( aReminder ){
      int aCurrSize = TE_BUFFER_SIZE;
      if( aReminder < TE_BUFFER_SIZE ){
         aCurrSize = aReminder;
      }
      if( mtar_read_data(theTar, aBuffer, aCurrSize) != MTAR_ESUCCESS ){
         ESP_LOGE(TAG,"Can't read tar archive.");
         return ESP_FAIL;
      }
      int aWrCnt = fwrite(aBuffer, sizeof(char), aCurrSize, aFile);
      if( aWrCnt != aCurrSize ){
         ESP_LOGE(TAG,"Error write to file %s", aDstFileName);
         return ESP_FAIL;
      }
      aReminder -= aCurrSize;
   }
   fclose(aFile);
   return aRes;
}

esp_err_t TE_Extract(const char* theTarFilePath)
{
   esp_err_t   aRes;
   mtar_t      aTar;
   int         aTarRes;
   mtar_header_t aTarHeader;

/* Open archive for reading */
   aTarRes = mtar_open(&aTar, theTarFilePath, "r");
   if( aTarRes != MTAR_ESUCCESS ){
      ESP_LOGE(TAG, "Error open tar file %s", theTarFilePath);
   }
/* Print all file names and sizes */
   while ( (aTarRes = mtar_read_header(&aTar, &aTarHeader)) == MTAR_ESUCCESS ) {
//      ESP_LOGI(TAG, "Header found %s (%d bytes) type=%d", aTarHeader.name, aTarHeader.size, aTarHeader.type);
/* Process only folders and regular files */
      if(aTarHeader.type == MTAR_TDIR){
         aRes = te_CreateDir(aTarHeader.name);
         if( aRes != ESP_OK ){
            return aRes;
         }
      }
      if(aTarHeader.type == MTAR_TREG){
         aRes = te_SaveFile(&aTar, aTarHeader.name, aTarHeader.size);
         if( aRes != ESP_OK ){
            return aRes;
         }
      }
      mtar_next(&aTar);
   }
   mtar_close(&aTar);
   if( aTarRes != MTAR_ENULLRECORD ){
      ESP_LOGE(TAG, "Error read tar file %s", theTarFilePath);
      return ESP_FAIL;
   }
   return ESP_OK;
}
