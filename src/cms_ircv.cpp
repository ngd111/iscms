#include "cms_ircv.hpp"

// 임시 로그 디렉토리 설정
static string LogDir= "/home/winddori/cms/log";
static string Log_prefix="SLOG_I";


EventFilter::EventFilter() {
	pHandler = this;
};
EventFilter::~EventFilter() {
};
		
void EventFilter::InitCSTA() {
	// construct notifier to receive events from PBX driver
	pnotee_ = new CSTANotifier;
	
	// construct and initialize CLogConfig	
	plogConfig_	= new CLogConfig;
	
	ppbxConfig_ = new CPBXConfig;
	
	plogConfig_->SetLogLevel(LOG_MAX);
	
	SetEventLogFileName();

	// get CSTA interface with ::GetCSTA()
	pcsta_ = ::GetCSTA(*pnotee_, *plogConfig_);
}
		
void EventFilter::ExitCSTA() {
	if (pnotee_) {
		delete pnotee_; pnotee_ = NULL;
	}

	if (plogConfig_) {
		delete plogConfig_;	plogConfig_ = NULL;
	}

	if (ppbxConfig_) {
		delete ppbxConfig_; ppbxConfig_ = NULL;
	}

	if (pcsta_) {
		// before destroy CSTA I/F, release that object.
		pcsta_->Release(); pcsta_ = NULL;
	}
}

void EventFilter::SetEventLogFileName() {

	if (plogConfig_ == NULL) return;

	string logfile =  LogDir;
	logfile = logfile + "/elog/ELOG";
	plogConfig_->SetLogFilePath(logfile);
}
		
void EventFilter::TryMonitorWholeDevice() {
	MonitorDevice("sip:9001@cube-x19.hansol.com");
	MonitorDevice("sip:9002@cube-x19.hansol.com");
	MonitorDevice("sip:9003@cube-x19.hansol.com");
//			MonitorDevice("sip:9004@cube-x19.hansol.com");
//			MonitorDevice("sip:9005@cube-x19.hansol.com");
//			MonitorDevice("sip:9008@cube-x19.hansol.com");
//			MonitorDevice("sip:9009@cube-x19.hansol.com");
//			MonitorDevice("sip:9101@cube-x19.hansol.com");

	///////////////////////////////////////////////////////////
	
	//Monitor VDN
	MonitorCallViaDevice("sip:1000@cube-x19.hansol.com");
	MonitorCallViaDevice("sip:1001@cube-x19.hansol.com");
	
	
	// Setting monitoring status
	pnotee_->SetMonitorStatus(1);	
}

void EventFilter::TryGetAgentState() {
	GetAgentState("sip:9001@cube-x19.hansol.com");
	GetAgentState("sip:9002@cube-x19.hansol.com");
	GetAgentState("sip:9003@cube-x19.hansol.com");
}
		
		// monitor function.
int EventFilter::MonitorDevice(string device) {
	if (!pcsta_)			return -1;
	
	MONITORING_LIST_SHMKEY_t Key;
	MONITORING_LIST_SHM_t    Data;
	
	char extension[EXTENSION_LEN];
	char domain   [DOMAIN_LEN];
	/////////////////////////////////////////////////////
	
	memset(&Key, 0x00, sizeof(MONITORING_LIST_SHMKEY_t));
	memset(&Data, 0x00, sizeof(MONITORING_LIST_SHM_t));	
	
	////////////////////////////////////////////		
	tDeviceID		deviceID;
	strcpy(deviceID, device.c_str());
	
std::cout << "deviceID : " << deviceID << std::endl;	

	tMonitorFilter	monitorFilter;
	monitorFilter.agent = 0;
	monitorFilter.call = 0;
	monitorFilter.feature = 0;
	////////////////////////////////////////////

	memset(&extension, 0x00, sizeof(extension));
	memset(&domain, 0x00, sizeof(domain));
	
	hcommon::ExtractExtension(deviceID, extension);
	hcommon::ExtractDomain(deviceID, domain);
std::cout << "after deviceID : " << deviceID << std::endl;	
	strcpy(Key.extension, extension);
	strcpy(Key.domain, domain);	
	
	Data.device_type = STATION;  // Station
	strcpy(Data.extension, Key.extension);

	Data.serviceCrossRefID = pcsta_->MonitorDevice(deviceID, monitorFilter);

	if (pDevice_status_shm->register_data(Key, Data) == false) {
		std::cout << "Failed to insert monitoring device data to the shared memory with "
					<< Data.extension << std::endl;
		return -1;
	}

	return Data.serviceCrossRefID;
	
	////////////////////////////////////////////
	//return pcsta_->MonitorDevice(deviceID, monitorFilter);
}

