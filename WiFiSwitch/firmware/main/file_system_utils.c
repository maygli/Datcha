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
#include <sys/stat.h>
#include <sys/unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>

#include "common_def.h"
#include "file_system_utils.h"

#define COPY_BUFFER_SIZE 4096

static const char FSU_TAG[]="fsu";

int FSU_AppendNPath(char* theBuffer, int theBaseSize, char* theFileName, int theFileNameSize)
{
    int aSize = theBaseSize;
    if( theBaseSize >= MAX_PATH_SIZE ){
        return -1;
    }
    if( theFileNameSize == 0 ){
        return theBaseSize;
    }
    if( ( theBuffer[aSize-1] != '/' ) && ( theFileName[0] != '/' )){
        theBuffer[aSize] = '/';
        aSize++;
    }
    if( (aSize + theFileNameSize) > MAX_PATH_SIZE )
        return -1;
    if( theFileNameSize > 0 ){
        strncpy(theBuffer+aSize,theFileName, theFileNameSize);
        aSize += theFileNameSize;
    }
    theBuffer[aSize] = 0;
    return aSize;
}

int FSU_AppendPath(char* theBuffer, int theBaseSize, char* theFileName)
{
    int aFileNameSize = 0;
    if( theFileName != NULL ){
        aFileNameSize = strlen(theFileName);
    }
    return FSU_AppendNPath(theBuffer, theBaseSize, theFileName, aFileNameSize);
}

esp_err_t fsu_CopyFile(char* theSrcPath, char* theDstPath)
{
    FILE* aSrcFile = NULL;
    FILE* aDstFile = NULL;
    char aBuffer[COPY_BUFFER_SIZE];
    int aReadSize;
    int aWriteSize;
    esp_err_t aRes = ESP_OK;

    aSrcFile = fopen(theSrcPath,"r");
    if( !aSrcFile ){
        ESP_LOGE(FSU_TAG,"Can't open source file %s", theSrcPath);
        return ESP_FAIL;
    }
    aDstFile = fopen(theDstPath,"w");
    if( !aDstFile ){
        ESP_LOGE(FSU_TAG, "Can't open destination file %s", theDstPath);
        fclose(aSrcFile);
        return ESP_FAIL;
    }
    do{
        aReadSize = fread(aBuffer,1,COPY_BUFFER_SIZE, aSrcFile);
        if( aReadSize > 0 ){
            aWriteSize = fwrite(aBuffer,1,aReadSize,aDstFile);
            if( aWriteSize != aReadSize ){
                ESP_LOGE(FSU_TAG, "Read %d bytes, write %d bytes. Source file %s, Destination file %s", aReadSize, aWriteSize, theSrcPath, theDstPath);
                aRes = ESP_FAIL;
                break;
            }
        }
    } while(aReadSize > 0 );
    fclose(aSrcFile);
    fclose(aDstFile);
    return aRes;
}

void fsu_PathJoin(char* thePath, char* theName, char* theResult)
{
    if( thePath == NULL ){
        theResult[0] = 0;
    }
    else{
        strcpy(theResult, thePath);
    }
    int aSize = strlen(theResult);
    theResult[aSize] = '/';
    if( theName == NULL )
        return;
    strcpy(theResult + aSize+1,theName);
}

