/**
            common.hpp header file --------------------------------------------

Description
CMS에서 공통으로 사용될 class인 common class로 구성된 헤더파일
common class는 모든 NR 모듈에서 상속되어 사용되므로 범용으로 사용될 
기능이나 변수의 추가가 필요할 경우 본 common class에 추가하면 된다.

개발일자 : 2014-08-31
작성자   : 정진각
개정이력
---------------------------------------------------------------------
[개정일자] [개정사항 기술]

**/

#ifndef _COMMON_HPP
#define _COMMON_HPP

#include <iostream>
#include <assert.h>
#include "errordef.h"

namespace WriteOption
{
	enum Type
	{
		LOG_HEADER,    // 로그 헤더를 Write한다
		LOG_DESC,      // Description을 Write한다
		LOG_CREATE,    // 파일을 생성한다(Overwrite)
		LOG_DATA       // 데이터를 Write한다(Append)
	};
}

#define NEW(PTR, TYPE) \
try{ (PTR) = new TYPE; } \
catch (std::bad_alloc) { assert(0); }


using namespace std;

class hobject
{
	public :
		string m_LogFile;     // Log filename(full path + filename)
		string m_sModuleName; // Logging용 Module명 변수
		string m_sMessage;    // Logging용 Message 변수
};

#endif // common.hpp
