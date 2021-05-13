//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
//
//////////////////////////////////////////////////////////////////////////////

#ifndef VDN_STATUS_SHM_INCLUDED
#define VDN_STATUS_SHM_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "XPRJACEDLLEx/CSTADefineBaseType.h"
#include "statbase_shm.hpp"
#include "manage_umap.hpp"
#include "length_def.hpp"
#include "hutil.hpp"

using namespace IS;


//namespace VDN_STATUS
//{
//	enum COLUMN_NAME
//	{
//		INPROGRESS = 1,
//		INQUEUE,
//		INRING,
//		ATAGENT
//	};
//};

// For VDN realtime status list management
typedef struct _vdn_status_key {
	
	tDeviceID vdn;
	
    bool operator==(const struct _vdn_status_key &p) const {
        return (strcmp(vdn, p.vdn) == 0);
    }
} VDN_STATUS_LIST_SHMKEY_t;

typedef struct _vdn_status_data {
//	int activecalls;
	int atagent;
	int inprogress;
	int inqueue;
	int inring;
	int oldestcall;   // 다른 항목구현 후 생각해보자(vdn_call_list에서 
	                  // 가장 오래 대기한 콜로 update하는 것 고려)

	///////////// for management /////////////////
	time_t    tevent_time;       // 이벤트 발생 시각

	_vdn_status_data & operator=(_vdn_status_data& __data) {
		// Logic ~~~~
		
		
		tevent_time = __data.tevent_time;
			
		return *this;
	}

	_vdn_status_data & operator+=(_vdn_status_data& __data) {
//		activecalls += __data.activecalls;
		atagent += __data.atagent;
		inprogress += __data.inprogress;
		inqueue += __data.inqueue;
		inring += __data.inring;
			
		return *this;
	}
		
} VDN_STATUS_LIST_SHM_t;

inline std::size_t hash_value(struct _vdn_status_key const &p)
{
    std::size_t seed = 0;
    boost::hash_combine(seed, p.vdn);
    return seed;
};


class vdn_status_shm
	 : public statbase_shm<VDN_STATUS_LIST_SHMKEY_t, VDN_STATUS_LIST_SHM_t>
{
public:

	~vdn_status_shm();
	vdn_status_shm(manage_shared_memory *__shmobj, shared_memory_t *__shm, hcommon *__commobj);
	
//	void set_call_progress_status(VDN_STATUS::COLUMN_NAME __column
//		                 		, VDN_STATUS_LIST_SHM_t &__data_out
//		                 		, tUnsolicitedEventType __event, time_t __event_time);

private:
	hcommon *pCommObj;
};

#endif // VDN_STATUS_SHM_INCLUDED