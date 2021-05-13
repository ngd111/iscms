//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
//
//////////////////////////////////////////////////////////////////////////////

#ifndef VDN_CALL_LIST_SHM_INCLUDED
#define VDN_CALL_LIST_SHM_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "XPRJACEDLLEx/CSTADefineBaseType.h"
#include "XPRJACEDLLEx/CSTADefineEvent.h"
#include "statbase_shm.hpp"
#include "manage_umap.hpp"
#include "length_def.hpp"

using namespace IS;


//////////////////////////////////////////////////////////////


// For management call list in VDN
typedef struct _vdn_call_list_key {
	long callID;
	
    bool operator==(const struct _vdn_call_list_key &p) const {
    	if (callID == p.callID)
    		return true;
    	else
    		return false;
    }
} VDN_CALL_LIST_SHMKEY_t;

typedef struct _vdn_call_list_data {
	tDeviceID vdn;               // Delivered event의 calledDevice
	
	tDeviceID calling_device;    // ANI
	tDeviceID skill;             // QueueEvent 이후 발생(INPROGRESS 상태에서는 blank)
	
	tDeviceID agentID;           // INRING상태부터 발생(DeliveredEvent to Agent, but device number임, 추후개발)
	tDeviceID agent_device;      // INRING상태부터 발생
	
	time_t    tlast_event_time;  // Last received event time(status changed time)
	time_t    tentry_time;       // VDN 진입시간(Delivered time)

	///////////// for management /////////////////

	_vdn_call_list_data & operator=(_vdn_call_list_data& __data) {
		if (strcmp(__data.vdn, NOUPDATE) != 0) 	strcpy(vdn, __data.vdn);
		if (strcmp(__data.calling_device, NOUPDATE) != 0) strcpy(calling_device, __data.calling_device);
		if (strcmp(__data.skill, NOUPDATE) != 0) strcpy(skill, __data.skill);
		// agentID는 추후개발
		if (strcmp(__data.agent_device, NOUPDATE) != 0) strcpy(agent_device, __data.agent_device);
		if (__data.tentry_time > 0) tentry_time = __data.tentry_time;

		tlast_event_time = __data.tlast_event_time;
				
		return *this;
	}

	_vdn_call_list_data & operator+=(_vdn_call_list_data& __data) {
		
		return *this;
	}
		
} VDN_CALL_LIST_SHM_t;

inline std::size_t hash_value(struct _vdn_call_list_key const &p)
{
    std::size_t seed = 0;
    boost::hash_combine(seed, p.callID);
    return seed;
};



class vdn_call_list_shm
	: public statbase_shm<VDN_CALL_LIST_SHMKEY_t, VDN_CALL_LIST_SHM_t>
{
public:
	~vdn_call_list_shm();
	vdn_call_list_shm(manage_shared_memory *__shmobj, shared_memory_t *__shm);
	
	bool register_data(VDN_CALL_LIST_SHMKEY_t __key, VDN_CALL_LIST_SHM_t __data);
};

#endif // VDN_CALL_LIST_SHM_INCLUDED
