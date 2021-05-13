//////////////////////////////////////////////////////////////////////////////
////
//// (C) Copyright Hansol inticube 2014
////
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_HANDLER_BASE_INCLUDED
#define EVENT_HANDLER_BASE_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "statbase_shm.hpp"
#include "device_manage_shm.hpp"
#include "agent_status_shm.hpp"
#include "length_def.hpp"
#include "tracking_call.hpp"
#include "hutil.hpp"
#include <queue>


class event_handler_base
{
	public:
		event_handler_base(manage_shared_memory *__shmobj, shared_memory_t *__shm, hcommon *__commobj);
		~event_handler_base();

		void set_shared_report_objects(tracking_call *__pcall_tracking);
		void set_log_queue_objects(queue<LOGGINGDATA_t> *__logqueue
							, pthread_mutex_t* __mut, pthread_cond_t* __cond);

		virtual bool register_data(tUnsolicitedEventType __event, void* __event_data
				, tPrivateData __private_data, time_t __event_time
				, MONITOR_CROSS_REFID_t __monitored_dev_info) = 0;							

	protected:
		bool complete_call_log(long __callID);
		bool set_agt_released(long __callID, tDeviceID __releasingDevice, TRACKING_CALL_t __current_call_log, TRACKING_CALL_t &__call_log_data);
		
	private:
		void push_queue(TRACKING_CALL_KEY_t __key_call_log, TRACKING_CALL_t __call_log_data);

	protected:
		agent_status_shm *pagent_status_rpt;  // For manage agent status
		tracking_call *pcall_tracking;  // For Call log
		device_manage_shm *pDevice_shm;
		hcommon *pCommObj = NULL;
		
	private:
		
		queue<LOGGINGDATA_t> *logqueue = NULL;  // Event & Call log
		
		pthread_mutex_t *log_mutex;     // mutex for logqueue
		pthread_cond_t *log_cond;		// Signal condition for logqueue
};

#endif // EVENT_HANDLER_BASE_INCLUDED
