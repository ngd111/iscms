//////////////////////////////////////////////////////////////////////////////
////
//// (C) Copyright Hansol inticube 2014
////
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_HANDLER_NON_VDN_INCLUDED
#define EVENT_HANDLER_NON_VDN_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif


#include "event_handler_base.hpp"


class event_handler_extn : public event_handler_base
{
	public:
		event_handler_extn(manage_shared_memory *__shmobj, shared_memory_t *__shm, hcommon *__commobj);
		~event_handler_extn();

		bool register_data(tUnsolicitedEventType __event, void* __event_data
				, tPrivateData __private_data, time_t __event_time
				, MONITOR_CROSS_REFID_t __monitored_dev_info);
};

#endif // EVENT_HANDLER_NON_VDN_INCLUDED
