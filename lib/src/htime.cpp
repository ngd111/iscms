/**
libNRcom.so htime.cpp source file ------------------------------------------

Description
시간 처리 관련 기능을 구현한다.
주로 시간을 더하고 빼는 계산에 관련된 기능을 구현한다.

개발일자 : 2010-10-31
작성자   : 정진각
개정이력
---------------------------------------------------------------------
[개정일자] [개정사항 기술]

**/

#include <stdlib.h>
#include "htime.hpp"


hdatetime::hdatetime()
{
	time(&m_time);
}

hdatetime::hdatetime(int __Hour, int __Min)
{
	struct tm* tmTime;
	
	memset(&m_tv, 0x00, sizeof(m_tv));

	time(&m_time);
	tmTime = localtime(&m_time);

	m_tv.tm_year = tmTime->tm_year;
	m_tv.tm_mon = tmTime->tm_mon;
	m_tv.tm_mday =tmTime->tm_mday;
    m_tv.tm_hour = __Hour;
    m_tv.tm_min = __Min;
    m_tv.tm_sec = 0;

    m_time = mktime(&m_tv);
}

hdatetime::hdatetime(int __Year, int __Month, int __Day,
	               int __Hour, int __Min, int __Sec)
{
	memset(&m_tv, 0x00, sizeof(m_tv));
	
    m_tv.tm_year = __Year-1900;
    m_tv.tm_mon = __Month-1;
    m_tv.tm_mday = __Day;
    m_tv.tm_hour = __Hour;
    m_tv.tm_min = __Min;
    m_tv.tm_sec = __Sec;

    m_time = mktime(&m_tv);
}

hdatetime::hdatetime(string __DateTime) throw(length_error)
{
	string sTmpMsg;
	char tmpmsg[1024];
	int nLength;
	struct tm* tmTime;
	
	memset(&m_tv, 0x00, sizeof(m_tv));
	
	nLength = __DateTime.length();
	
	if (nLength != 8 && nLength != 4)
	{
		sprintf(tmpmsg, "길이오류 8자리(YYYYMMDD)나 4자리(HH24MI)로 "
		                "지정하세요(입력길이:%d)", 
		                nLength);
		throw length_error(tmpmsg);
	}
	
	switch (nLength)
	{
		case 4:
			time(&m_time);              
			tmTime = localtime(&m_time);
			
			m_tv.tm_year = tmTime->tm_year;
			m_tv.tm_mon = tmTime->tm_mon;
			m_tv.tm_mday =tmTime->tm_mday;			
    		m_tv.tm_hour = atoi(__DateTime.substr(0, 2).c_str());
    		m_tv.tm_min = atoi(__DateTime.substr(2, 2).c_str());
			break;
			
		case 8:
			m_tv.tm_year = atoi(__DateTime.substr(0, 4).c_str())-1900;
			m_tv.tm_mon = atoi(__DateTime.substr(4, 2).c_str())-1;
			m_tv.tm_mday = atoi(__DateTime.substr(6, 2).c_str());
			
			break;
			
		default:
			break;
	}

	m_time = mktime(&m_tv);	
}
/*
hdatetime::hdatetime(string __YYYYMMDD) throw(const char *)
{
	memset(&m_tv, 0x00, sizeof(m_tv));
	
	if (__YYYYMMDD.length() != 8)
	{
		cout << "날짜오류" << __YYYYMMDD << endl;
		throw ((const char*)__YYYYMMDD.c_str());
	}	
	m_tv.tm_year = atoi(__YYYYMMDD.substr(0, 4).c_str())-1900;
	m_tv.tm_mon = atoi(__YYYYMMDD.substr(4, 2).c_str())-1;
	m_tv.tm_mday = atoi(__YYYYMMDD.substr(6, 2).c_str());	
	
	m_time = mktime(&m_tv);	
}
*/

time_t* hdatetime::get_time()
{
	return (&m_time);
}

