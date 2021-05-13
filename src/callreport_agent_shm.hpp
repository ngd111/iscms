//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
//
//////////////////////////////////////////////////////////////////////////////

#ifndef RPT_CALLREPORT_AGENT_SHM_INCLUDED
#define RPT_CALLREPORT_AGENT_SHM_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "statbase_shm.hpp"


typedef struct _key {
	char row_date   [SZ_DATE  +1];
	char skill     [SZ_DIALPLN+1];
	short pbx_id;
	char domain     [SZ_DOMAIN+1];  // Tenant
	char logid     [SZ_DIALPLN+1];	
   
    bool operator==(const struct _key &p) const {
        std::cout << "in operator= _key" << std::endl;
        return (pbx_id == p.pbx_id &&
                strcmp(row_date, p.row_date) == 0 &&
                strcmp(skill, p.skill) == 0 &&
                strcmp(domain, p.domain) == 0 &&
                strcmp(logid, p.logid) == 0
                );
    }
} CR_HIST_AGENT_SHMKEY_SHM;

typedef struct _data {
	char row_date   [SZ_DATE  +1];
	char skill     [SZ_DIALPLN+1];
	short pbx_id;
	char domain     [SZ_DOMAIN+1];  // Tenant
	char logid     [SZ_DIALPLN+1];
	
    /////////////////////////// Data columns ///////////////////////
   
	int acdCalls;
	int acdTime;
	
	_data & operator+=(_data& __data) {
		acdCalls += __data.acdCalls;
		acdTime  += __data.acdTime;
		
		std::cout << "in operator +" << std::endl;
		
		return *this;
	}
	
} CR_HIST_AGENT_SHM;

inline std::size_t hash_value(struct _key const &p)
{
    std::size_t seed = 0;
    boost::hash_combine(seed, p.logid);
    return seed;
};

class callreport_agent_shm
	 : public statbase_shm<CR_HIST_AGENT_SHMKEY_SHM, CR_HIST_AGENT_SHM>
{
public:
	
//	callreport_agent_shm(void);
	~callreport_agent_shm();
	callreport_agent_shm(manage_shared_memory *__shmobj, shared_memory_t *__shm);


	// Report별로 별도로 필요한 함수는 여기에 작성
	
//  void copyElements(CProcessAgentCallStats *_pDest, CRITICAL_SECTION csVar);
//	bool updateReort_i(char * __workdate, char * __starttime, AGENTSTATUS _data
//		, pthread_mutex_t mut);

private:
//	umap_t *s;        // unordered map pointer
//	umap_itor_t itor; // unordered map iterator
	
};

#endif // RPT_CALLREPORT_AGENT_SHM_INCLUDED
