/* Name   : MultPartParser.c
 * Purpose:
 * Contacts: mmaygli@gmail.com
 *
 * History:
 * 30.09.2021 - Maygli - Creation of the file
 *
 * You can use this file as you want.
 * Link to author is welcome but don't required
 */

#include "stddef.h"
#include "string.h"
#include "ctype.h"
#include "stdlib.h"

#include "mpp_parser.h"

#define BOUNDARY_NAME "boundary"
#define BOUNDARY_SIZE 8

char* MPP_GetBoundary( char* theContentType, int* theSize)
{
    char* aCurrPtr=NULL;
    char* aBoundary = strstr(theContentType, BOUNDARY_NAME);
    if( aBoundary == NULL ){
        return NULL;
    }
    aBoundary += BOUNDARY_SIZE;
    while( *aBoundary != 0 ){
        if( (*aBoundary != '=') &&  (!isspace(*aBoundary)) ){
            aCurrPtr = aBoundary;
            *theSize = 0;
            while( *aCurrPtr != 0 ){
                if( isspace(*aCurrPtr) || (*aCurrPtr == ';')){
                    return aBoundary;
                }
                (*theSize)++;
                aCurrPtr++;
            }
            return aBoundary;
        }
        aBoundary++;
    }
    return NULL;
}

int mpp_strncmp(char* theBuffer, int theSize, char* theStringToFind)
{
  int aFindStrLen = strlen(theStringToFind);
  if( theSize < aFindStrLen )
    return -1;
  return memcmp(theBuffer,theStringToFind,aFindStrLen);
}

int mpp_memncmp(char* theBuffer, int theSize, char* theBuffToFind, int theFindSize)
{
  if( theSize < theFindSize )
    return -1;
  return memcmp(theBuffer,theBuffToFind,theFindSize);
}

void mpp_trim(char* theBuffer,int* theStart, int* theSize)
{
  char* aPtr;
  while( (*theSize) > 0){
    if( isspace(theBuffer[*theStart]) ){
      (*theStart)++;
      (*theSize)--;
    }
    else{
      break;
    }
  }
  while( (*theSize) > 0){
    aPtr = theBuffer + (*theSize) - 1;
    if( isspace(*aPtr) ){
      (*theSize)--;
    }
    else{
      break;
    }
  }
}

void mpp_remove_quotas(char* theBuffer,int* theStart, int* theSize)
{
  if( theBuffer[*theStart] == '"' && theBuffer[*theStart + *theSize -1]){
    (*theStart) += 1;
    (*theSize) -= 2;
  }
}

void mpp_shift(MultipartProcessor* theProcessor, char* theBuffer)
{
  if( theBuffer[theProcessor->m_Indx] == '\r' || theBuffer[theProcessor->m_Indx] == '\n'){
    theProcessor->m_Indx++;
    theProcessor->m_Remain--;
    if( theBuffer[theProcessor->m_Indx] == '\n'){
      theProcessor->m_Indx++;
      theProcessor->m_Remain--;
    }
    return;
  }
  theProcessor->m_Indx++;
  theProcessor->m_Remain--;
}

void MPP_MultiPartInit(MultipartProcessor* theProcessor, char* theBoundStr, int theBoundStrLen)
{
    theProcessor->m_BoundStr = (char*)malloc(theBoundStrLen+1);
    strncpy(theProcessor->m_BoundStr, theBoundStr, theBoundStrLen);
    theProcessor->m_BoundStr[theBoundStrLen] = 0;
    theProcessor->m_BoundStrLen = theBoundStrLen;
    theProcessor->m_State = MPS_FIND_FIRST_DELIMETER;
    theProcessor->m_CBContext = NULL;
    theProcessor->cb_GetHeader = NULL;
    theProcessor->cb_GetHeaderParameter = NULL;
    theProcessor->cb_DataPart = NULL;
    theProcessor->cb_DataFinished = NULL;
}

