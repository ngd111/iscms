/**
libNRcom.so hutil.cpp source file ------------------------------------------

Description
프로그램 개발에 사용될 공통 기능을 모아놓은 구현파일로 문자열 처리기능
, logging 관련 기능이 주요 구현내용이다


개발일자 : 2014-08-12
작성자   : 정진각
개정이력
---------------------------------------------------------------------
[개정일자] [개정사항 기술]

**/

#include <algorithm>
#include <stdlib.h>    // for atoi
#include <string.h>    // for memset, strcpy
#include <dirent.h>    // for opendir, closedir
#include <fcntl.h>     // for open
#include <unistd.h>    // for close
#include <sys/stat.h>  // for mkdir
#include <sys/types.h>

#include <fstream>
#include "hutil.hpp"


#define LOG_DIR_MAIN "slog"

hcommon::hcommon()
{	
	m_LogFile = "./system.log";
//	m_sModuleName = __func__;
	m_sModuleName = __FILE__;
}

hcommon::~hcommon()
{
	m_LogFile.clear();
	m_sModuleName.clear();
}

hcommon::hcommon(char *__logfilename)
{
	m_LogFile = __logfilename;
	m_sModuleName = __FILE__;
}

void hcommon::SetSystemLogFileName(string __log_path, const string __filename_prefix) {
	string currentDate = get_systemtime(TimeOption::YYYYMMDD);
	string logfilename = __log_path;
		
	logfilename = logfilename + "/" + LOG_DIR_MAIN + "/" + __filename_prefix + "." + currentDate;
	m_LogFile = logfilename;
}

string hcommon::GetSystemLogFileName() {
	return m_LogFile;
}

int hcommon::IsNumber(const char *__SourceString)
{
    char sDst [8192+1];

    memset(sDst, 0x00, sizeof(sDst));

    long n = atoi(__SourceString);

    sprintf(sDst, "%d", n);

    if (strcmp(__SourceString, sDst) == 0)
        return 1;
    else
        return 0;
}

int hcommon::ReadConfig(char *__configfilename, 
                         char *__itemname, char *__itemvalue)
{
	int i,j;
	char iBuffer[2][4096];
	char *sValues = __itemvalue;
	string sBuffer;
	ifstream rfs(__configfilename, ios::in);

	if (!rfs.is_open())
	{
		// failed to open file
		return -1;
	}

	while(getline(rfs, sBuffer))
	{
        sBuffer = RTrim(sBuffer);
		memset(iBuffer ,0x00 ,sizeof(iBuffer));

        i=j=0;

		for (string::const_iterator ci=sBuffer.begin();ci!=sBuffer.end();ci++)
		{
			switch (*ci)
			{
				case '#':
					break;
				
				case '=':
					RTrim(iBuffer[i]);
					i++;
					j=0;
					break;
				
				case ';':
					RTrim(iBuffer[i]);
					break;
				
				default:
					iBuffer[i][j] = *ci;
					j++;
					break;
			}

			if (*ci==';')
				if (strcmp(iBuffer[0], __itemname)==0)
				{
					RTrim(iBuffer[1]);
					strcpy(sValues, iBuffer[1]);
/*					m_sMessage = ">";
					m_sMessage = m_sMessage + "Read-Item " + iBuffer[0]
					             + "[" + __itemvalue + "]";
					writelog(WriteOption::LOG_HEADER, (char *)m_LogFile.c_str()
					         , (char *)m_sMessage.c_str());
*/
					break;
				}

			if (*ci=='#') break;
		}
	}
	
	return 0;
}

