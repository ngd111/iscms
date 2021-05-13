//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
//
//////////////////////////////////////////////////////////////////////////////

#ifndef CMS_ANL_INCLUDED
#define CMS_ANL_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "XPRJACEDLLEx/ICSTA.h"
#include "XPRJACEDLLEx/LogConfig.h"
#include "XPRJACEDLLEx/PBXConfig.h"
#include "XPRJACEDLLEx/CSTANameString.h"

#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include "mqueue.hpp"
#include "common.h"
#include "hutil.hpp"
#include "ipc_res.hpp"
#include "ipc_def.hpp"

#include "manage_umap.hpp"
#include "manage_shared_memory.hpp"
#include "manage_shared_umap.hpp"

#include "agent_status_shm.hpp"
#include "event_handler_vdn.hpp"
#include "event_handler_extn.hpp"
#include "device_manage_monref_shm.hpp"

// Historical report
#include "callreport_agent.hpp"



#define WAIT_SECOND 5                 // used in timed_receive function
#define WAIT_JOINING WAIT_SECOND + 2  // used in termination function
#define INTERVAL    15                // stat. interval

static MONITOR_CROSS_REFID_t g_default_mon_cross_refid = {UNDEFINED, "0", ""};

class event_handler
{
	
	public:
		event_handler();
		~event_handler();
		
		void do_report();
		
		static void FuncSIGTERM(int __signo);
		
		static void *analysisMain(void* __params);
		
		// Write data to write message queue using event list
		static void *loggerMain(void* __params);
		
		static void *archiverMain(void* __params);
		
	private:
		void* extract_data(EVENTDATA_t __data
							, device_manage_monref_shm *__pDeviceInfo = NULL
							, MONITOR_CROSS_REFID_t &__device_info_out = g_default_mon_cross_refid);

		// VDN 기준의 통계 처리가 여기서 이뤄짐(VDN, skill status report, call log(VDN기준 이벤트로 생성됨)
		// => Call cleared시 call_log를 이용해 VDN,skill 통계 생성
//		bool process_vdn_criterion_event(EVENTDATA_t __data, event_handler_vdn *__pReport
//											, device_manage_monref_shm *__pDeviceInfo);
		
//		bool process_agent_criterion_event(EVENTDATA_t __data, agent_status_shm *__pReport
//											, hcommon *__pCommObj);		
		bool process_event(EVENTDATA_t __data, 
						event_handler_vdn *__pVDN,
						event_handler_extn *__pExtn,
						device_manage_monref_shm *__pDeviceInfo);						

											
		void WaitForJoiningMain();											
	
	private:
		
		static event_handler *pHandler;
		
		pthread_t th_arch, th_stats, th_log;
		
		pthread_mutex_t rm_mutex;

		
		pthread_mutex_t log_cond_mutex;  // Mutex for condition log_cond
		pthread_mutex_t arch_cond_mutex; // Mutex for condition arch_cond		
		pthread_mutex_t log_mutex;  // Mutex for threads related logging event and call log

		pthread_cond_t log_cond;    // Condition for threads related logging event and call log
		pthread_cond_t arch_cond;   // Condition for threads related archiver
		
		queue<LOGGINGDATA_t> logqueue;  // Event & Call log
		

		hcommon *pCommObj = NULL;
		//HOCIHandler *pOraObj; -> MySQL 처리
		int remove_thread;
};


#endif //CMS_ANL_INCLUDED
