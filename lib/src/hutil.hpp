/**
libNRcom.so hutil.hpp header file ------------------------------------------

Description
NR 프로그램 개발에 사용될 공통 기능을 모아놓은 class로 구성된다.
본 hutil class는 HDateTime class와 HObject class를 상속받는다.

개발일자 : 2010-10-31
작성자   : 정진각
개정이력
---------------------------------------------------------------------
[개정일자] [개정사항 기술]

**/

#ifndef _HUTIL_HPP
#define _HUTIL_HPP

#include "htime.hpp"
#include "common.hpp"
#include "length_def.hpp"
#include "hstringutil.hpp"

/*
typedef struct __errorlist ERROR_LIST;

struct __errorlist {
	public:
		long nErrorCode,
		char sErrorMessage[200];
		
		bool operator==(const ERROR_LIST& other)
		{
			return (nErrorCode == other.nErrorCode);
		};
}
*/

// 멤버함수는 전부 소문자로 처리하자
class hcommon : public hdatetime, public hobject, public hstringutil
{
    public:
    	
		hcommon();
		hcommon(char *__logfilename);
		~hcommon();
		

		// 현재 시스템 시간 기준으로 로그파일명을 생성한다.
		void SetSystemLogFileName(const string __log_path, const string __filename_prefix);
		string GetSystemLogFileName();
		
		int ReadConfig(char *__configfilename, char *__itemname, 
		                char *__itemvalue);

		// CMS의 Interval을 이용 종료시간을 구한다
		// Parameter 1. StartTime : 시작시간(HH24MI)
		//           2. Interval  : 간격(단위, 분)
		// Return Value : 종료시간을 반환한다.
		string GetEndTime(char* __StartTime, int __nInterval);
		
		
		// File Log를 남긴다
		void LoggingError(const char *__ModuleName, const char *__ActionName, 
		          const long __nErrCode, const char *__ErrorMessage);

		void LoggingData(const char *__ModuleName, const char *__ActionName, 
		          const char *__Data);
		          
		// 문자열을 파악해 수치형 데이터인지 char형 데이터인지 반환한다.
		// Parameter SourceString : 분석할 문자열
		// Return Value ; 1:number,0:char
		int IsNumber(const char *__SourceString);
		
		// SIP URI형식의 데이터에서 실제 내선정보만 반환한다.
		static void ExtractExtension(char* __srcData, char __Data_out[EXTENSION_LEN]);
		
		// SIP URI형식에서 Domain name만 추출해서 반환한다.
		static void ExtractDomain(char* __srcData, char __Data_out[DOMAIN_LEN]);
		
		typedef int (hcommon::*fp)(WriteOption::Type, char*, char*);
		fp get_writelog_ptr();

	protected:
		int writelog(WriteOption::Type __Type, char *__filename, char *__msgbuf);
		

	private:
		//string LogFileFullPath;
		// Error메시지를 vector에 등록한다 - This method has become obsolete.
//		void LoadErrorMessage();
//		
//		vector<ERROR_LIST> m_errvec;
//		//list<ERROR_LIST> m_list;

};

#endif // hutil.hpp
