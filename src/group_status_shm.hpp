//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
//
//////////////////////////////////////////////////////////////////////////////

#ifndef GROUP_STATUS_SHM_INCLUDED
#define GROUP_STATUS_SHM_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "XPRJACEDLLEx/CSTADefineBaseType.h"
#include "statbase_shm.hpp"
#include "length_def.hpp"
#include "hutil.hpp"

using namespace IS;


typedef struct _group_status_key {

    tDeviceID acdGroup;

    bool operator==(const struct _group_status_key &p) const {
        return (strcmp(acdGroup, p.acdGroup) == 0);
    }
} GROUP_STATUS_LIST_SHMKEY_t;

typedef struct _group_status_data_inaux_count {
	long inaux;
} GROUP_STATUS_DATA_INAUX_COUNT_t;

// 상담원이 보유한 모든 스킬에 동일하게 Status관리가 되어야 한다.
typedef struct _group_status_data_count {
	long aginring;
	long available;
	long inacw;
	long inaux;   // Total of AUX state agents
	GROUP_STATUS_DATA_INAUX_COUNT_t inaux_[INAUX_MAX+1];
	
	long onacd;
	long onacdauxout;  // 전화기에서 hold상태에서 발신하는 기능이 지원되야 가능하다.일단 보류 
	long onacwin;
	long onacwout;
	long onauxin;
	long onauxout;
	long other;
	
	long staffed; // Total count of logged-on agents
} GROUP_STATUS_DATA_COUNT_t;

typedef struct _group_status_data {
	long inqueue;
	long inring;
	long onhold;
	time_t 	oldestcall;
	
	GROUP_STATUS_DATA_COUNT_t agent_count_by_status;

    ///////////// for management /////////////////
    time_t    tevent_time;	
	
    _group_status_data & operator=(_group_status_data& __data) {
        inqueue += __data.inqueue;
        inring += __data.inring;
		if (__data.inring > 0) inqueue--;    //VDN event 수신시 +처리할 항목 결정해서 처리
		onhold += __data.onhold;
        
        agent_count_by_status.aginring += __data.agent_count_by_status.aginring;
        agent_count_by_status.available += __data.agent_count_by_status.available;
        agent_count_by_status.inacw += __data.agent_count_by_status.inacw;
        agent_count_by_status.inaux += __data.agent_count_by_status.inaux;        
        for (int i = 0; i < INAUX_MAX; i++) {
        	agent_count_by_status.inaux_[i].inaux += __data.agent_count_by_status.inaux_[i].inaux;
        }
        agent_count_by_status.onacd += __data.agent_count_by_status.onacd;
        agent_count_by_status.onacdauxout += __data.agent_count_by_status.onacdauxout;
        agent_count_by_status.onacwin += __data.agent_count_by_status.onacwin;
        agent_count_by_status.onacwout += __data.agent_count_by_status.onacwout;
        agent_count_by_status.onauxin += __data.agent_count_by_status.onauxin;
        agent_count_by_status.onauxout += __data.agent_count_by_status.onauxout;
        agent_count_by_status.other += __data.agent_count_by_status.other;

        tevent_time = __data.tevent_time;

        return *this;
    }

    _group_status_data & operator+=(_group_status_data& __data) {
        return *this;
    }

} GROUP_STATUS_LIST_SHM_t;

inline std::size_t hash_value(struct _group_status_key const &p)
{
    std::size_t seed = 0;
    boost::hash_combine(seed, p.acdGroup);
    return seed;
};


class group_status_shm
     : public statbase_shm<GROUP_STATUS_LIST_SHMKEY_t, GROUP_STATUS_LIST_SHM_t>
{
public:

    ~group_status_shm();
    group_status_shm(manage_shared_memory *__shmobj, shared_memory_t *__shm);

private:
    hcommon *pCommObj;
};

#endif // GROUP_STATUS_SHM_INCLUDED
