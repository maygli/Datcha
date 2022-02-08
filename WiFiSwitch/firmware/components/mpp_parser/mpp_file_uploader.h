/* Name   : mpp_file_uploader.h
 * Purpose:
 * Contacts: mmaygli@gmail.com
 *
 * History:
 * 04.10.2021 - Maygli - Creation of the file
 *
 * You can use this file as you want.
 * Link to author is welcome but don't required
 */

#ifndef MPP_FILE_UPLOADER_H
#define MPP_FILE_UPLOADER_H

#include "mpp_parser.h"

typedef enum _FileUploaderState{ FUS_WAIT_FOR_PARAMETER_DATA, FUS_WAIT_FOR_FILE_DATA} FileUploaderState;

typedef struct _FileUploader{
  MultipartProcessor m_MPPProc;
  FileUploaderState  m_State;
  void*              m_CBContext;
  char*              m_Name;
  int                m_NameSize;
  int (*cb_GetParameter)(char* theName, int theNameSize, char* theData, int theDataSize,  void* theContext);
  int (*cb_OpenFile)(char* theFileName, int theNameSize, void* theContext);
  int (*cb_WriteData)(char* theDataPtr, int theLen, void* theContext);
} FileUploader;

#ifdef __cplusplus
  extern "C" {
#endif

void MPP_FileUploaderInit(FileUploader* theUploader, char* theBoundStr, int theBoundStrLen, void* theContext);
int  MPP_FileUploaderProcess(FileUploader* theUploader, char* theBuffer, int theBufferSize);

#ifdef __cplusplus
  }
#endif

#endif
