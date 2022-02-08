/* Name   : MultiPartParser.h
 * Purpose:
 * Contacts: mmaygli@gmail.com
 *
 * History:
 * 30.09.2021 - Maygli - Creation of the file
 *
 * You can use this file as you want.
 * Link to author is welcome but don't required
 */

#ifndef MULTIPARTPARSER_H
#define MULTIPARTPARSER_H

typedef enum _MultiPartState{MPS_FIND_FIRST_DELIMETER,
                             MPS_HEADER_NAME,
                             MPS_HEADER_VALUE,
                             MPS_HEADER_PARAMETER_NAME,
                             MPS_HEADER_PARAMETER_VALUE,
                             MPS_DATA_PART,
                             MPS_SKIP_CRLF} MultipartState;

typedef struct _MultiPartProcessor{
  char*           m_BoundStr;
  int             m_BoundStrLen;
  MultipartState  m_State;
  int             m_Indx;
  int             m_Remain;
  void*           m_CBContext;
  int             (*cb_GetHeader)(char* theNameBuffer, int theNameSize, char* theValueBuffer, int theValueSize, void* theContext);
  int             (*cb_GetHeaderParameter)(char* theBuffer, int theSize, char* theValueBuffer, int theValueSize, void* theContext);
  int             (*cb_DataPart)(char* theBuffer, int theSize, void* theContext);
  int             (*cb_DataFinished)(void* theContext);
} MultipartProcessor;

#ifdef __cplusplus
  extern "C" {
#endif


char* MPP_GetBoundary( char* theContentType, int* theSize);
void MPP_MultiPartInit(MultipartProcessor* theProcessor, char* theBoundStr, int theBoundStrLen);
int  MPP_MultiPartProcess(MultipartProcessor* theProcessor, char* theBuffer, int theBufferSize);

#ifdef __cplusplus
  }
#endif

#endif // MULTIPARTPARSER_H
