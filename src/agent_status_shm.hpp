//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AGENT_STATUS_SHM_INCLUDED
#define AGENT_STATUS_SHM_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "XPRJACEDLLEx/CSTADefineBaseType.h"
#include "XPRJACEDLLEx/CSTADefineEvent.h"
#include "statbase_shm.hpp"
#include "manage_umap.hpp"
#include "length_def.hpp"
#include "group_status_shm.hpp"
#include "hutil.hpp"

using namespace IS;

// For workmode - mode changed on logical event 
#define STATUS_AUX     "AUX"             // AgentLoggedOn, AgentNotReady
#define STATUS_ACW     "ACW"             // AgentWorkingAfterCall
#define STATUS_READY   "AVAIL"           // AgentReady
//#define STATUS_BLANK   ""

// For sub_workmode - mode changed on physical event
#define STATUS_ACD     "ACD"             // Established
#define STATUS_DACD    "DACD"            // Established
#define STATUS_DACW    "DACW"            // AgentWorkingAfterCall 
#define STATUS_RING    "RINGING"         // Delivered
#define STATUS_OTHER   "OTHER"           // Originated

// For agstate - mode changed on mixed situation(logical & physical event)
#define AGSTATE_AUX        STATUS_AUX
#define AGSTATE_ACW        STATUS_ACW
#define AGSTATE_READY      STATUS_READY
#define AGSTATE_UNSTAFFED  "Unstaffed"   // AgentLoggedOff
#define AGSTATE_ACD        STATUS_ACD    // Established
#define AGSTATE_DACD       STATUS_DACD   // Established
#define AGSTATE_ACWIN      "ACWIN"       // Established
#define AGSTATE_AUXIN      "AUXIN"       // Established
#define AGSTATE_DACWIN     "DACWIN"      // Established
#define AGSTATE_DACWOUT    "DACWOUT"     // Established
#define AGSTATE_ACWOUT     "ACWOUT"      // Established
#define AGSTATE_AUXOUT     "AUXOUT"      // Established


//////////////////////////////////////////////////////////////

namespace AGENT_RT_ITEMS
{
	enum Item
	{
		WORKMODE,
		AUXREASON,
		DIRECTION,
		AGSTATE,
		WORKSKILL
	};
}

//////////////////////////////////////////////////////////////

typedef struct _agent_group_information {
	tDeviceID acdGroup;
	short     level;
} AGENT_GROUP_ASSIGNED_LIST_t;


typedef struct _agent_affect_to_group_info {
	AGENT_GROUP_ASSIGNED_LIST_t group_info;
	GROUP_STATUS_DATA_COUNT_t   data;
} AGENT_AFFECT_TO_GROUP_t;


// For agent realtime status list management
typedef struct _agent_status_key {
	
	tAgentID agentID;
	
    bool operator==(const struct _agent_status_key &p) const {
        return (strcmp(agentID, p.agentID) == 0);
    }
} AGENT_STATUS_LIST_SHMKEY_t;

