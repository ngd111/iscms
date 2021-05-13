/**
           errordef.h header file ---------------------------------------------

Description
NR 프로그램에서 사용될 오류 코드를 기술한다.

개발일자 : 2010-10-31
작성자   : 정진각
개정이력
---------------------------------------------------------------------
[개정일자] [개정사항 기술]

**/

#ifndef _ERRORDEF_H
#define _ERRORDEF_H

#define NR_READ_SQL_FAILED   10001L
#define NR_READ_SQL_FAILED2  10002L
#define NR_READ_SQL_FAILED3  10003L
#define NR_INVALID_DATA_FILE 10004L
#define NR_INVALID_SQL_FILE  10005L  //SQL파일명은 지정되었으나 not found
#define NR_REQUEST_SQL_FILE  10006L  //SQL파일명이 지정되지 않은 경우
#define NR_REQUEST_DSN       10007L
#define NR_INVALID_DSN       10008L
#define NR_INVALID_DSN2      10009L
#define NR_REQUEST_JOB_PARAM 10010L  //Job Schedule Parameter 정보 누락
#define NR_BAD_DATA_FILE     10011L  //DATA file명이 처리대상이 아닌 경우


#endif // errordef.h