int hcommon::writelog(WriteOption::Type __Type, char *__filename, char *__msgbuf)
{
	ofstream wfs;
    int fd,n,i;
	char tmp[255], dir_path[255];
	DIR	*dp;
	string sDateTime;

	// 디렉토리가 존재하지 않을 경우 디렉토리 생성
	memset(tmp,	0x00, sizeof(tmp));
	strncpy(tmp, __filename, strlen(__filename));

	for(i =	sizeof(tmp)	- 1; tmp[i]	!= '/';	i--);
	if (i < 0) return -1; //directory 정보(/) 없을 경우
	memset(dir_path, 0x00, sizeof(dir_path));
	strncpy(dir_path, tmp, i + 1);

	if((dp = opendir(dir_path))	== (DIR	*)NULL)
	{
		if(mkdir(dir_path, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP)!=0)
			return -1;
	} 
	else
		closedir(dp);

	// log file을 append모드로 open한다
	if (__Type == WriteOption::LOG_CREATE)
		wfs.open(__filename, ios::trunc|ios::out);
	else
		wfs.open(__filename, ios::app|ios::out);

	if (!wfs.is_open())
	{
		// failed to open file
		return -1;
	}
	
	sDateTime = get_systemtime(TimeOption::YYYYMMDDHHMISS);

	// Log를 writing한다
	if ( (__Type == WriteOption::LOG_DATA) ||
		 (__Type == WriteOption::LOG_CREATE) )
		wfs << (char *)__msgbuf;		
	else if (__Type == WriteOption::LOG_HEADER)
	{
		wfs << sDateTime.substr(0, 8) << "-" << sDateTime.substr(8, 6) 
		    << ">" << __msgbuf << "\n";
	}
	else if (__Type == WriteOption::LOG_DESC)
		wfs << __msgbuf << endl;
	
	wfs.close();
	
	return 0;
}

string hcommon::GetEndTime(char* __StartTime, int __nInterval)
{
	hdatetime tmfn(__StartTime);
	
	tmfn+=((__nInterval * 60) - 1);
	
	return tmfn.get_time(TimeOption::HHMI);
}

void hcommon::ExtractExtension(char* __srcData, char __Data_out[EXTENSION_LEN]) 
{
    char *retData;
    char tempBuffer[128];
    bool bFind;
    int  i;

    memset(__Data_out, 0x00, sizeof(__Data_out));
    memset(tempBuffer, 0x00, sizeof(tempBuffer));
    
    bFind = false;
    i = 0;
    while(*__srcData) {
    	if (*__srcData == '@') {
    		bFind = true;
    		break;
    	}
    	tempBuffer[i] = *__srcData;
    	i++;
    	*__srcData++;	
    }
    
    if (bFind == false) return;

	retData = strstr(tempBuffer, ":");
    if (retData != NULL) {
		if (strlen(retData) > 1) {
        	memcpy(__Data_out, retData+1, strlen(retData)-1);
        }
	}

	return;
}

void hcommon::ExtractDomain(char* __srcData, char __Data_out[DOMAIN_LEN]) 
{
    char *retData;

    memset(__Data_out, 0x00, sizeof(__Data_out));

    retData = strstr(__srcData, "@");
    if (retData != NULL) {
        if (strlen(retData) > 1) {
            memcpy(__Data_out, retData+1, strlen(retData)-1);
        }
    }

    return;
}

void hcommon::LoggingError(const char *__ModuleName, 
	                        const char *__ActionName,
		                    const long  __nErrCode,
		                    const char *__ErrorMessage)
{
	char sErrCode[10];
	
	sprintf(sErrCode, "%ld", __nErrCode);

	// Header
	m_sMessage = "[";
	m_sMessage = m_sMessage + __ModuleName + "]-[";
	m_sMessage = m_sMessage + __ActionName + "]";
	writelog(WriteOption::LOG_HEADER, (char *)m_LogFile.c_str(), (char *)m_sMessage.c_str());
	// Description
	m_sMessage = "\t";
	m_sMessage = m_sMessage + "[CST-" + sErrCode + ", " + __ErrorMessage + "]";
	writelog(WriteOption::LOG_DESC, (char *)m_LogFile.c_str(), (char *)m_sMessage.c_str());

	return;
}

void hcommon::LoggingData(const char *__ModuleName, 
	                      const char *__ActionName, 
                          const char *__Data)
{	
	writelog(WriteOption::LOG_DATA, (char *)m_LogFile.c_str(), (char *)__Data);
		
	return;
}

hcommon::fp hcommon::get_writelog_ptr()
{
	return (&hcommon::writelog);
}

