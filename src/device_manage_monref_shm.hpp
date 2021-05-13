//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
//
//////////////////////////////////////////////////////////////////////////////

#ifndef DEVICE_MANAGEMENT_MON_REF_SHM_INCLUDED
#define DEVICE_MANAGEMENT_MON_REF_SHM_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "XPRJACEDLLEx/CSTADefineBaseType.h"
#include "statbase_shm.hpp"
#include "manage_umap.hpp"

using namespace IS;

///////////////////////////////////////////////////////////////////////////////
// To use in event processing(Check device type)
///////////////////////////////////////////////////////////////////////////////
typedef struct _key_monitor_refid {
	tMonitorCrossRefID monitorCrossRefID;
	
	bool operator==(const struct _key_monitor_refid &p) const {
		return monitorCrossRefID == p.monitorCrossRefID;
	}
} MONITOR_CROSS_REFID_KEY_t;

typedef struct _data_monitor_refid {
//	short device_type;   // 1: 내선, 2: VDN
	DEVICE_TYPE device_type;
	tDeviceID extension;
	tDeviceID domain;
	
	_data_monitor_refid & operator+=(_data_monitor_refid& __data) {
		//acdCalls += __data.acdCalls;
		//acdTime  += __data.acdTime;
		
		return *this;
	}	
} MONITOR_CROSS_REFID_t;


inline std::size_t hash_value(struct _key_monitor_refid const &p)
{
    std::size_t seed = 0;
    boost::hash_combine(seed, p.monitorCrossRefID);
    return seed;
};


class device_manage_monref_shm
	 : public statbase_shm<MONITOR_CROSS_REFID_KEY_t, MONITOR_CROSS_REFID_t>
{
public:
	
	~device_manage_monref_shm();
	device_manage_monref_shm(manage_shared_memory *__shmobj, shared_memory_t *__shm);
	
	bool register_data(MONITOR_CROSS_REFID_KEY_t __key, MONITOR_CROSS_REFID_t __data);
};

#endif // DEVICE_MANAGEMENT_MON_REF_SHM_INCLUDED