esp_err_t fsu_CopyFolder(char* theSrcPath, char* theDstPath)
{
    esp_err_t aRes;
    char aDstFullPath[MAX_PATH_SIZE];
    char aSrcFullPath[MAX_PATH_SIZE];

    aRes = mkdir(theDstPath,  0777);
    if( aRes != ESP_OK ){
        ESP_LOGE(FSU_TAG, "Error %d create dir %s", errno, theDstPath);
        return aRes;
    }
    DIR* aDir = opendir(theSrcPath);
    if( !aDir ){
        ESP_LOGE(FSU_TAG,"Can't open dir %s", theSrcPath);
        return ESP_FAIL;
    }
    while(true){
        struct dirent* de = readdir(aDir);
        if( !de ){
            break;
        }
        ESP_LOGI(FSU_TAG," Found entity %s type=%d", de->d_name, de->d_type);
        fsu_PathJoin(theSrcPath, de->d_name, aSrcFullPath);
        fsu_PathJoin(theDstPath, de->d_name, aDstFullPath);
        if( de->d_type == DT_DIR ){
            aRes = fsu_CopyFolder(aSrcFullPath, aDstFullPath);
            if( aRes != ESP_OK ){
                ESP_LOGE(FSU_TAG,"Can't copy folder from %s to %s", aSrcFullPath, aDstFullPath);
                closedir(aDir);
                return aRes;
            } 
        }
        else{
            aRes = fsu_CopyFile(aSrcFullPath,aDstFullPath);
            if( aRes != ESP_OK ){
                ESP_LOGE(FSU_TAG,"Can't copy file from %s to %s", aSrcFullPath, aDstFullPath);
                closedir(aDir);
                return aRes;
            } 
        }
    }
    closedir(aDir);    
    return ESP_OK;
}

esp_err_t FSU_FCopy(char* theSrcPath, char* theDstPath)
{
    esp_err_t aRes;
    struct stat aStat;
    aRes = stat(theSrcPath,&aStat);
    if( aRes != ESP_OK ){
        ESP_LOGE(FSU_TAG,"Can't get statistic for %s", theSrcPath);
        return ESP_FAIL;
    }
    if( S_ISREG(aStat.st_mode) ){
        return fsu_CopyFile(theSrcPath, theDstPath);
    }
    if( S_ISDIR(aStat.st_mode) ){
        aRes = stat(theDstPath, &aStat);
        if( aRes == ESP_OK ){
            aRes = FSU_RmTree(theDstPath);
            if( aRes != ESP_OK ){
                ESP_LOGE(FSU_TAG,"Destination folder already exists. Can't remove. Dst folder %s", theDstPath);
                return ESP_FAIL;
            }
        }
        return fsu_CopyFolder(theSrcPath,theDstPath);
    }
    ESP_LOGE(FSU_TAG,"Unsopported file type %d",aStat.st_mode);
    return ESP_FAIL;
}

esp_err_t FSU_RmTree(char* thePath)
{
    char aFullPath[MAX_PATH_SIZE];
    esp_err_t aRes;
    DIR* aDir = opendir(thePath);
    if( !aDir ){
        ESP_LOGE(FSU_TAG,"Can't open dir %s", thePath);
        return ESP_FAIL;
    }
    while(true){
        struct dirent* de = readdir(aDir);
        if( !de ){
            break;
        }
        fsu_PathJoin(thePath, de->d_name, aFullPath);
        if( de->d_type == DT_DIR ){
            aRes = FSU_RmTree(aFullPath);
            if( aRes != ESP_OK ){
                ESP_LOGE(FSU_TAG,"Can't remove folder %s", aFullPath );
                closedir(aDir);
                return aRes;
            }
        }
        else{
            aRes = unlink(aFullPath);
            if( aRes != ESP_OK ){
                ESP_LOGE(FSU_TAG,"RmTree: Can't remove file %s", aFullPath );
                closedir(aDir);
                return aRes;
            }
        }
        ESP_LOGI(FSU_TAG," Found entity %s type=%d", de->d_name, de->d_type);
    }
    closedir(aDir);
    return rmdir(thePath);
}

esp_err_t FSU_CheckOrCreateDir( const char* theDirName )
{
  esp_err_t aRes;
  struct stat aStat;
  aRes = stat(theDirName,&aStat);
  if( aRes != ESP_OK ){
    ESP_LOGI(FSU_TAG,"Folder %s not found. Create", theDirName);
    aRes = mkdir(theDirName,  0777);
    if( aRes != ESP_OK ){
      ESP_LOGE(FSU_TAG, "Can't create folder %s", theDirName);
    }
  }
  return aRes;
}

char* FSU_GetFileNameByPath(char* thePath)
{
  char* aRes = strrchr(thePath,'/');
  if(aRes == NULL){
    return thePath;
  }
  return aRes+1;
}