string hdatetime::timeToStr(time_t __Tm)
{
	struct tm* tmTime;
	char sTime[14+1];
	
	tmTime = localtime(&__Tm);
    sprintf(sTime, "%04d%02d%02d%02d%02d%02d",
                    ((tmTime->tm_year) +1900/*1900년부터 시작됨.*/),
                    ((tmTime->tm_mon)  +1/*0~11 까지.*/),
                    (tmTime->tm_mday),
                    (tmTime->tm_hour),
                    (tmTime->tm_min),
                    (tmTime->tm_sec));

	return sTime;
}

string hdatetime::get_time(TimeOption::Format __Option)
{
	static struct tm* tmTime;
	char sTime[14+1];
	
	tmTime = localtime(&m_time);
	string strTime;
	
	switch (__Option)
	{
		case TimeOption::YYYYMMDDHHMISS:
			sprintf(sTime, "%04d%02d%02d%02d%02d%02d",
			               ((tmTime->tm_year)+1900), //1900년부터 시작됨
			               ((tmTime->tm_mon)+1),     //0~11
			               (tmTime->tm_mday),
			               (tmTime->tm_hour),
			               (tmTime->tm_min),
			               (tmTime->tm_sec));
			break;
		
		case TimeOption::YYYYMMDDHHMI:
			sprintf(sTime, "%04d%02d%02d%02d%02d",
			               ((tmTime->tm_year)+1900),
			               ((tmTime->tm_mon)+1),
			               (tmTime->tm_mday),
			               (tmTime->tm_hour),
			               (tmTime->tm_min));
			break;
		
		case TimeOption::YYYYMMDD:
			sprintf(sTime, "%04d%02d%02d",
			               ((tmTime->tm_year)+1900),
			               ((tmTime->tm_mon)+1),
			               (tmTime->tm_mday));
			break;
			
		case TimeOption::HHMISS:
			sprintf(sTime, "%02d%02d%02d",
			               (tmTime->tm_hour),
			               (tmTime->tm_min),
			               (tmTime->tm_sec));
			break;
			
		case TimeOption::HHMI:
			sprintf(sTime, "%02d%02d",
			               (tmTime->tm_hour),
			               (tmTime->tm_min));
			break;
			
		default:
			break;
	}
//std::cout << "sTime : " << sTime << std::endl;
	
	return sTime;
}

string hdatetime::get_systemtime(TimeOption::Format __Option)
{

	static struct tm* tmTime;
	time_t     lTime;
	
	time(&m_time);
	tmTime = localtime(&m_time);

	return (get_time(__Option));
}

time_t* hdatetime::add_time(TimeOption::Type __Option, long __Value)
{
	switch (__Option)
	{
		case TimeOption::SECS:
			m_time+=__Value;
			break;
			
		case TimeOption::MIN:
			m_time+=(__Value * 60);
			break;
			
		case TimeOption::DAY:
			m_time+=(__Value * 60 * 60 * 24);
			break;
			
		default:
			break;
	}
	
	return (&m_time);	
}

long hdatetime::get_batchwaitingtime(short __CollectionInterval)
{
	time_t tSystemTime;
	time_t tAdjustTime;
	time_t tResultTime;
	long   lAddSeconds;

	time(&tSystemTime);

	tAdjustTime = tSystemTime;
	lAddSeconds = (__CollectionInterval * 60);
	tAdjustTime+= lAddSeconds;
	//COLLETION_INTERVAL을 이용 현재 시스템 시간을 이용
	//다음 collection time때까지의 waiting time을 초단위로 반환한다.
	tResultTime = get_nextbatchtime_t(tAdjustTime, __CollectionInterval);

	return (long)(tResultTime - tSystemTime) + 1;	
}


string hdatetime::get_previntervalstarttime(short __CollectionInterval, time_t __tCurrentTime)
{
	__tCurrentTime -= __CollectionInterval * 60;

	return get_currentintervalstarttime(__CollectionInterval, __tCurrentTime);
}

/*
string hdatetime::get_previntervalstarttime(short __CollectionInterval)
{
	time_t tCurrentTime;
	
	time(&tCurrentTime);
	
	tCurrentTime -= __CollectionInterval * 60;
	
std::cout << "get_precintervalstarttime._tCurrentTime : " << tCurrentTime << std::endl;	

	return get_currentintervalstarttime(__CollectionInterval, tCurrentTime);
}
*/

