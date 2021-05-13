#ifndef _HSTRINGUTIL_HPP
#define _HSTRINGUTIL_HPP

#include <string>
#include <string.h>
#include <iostream>
#include <algorithm> //transform

using namespace std;

class hstringutil
{
	public:
		hstringutil();
		~hstringutil();
		
		// left space trim : handling string class type variable
		string LTrim(const string& __buffer);
		// right space trim : handling string class type variable
		string RTrim(const string& __buffer);
		// left space trim : handling char * type variable
		char* LTrim(char *__val);
		// right space trim : handling char * type variable
		char* RTrim(char *__val);
		// To Make Uppercase
		void ToUpperStr(string& __str);
		// To Make Lowercase
		void ToLowerStr(string& __str);	
		
		// Source문자열 중에서 변경할 문자열을 찾아 특정문자열로 변경한다.
		// Parameter 1. SourceString : Source String(변경된 문자열을 반환)
		//           2. TargetString : 변경대상 문자열
		//           3. ChangeString : TargetString을 대치할 문자열
		void ReplaceString(char* __src, const char *__targetstr, 
		                   const char *__changestr);
		                   
		// Delimiter를 기준으로 Source문자열을 분리해 2차원배열로 반환한다
		// 반환값은 2차원배열의 포인터를 반환하며 처리중 오류시 NULL포인터를 
		// 반환한다
		char** DivStringToArr(string __sSource, char __cDelimiter
		                      , long* __nColCount);
		                     
	private: 
		// DivStringToArr 메소드에서 사용되는 sStringBuffer변수의 
		// 할당된 메모리를 해제한다
		void ReleaseBuffer();

		char **sStringBuffer;
		long nColCount;
};

#endif
