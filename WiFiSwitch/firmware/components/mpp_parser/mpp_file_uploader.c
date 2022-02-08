/* Name   : mpp_file_uploader.c
 * Purpose:
 * Contacts: mmaygli@gmail.com
 *
 * History:
 * 04.10.2021 - Maygli - Creation of the file
 *
 * You can use this file as you want.
 * Link to author is welcome but don't required
 */
#include "stddef.h"
#include "string.h"

#include "mpp_file_uploader.h"

#define NAME_NAME       "name"
#define NAME_SIZE       4
#define FILE_NAME       "filename"
#define FILE_NAME_SIZE  8
#define MAX_NAME_SIZE   256

int mpp_DataFinished(void* theContext)
{
  FileUploader* anUploader = (FileUploader*)theContext;
  anUploader->m_State = FUS_WAIT_FOR_PARAMETER_DATA;
  return 0;
}

int mpp_ReadHeaderParameter(char* theNamePtr, int theNameSize, char* theValuePtr, int theValueSize, void* theContext)
{
  FileUploader* anUploader = (FileUploader*)theContext;
  if( (theNameSize == NAME_SIZE) && (strncmp(theNamePtr,NAME_NAME,NAME_SIZE) == 0) ){
    anUploader->m_Name = theValuePtr;
    anUploader->m_NameSize = theValueSize;
    if( anUploader->m_State != FUS_WAIT_FOR_FILE_DATA ){
      anUploader->m_State = FUS_WAIT_FOR_PARAMETER_DATA;
    }
  }
  if( (theNameSize == FILE_NAME_SIZE) && (strncmp(theNamePtr,FILE_NAME,FILE_NAME_SIZE) == 0)){
    anUploader->m_State = FUS_WAIT_FOR_FILE_DATA;
    if( anUploader->cb_OpenFile ){
      return anUploader->cb_OpenFile(theValuePtr,theValueSize,anUploader->m_CBContext);
    }
  }
  return 0;
}

int mpp_ReadData(char* theBuffer, int theSize, void* theContext)
{
  FileUploader* anUploader = (FileUploader*)theContext;
  if( anUploader->m_State == FUS_WAIT_FOR_PARAMETER_DATA ){
    if( anUploader->cb_GetParameter != NULL ){
      return anUploader->cb_GetParameter(anUploader->m_Name,anUploader->m_NameSize,theBuffer, theSize, anUploader->m_CBContext);
    }
  }
  if( anUploader->m_State == FUS_WAIT_FOR_FILE_DATA ){
    if( anUploader->cb_WriteData != NULL ){
      return anUploader->cb_WriteData(theBuffer,theSize, anUploader->m_CBContext);
    }
  }
  return 0;
}

void MPP_FileUploaderInit(FileUploader* theUploader, char* theBoundStr, int theBoundStrLen, void* theContext)
{
  MPP_MultiPartInit(&theUploader->m_MPPProc,theBoundStr,theBoundStrLen);
  theUploader->m_MPPProc.m_CBContext = (void*)theUploader;
  theUploader->m_MPPProc.cb_GetHeaderParameter = &mpp_ReadHeaderParameter;
  theUploader->m_MPPProc.cb_DataPart = &mpp_ReadData;
  theUploader->m_MPPProc.cb_DataFinished = &mpp_DataFinished;
  theUploader->m_State = FUS_WAIT_FOR_PARAMETER_DATA;
  theUploader->m_CBContext = theContext;
  theUploader->cb_GetParameter = NULL;
  theUploader->cb_OpenFile = NULL;
  theUploader->cb_WriteData = NULL;
}

int  MPP_FileUploaderProcess(FileUploader* theUploader, char* theBuffer, int theBufferSize)
{
  return MPP_MultiPartProcess(&theUploader->m_MPPProc, theBuffer, theBufferSize);
}