int EventFilter::GetAgentState(string __extension) {
	if (!pcsta_) return -1;
		
	return (pnotee_->GetAgentState(__extension));
}

//int EventFilter::GetAgentState(string __extension) {
//	if (!pcsta_) return -1;
//		
//	SERVICE_CROSS_REFID_KEY_t Key;
//	SERVICE_CROSS_REFID_t     Data;
//
//	char extension[EXTENSION_LEN];
//	char domain   [DOMAIN_LEN];
//	///////////////////////////////////////////////////////
//	
//	memset(&Key, 0x00, sizeof(SERVICE_CROSS_REFID_KEY_t));
//	memset(&Data, 0x00, sizeof(SERVICE_CROSS_REFID_t));	
//	
//	///////////////////////////////////////////////////////
//	tAgentID		agentID;		
//	hstrcpy(agentID, __extension.c_str());
//	
//	memset(&extension, 0x00, sizeof(extension));
//	memset(&domain, 0x00, sizeof(domain));
//	
//	hcommon::ExtractExtension(agentID, extension);
//	hcommon::ExtractDomain(agentID, domain);
//
//	Data.device_type = STATION;
//	strcpy(Data.extension, extension);	
//	strcpy(Data.domain, domain);
//		
//	Key.serviceCrossRefID = pcsta_->GetAgentState(agentID);
//	
//	if (pServiceRequested->insert(Key, Data) == false) {
//		std::cout << "Failed to insert serviceRequest data" << std::endl;
//		return -1;
//	}
//
//	return Key.serviceCrossRefID;
//}		

		
int EventFilter::MonitorCallViaDevice(string device) {
	if (!pcsta_)			return -1;
		

	MONITORING_LIST_SHMKEY_t Key;
	MONITORING_LIST_SHM_t    Data;
	
	char extension[EXTENSION_LEN];
	char domain   [DOMAIN_LEN];
	///////////////////////////////////////////////////////
	
	memset(&Key, 0x00, sizeof(MONITORING_LIST_SHMKEY_t));
	memset(&Data, 0x00, sizeof(MONITORING_LIST_SHM_t));	
	
	///////////////////////////////////////////////////////		
	tDeviceID		deviceID;
	strcpy(deviceID, device.c_str());

	tMonitorFilter	monitorFilter;
	monitorFilter.agent = 0;
	monitorFilter.call = 0;
	monitorFilter.feature = 0;
	///////////////////////////////////////////////////////	
	memset(&extension, 0x00, sizeof(extension));
	memset(&domain, 0x00, sizeof(domain));
	
	hcommon::ExtractExtension(deviceID, extension);
	hcommon::ExtractDomain(deviceID, domain);

	strcpy(Key.extension, extension);	
	strcpy(Key.domain, domain);	
	
	Data.device_type = VDN;    // VDN
	strcpy(Data.extension, Key.extension);

	Data.serviceCrossRefID = pcsta_->MonitorCallViaDevice(deviceID, monitorFilter);

	if (pDevice_status_shm->register_data(Key, Data) == false) {
		std::cout << "Failed to insert monitoring device data to the shared memory with "
					<< Data.extension << std::endl;
		return -1;
	}

	return Data.serviceCrossRefID;
	
	////////////////////////////////////////////		
//			return pcsta_->MonitorCallViaDevice(deviceID, monitorFilter);
}

int	EventFilter::MonitorStop(int monitorID) {
	if (!pcsta_) return -1;

	tMonitorCrossRefID	monitorCrossRefID = monitorID;

	return pcsta_->MonitorStop(monitorCrossRefID);
}					