typedef struct _agent_status_data {
	tDeviceID extension;
	tDeviceID domain;			// 추후 tenant 신설 후 Key로 올리자

	char      workmode[WORKMODE_LEN+1];
	time_t    agtime;         // 현 Skill에 대한 workmode 변경 이후 경과된 시간길이를 측정하기 위한 변수
							  // (초기화를 제외한 시간값은 조회시점에 시스템 time으로 계산해서 제공)
	                          // direction의 변화시에는 초기화되지 않는다.
	time_t    agduration;     // 상담원이 workmode 혹은 direction이 변경되는데 소비된 시간측정위한 변수
	                          // direction 및 workmode의 변화시 초기화 됨
	                          // (초기화를 제외한 시간값은 조회시점에 시스템 time으로 계산해서 제공)
	char      auxreason[REASONCODE_LENGTH+1];
	char      direction[DIRECTION_LEN+1];
	char      agstate[AGSTATE_LEN+1];
	tDeviceID workskill;      // 상담원이 작업중인 skill
	
	///////////// Assigned skill group information /////////////////////
	AGENT_GROUP_ASSIGNED_LIST_t group_info[AGENT_GROUP_MAX+1];
	
	///////////// For management ///////////////////////////////////////	
	tUnsolicitedEventType last_event;
	
	time_t    tevent_time;    // 이벤트 발생 시각
	char      sub_workmode[WORKMODE_LEN+1];   // Sub workmode관리(상태가 해제되면 Clear)
	
	// Temporary variables not updated
	bool      bInitialize;    // 초기화인지 여부(Is data originated from result of run GetAgentState service or not?)
//	bool      bIsHeld;        // Held상태인지 여부 -> 추후 onacdauxout구현시 사용

	///////////// For ACD-group status /////////////////////////////////
	// 이전 상태변화시점에 영향을 미친 group 상태 for rollback
	AGENT_AFFECT_TO_GROUP_t     affect_directly;
	AGENT_AFFECT_TO_GROUP_t     affect_subsidiary;
//	GROUP_STATUS_DATA_COUNT_t   affected_group_status;


	_agent_status_data & operator=(_agent_status_data& __data) {
		// 초기화(agtime, agdration)
	
		if (strlen(__data.domain) > 0) strcpy(domain, __data.domain);
			
		strcpy(direction, __data.direction);
		strcpy(workskill, __data.workskill);
		strcpy(sub_workmode, __data.sub_workmode);
		if (strlen(__data.extension) > 0) strcpy(extension, __data.extension);

		if (strlen(__data.workmode) > 0) {
			if (strcmp(workmode, __data.workmode) != 0) {
				agtime = __data.tevent_time;
				agduration = __data.tevent_time;
			}
			else {
				if (strcmp(direction, __data.direction) != 0)
					agduration = __data.tevent_time;
			}
			
			strcpy(workmode, __data.workmode);
			
			if (strcmp(__data.workmode, STATUS_AUX) == 0)
				strcpy(auxreason, __data.auxreason);
			else
				strcpy(auxreason, "");
		}
		// workmode는 unstaffed가 아니면 초기화될 일이 없다
//		else {
//			strcpy(workmode, __data.workmode);
//		}
		
		if (strlen(__data.agstate) > 0) {
			strcpy(agstate, __data.agstate);
			if (strcmp(__data.agstate, AGSTATE_UNSTAFFED) == 0) {
				// Init Logged ACD group
				memset(&group_info, 0x00, sizeof(AGENT_GROUP_ASSIGNED_LIST_t) * AGENT_GROUP_MAX);
				strcpy(auxreason, "");
				strcpy(extension, "");
				strcpy(workmode, "");
			}
		}
			
		if ( (__data.group_info[0].acdGroup != NULL) &&
			 (strlen(__data.group_info[0].acdGroup) > 0) ) {
			if (__data.bInitialize == true)
				memcpy(&group_info, &__data.group_info, sizeof(AGENT_GROUP_ASSIGNED_LIST_t) * AGENT_GROUP_MAX);
			else {
				for (int i = 0; i < AGENT_GROUP_MAX; i++) {
					if ( (group_info[i].acdGroup == NULL) || 
						 (strlen(group_info[i].acdGroup) == 0) ) {
						for (int j = 0; j < AGENT_GROUP_MAX; j++) {
							if ( (__data.group_info[j].acdGroup == NULL) ||
								 (strlen(__data.group_info[j].acdGroup) == 0) )
								break;
								
							memcpy(&group_info[i], &__data.group_info[j], sizeof(AGENT_GROUP_ASSIGNED_LIST_t));
							i++;
						}
						
						break;
					}
				}
			}
		}
		
		memcpy(&affect_directly, &__data.affect_directly, sizeof(AGENT_AFFECT_TO_GROUP_t));
		memcpy(&affect_subsidiary, &__data.affect_subsidiary, sizeof(AGENT_AFFECT_TO_GROUP_t));		

		last_event  = __data.last_event;
		tevent_time = __data.tevent_time;
			
		return *this;
	}

	_agent_status_data & operator+=(_agent_status_data& __data) {
		
		return *this;
	}
		
} AGENT_STATUS_LIST_SHM_t;

inline std::size_t hash_value(struct _agent_status_key const &p)
{
    std::size_t seed = 0;
    boost::hash_combine(seed, p.agentID);
    return seed;
};



class agent_status_shm
	 : public statbase_shm<AGENT_STATUS_LIST_SHMKEY_t, AGENT_STATUS_LIST_SHM_t>
{
public:

	~agent_status_shm();
	agent_status_shm(manage_shared_memory *__shmobj, shared_memory_t *__shm);
	
	bool register_data(AGENT_STATUS_LIST_SHMKEY_t __key, AGENT_STATUS_LIST_SHM_t __data);
	bool update_column(AGENT_STATUS_LIST_SHMKEY_t __key, AGENT_RT_ITEMS::Item __item, void* __value);
	AGENT_STATUS_LIST_SHM_t* check_agentID(tDeviceID __device, tAgentID __out_agentID, bool __out_bagentID);
	// Find agentID using device #
	// Return : Found => true, Not found => false
	bool get_agentID(tDeviceID __device, tAgentID __out_agentid);
	
private:
	group_status_shm *pgroup_status_rpt;
	hcommon *pCommObj;
};

#endif // AGENT_STATUS_SHM_INCLUDED
