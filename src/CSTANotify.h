#pragma once

#include <vector>

#include "XPRJACEDLLEx/ICSTA.h"
#include "XPRJACEDLLEx/IcstaNotifier.h"
#include "XPRJACEDLLEx/CSTADefine.h"

#include "mqueue.hpp"
#include "device_manage_shm.hpp"
#include "agent_status_shm.hpp"
#include "length_def.hpp"
#include "hutil.hpp"

using namespace IS;


class CSTANotifier : public ICSTANotifier
{
public:
	CSTANotifier(void);
	~CSTANotifier(void);

	void NotifyEvent(CCSTAEvent& event);


	string GetSessionID()		{	return sessionID_;	};
//	void SetLinkStatus(int __linkStatus) { linkStatus = __linkStatus; };
	int  GetLinkStatus() { return linkStatus; };
	int  GetMonitorStatus() { return monitoringStatus; };
	void SetMonitorStatus(int __monitoringStatus) { monitoringStatus = __monitoringStatus; };
	
	void SetMessageQueue(mqueue *__pMQ);
	void SetSharedMemory(device_manage_shm *__pDevice_status_shm, agent_status_shm *__pAgent_status_shm);
	void SetICSTA(ICSTA *__pcsta_);
	void SetCommonObject(hcommon *__pCommObj);	
	int  GetAgentState(string __extension);
	
private:
	
	void OnConfirmationEvent(tConfirmationEventType conf, CSTAConfirmationEvent confEvent, tPrivateData privData);
	void OnUnsolicitedEvent(tUnsolicitedEventType unsol, CSTAUnsolicitedEvent unsolEvent, tPrivateData privData);
	void OnSystemEvent(tSystemEventType sys, CSTASystemEvent sysEvent, tPrivateData privData);

	void AddLocalConnState2List(tLocalConnectionState localstate);
	void AddCause2List(tEventCause cause);
	void AddLastRedirectDevice2List(tDeviceID lastRedirectDevice);

	string sessionID_;


	//////////// For cumstomizing class //////////////////////
	
	int linkStatus;  // 1 : connected, 0 : closed
	int monitoringStatus; // 1 : monitor on, 0 : monitor off
	
	mqueue  *pMQ;   // message queue class
	device_manage_shm *pDevice_status_shm;  // Montitoring device list
	agent_status_shm *pAgent_status_shm; // Agent status
	ICSTA *pcsta_;
	
	manage_umap<SERVICE_CROSS_REFID_KEY_t, SERVICE_CROSS_REFID_t> *pServiceRequested;
	
	
	// Query Agent status using deviceID
	
	
	
	hcommon *pCommObj;
};