int	EventFilter::OpenSession(string ip, int port, string appInfo, string domain, int clientPort, string id, string password) {
	if (!pcsta_) return -1;

	ppbxConfig_->SetServerIP(ip);
	ppbxConfig_->SetServerPort(port);
	ppbxConfig_->SetLoginID(id);
	ppbxConfig_->SetLoginPassword(password);
	ppbxConfig_->SetURIDomain(domain);
	ppbxConfig_->SetClientPort(clientPort);
	ppbxConfig_->SetSessionCheckPeriod(145);

	pcsta_->SetPBXInfo(*ppbxConfig_);

	return pcsta_->OpenCSTASession(appInfo);
}
		
int	EventFilter::CloseSession() {
	if (!pcsta_) return -1;

	return pcsta_->CloseCSTASession(pnotee_->GetSessionID());
}
	
// Unique exposed method(child process logic)
void EventFilter::do_receive_event() {
	string strIP = "10.1.15.130";
	int    nPort = 5060;
	string strAppInfo = "StatsConnector";
	string strDomain = "cube-x19.hansol.com";
	int    nClientPort = 50000;
	string strUsrID = "userid";
	string strPasswd = "passwd";
	
	MONITORING_LIST_SHMKEY_t Key;
	MONITORING_LIST_SHM_t    Data;
	
	std::cout << "do receiving event" << std::endl;

	pHandler->exit_flag = 0; // exit flag : no exit 0, exit 1
	
	// Create utility object
	pCommObj = new hcommon;
	if (pCommObj == NULL) {
		std::cerr << "Fail to allocate memory for utility object" << std::endl;
		return;
	}
	
	// Set system logfile
	pCommObj->SetSystemLogFileName(LogDir, Log_prefix);

	strcpy(message_text, "\n\n ================================================================");
	pCommObj->LoggingError(__FILE__, __func__, 0, message_text);
	
	strcpy(message_text, "Event Receiver start now");
	pCommObj->LoggingError(__FILE__, __func__, 0, message_text);
		
	// Create shared memory
	shmobj.remove_shared_memory(REPORT_AREA_SHM);
	
	try {
		// Make 1MB shared memory area
		shm = shmobj.create_shared_memory(REPORT_AREA_SHM, 1024 * 1024);
	}
	catch (std::string ex)
	{
		std::cerr << ex << std::endl;
		strcpy(message_text, ex.c_str());
		pCommObj->LoggingError(__FILE__, __func__, 0, message_text);			
		delete pCommObj; pCommObj = NULL;
		return;
	}

    strcpy(message_text, "Shared memory realm creation success");
    pCommObj->LoggingError(__FILE__, __func__, 0, message_text);	

	pDevice_status_shm = new device_manage_shm(&shmobj, shm);
	if (pDevice_status_shm == NULL) {
		std::cerr << "Failed to allocate Device manage object" << std::endl;
	    strcpy(message_text, "Failed to allocate Device manage object");
	    pCommObj->LoggingError(__FILE__, __func__, 0, message_text);
		delete pCommObj; pCommObj = NULL;
		return;
	}
	
	pAgent_status_shm = new agent_status_shm(&shmobj, shm);
	if (pAgent_status_shm == NULL) {
		std::cerr << "Failed to allocate agent_status_shm object" << std::endl;
	    strcpy(message_text, "Failed to allocate agent_status_shm object");
	    pCommObj->LoggingError(__FILE__, __func__, 0, message_text);
		delete pCommObj; pCommObj = NULL;
		delete pDevice_status_shm; pDevice_status_shm = NULL;
		return;			
	}
	
	// Create message queue
	pMQ = new mqueue;
	if (pMQ == NULL) {
		std::cerr << "Failed to allocate message queue object" << std::endl;
	    strcpy(message_text, "Failed to message queue object");
	    pCommObj->LoggingError(__FILE__, __func__, 0, message_text);
	    
		delete pDevice_status_shm; pDevice_status_shm = NULL;
		delete pAgent_status_shm; pAgent_status_shm = NULL;
		delete pCommObj; pCommObj = NULL;
		return;				
	}
		
	if (CreateMessageQueue(QUE_EVENT, 200, sizeof(EVENTDATA_t)) == false) {
		std::cout << "Failed to create message queue" << std::endl;
	    strcpy(message_text, "Failed to create message queue");
	    pCommObj->LoggingError(__FILE__, __func__, 0, message_text);
	    			
		delete pMQ; pMQ = NULL;
		delete pDevice_status_shm; pDevice_status_shm = NULL;
		delete pAgent_status_shm; pAgent_status_shm = NULL;
		delete pCommObj; pCommObj = NULL;
		return;
	}
	
    strcpy(message_text, "Message queue creation success");
    pCommObj->LoggingError(__FILE__, __func__, 0, message_text);
    
		
	InitCSTA();
	
	// Set message queue
	pnotee_->SetMessageQueue(pMQ);
	
	// Set shared memory realm
	pnotee_->SetSharedMemory(pDevice_status_shm, pAgent_status_shm);
	
	pnotee_->SetICSTA(pcsta_);
	
	pnotee_->SetCommonObject(pCommObj);
	
	
	////////////////////////////////////////////////////////////////////////////
	
	if (OpenSession(strIP, nPort, strAppInfo, strDomain, nClientPort, strUsrID, strPasswd) != 1) {
		std::cout << "Error while OpenSession" << std::endl;
	    strcpy(message_text, "Error while OpenSession");
	    pCommObj->LoggingError(__FILE__, __func__, 0, message_text);			
		CloseSession();
	}
	else {
		std::cout << "OpenSession call success" << std::endl;
	    strcpy(message_text, "OpenSession call success");
	    pCommObj->LoggingError(__FILE__, __func__, 0, message_text);			
	}

	// 1. Monitor extension
	while(1) {
		if (pnotee_->GetLinkStatus() == 1) {
			// Monitoring device list
			TryMonitorWholeDevice();
			
			break;
		}
		
		usleep(10);
	}
	
	pnotee_->SetMonitorStatus(1);
	
    strcpy(message_text, "Monitoring device finished");
    pCommObj->LoggingError(__FILE__, __func__, 0, message_text);
	
	// 2. Get agent status(IS-CM의 교환기 데이터를 이용 AgentID로 개별 Query 날려 현재 상태 조회
	TryGetAgentState();
			
	// Wait for CSTA event
	while(1) {
		SetEventLogFileName();
		// Set system logfile
		pCommObj->SetSystemLogFileName(LogDir, Log_prefix);

		if (pHandler->exit_flag == 1) {
			std::cout << "Break running" << std::endl;
			break;
		}

		if (pnotee_ == NULL) {
			std::cout << "pnotee_ == NULL" << std::endl;
		    strcpy(message_text, "pnotee_ == NULL");
		    pCommObj->LoggingError(__FILE__, __func__, 0, message_text);				
			ExitCSTA();
			InitCSTA();
		}
		
		// Try re-connect to IS-CM when link disconnected
		if (pnotee_->GetLinkStatus() == 0) {
			if (OpenSession(strIP, nPort, strAppInfo, strDomain, nClientPort, strUsrID, strPasswd) != 1) {
				std::cout << "Error while OpenSession" << std::endl;
			    strcpy(message_text, "Error while OpenSession");
			    pCommObj->LoggingError(__FILE__, __func__, 0, message_text);			
//				CloseSession();
			}
			else {
				std::cout << "OpenSession call success" << std::endl;
			    strcpy(message_text, "OpenSession call success");
			    pCommObj->LoggingError(__FILE__, __func__, 0, message_text);			
			}
		
			// 1. Monitor extension
			while(1) {
				if (pnotee_->GetLinkStatus() == 1) {
					// Monitoring device list
					TryMonitorWholeDevice();
					
					break;
				}
				
				usleep(10);
			}			
		}
				
		// Try monitoring device when link is suceessfully re-connected.		
		if ( (pnotee_->GetLinkStatus() == 1)  && (pnotee_->GetMonitorStatus() == 0) ) {
			std::cout << "RE-CONNET to CM" << std::endl;
		    strcpy(message_text, "Re-connect to CM");
		    pCommObj->LoggingError(__FILE__, __func__, 0, message_text);
			// Monitoring device list
			TryMonitorWholeDevice();
		}
		
		// Check device info. to re-monitoring device
		
		
		sleep(5);
	}
	
	shmobj.remove_shared_memory(REPORT_AREA_SHM);
	
	ExitCSTA();
	
	if (pMQ != NULL) {
		pMQ->remove_message_queue(QUE_EVENT);
		delete pMQ; pMQ = NULL;
	}
		
	if (pDevice_status_shm != NULL) {
		delete pDevice_status_shm;	pDevice_status_shm = NULL;	
	}
	
	if (pAgent_status_shm != NULL) {
		delete pAgent_status_shm; pAgent_status_shm = NULL;
	}
	
	strcpy(message_text, "Program exit gracefully");
	pHandler->pCommObj->LoggingError(__FILE__, __func__, 0, message_text);		
	strcpy(message_text, " ================================================================\n\n");
	pHandler->pCommObj->LoggingError(__FILE__, __func__, 0, message_text);	
	
	if (pCommObj != NULL) {
		delete pCommObj; pCommObj = NULL;
	}	
}
		
