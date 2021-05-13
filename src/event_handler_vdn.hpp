//////////////////////////////////////////////////////////////////////////////
////
//// (C) Copyright Hansol inticube 2014
////
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_HANDLER_VDN_INCLUDED
#define EVENT_HANDLER_VDN_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif


#include "event_handler_base.hpp"
#include "vdn_call_list_shm.hpp"
#include "vdn_status_shm.hpp"

//using namespace IS;

class event_handler_vdn : public event_handler_base
{
	public:
		event_handler_vdn(manage_shared_memory *__shmobj, shared_memory_t *__shm, hcommon *__commobj);
		~event_handler_vdn();

		bool register_data(tUnsolicitedEventType __event, void* __event_data
				, tPrivateData __private_data, time_t __event_time
				, MONITOR_CROSS_REFID_t __monitored_dev_info);
				
	public:
		vdn_status_shm *pstatus;        // For VDN real-time status monitoring
		vdn_call_list_shm *pcall_list;	// For VDN real-time call list

};

#endif // EVENT_HANDLER_VDN_INCLUDED
