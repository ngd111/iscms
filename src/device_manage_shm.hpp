//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
//
//////////////////////////////////////////////////////////////////////////////

#ifndef DEVICE_MANAGEMENT_MON_SHM_INCLUDED
#define DEVICE_MANAGEMENT_MON_SHM_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "XPRJACEDLLEx/CSTADefineBaseType.h"
#include "statbase_shm.hpp"
#include "manage_umap.hpp"
#include "device_manage_monref_shm.hpp"

using namespace IS;


// For monitoring device list management
typedef struct _dev_status_key {
	tDeviceID extension;
	tDeviceID domain;            // Domain name	
	
    bool operator==(const struct _dev_status_key &p) const {
        return ( (strcmp(extension, p.extension) == 0) && 
                 (strcmp(domain, p.domain) == 0) );
    }
} MONITORING_LIST_SHMKEY_t;

typedef struct _dev_status_data {
//	short device_type;   // 1: STATION 2: VDN
	DEVICE_TYPE device_type;
	tDeviceID extension;
	tServiceCrossRefID serviceCrossRefID; // 모니터링 요청 Unique 번호(MonitorDevice 함수의 결과를 받아서 저장)
	tMonitorCrossRefID monitorCrossRefID; // 모니터링 성공시 관리 Unique 번호(추후 MonitorStop시 사용)
	char monitor_yn [1+1];       // Y; 모니터링 성공, N; 모니터링 실패, ' '; 결과 수신전
	char message [1024+1];       // Error 시의 message
	
	_dev_status_data & operator+=(_dev_status_data& __data) {
		
		return *this;
	}
} MONITORING_LIST_SHM_t;

inline std::size_t hash_value(struct _dev_status_key const &p)
{
    std::size_t seed = 0;
    boost::hash_combine(seed, p.extension);
    return seed;
};


///////////////////////////////////////////////////////////////////////////////
// Temporary data structure (To use searching extension by serviceCrossRefID)
///////////////////////////////////////////////////////////////////////////////

typedef struct _key_svc_refid {
	tServiceCrossRefID serviceCrossRefID;
	
	bool operator==(const struct _key_svc_refid &p) const {
		return serviceCrossRefID == p.serviceCrossRefID;
	}
} SERVICE_CROSS_REFID_KEY_t;

typedef struct _data_svc_refid {
//	short device_type;   // 1: 내선, 2: VDN
	DEVICE_TYPE device_type;
	tDeviceID extension;
	tDeviceID domain;
} SERVICE_CROSS_REFID_t;


inline std::size_t hash_value(struct _key_svc_refid const &p)
{
    std::size_t seed = 0;
    boost::hash_combine(seed, p.serviceCrossRefID);
    return seed;
};



class device_manage_shm
	 : public statbase_shm<MONITORING_LIST_SHMKEY_t, MONITORING_LIST_SHM_t>
{
public:

	~device_manage_shm();
	device_manage_shm(manage_shared_memory *__shmobj, shared_memory_t *__shm);
	
	bool register_data(MONITORING_LIST_SHMKEY_t __key, MONITORING_LIST_SHM_t __data);
	
	
	bool update_result(tServiceCrossRefID __serviceCrossRefID, tMonitorCrossRefID __monitorCrossRefID
						, char *__result_yn, const char *__err_msg);
	void update_result(tMonitorCrossRefID __monitorCrossRefID);
	MONITORING_LIST_SHM_t* find_data(tMonitorCrossRefID __monitorCrossRefID);
	bool delete_data(tMonitorCrossRefID __monitorCrossRefID);
	
	void truncate();
	
	
public:
	manage_umap<SERVICE_CROSS_REFID_KEY_t, SERVICE_CROSS_REFID_t> *pTempCrossRef;
	
private:
	device_manage_monref_shm *pDeviceList_byRefID;
};

#endif // DEVICE_MANAGEMENT_MON_SHM_INCLUDED