void EventFilter::FuncSIGUSR1(int __signo) {
	std::cout << "Setting up exit_flag = 1" << std::endl;
	
	pHandler->CloseSession();
	
	while (1) {
		if (pHandler->pnotee_->GetLinkStatus() == 0)
			break;
		
		usleep(100);
	}
	
	pHandler->exit_flag = 1;
}

void EventFilter::FuncSIGTERM(int __signo) {

	char message_text[LOG_LEN];
//	for (pHandler->pnotee_->vmonIter = pHandler->pnotee_->vmonlist.begin(); 
//			pHandler->pnotee_->vmonIter != pHandler->pnotee_->vmonlist.end(); 
//			++pHandler->pnotee_->vmonIter) {
//		printf("MonitorStop : [%d]\n", *pHandler->pnotee_->vmonIter);
//		pHandler->MonitorStop(*pHandler->pnotee_->vmonIter);
//	}

	pHandler->CloseSession();
	
	// MonitorStop 처리
	
	while (1) {
		if (pHandler->pnotee_->GetLinkStatus() == 0)
			break;
		
		usleep(100);
	}
	
	pHandler->ExitCSTA();
	
	if (pHandler->pMQ != NULL)
		delete pHandler->pMQ;
		
	if (pHandler->pDevice_status_shm != NULL)
		delete pHandler->pDevice_status_shm;
		
	std::cout << "Program exit forcefully. " << std::endl;

	strcpy(message_text, "Program exit forcefully");
	pHandler->pCommObj->LoggingError(__FILE__, __func__, 0, message_text);		
	strcpy(message_text, " ================================================================\n\n");
	pHandler->pCommObj->LoggingError(__FILE__, __func__, 0, message_text);
		
	exit(0);
}
		
bool EventFilter::CreateMessageQueue(const char* name, size_t max_size, size_t message_size) {
	try {
		pMQ->create_message_queue(name, max_size, message_size);
	}
	catch (std::string err_msg)
	{
		std::cerr << err_msg << std::endl;
		return false;
	}
		
	return true;
};


EventFilter* EventFilter::pHandler = 0;

int main()
{
	int pid, chpid;
	EventFilter *evtf = new EventFilter;

	if ((pid = fork()) < 0) {
		perror("fork() error:");
		exit(0);
	}
	else if (pid != 0) {
		perror("Parent process end:");
		exit(0);
	}
	
	signal(SIGPIPE, SIG_IGN);
	Signal(SIGUSR1, (void *)EventFilter::FuncSIGUSR1);
	Signal(SIGTERM, (void *)EventFilter::FuncSIGTERM);

	evtf->do_receive_event();
	
	delete evtf;
	
	std::cout << "Program exit gracefully. " << std::endl;
}