string hdatetime::get_currentintervalstarttime(short __CollectionInterval
	                                         , time_t __tCurrentTime)
{
	struct tm ptm;
	int i = 60 / __CollectionInterval;
	int nTemp;
	string str;
	char  temp_str[100];
	
	memset(temp_str, 0x00, sizeof(temp_str));
	
	if (localtime_r(&__tCurrentTime, &ptm) == NULL)
	{
		str.empty();
		return str;
	}

	for (nTemp = 60; nTemp >= 0;) {
		i--;
		nTemp = i * __CollectionInterval;
		if (nTemp <= ptm.tm_min) {
			sprintf(temp_str, "%02d%02d", ptm.tm_hour, nTemp);
			str = temp_str;
			break;
		}
	}

	return str;
}

string hdatetime::get_lastintervalstarttime(short __CollectionInterval)
{
	time_t tIntervalEnd;

	// Calculate current interval end time 
	tIntervalEnd = get_intervalendtime_t(__CollectionInterval
	                      , get_time(TimeOption::YYYYMMDD).c_str(), "0000");

	return get_previntervalstarttime(__CollectionInterval, tIntervalEnd);
}

time_t hdatetime::get_intervalendtime_t(short __CollectionInterval
	                                  , const char* _pszWorkDate, const char* _pszStartTime)
{
	char szYYYY[4+1], szMM[2+1], szDD[2+1];
	char szHour[2+1], szMin[2+1];
	struct tm ptm;
	time_t tStartTime;

	memset(szYYYY, 0x00, sizeof(szYYYY));
	memset(szMM, 0x00, sizeof(szMM));
	memset(szDD, 0x00, sizeof(szDD));
	memset(szHour, 0x00, sizeof(szHour));
	memset(szMin, 0x00, sizeof(szMin));
	memset(&ptm, 0x00, sizeof(struct tm));

	//length check
	sprintf(szYYYY, "%.4s", _pszWorkDate);
	sprintf(szMM, "%.2s", _pszWorkDate+4);
	sprintf(szDD, "%.2s", _pszWorkDate+6);
	sprintf(szHour, "%.2s", _pszStartTime);
	sprintf(szMin, "%.2s", _pszStartTime+2);

	ptm.tm_year = atoi(szYYYY) - 1900;
	ptm.tm_mon = atoi(szMM) - 1;
	ptm.tm_mday = atoi(szDD);
	ptm.tm_hour = atoi(szHour);
	ptm.tm_min = atoi(szMin);
	ptm.tm_sec = 0;
	
	tStartTime = mktime(&ptm);

	tStartTime = tStartTime + (__CollectionInterval * 60) - 1;

	return tStartTime;
}

time_t hdatetime::get_nextbatchtime_t(time_t __tCurrentTime
	                                , short __CollectionInterval)
{
	struct tm ptm;
	int i = 60 / __CollectionInterval;

	//localtime_s(&ptm, &__tCurrentTime);
	localtime_r(&__tCurrentTime, &ptm);

	for (int nTemp = 60; nTemp >= 0;)
	{
		i--;
		nTemp = i * __CollectionInterval;
		if (nTemp <= ptm.tm_min)
		{
			ptm.tm_min = nTemp;
			ptm.tm_sec = 2;
			break;
		}
	}

	return mktime(&ptm);
}

const hdatetime& hdatetime::operator+(long __Sec)
{
	m_time+=__Sec;
	return *this;
}

const hdatetime& hdatetime::operator+=(long __Sec)
{
	m_time+=__Sec;
	return *this;
}

const hdatetime& hdatetime::operator-=(long __Sec)
{
	m_time-=__Sec;
	return *this;
}

const hdatetime& hdatetime::operator-(long __Sec)
{
	m_time=m_time-__Sec;	
	return *this;
}

const time_t hdatetime::operator-(hdatetime& __Time)
{
	time_t tmpTime;

	tmpTime = this->m_time - __Time.m_time;
	return tmpTime;
}

