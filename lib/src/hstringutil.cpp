#include "hstringutil.hpp"



hstringutil::hstringutil()
{
	sStringBuffer = NULL;
	nColCount = 0;	
}

hstringutil::~hstringutil()
{
	ReleaseBuffer();
}

string hstringutil::LTrim(const string& __buffer)
{
	int n;
	
	n = __buffer.find_first_not_of(" \t\v\n");
	return (n == string::npos ? __buffer : 
		                        __buffer.substr(n, __buffer.length()));
}

string hstringutil::RTrim(const string& __buffer)
{
	int n;
	
	n = __buffer.find_last_not_of(" \t\v\n");
	return (n == string::npos ? __buffer : 
		                        __buffer.substr(0, n+1));
}

char* hstringutil::RTrim(char *__val)
{
    int  i;

    for(i = strlen(__val) - 1; i >= 0; i--) {
        if((__val[i] != ' ')&&(__val[i] != 13)&&(__val[i] != 10)) 
        	break;
        else __val[i] = 0x00;
    }
    
    return __val;
}

char* hstringutil::LTrim(char *__val)
{
    int  i;

    for(i = 0; i <= strlen(__val) - 1; i++) {
        if((__val[i] != ' ')&&(__val[i] != 13)&&(__val[i] != 10))
        {
        	memcpy(__val, __val+i, sizeof(char) * strlen(__val) - i);
        	__val[strlen(__val) - i] = 0x00;
        	
        	break;
        }
    }
    
    return __val;
}

void hstringutil::ToUpperStr(string& __str)
{
    transform(__str.begin(), __str.end(), __str.begin(),
              (int(*)(int)) toupper);
}

void hstringutil::ToLowerStr(string& __str)
{
    transform(__str.begin(), __str.end(), __str.begin(),
              (int(*)(int)) tolower);
}

//문자열을 delimiter를 기준으로 이차원배열로 return
char** hstringutil::DivStringToArr(string __sSource, char __cDelimiter, long* __nRetCount)
{
	int nLen; //컬럼 데이터 길이
	int i, nPos, nPosNext;
	
	
	ReleaseBuffer();

    // Data가 아예 없는 것이 아니면 1건이 있다고 봐야함
	if (__sSource.length() == 0)
		return NULL;
	
	nColCount = 0;
	
	for (i=0; i < __sSource.length();)
	{
		nPos = __sSource.find(__cDelimiter, i);
		if (nPos != string::npos)
		{
			//cout << "i:" << i << " npos:" << nPos << endl;
			nColCount++;
			i = nPos + 1;        	
		}
		else
		{
			nColCount++;
			break;
		}
	}
   
	//cout << "count : [" << nColCount << "]" << endl;
	
    // Col수 만큼 할당
    sStringBuffer = new char*[nColCount];
	nPos = 0;
	//cout << "length:" << __sSource.length() << endl;
	nColCount = 0;
    for (i=0; i<__sSource.length();)
    {
		//cout << "i:[" << i << "]";
        nPos = __sSource.find(__cDelimiter, i);
        if (nPos != string::npos)
        {
        	if (i == 0)
        	{
        		//cout << "[" << __sSource.substr(i, nPos-i) << "]" << endl;
        		nLen = nPos-i+1;
        		sStringBuffer[nColCount] = new char[nLen * sizeof(char)];
        		memset(sStringBuffer[nColCount], 0x00, nLen);
        		memcpy(sStringBuffer[nColCount], (const char *)__sSource.substr(i, nPos).c_str(), nPos-i);
        	}
        	else
        	{
        		nPosNext = __sSource.find(__cDelimiter, nPos);
        		if (nPosNext != string::npos)
        		{
        			//cout << ",nPosNext:[" << nPosNext << "]" << "i:" << i << " ";
        			//cout << "[" << __sSource.substr(i, nPosNext-i) << "]" << endl;
	        		nLen = nPosNext-i+1;
	        		sStringBuffer[nColCount] = new char[nLen * sizeof(char)];
	        		memset(sStringBuffer[nColCount], 0x00, nLen);
        			memcpy(sStringBuffer[nColCount], (const char *)__sSource.substr(i, nPosNext-i).c_str(), nPosNext-i);
        		}
        		/*
        		else
        		{
        			cout << "[" << __sSource.substr(i) << endl;
        			memcpy(sStringBuffer[nColCount], (const char *)__sSource.substr(i).c_str(), __sSource.length()-i);
        		}
        		*/
	        }

            nColCount++;
            i = nPos + 1;
        }
        else
        {
        	//cout << "[" << __sSource.substr(i) << "]" << endl;
    		nLen = __sSource.length()-i+1;
    		sStringBuffer[nColCount] = new char[nLen * sizeof(char)];
    		memset(sStringBuffer[nColCount], 0x00, nLen);
        	memcpy(sStringBuffer[nColCount], (const char *)__sSource.substr(i).c_str(), __sSource.length()-i);
        	nColCount++;
        	break;
        }
    }

    memcpy(__nRetCount, &nColCount, sizeof(long));    

    return (char **)sStringBuffer;
}

void hstringutil::ReplaceString(char* __src, const char *__targetstr, 
                   const char *__changestr)
{
    string::size_type nPos = 0;
    string sTmp = __src;
    long ntgtlen = strlen(__targetstr);
	long nchglen = strlen(__changestr);

    for (;;)
    {
        nPos = sTmp.find(__targetstr, nPos);
        if (nPos == string::npos)
            break;

        sTmp.replace(nPos, ntgtlen, __changestr, nchglen);
        nPos=nPos+nchglen;
    }

    memset(__src, 0x00, sTmp.length());  		
    memcpy(__src, sTmp.c_str(), sTmp.length());
   
    
    return;
}

void hstringutil::ReleaseBuffer()
{
	if (sStringBuffer != NULL)
	{
		for (int i=0; i < nColCount; i++)
		{
			if (sStringBuffer[i] != NULL) 
			{
				delete[] sStringBuffer[i];  // Row별로 메모리 해제		
			}
	    }
		delete []sStringBuffer;
		sStringBuffer = NULL;
	}
}

