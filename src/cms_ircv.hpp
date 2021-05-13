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

#include "XPRJACEDLLEx/LogConfig.h"
#include "XPRJACEDLLEx/PBXConfig.h"
#include "XPRJACEDLLEx/CSTANameString.h"

#include "CSTANotify.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include "mqueue.hpp"
#include "hutil.hpp"
#include "manage_shared_memory.hpp"
//#include "agent_status_shm.hpp"
#include "common.h"
#include "ipc_res.hpp"
#include "ipc_def.hpp"
#include "base_def.hpp"
#include "length_def.hpp"


class EventFilter
{
	public:
		
		EventFilter();
		~EventFilter();
		
		void InitCSTA();
		
		void ExitCSTA();
		
		void TryMonitorWholeDevice();
		
		void TryGetAgentState();
		
		void SetEventLogFileName();
		
		// monitor function.
		int MonitorDevice(string device);
		
		int GetAgentState(string agent);
		
		int MonitorCallViaDevice(string device);
		
		int	MonitorStop(int monitorID);

		int	OpenSession(string ip, int port, string appInfo, string domain, int clientPort, string id, string password);
		
		int	CloseSession();
	
		// Unique exposed method(child process logic)
		void do_receive_event();
		
		static void FuncSIGUSR1(int __signo);
		static void FuncSIGTERM(int __signo);
		
	private:
		bool CreateMessageQueue(const char* name, size_t max_size, size_t message_size);

		static EventFilter *pHandler;
		int exit_flag;
		
		mqueue *pMQ;
		device_manage_shm *pDevice_status_shm;  // Montitoring device list
		agent_status_shm *pAgent_status_shm;
		hcommon *pCommObj;
				
		manage_shared_memory shmobj;
		shared_memory_t *shm;
		
//		manage_umap<SERVICE_CROSS_REFID_KEY_t, SERVICE_CROSS_REFID_t> *pServiceRequested;
		
		char message_text[LOG_LEN];
		
	
		ICSTA*        pcsta_;
		CSTANotifier* pnotee_;
		CLogConfig*   plogConfig_;
		CPBXConfig*   ppbxConfig_;
};

#endif //CMS_ANL_INCLUDED
