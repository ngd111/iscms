/**
            common.hpp header file --------------------------------------------

Description
CMS���� �������� ���� class�� common class�� ������ �������
common class�� ��� NR ��⿡�� ��ӵǾ� ���ǹǷ� �������� ���� 
����̳� ������ �߰��� �ʿ��� ��� �� common class�� �߰��ϸ� �ȴ�.

�������� : 2014-08-31
�ۼ���   : ������
�����̷�
---------------------------------------------------------------------
[��������] [�������� ���]

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
		LOG_HEADER,    // �α� ����� Write�Ѵ�
		LOG_DESC,      // Description�� Write�Ѵ�
		LOG_CREATE,    // ������ �����Ѵ�(Overwrite)
		LOG_DATA       // �����͸� Write�Ѵ�(Append)
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
		string m_sModuleName; // Logging�� Module�� ����
		string m_sMessage;    // Logging�� Message ����
};

#endif // common.hpp
