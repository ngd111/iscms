//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
//
//////////////////////////////////////////////////////////////////////////////

#ifndef RPT_CALLREPORT_AGENT_INCLUDED
#define RPT_CALLREPORT_AGENT_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "statbase.hpp"


// Key value structure
struct CR_HIST_AGENTKEY
{
	struct KEY
	{
		char row_date   [SZ_DATE  +1];
		char starttime  [SZ_STTIME+1];
		char skill     [SZ_DIALPLN+1];
		short pbx_id;
		char domain     [SZ_DOMAIN+1];  // Tenant
		char logid     [SZ_DIALPLN+1];
	};

	KEY Key;

	CR_HIST_AGENTKEY()
	{
		memset(&Key, 0x00, sizeof(Key));
	}
	
	CR_HIST_AGENTKEY(char* __row_date, char* __starttime, char* __skill
	               , short __pbx_id, char* __domain, char* __logid)
	{
		memcpy(Key.row_date, __row_date, SZ_DATE);
		memcpy(Key.starttime, __starttime, SZ_STTIME);
		memcpy(Key.skill, __skill, SZ_DIALPLN);
		Key.pbx_id = __pbx_id;
		memcpy(Key.domain, __domain, SZ_DOMAIN);
		memcpy(Key.logid, __logid, SZ_DIALPLN);
	}
};

// 
struct CR_HIST_AGENT
{
	/////////////////////////// Key columns ////////////////////////
	
	char row_date   [SZ_DATE  +1];
	char starttime  [SZ_STTIME+1];
	char skill     [SZ_DIALPLN+1];
	short pbx_id;
	char domain     [SZ_DOMAIN+1];  // Tenant
	char logid     [SZ_DIALPLN+1];

    /////////////////////////// Data columns ///////////////////////
    
//	size_t staffTime;    // 추후 적재 방식 연구(archiving시에 할지 다른 시점에 할지...)
	size_t acdCalls;
	size_t acdTime;
	
	time_t updateTime;    // 최종 갱신 시각
	
	CR_HIST_AGENT& operator+=(CR_HIST_AGENT& data) {
		acdCalls += data.acdCalls;
		acdTime  += data.acdTime;
		
		if (data.updateTime > 0)
			updateTime = data.updateTime;
			
		return *this;
	}
	
	void get_buffer(char * __pOut) {
		sprintf(__pOut, "%s|%s|"
		              , row_date, starttime);
	}
};


class callreport_agent
	 : public statbase<CR_HIST_AGENTKEY, CR_HIST_AGENT>
{
public:
	callreport_agent(void);
	~callreport_agent(void);
	
	// Report별로 별도로 필요한 함수는 여기에 작성
	
//  void copyElements(CProcessAgentCallStats *_pDest, CRITICAL_SECTION csVar);

//	bool updateReort_i(char * __workdate, char * __starttime, AGENTSTATUS _data
//		, pthread_mutex_t mut);
	
};

#endif // RPT_CALLREPORT_AGENT_INCLUDED
