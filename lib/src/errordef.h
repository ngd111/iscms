/**
           errordef.h header file ---------------------------------------------

Description
NR ���α׷����� ���� ���� �ڵ带 ����Ѵ�.

�������� : 2010-10-31
�ۼ���   : ������
�����̷�
---------------------------------------------------------------------
[��������] [�������� ���]

**/

#ifndef _ERRORDEF_H
#define _ERRORDEF_H

#define NR_READ_SQL_FAILED   10001L
#define NR_READ_SQL_FAILED2  10002L
#define NR_READ_SQL_FAILED3  10003L
#define NR_INVALID_DATA_FILE 10004L
#define NR_INVALID_SQL_FILE  10005L  //SQL���ϸ��� �����Ǿ����� not found
#define NR_REQUEST_SQL_FILE  10006L  //SQL���ϸ��� �������� ���� ���
#define NR_REQUEST_DSN       10007L
#define NR_INVALID_DSN       10008L
#define NR_INVALID_DSN2      10009L
#define NR_REQUEST_JOB_PARAM 10010L  //Job Schedule Parameter ���� ����
#define NR_BAD_DATA_FILE     10011L  //DATA file���� ó������� �ƴ� ���


#endif // errordef.h
