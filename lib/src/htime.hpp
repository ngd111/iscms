/**
libNRcom.so htime.cpp header file ------------------------------------------

Description
시간 처리 관련 class

개발일자 : 2010-10-31
작성자   : 정진각
개정이력
---------------------------------------------------------------------
[개정일자] [개정사항 기술]

**/

#ifndef _HTIME_HPP
#define _HTIME_HPP

#include <time.h>      // for localtime, time
#include <stdio.h>     // for sprintf
#include <string.h>
#include <iostream>
#include <stdexcept>

namespace TimeOption
{
	enum Type
	{
		SECS,
		MIN,
		DAY
	};
	
	enum Format
	{
		YYYYMMDDHHMISS,
		YYYYMMDDHHMI,
		YYYYMMDD,
		HHMISS,
		HHMI
	};
}

using namespace std;

class hdatetime
{
	public: 
		hdatetime();   // Default system time
		hdatetime(int __Year, int __Month, int __Day, 
				 int __Hour, int __Min, int __Sec);
		hdatetime(int __Hour, int __Min);
		//hdatetime(string __YYYYMMDD) throw(const char *);
		hdatetime(string __YYYYMMDD) throw(length_error);
		
		time_t* get_time();
		time_t* add_time(TimeOption::Type __Option, long __Value);

		//time_t type의 시간을 YYYYMMDDHH24MISS형태로 변환한다
		string timeToStr(time_t __Tm);
		
		string get_time(TimeOption::Format);
		string get_systemtime(TimeOption::Format);
			
		long get_batchwaitingtime(short __CollectionInterval);
		time_t get_nextbatchtime_t(time_t __tCurrentTime, short __CollectionInterval);

		string get_previntervalstarttime(short __CollectionInterval, time_t __tCurrentTime);
		string get_currentintervalstarttime(short __CollectionInterval, time_t __tCurrentTime);
		time_t get_intervalendtime_t(short __CollectionInterval
		                           , const char* _pszWorkDate, const char* _pszStartTime);
		
		// 현 interval 기준으로 하루중 최종 interval의 starttime 계산
		// 예) Interval : 15 -> result : 2345
		//     interval : 30 -> result : 2330
		string get_lastintervalstarttime(short __CollectionInterval);

		const hdatetime& operator+(long __Sec);
		const hdatetime& operator+=(long __Sec);
		const hdatetime& operator-=(long __Sec);
		const hdatetime& operator-(long __Sec);
		const time_t operator-(hdatetime& __Time);

	private:
		struct tm m_tv;
		time_t m_time;
};

#endif // htime.hpp 