int MPP_MultiPartProcess(MultipartProcessor* theProcessor, char* theBuffer, int theBufferSize)
{
  theProcessor->m_Indx = 0;
  theProcessor->m_Remain = theBufferSize;
  int   aSectionStart=0;
  int   aRes;
  char* aNamePtr = NULL;
  int   aNameSize = -1;
  int   aSize;
  while( theProcessor->m_Indx < theBufferSize ){
    switch(theProcessor->m_State){
      case MPS_FIND_FIRST_DELIMETER:{
        if(mpp_memncmp(theBuffer+theProcessor->m_Indx,theProcessor->m_Remain,theProcessor->m_BoundStr,theProcessor->m_BoundStrLen) == 0){
          theProcessor->m_Indx += theProcessor->m_BoundStrLen;
          theProcessor->m_Remain -= theProcessor->m_BoundStrLen;
          mpp_shift(theProcessor,theBuffer);
          aSectionStart = theProcessor->m_Indx;
          theProcessor->m_State = MPS_HEADER_NAME;
          continue;
        }
      }
      break;
      case MPS_HEADER_NAME:{
        if( theBuffer[theProcessor->m_Indx] == '\r' || theBuffer[theProcessor->m_Indx] == '\n'){
          mpp_shift(theProcessor,theBuffer);
          theProcessor->m_State = MPS_DATA_PART;
          aSectionStart = theProcessor->m_Indx;
          continue;
        }
        if( theBuffer[theProcessor->m_Indx] == ':'){
          aNameSize = theProcessor->m_Indx-aSectionStart;
          mpp_trim(theBuffer,&aSectionStart,&aNameSize);
          aNamePtr = theBuffer + aSectionStart;
          theProcessor->m_State = MPS_HEADER_VALUE;
          aSectionStart = theProcessor->m_Indx+1;
        }
      }
      break;
      case MPS_HEADER_VALUE:{
        if( theBuffer[theProcessor->m_Indx] == ';' || theBuffer[theProcessor->m_Indx] == '\r' || theBuffer[theProcessor->m_Indx] == '\n'){
          if( theProcessor->cb_GetHeader ){
            aSize = theProcessor->m_Indx-aSectionStart;
            mpp_trim(theBuffer,&aSectionStart,&aSize);
            aRes = theProcessor->cb_GetHeader(aNamePtr, aNameSize, theBuffer+aSectionStart, aSize, theProcessor->m_CBContext);
            if( aRes != 0 )
              return aRes;
          }
          if( theBuffer[theProcessor->m_Indx] == '\r' || theBuffer[theProcessor->m_Indx] == '\n'){
            theProcessor->m_State = MPS_HEADER_NAME;
          }
          else{
            theProcessor->m_State = MPS_HEADER_PARAMETER_NAME;
            aSectionStart = theProcessor->m_Indx + 1;
          }
        }
      }
      break;
      case MPS_HEADER_PARAMETER_NAME:{
/*Parse parameter name*/
        if( theBuffer[theProcessor->m_Indx] == '\r' || theBuffer[theProcessor->m_Indx] == '\n'){
          theProcessor->m_Indx++;
          theProcessor->m_Remain--;
          if( theBuffer[theProcessor->m_Indx] == '\n'){
            theProcessor->m_Indx++;
            theProcessor->m_Remain--;
          }
          aSectionStart = theProcessor->m_Indx + 1;
          theProcessor->m_State = MPS_HEADER_NAME;
          continue;
        }
        if( theBuffer[theProcessor->m_Indx] == '=' ){
          theProcessor->m_State = MPS_HEADER_PARAMETER_VALUE;
          aNameSize = theProcessor->m_Indx-aSectionStart;
          mpp_trim(theBuffer,&aSectionStart,&aNameSize);
          aNamePtr = theBuffer + aSectionStart;
          aSectionStart = theProcessor->m_Indx + 1;
        }
      }
      break;
      case MPS_HEADER_PARAMETER_VALUE:{
/*Parse parameter value*/
        if( theBuffer[theProcessor->m_Indx] == ';' || theBuffer[theProcessor->m_Indx]=='\r' || theBuffer[theProcessor->m_Indx]=='\r' ){
          if( theProcessor->cb_GetHeaderParameter ){
            aSize = theProcessor->m_Indx-aSectionStart;
            mpp_trim(theBuffer,&aSectionStart,&aSize);
            mpp_remove_quotas(theBuffer,&aSectionStart,&aSize);
            aRes = theProcessor->cb_GetHeaderParameter(aNamePtr, aNameSize, theBuffer+aSectionStart, aSize, theProcessor->m_CBContext);
            if( aRes != 0 )
              return aRes;
          }
          if( theBuffer[theProcessor->m_Indx] == ';'){
            theProcessor->m_State = MPS_HEADER_PARAMETER_NAME;
            aSectionStart = theProcessor->m_Indx + 1;
          }
          else{
            mpp_shift(theProcessor,theBuffer);
            aSectionStart = theProcessor->m_Indx;
            theProcessor->m_State = MPS_HEADER_NAME;
            continue;
          }
        }
      }
      break;
      case MPS_DATA_PART:{
/* Process data */
        if(mpp_memncmp(theBuffer+theProcessor->m_Indx,theProcessor->m_Remain,theProcessor->m_BoundStr,theProcessor->m_BoundStrLen) == 0){
          if( theProcessor->cb_DataPart ){
            aRes = theProcessor->cb_DataPart(theBuffer+aSectionStart,theProcessor->m_Indx-aSectionStart-4, theProcessor->m_CBContext);
            if( aRes != 0 )
              return aRes;
          }
          if( theProcessor->cb_DataFinished ){
            aRes = theProcessor->cb_DataFinished(theProcessor->m_CBContext);
            if( aRes != 0 )
              return aRes;
          }
          theProcessor->m_State = MPS_SKIP_CRLF;
          theProcessor->m_Indx += theProcessor->m_BoundStrLen;
          theProcessor->m_Remain -= theProcessor->m_BoundStrLen;
          continue;
        }
      }
      break;
      case MPS_SKIP_CRLF:{
/*Skip the data after delimeter string '--\r\n'*/
        if( theBuffer[theProcessor->m_Indx]=='\r' || theBuffer[theProcessor->m_Indx]=='\n' ){
          theProcessor->m_State = MPS_HEADER_NAME;
          mpp_shift(theProcessor,theBuffer);
          aSectionStart = theProcessor->m_Indx;
          continue;
        }
      }
      break;
    }
    mpp_shift(theProcessor,theBuffer);
  }
/*We reach the end of buffer but delimeter have not been found yet */
  if( theProcessor->m_State == MPS_DATA_PART ){
    if( theProcessor->cb_DataPart ){
      aRes = theProcessor->cb_DataPart(theBuffer+aSectionStart,theProcessor->m_Indx-aSectionStart, theProcessor->m_CBContext);
      if( aRes != 0 )
        return aRes;
    }
  }
  return 0;
}
