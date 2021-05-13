#include "cms_anl.hpp"


// 로그 디렉토리 추후 다른 정책으로 변경
static string LogDir= "/home/winddori/cms/log";
static string Log_prefix="SLOG_A";
static char *sAppName = (char *)"cms_anl";


struct __stats_thread_params
{
	hcommon* pCommObj;
	pthread_cond_t *cond;      // Condition for logging action
	pthread_mutex_t *mut;      // Mutex for logging action
	pthread_mutex_t *mut_cond; // Mutex for condition variable cond 
	char log_dir [1024+1];     // Log directory(Full path)
};

struct __arch_thread_params
{
	hcommon* pCommObj;
	pthread_cond_t *cond;      // Condition for exit thread
	pthread_mutex_t *mut_cond; // Mutex for condition variable cond
	char log_dir [1024+1];     // Log directory(Full path)
};
			
struct __log_thread_params
{
	hcommon* pCommObj;
	pthread_cond_t *cond;
	pthread_mutex_t *mut;      // Mutex for logging action
	pthread_mutex_t *mut_cond; // Mutex for condition variable cond
	char log_dir [1024+1];
};

typedef struct __stats_thread_params  TH_PARAMS_STATS;
typedef struct __arch_thread_params   TH_PARAMS_ARCH;
typedef struct __log_thread_params    TH_PARAMS_LOG;


event_handler::event_handler() {
	pHandler = this;
};
event_handler::~event_handler() {
};
		
void event_handler::do_report() {
	string sCurrentDate;
	char log_dir  [1024+1]; // 로그파일명(full path)
	char cMessage [4096+1];
	int status;
	
	TH_PARAMS_STATS *params_stats;
	TH_PARAMS_ARCH  *params_arch;
	TH_PARAMS_LOG   *params_log;
	
	pCommObj = new hcommon;
	if (pCommObj == NULL) return;
	
//			sCurrentDate = pCommObj->get_systemtime(TimeOption::YYYYMMDD);
		
	remove_thread = 0;
	
	pthread_mutex_init(&rm_mutex, NULL);
	pthread_mutex_init(&log_mutex, NULL);
	pthread_mutex_init(&log_cond_mutex, NULL);
	pthread_mutex_init(&arch_cond_mutex, NULL);
	
	pthread_cond_init(&log_cond, NULL);
	pthread_cond_init(&arch_cond, NULL);

	pCommObj->SetSystemLogFileName(LogDir, Log_prefix);

	// MySQL 관련 연결~~
	
	// Configure thread parameter
	params_stats = new TH_PARAMS_STATS;
	if (params_stats == NULL) {
		delete pCommObj;
		return;
	}
	memset(params_stats, 0x00, sizeof(TH_PARAMS_STATS));
	
	params_stats->pCommObj = pCommObj;
	params_stats->cond = &log_cond;
	params_stats->mut = &log_mutex;
	params_stats->mut_cond = &log_cond_mutex;
	strcpy(params_stats->log_dir, LogDir.c_str());

	params_arch = new TH_PARAMS_ARCH;
	if (params_arch == NULL) {
		delete params_stats;
		delete pCommObj;
		return;
	}
	memset(params_arch, 0x00, sizeof(TH_PARAMS_ARCH));

	params_arch->pCommObj = pCommObj;
	params_arch->cond = &arch_cond;
	params_arch->mut_cond = &arch_cond_mutex;
	strcpy(params_arch->log_dir, LogDir.c_str());
	
	params_log = new TH_PARAMS_LOG;
	if (params_log == NULL) {
		delete params_stats;
		delete params_arch;
		delete pCommObj;
		return;
	}
	memset(params_log, 0x00, sizeof(TH_PARAMS_LOG));

	params_log->pCommObj = pCommObj;
	params_log->cond = &log_cond;
	params_log->mut = &log_mutex;
	params_log->mut_cond = &log_cond_mutex;
	strcpy(params_log->log_dir, LogDir.c_str());
			
	// Thread 1 : Thread for generating call statistics
	//            Create monitoring report(VDN status, Skill queue, Agent status)
	//            Create historical report(VDN, Skill, Agent)
	//            4th parameter : argument providing data over the thread
	pthread_create(&th_stats, NULL, &analysisMain, (void*)params_stats);
	
	// Thread 2 : Thread for archiving historical data
    //            Scheduling for DB writing
    pthread_create(&th_arch, NULL, &archiverMain, (void*)params_arch);
    
    // Thread 3 : Thread for writing event to message queue(for DB logging)
	pthread_create(&th_log, NULL, &loggerMain, (void*)params_log);

	std::cout << "after create pthread" << std::endl;

	pthread_join(th_stats, (void **)&status);
	std::cout << "after pthread_join : th_stats" << std::endl;				


	pthread_join(th_arch, (void **)&status);
	std::cout << "after pthread_join : th_arch" << std::endl;
	
	pthread_join(th_log, (void **)&status);
	std::cout << "after pthread_join : th_log" << std::endl;		
	
	return;
};

void event_handler::FuncSIGTERM(int __signo) {
	char sMsg[1024];
	
	sprintf(sMsg, "Pid:[%d], SIGTERM signal caught[%d]", getpid(), __signo);
	pHandler->pCommObj->LoggingError(__FILE__, __func__, 0, sMsg);
	
	pHandler->WaitForJoiningMain();  //Exit scheduler & event handler

	pthread_cond_destroy(&pHandler->log_cond);
	pthread_cond_destroy(&pHandler->arch_cond);			
	pthread_mutex_destroy(&pHandler->rm_mutex);
	pthread_mutex_destroy(&pHandler->log_cond_mutex);
	pthread_mutex_destroy(&pHandler->log_mutex);
	pthread_mutex_destroy(&pHandler->arch_cond_mutex);
	
	if (pHandler->pCommObj!=NULL) delete pHandler->pCommObj;

	exit(0);
};
		
void* event_handler::extract_data(EVENTDATA_t __data, 
		device_manage_monref_shm *__pDeviceInfo, MONITOR_CROSS_REFID_t &__device_info_out) {
	MONITOR_CROSS_REFID_KEY_t key;
	MONITOR_CROSS_REFID_t*    data;
	
	char extension[EXTENSION_LEN];
	
	void* result;
	
	memset(&key, 0x00, sizeof(MONITOR_CROSS_REFID_KEY_t));
	memset(&__device_info_out, 0x00, sizeof(MONITOR_CROSS_REFID_KEY_t));
	memset(extension, 0x00, sizeof(extension));
	
	if (__data.event_type == etConfirmation) {
	}
	else if (__data.event_type == etUnsolicited) {
		switch((tUnsolicitedEventType)__data.event_type_detail)
		{
			case CSTABridged:
				static tBridgedEvent bridged;
				memset(&bridged, 0x00, sizeof(tBridgedEvent));
				memcpy(&bridged, &__data.u.unsoEvent.u.bridged, sizeof(tBridgedEvent));
				key.monitorCrossRefID = bridged.monitorCrossRefID;

				hcommon::ExtractExtension(bridged.bridgedConnection.deviceID, extension);
				strcpy(bridged.bridgedConnection.deviceID, extension);
				hcommon::ExtractExtension(bridged.bridgedAppearance, extension);
				strcpy(bridged.bridgedAppearance, extension);				
				
				result = &bridged;
				break;
			case CSTACallCleared:
				static tCallClearedEvent callcleared;
				memset(&callcleared, 0x00, sizeof(tCallClearedEvent));
				memcpy(&callcleared, &__data.u.unsoEvent.u.callCleared, sizeof(tCallClearedEvent));
				key.monitorCrossRefID = callcleared.monitorCrossRefID;
				
				hcommon::ExtractExtension(callcleared.clearedCall.deviceID, extension);
				strcpy(callcleared.clearedCall.deviceID, extension);
											
				result = &callcleared;
				break;
			case CSTAConferenced:
				static tConferencedEvent conferenced;
				memset(&conferenced, 0x00, sizeof(tConferencedEvent));
				memcpy(&conferenced, &__data.u.unsoEvent.u.conferenced, sizeof(tConferencedEvent));
				key.monitorCrossRefID = conferenced.monitorCrossRefID;
				
				hcommon::ExtractExtension(conferenced.primaryOldCall.deviceID, extension);
				strcpy(conferenced.primaryOldCall.deviceID, extension);
				hcommon::ExtractExtension(conferenced.secondaryOldCall.deviceID, extension);
				strcpy(conferenced.secondaryOldCall.deviceID, extension);
				hcommon::ExtractExtension(conferenced.conferencingDevice, extension);
				strcpy(conferenced.conferencingDevice, extension);
				hcommon::ExtractExtension(conferenced.addedParty, extension);
				strcpy(conferenced.addedParty, extension);
				
				//tConnectionIDList은 추후 필요하면 device ID추출
																
				result = &conferenced;
				break;
			case CSTAConnectionCleared:
				static tConnectionClearedEvent conncleared;
				memset(&conncleared, 0x00, sizeof(tConnectionClearedEvent));
				memcpy(&conncleared, &__data.u.unsoEvent.u.connectionCleared, sizeof(tConnectionClearedEvent));
				key.monitorCrossRefID = conncleared.monitorCrossRefID;

				hcommon::ExtractExtension(conncleared.droppedConnection.deviceID, extension);
				strcpy(conncleared.droppedConnection.deviceID, extension);
				hcommon::ExtractExtension(conncleared.releasingDevice, extension);
				strcpy(conncleared.releasingDevice, extension);
				
				result = &conncleared;
				break;
			case CSTADelivered:
				static tDeliveredEvent delivered;
				memset(&delivered, 0x00, sizeof(tDeliveredEvent));
				memcpy(&delivered, &__data.u.unsoEvent.u.delivered, sizeof(tDeliveredEvent));
				key.monitorCrossRefID = delivered.monitorCrossRefID;

				hcommon::ExtractExtension(delivered.connection.deviceID, extension);
				strcpy(delivered.connection.deviceID, extension);
				hcommon::ExtractExtension(delivered.alertingDevice, extension);
				strcpy(delivered.alertingDevice, extension);
				hcommon::ExtractExtension(delivered.callingDevice, extension);
				strcpy(delivered.callingDevice, extension);
				hcommon::ExtractExtension(delivered.calledDevice, extension);
				strcpy(delivered.calledDevice, extension);
				hcommon::ExtractExtension(delivered.lastRedirectionDevice, extension);
				strcpy(delivered.lastRedirectionDevice, extension);
																								
				result = &delivered;
				break;
			case CSTADigitsDialled:
				static tDigitsDialledEvent digitsDialled;
				memset(&digitsDialled, 0x00, sizeof(tDigitsDialledEvent));
				memcpy(&digitsDialled, &__data.u.unsoEvent.u.digitsDialled, sizeof(tDigitsDialledEvent));
				key.monitorCrossRefID = digitsDialled.monitorCrossRefID;

				hcommon::ExtractExtension(digitsDialled.diallingConnection.deviceID, extension);
				strcpy(digitsDialled.diallingConnection.deviceID, extension);
				hcommon::ExtractExtension(digitsDialled.diallingDevice, extension);
				strcpy(digitsDialled.diallingDevice, extension);
				hcommon::ExtractExtension(digitsDialled.diallingSequence, extension);
				strcpy(digitsDialled.diallingSequence, extension);
												
				result = &digitsDialled;
				break;
			case CSTADiverted:
				static tDivertedEvent diverted;
				memset(&diverted, 0x00, sizeof(tDivertedEvent));
				memcpy(&diverted, &__data.u.unsoEvent.u.diverted, sizeof(tDivertedEvent));
				key.monitorCrossRefID = diverted.monitorCrossRefID;

				hcommon::ExtractExtension(diverted.connection.deviceID, extension);
				strcpy(diverted.connection.deviceID, extension);
				hcommon::ExtractExtension(diverted.divertingDevice, extension);
				strcpy(diverted.divertingDevice, extension);
				hcommon::ExtractExtension(diverted.newDestination, extension);
				strcpy(diverted.newDestination, extension);
				hcommon::ExtractExtension(diverted.lastRedirectionDevice, extension);
				strcpy(diverted.lastRedirectionDevice, extension);
																				
				result = &diverted;
				break;
			case CSTAEstablished:
				static tEstablishedEvent established;
				memset(&established, 0x00, sizeof(tEstablishedEvent));
				memcpy(&established, &__data.u.unsoEvent.u.established, sizeof(tEstablishedEvent));
				key.monitorCrossRefID = established.monitorCrossRefID;
				
				hcommon::ExtractExtension(established.establishedConnection.deviceID, extension);
				strcpy(established.establishedConnection.deviceID, extension);
				hcommon::ExtractExtension(established.answeringDevice, extension);
				strcpy(established.answeringDevice, extension);
				hcommon::ExtractExtension(established.callingDevice, extension);
				strcpy(established.callingDevice, extension);
				hcommon::ExtractExtension(established.calledDevice, extension);
				strcpy(established.calledDevice, extension);
				hcommon::ExtractExtension(established.lastRedirectionDevice, extension);
				strcpy(established.lastRedirectionDevice, extension);
																
				result = &established;
				break;
			case CSTAFailed:
				static tFailedEvent failed;
				memset(&failed, 0x00, sizeof(tFailedEvent));
				memcpy(&failed, &__data.u.unsoEvent.u.failed, sizeof(tFailedEvent));
				key.monitorCrossRefID = failed.monitorCrossRefID;

				hcommon::ExtractExtension(failed.failedConnection.deviceID, extension);
				strcpy(failed.failedConnection.deviceID, extension);
				hcommon::ExtractExtension(failed.faillingDevice, extension);
				strcpy(failed.faillingDevice, extension);
				hcommon::ExtractExtension(failed.callingDevice, extension);
				strcpy(failed.callingDevice, extension);
				hcommon::ExtractExtension(failed.calledDevice, extension);
				strcpy(failed.calledDevice, extension);
				hcommon::ExtractExtension(failed.lastRedirectionDevice, extension);
				strcpy(failed.lastRedirectionDevice, extension);

				result = &failed;
				break;												
			case CSTAHeld:
				static tHeldEvent held;
				memset(&held, 0x00, sizeof(tHeldEvent));
				memcpy(&held, &__data.u.unsoEvent.u.held, sizeof(tHeldEvent));
				key.monitorCrossRefID = held.monitorCrossRefID;
				
				hcommon::ExtractExtension(held.heldConnection.deviceID, extension);
				strcpy(held.heldConnection.deviceID, extension);
				hcommon::ExtractExtension(held.holdingDevice, extension);
				strcpy(held.holdingDevice, extension);
				hcommon::ExtractExtension(held.lastRedirectionDevice, extension);
				strcpy(held.lastRedirectionDevice, extension);

				result = &held;
				break;
			case CSTANetworkReached:
				static tNetworkReachedEvent networkReached;
				memset(&networkReached, 0x00, sizeof(tNetworkReachedEvent));
				memcpy(&networkReached, &__data.u.unsoEvent.u.networkReached, sizeof(tNetworkReachedEvent));
				key.monitorCrossRefID = networkReached.monitorCrossRefID;

				hcommon::ExtractExtension(networkReached.outboundConnection.deviceID, extension);
				strcpy(networkReached.outboundConnection.deviceID, extension);
				hcommon::ExtractExtension(networkReached.networkInterfaceUsed, extension);
				strcpy(networkReached.networkInterfaceUsed, extension);
				hcommon::ExtractExtension(networkReached.callingDevice, extension);
				strcpy(networkReached.callingDevice, extension);
				hcommon::ExtractExtension(networkReached.calledDevice, extension);
				strcpy(networkReached.calledDevice, extension);
				hcommon::ExtractExtension(networkReached.lastRedirectionDevice, extension);
				strcpy(networkReached.lastRedirectionDevice, extension);

				result = &networkReached;
				break;
			case CSTAOffered:
				static tOfferedEvent offered;
				memset(&offered, 0x00, sizeof(tOfferedEvent));
				memcpy(&offered, &__data.u.unsoEvent.u.offered, sizeof(tOfferedEvent));
				key.monitorCrossRefID = offered.monitorCrossRefID;

				hcommon::ExtractExtension(offered.offeredConnection.deviceID, extension);
				strcpy(offered.offeredConnection.deviceID, extension);
				hcommon::ExtractExtension(offered.offeredDevice, extension);
				strcpy(offered.offeredDevice, extension);
				hcommon::ExtractExtension(offered.callingDevice, extension);
				strcpy(offered.callingDevice, extension);
				hcommon::ExtractExtension(offered.calledDevice, extension);
				strcpy(offered.calledDevice, extension);
				hcommon::ExtractExtension(offered.lastRedirectionDevice, extension);
				strcpy(offered.lastRedirectionDevice, extension);
																				
				result = &offered;
				break;
			case CSTAOriginated:
				static tOriginatedEvent originated;
				memset(&originated, 0x00, sizeof(tOriginatedEvent));
				memcpy(&originated, &__data.u.unsoEvent.u.originated, sizeof(tOriginatedEvent));
				key.monitorCrossRefID = originated.monitorCrossRefID;

				hcommon::ExtractExtension(originated.originatedConnection.deviceID, extension);
				strcpy(originated.originatedConnection.deviceID, extension);
				hcommon::ExtractExtension(originated.callingDevice, extension);
				strcpy(originated.callingDevice, extension);
				hcommon::ExtractExtension(originated.calledDevice, extension);
				strcpy(originated.calledDevice, extension);
																
				result = &originated;
				break;
			case CSTAQueued:
				static tQueuedEvent queued;
				memset(&queued, 0x00, sizeof(tQueuedEvent));
				memcpy(&queued, &__data.u.unsoEvent.u.queued, sizeof(tQueuedEvent));
				key.monitorCrossRefID = queued.monitorCrossRefID;

				hcommon::ExtractExtension(queued.queuedConnection.deviceID, extension);
				strcpy(queued.queuedConnection.deviceID, extension);
				hcommon::ExtractExtension(queued.callingDevice, extension);
				strcpy(queued.callingDevice, extension);
				hcommon::ExtractExtension(queued.calledDevice, extension);
				strcpy(queued.calledDevice, extension);
				hcommon::ExtractExtension(queued.lastRedirectionDevice, extension);
				strcpy(queued.lastRedirectionDevice, extension);
									
				result = &queued;
				break;
			case CSTARetrieved:
				static tRetrievedEvent retrieved;
				memset(&retrieved, 0x00, sizeof(tRetrievedEvent));
				memcpy(&retrieved, &__data.u.unsoEvent.u.retrieved, sizeof(tRetrievedEvent));
				key.monitorCrossRefID = retrieved.monitorCrossRefID;

				hcommon::ExtractExtension(retrieved.retrievedConnection.deviceID, extension);
				strcpy(retrieved.retrievedConnection.deviceID, extension);
				hcommon::ExtractExtension(retrieved.retrievingDevice, extension);
				strcpy(retrieved.retrievingDevice, extension);
								
				result = &retrieved;
				break;
			case CSTAServiceInitiated:
				static tServiceInitiatedEvent serviceInitiated;
				memset(&serviceInitiated, 0x00, sizeof(tServiceInitiatedEvent));
				memcpy(&serviceInitiated, &__data.u.unsoEvent.u.serviceInitiated, sizeof(tServiceInitiatedEvent));
				key.monitorCrossRefID = serviceInitiated.monitorCrossRefID;

				hcommon::ExtractExtension(serviceInitiated.initiatedConnection.deviceID, extension);
				strcpy(serviceInitiated.initiatedConnection.deviceID, extension);
				hcommon::ExtractExtension(serviceInitiated.initiatingDevice, extension);
				strcpy(serviceInitiated.initiatingDevice, extension);
								
				result = &serviceInitiated;
				break;
			case CSTATransferred:
				static tTransferredEvent transferred;
				memset(&transferred, 0x00, sizeof(tTransferredEvent));
				memcpy(&transferred, &__data.u.unsoEvent.u.transferred, sizeof(tTransferredEvent));
				key.monitorCrossRefID = transferred.monitorCrossRefID;

				hcommon::ExtractExtension(transferred.primaryOldCall.deviceID, extension);
				strcpy(transferred.primaryOldCall.deviceID, extension);
				hcommon::ExtractExtension(transferred.secondaryOldCall.deviceID, extension);
				strcpy(transferred.secondaryOldCall.deviceID, extension);
				hcommon::ExtractExtension(transferred.transferringDevice, extension);
				strcpy(transferred.transferringDevice, extension);
				hcommon::ExtractExtension(transferred.transferredToDevice, extension);
				strcpy(transferred.transferredToDevice, extension);
				
				//tConnectionIDList 추후 필요하면 추출
												
				result = &transferred;
				break;
																																		
			case CSTAAgentBusy:
				static tAgentBusyEvent agentBusy;
				memset(&agentBusy, 0x00, sizeof(tAgentBusyEvent));
				memcpy(&agentBusy, &__data.u.unsoEvent.u.agentBusy, sizeof(tAgentBusyEvent));
				key.monitorCrossRefID = agentBusy.monitorCrossRefID;

				hcommon::ExtractExtension(agentBusy.agentDevice, extension);
				strcpy(agentBusy.agentDevice, extension);
				hcommon::ExtractExtension(agentBusy.agentID, extension);
				strcpy(agentBusy.agentID, extension);
				hcommon::ExtractExtension(agentBusy.acdGroup, extension);
				strcpy(agentBusy.acdGroup, extension);
																
				result = &agentBusy;
				break;
			case CSTAAgentLoggedOff:
				static tLoggedOffEvent agentLoggedOff;
				memset(&agentLoggedOff, 0x00, sizeof(tLoggedOffEvent));
				memcpy(&agentLoggedOff, &__data.u.unsoEvent.u.loggedOff, sizeof(tLoggedOffEvent));
				key.monitorCrossRefID = agentLoggedOff.monitorCrossRefID;

				hcommon::ExtractExtension(agentLoggedOff.agentDevice, extension);
				strcpy(agentLoggedOff.agentDevice, extension);
				hcommon::ExtractExtension(agentLoggedOff.agentID, extension);
				strcpy(agentLoggedOff.agentID, extension);
				hcommon::ExtractExtension(agentLoggedOff.acdGroup, extension);
				strcpy(agentLoggedOff.acdGroup, extension);
																
				result = &agentLoggedOff;
				break;
			case CSTAAgentLoggedOn:
				static tLoggedOnEvent agentLoggedOn;
				memset(&agentLoggedOn, 0x00, sizeof(tLoggedOnEvent));
				memcpy(&agentLoggedOn, &__data.u.unsoEvent.u.loggedOn, sizeof(tLoggedOnEvent));
				key.monitorCrossRefID = agentLoggedOn.monitorCrossRefID;

				hcommon::ExtractExtension(agentLoggedOn.agentDevice, extension);
				strcpy(agentLoggedOn.agentDevice, extension);
				hcommon::ExtractExtension(agentLoggedOn.agentID, extension);
				strcpy(agentLoggedOn.agentID, extension);
				hcommon::ExtractExtension(agentLoggedOn.acdGroup, extension);
				strcpy(agentLoggedOn.acdGroup, extension);
																
				result = &agentLoggedOn;
				break;
			case CSTAAgentNotReady:
				static tNotReadyEvent agentNotReady;
				memset(&agentNotReady, 0x00, sizeof(tNotReadyEvent));
				memcpy(&agentNotReady, &__data.u.unsoEvent.u.notReady, sizeof(tNotReadyEvent));
				key.monitorCrossRefID = agentNotReady.monitorCrossRefID;

				hcommon::ExtractExtension(agentNotReady.agentDevice, extension);
				strcpy(agentNotReady.agentDevice, extension);
				hcommon::ExtractExtension(agentNotReady.agentID, extension);
				strcpy(agentNotReady.agentID, extension);
				hcommon::ExtractExtension(agentNotReady.acdGroup, extension);
				strcpy(agentNotReady.acdGroup, extension);
								
				result = &agentNotReady;
				break;
			case CSTAAgentReady:
				static tReadyEvent agentReady;
				memset(&agentReady, 0x00, sizeof(tReadyEvent));
				memcpy(&agentReady, &__data.u.unsoEvent.u.ready, sizeof(tReadyEvent));
				key.monitorCrossRefID = agentReady.monitorCrossRefID;

				hcommon::ExtractExtension(agentReady.agentDevice, extension);
				strcpy(agentReady.agentDevice, extension);
				hcommon::ExtractExtension(agentReady.agentID, extension);
				strcpy(agentReady.agentID, extension);
				hcommon::ExtractExtension(agentReady.acdGroup, extension);
				strcpy(agentReady.acdGroup, extension);
								
				result = &agentReady;
				break;
			case CSTAAgentWorkingAfterCall:
				static tNotReadyEvent agentACW;
				memset(&agentACW, 0x00, sizeof(tWorkingAfterCallEvent));
				memcpy(&agentACW, &__data.u.unsoEvent.u.workingAfterCall, sizeof(tWorkingAfterCallEvent));
				key.monitorCrossRefID = agentACW.monitorCrossRefID;

				hcommon::ExtractExtension(agentACW.agentDevice, extension);
				strcpy(agentACW.agentDevice, extension);
				hcommon::ExtractExtension(agentACW.agentID, extension);
				strcpy(agentACW.agentID, extension);
				hcommon::ExtractExtension(agentACW.acdGroup, extension);
				strcpy(agentACW.acdGroup, extension);
								
				result = &agentACW;
				break;
			case CSTAAutoAnswer:
				static tAutoAnswerEvent autoAnswer;
				memset(&autoAnswer, 0x00, sizeof(tAutoAnswerEvent));
				memcpy(&autoAnswer, &__data.u.unsoEvent.u.autoAnswer, sizeof(tAutoAnswerEvent));
				key.monitorCrossRefID = autoAnswer.monitorCrossRefID;
				
				hcommon::ExtractExtension(autoAnswer.invokingDevice, extension);
				strcpy(autoAnswer.invokingDevice, extension);

				result = &autoAnswer;
				break;
			case CSTAAutoWorkMode:
				static tAutoWorkModeEvent autoWorkMode;
				memset(&autoWorkMode, 0x00, sizeof(tAutoWorkModeEvent));
				memcpy(&autoWorkMode, &__data.u.unsoEvent.u.autoWorkMode, sizeof(tAutoWorkModeEvent));
				key.monitorCrossRefID = autoWorkMode.monitorCrossRefID;
				
				hcommon::ExtractExtension(autoWorkMode.invokingDevice, extension);
				strcpy(autoWorkMode.invokingDevice, extension);
								
				result = &autoWorkMode;
				break;
			case CSTACallBack:
				static tCallBackEvent callBack;
				memset(&callBack, 0x00, sizeof(tCallBackEvent));
				memcpy(&callBack, &__data.u.unsoEvent.u.callBack, sizeof(tCallBackEvent));
				key.monitorCrossRefID = callBack.monitorCrossRefID;

				hcommon::ExtractExtension(callBack.originatingDevice, extension);
				strcpy(callBack.originatingDevice, extension);
				hcommon::ExtractExtension(callBack.targetDevice, extension);
				strcpy(callBack.targetDevice, extension);
								
				result = &callBack;
				break;
			case CSTACallBackMessage:
				static tCallBackMessageEvent callBackMessage;
				memset(&callBackMessage, 0x00, sizeof(tCallBackMessageEvent));
				memcpy(&callBackMessage, &__data.u.unsoEvent.u.callBackMessage, sizeof(tCallBackMessageEvent));
				key.monitorCrossRefID = callBackMessage.monitorCrossRefID;

				hcommon::ExtractExtension(callBackMessage.originatingDevice, extension);
				strcpy(callBackMessage.originatingDevice, extension);
				hcommon::ExtractExtension(callBackMessage.targetDevice, extension);
				strcpy(callBackMessage.targetDevice, extension);
				
				result = &callBackMessage;
				break;
			case CSTACallerIDStatus:
				static tCallerIDStatusEvent callerIDStatus;
				memset(&callerIDStatus, 0x00, sizeof(tCallerIDStatusEvent));
				memcpy(&callerIDStatus, &__data.u.unsoEvent.u.callerIDStatus, sizeof(tCallerIDStatusEvent));
				key.monitorCrossRefID = callerIDStatus.monitorCrossRefID;
				
				hcommon::ExtractExtension(callerIDStatus.device, extension);
				strcpy(callerIDStatus.device, extension);				
				
				result = &callerIDStatus;
				break;	
			case CSTADoNotDisturb:
				static tDoNotDisturbEvent doNotDisturb;
				memset(&doNotDisturb, 0x00, sizeof(tDoNotDisturbEvent));
				memcpy(&doNotDisturb, &__data.u.unsoEvent.u.doNotDisturb, sizeof(tDoNotDisturbEvent));
				key.monitorCrossRefID = doNotDisturb.monitorCrossRefID;

				hcommon::ExtractExtension(doNotDisturb.device, extension);
				strcpy(doNotDisturb.device, extension);

				result = &doNotDisturb;
				break;																							
			case CSTAForwarding:
				static tForwardingEvent forwarding;
				memset(&forwarding, 0x00, sizeof(tForwardingEvent));
				memcpy(&forwarding, &__data.u.unsoEvent.u.forwarding, sizeof(tForwardingEvent));
				key.monitorCrossRefID = forwarding.monitorCrossRefID;
				
				hcommon::ExtractExtension(forwarding.device, extension);
				strcpy(forwarding.device, extension);
				hcommon::ExtractExtension(forwarding.forwardTo, extension);
				strcpy(forwarding.forwardTo, extension);

				result = &forwarding;
				break;
			case CSTARouteingMode:
				static tRouteingModeEvent routeingMode;
				memset(&routeingMode, 0x00, sizeof(tRouteingModeEvent));
				memcpy(&routeingMode, &__data.u.unsoEvent.u.routeingMode, sizeof(tRouteingModeEvent));
				key.monitorCrossRefID = routeingMode.monitorCrossRefID;
				
				hcommon::ExtractExtension(routeingMode.device, extension);
				strcpy(routeingMode.device, extension);
				
				result = &routeingMode;
				break;
			default:
				std::cout << "Unknown event : " << __data.event_type_detail << std::endl;
				return NULL;
				break;
		}
	}
	else if (__data.event_type == etSystem) {
		
	}

	// Read device type
	if (__pDeviceInfo != NULL && key.monitorCrossRefID > 0) {
		MONITOR_CROSS_REFID_t* device_info = __pDeviceInfo->read_data(key);
		if (device_info == NULL) 
			std::cout << "device info not found by monitorCrossRefID -> " << key.monitorCrossRefID << std::endl;
		else {
			std::cout << "device_info_out.device_type -> " << device_info->device_type << std::endl;
			std::cout << "device_info_out.extension   -> " << device_info->extension << std::endl;

			memcpy(&__device_info_out, device_info, sizeof(MONITOR_CROSS_REFID_t));
		}
	}

	return result;
}

// VDN 기준의 통계 처리가 여기서 이뤄짐(VDN, skill status report, call log(VDN기준 이벤트로 생성됨)
// => Call cleared시 call_log를 이용해 VDN,skill 통계 생성
bool event_handler::process_event(EVENTDATA_t __data, 
				event_handler_vdn *__pVDN,
				event_handler_extn *__pExtn,
				device_manage_monref_shm *__pDeviceInfo) {
	VDN_STATUS_LIST_SHMKEY_t key;
	VDN_STATUS_LIST_SHM_t    data;
	tPrivateData             private_data;
	MONITOR_CROSS_REFID_t    device_info;
	
	void *extracted_data;
	
	memset(&key, 0x00, sizeof(VDN_STATUS_LIST_SHMKEY_t));
	memset(&data, 0x00, sizeof(VDN_STATUS_LIST_SHM_t));
	memset(&private_data, 0x00, sizeof(tPrivateData));
	memset(&device_info, 0x00, sizeof(MONITOR_CROSS_REFID_t));
	
std::cout << "bEvent>>>>" << __data.event_type_detail << std::endl;
	memcpy(&private_data, &__data.privData, sizeof(tPrivateData));
	
	if (__data.event_type == etUnsolicited) // Unsolicited event
	{	
		extracted_data = extract_data(__data, __pDeviceInfo, device_info);
		if (extracted_data == NULL) {
			std::cout << "extract_data error" << std::endl;
			return false;
		}

		if (device_info.device_type == STATION) { // non-VDN
			// non-VDN event handling
			if (__pExtn->register_data((tUnsolicitedEventType)__data.event_type_detail, 
						extracted_data, private_data, __data.event_time, device_info) == false) {
				std::cout << "Register failed with non-VDN event" << std::endl;
				return false;
			}
		}
		else if (device_info.device_type == VDN) { // VDN
			// VDN event handling
			if (__pVDN->register_data((tUnsolicitedEventType)__data.event_type_detail, 
						extracted_data, private_data, __data.event_time, device_info) == false) {
				std::cout << "Register failed with VDN event" << std::endl;
				return false;
			}
		}
		else
			return true; // Fix true anyway
	}

	return true;
}
		
void *event_handler::analysisMain(void* __params){
	// message queue 에서 데이터를 읽어 분석작업을 해서 통계를 생성한다.
	// parameter 1 : Common lib object pointer
	// parameter 2 : Log directory
	// parameter 3 : Condition for threads related to logging event
	// parameter 4 : Mutex for threads related to logging event
	// parameter 5 : Mutex for condition variable cond
	TH_PARAMS_STATS *params = (TH_PARAMS_STATS *) __params;
	hcommon *pCommObj = (hcommon *)params->pCommObj;
	pthread_cond_t* cond = (pthread_cond_t*)params->cond;
	pthread_mutex_t* mut = (pthread_mutex_t*)params->mut;
	pthread_mutex_t* mut_cond = (pthread_mutex_t*)params->mut_cond;

	mqueue *pMQ_rcv = NULL;   // Message queue to read event from cms_ircv process
	manage_shared_memory shm_obj_th;
	shared_memory_t *shm_th;
	
	///////////////////// Declare report ////////////////////
	device_manage_monref_shm *pDevice_monlist_shm;  // Monitoring device list indexed by monitoringRefID
	event_handler_vdn *pVDN_event_handler;          // Handle VDN side device event
	event_handler_extn *pExtn_event_handler;        // Handle non-VDN side device event

	tracking_call *pTracking_call;                  // Call log
	
	/////////////////////////////////////////////////////////

	EVENTDATA_t data;
	LOGGINGDATA_t log_data;
	char log_dir[1024+1], current_date[8+1], message_text[4096];
	char log_filename[2048+1];
	string sCurrentDate;

	memset(log_dir, 0x00, sizeof(log_dir));
	strcpy(log_dir, params->log_dir);

	
	// Create tracking call log object is shared between event_handler_vdn and event_handler_extn
	pTracking_call = new tracking_call;
	if (pTracking_call == NULL) {
        strcpy(message_text, "Tracking call object memory allocation failed");
        pCommObj->LoggingError(__FILE__, __func__, -1, message_text);
        
        if (params != NULL) { delete params; params = NULL; }
        pthread_exit((void *) 0);
	}
	
	///////////////////// Shared memory ///////////////////////////////////////////////////
	shm_th = shm_obj_th.open_shared_memory(REPORT_AREA_SHM);
	
	///////////////////////////////////////////////////////////////////////////////////////	

	pVDN_event_handler = new event_handler_vdn(&shm_obj_th, shm_th, pCommObj);
	if (pVDN_event_handler == NULL) {
        strcpy(message_text, "Event handler for VDN object memory allocation failed");
        pCommObj->LoggingError(__FILE__, __func__, -1, message_text);

		if (params != NULL) { delete params; params = NULL; }
		if (pTracking_call != NULL) { delete pTracking_call; pTracking_call = NULL; }
		pthread_exit((void *) 0);
	}
	else {
		pVDN_event_handler->set_log_queue_objects(&pHandler->logqueue, mut, cond);
		pVDN_event_handler->set_shared_report_objects(pTracking_call);
	}
	
	pExtn_event_handler = new event_handler_extn(&shm_obj_th, shm_th, pCommObj);
	if (pExtn_event_handler == NULL) {
        strcpy(message_text, "Event handler for non-VDN object memory allocation failed");
        pCommObj->LoggingError(__FILE__, __func__, -1, message_text);

		if (params != NULL) { delete params; params = NULL; }
		if (pVDN_event_handler != NULL) { delete pVDN_event_handler; pVDN_event_handler = NULL; }
		if (pTracking_call != NULL) { delete pTracking_call; pTracking_call = NULL; }
		pthread_exit((void *) 0);
	}
	else {
		pExtn_event_handler->set_log_queue_objects(&pHandler->logqueue, mut, cond);
		pExtn_event_handler->set_shared_report_objects(pTracking_call);
	}	
	
	pDevice_monlist_shm = new device_manage_monref_shm(&shm_obj_th, shm_th);
	if (pDevice_monlist_shm == NULL) {
        strcpy(message_text, "Device monitoring info object memory allocation failed");
        pCommObj->LoggingError(__FILE__, __func__, -1, message_text);

		if (params != NULL) { delete params; params = NULL; }
		if (pVDN_event_handler != NULL) { delete pVDN_event_handler; pVDN_event_handler = NULL; }
		if (pExtn_event_handler != NULL) { delete pExtn_event_handler; pExtn_event_handler = NULL; }
		if (pTracking_call != NULL) { delete pTracking_call; pTracking_call = NULL; }			
		pthread_exit((void *) 0);
	}

	/////////////////////////////////////////////////////////////

	// Open message queue to receive event from cms_ircv
	pMQ_rcv = new mqueue;
	if (pMQ_rcv == NULL) {
        strcpy(message_text, "Message queue object memory allocation failed");
        pCommObj->LoggingError(__FILE__, __func__, -1, message_text);

		if (params != NULL) { delete params; params = NULL; }
		if (pVDN_event_handler != NULL) { delete pVDN_event_handler; pVDN_event_handler = NULL; }
		if (pExtn_event_handler != NULL) { delete pExtn_event_handler; pExtn_event_handler = NULL; }
		if (pDevice_monlist_shm != NULL) { delete pDevice_monlist_shm; pDevice_monlist_shm = NULL; }
		if (pTracking_call != NULL) { delete pTracking_call; pTracking_call = NULL; }			
		pthread_exit((void *) 0);
	}

	try {
		pMQ_rcv->open_message_queue(QUE_EVENT);
		
	}
	catch (std::string err_msg)
	{
        sprintf(message_text, "Message queue open failed with %s : [%s]", QUE_EVENT, err_msg.c_str());
        pCommObj->LoggingError(__FILE__, __func__, -1, message_text);

		if (pMQ_rcv != NULL) { delete pMQ_rcv; pMQ_rcv = NULL; }
		if (params != NULL) { delete params; params = NULL; }
		if (pVDN_event_handler != NULL) { delete pVDN_event_handler; pVDN_event_handler = NULL; }
		if (pExtn_event_handler != NULL) { delete pExtn_event_handler; pExtn_event_handler = NULL; }
		if (pDevice_monlist_shm != NULL) { delete pDevice_monlist_shm; pDevice_monlist_shm = NULL; }					
		if (pTracking_call != NULL) { delete pTracking_call; pTracking_call = NULL; }
			
		pthread_exit((void *) 0);				
	}

    sprintf(message_text, "Message queue open success with %s", QUE_EVENT);
    pCommObj->LoggingError(__FILE__, __func__, 0, message_text);
	
	
	///////////////////////////////////////////////////////////////////
	// 재기동시 현재까지의 DB 당일 누적 통계를 읽어 Shared memory에 
	// 기록한다.
	///////////////////////////////////////////////////////////////////
	
	while (1) {
		////////////// 1. message queue read //////////////////////////
		
		// Read message queue
		memset(&data, 0x00, sizeof(EVENTDATA_t));

		bool bRet = pMQ_rcv->message_recv_timed(&data, sizeof(EVENTDATA_t), WAIT_SECOND-3);
		if (bRet == false) {  // Timeout
			if (pHandler->remove_thread == 1) // exit thread
			{
				// Time wait while logger writing event & call log
				// 추후 pthread_cond_wait(logger thread로 개발시)나 
				//      다른 체크 방법(logger 별도 process로 개발시)으로 변경
				break;
			}
			continue;
		}
		
		std::cout << "Event received " << std::endl;
		
		//////// 2. analyze event to generate report //////////////////
		// event_type과 event_type_detail에 따라 작업대상이 될 함수 호출
		
		if (data.event_type == etConfirmation) // Conformation event
		{
		}
		else if (data.event_type == etUnsolicited)  // Unsolicited event
		{
			switch((tUnsolicitedEventType)data.event_type_detail)
			{
				case CSTAAgentBusy:   // Busy event시 특별한 처리 없이 넘어간다.								
					continue;
					break;
					
				case CSTAAgentLoggedOff:
				case CSTAAgentLoggedOn:
				case CSTAAgentNotReady:
				case CSTAAgentReady:
				case CSTAAgentWorkingAfterCall:
				case CSTAServiceInitiated:
				case CSTAOriginated:							
				case CSTABridged:
				case CSTACallCleared:
				case CSTAConferenced:
				case CSTAConnectionCleared:
				case CSTADelivered:
				case CSTADigitsDialled:
				case CSTADiverted:
				case CSTAEstablished:
				case CSTAFailed:
				case CSTANetworkReached:
				case CSTAOffered:
				case CSTAQueued:
				case CSTARetrieved:
				case CSTATransferred:
				case CSTAHeld:
					pHandler->process_event(data, pVDN_event_handler, pExtn_event_handler, pDevice_monlist_shm);
					break;
				default:
					break;
			}
		}
		else if (data.event_type == etSystem)  // System event
		{
		}
		
		memset(&log_data, 0x00, sizeof(LOGGINGDATA_t));
		
		log_data.data_type = 1;   // Event data
		memcpy(&log_data.u.eventData, &data, sizeof(EVENTDATA_t));
		
		//////////////////// screen out data //////////////////////////////////////
		
		std::cout << "EVENTDATA_t.event_type          : [" << data.event_type << "]" << std::endl;
		std::cout << "EVENTDATA_t.event_type_detil    : [" << data.event_type_detail << "]" << std::endl;
		std::cout << "EVENTDATA_t.event_time          : [" << data.event_time << "]" << std::endl;
		
		////////////////////// end of screen out //////////////////////////////////
		
		//////// 3. write event & call log to umap that 
		///////                   shared with queue writer thread  ////
		// mutex lock
		pthread_mutex_lock(mut);
		pHandler->logqueue.push(log_data);
		// mutex unlock
		pthread_mutex_unlock(mut);
		
		// Wake up threads to process logging event & call log
		pthread_cond_signal(cond);
	}

	if (pMQ_rcv != NULL) { delete pMQ_rcv; pMQ_rcv = NULL; }
	if (params != NULL) { delete params; params = NULL; }
	if (pVDN_event_handler != NULL) { delete pVDN_event_handler; pVDN_event_handler = NULL; }
	if (pExtn_event_handler != NULL) { delete pExtn_event_handler; pExtn_event_handler = NULL; }
	if (pDevice_monlist_shm != NULL) { delete pDevice_monlist_shm; pDevice_monlist_shm = NULL; }
	if (pTracking_call != NULL) { delete pTracking_call; pTracking_call = NULL; }
		
	pthread_exit((void *) 0);			
	
	return ((void*) 0);
};
		
// Write data to write message queue using event list
void *event_handler::loggerMain(void* __params){
	// list에서 데이터를 읽어 message queue에 등록한다.
	// cms_logger process 가 읽어서 DB에 등록처리한다.
	// parameter 1 : Common lib object pointer
	// parameter 2 : Log directory
	// parameter 3 : Condition for threads related to logging event
	// parameter 4 : Mutex for threads related to logging event
	// parameter 5 : Mutex for condition variable cond
	TH_PARAMS_LOG *params = (TH_PARAMS_LOG *) __params;
	hcommon *pCommObj = (hcommon *)params->pCommObj;
	
	pthread_cond_t* cond = (pthread_cond_t*)params->cond;
	pthread_mutex_t* mut = (pthread_mutex_t*)params->mut;
	pthread_mutex_t* mut_cond = (pthread_mutex_t*)params->mut_cond;
	
	mqueue *pMQ_send = NULL;  // Message queue to send event and call log to cms_logger process
	char log_dir[1024+1], current_date[8+1], message_text[4096];
	char log_filename[2048+1];

	
	struct timeval now;
	struct timespec ts;
	
	strcpy(log_dir, (char *)params->log_dir);
	
	// Open message queue to write event & call log to cms_logger
	pMQ_send = new mqueue;
	if (pMQ_send == NULL) {
        strcpy(message_text, "Message queue object memory allocation failed");
        pCommObj->LoggingError(__FILE__, __func__, -1, message_text);

		if (params != NULL) { delete params; params = NULL; }
		pthread_exit((void *) 0);
	}

	try {
		pMQ_send->create_message_queue(QUE_LOGGER, 300, sizeof(LOGGINGDATA_t));
	}
	catch (std::string err_msg)
	{

        sprintf(message_text, "Message queue open failed with %s : [%s]", QUE_LOGGER, err_msg.c_str());
        pCommObj->LoggingError(__FILE__, __func__, -1, message_text);

		if (pMQ_send != NULL) { delete pMQ_send; pMQ_send = NULL; }
		if (params != NULL) { delete params; params = NULL; }
		pthread_exit((void *) 0);
	}


    sprintf(message_text, "Message queue creation success with %s", QUE_LOGGER);
    pCommObj->LoggingError(__FILE__, __func__, 0, message_text);
				
	while(1) {
		LOGGINGDATA_t log_data;
		
		memset(&log_data, 0x00, sizeof(LOGGINGDATA_t));
		
		gettimeofday(&now, NULL);
		ts.tv_sec = now.tv_sec + 5;
		ts.tv_nsec = now.tv_usec * 1000;
		
//				std::cout << "Now waiting 5 seconds" << std::endl;
		int retval = pthread_cond_timedwait(cond, mut_cond, &ts);
		if (retval == ETIMEDOUT)  // Timed out
		{
//					std::cout << "Timed out" << std::endl;
		}
		else if (retval == 0) // Signal caught
		{
			std::cout << "Signal caught : memory to message queuing start" << std::endl;
			while(!pHandler->logqueue.empty()) {
				// Delete data finish queuing
				pthread_mutex_lock(mut);
				
				log_data = pHandler->logqueue.front();
				pHandler->logqueue.pop();
				
				pthread_mutex_unlock(mut);
				
if (log_data.data_type == 2) {
std::cout << __FILE__ << " log_data.u.callData.key.callID  : " << log_data.u.callData.key.callID << std::endl;
std::cout << __FILE__ << " log_data.u.callData.key.segment : " << log_data.u.callData.key.segment << std::endl;				
}
				pMQ_send->message_send(&log_data);
			}

		}

		if (pHandler->remove_thread == 1) {
			break;
		}
	}
	
	pMQ_send->remove_message_queue(QUE_LOGGER);
	
	if (pMQ_send != NULL) { delete pMQ_send; pMQ_send = NULL; }
	
	pthread_exit((void *) 0);
	return ((void*) 0);
};		

void *event_handler::archiverMain(void* __params){
	// 생성된 통계 데이터를 DB에 등록한다.
	// parameter 1 : Common lib object pointer
	// parameter 2 : Mutex for threads archiver
	// parameter 3 : Mutex for condition variable cond
	// parameter 4 : Log directory
	TH_PARAMS_ARCH *params = (TH_PARAMS_ARCH *) __params;
	hcommon *pCommObj = (hcommon *)params->pCommObj;			
	pthread_cond_t* cond = (pthread_cond_t*)params->cond;
	pthread_mutex_t* mut_cond = (pthread_mutex_t*)params->mut_cond;			

	hdatetime dttm;
	
	char log_dir[1024];
	char log_filename[2048+1];
	string sCurrentDate;
	time_t tCurrentTime;
	
	string sWorkDate, sWorkStartTime;
	
	
	
	struct timeval now;
	struct timespec ts;
	
	strcpy(log_dir, params->log_dir);

	while(1) {
		// log_filename 즉, m_LogFile의 값을 공유하도록 처리(타 thread에서는 아래 로직 제거하자
		// Archiving시 m_LogFile update처리(일자별 로그 생성 위함)
		// Multi-thread 하에서 문제 없이 구동되면 타 thread parameter중 log_dir은 제거한다.
		pCommObj->SetSystemLogFileName(LogDir, Log_prefix);
/*		
		sCurrentDate = pCommObj->get_systemtime(TimeOption::YYYYMMDD);
		sprintf(log_filename, "%s/%s/%s.log", log_dir,
		                      sCurrentDate.c_str(), sAppName);
		pCommObj->m_LogFile = log_filename;
*/
//   Archiving scheduling logic 개발 필요
		// 대기할 시간(초)
		// Parameter : 통계 interval(분)
		long nWaittime = dttm.get_batchwaitingtime(INTERVAL);
		
		gettimeofday(&now, NULL);
		ts.tv_sec = now.tv_sec + nWaittime;
		ts.tv_nsec = now.tv_usec * 1000;
		
		std::cout << "Arch Now waiting " << nWaittime << "s seconds" << std::endl;
		int retval = pthread_cond_timedwait(cond, mut_cond, &ts);
		//int retval = pthread_cond_timedwait(&pHandler->arch_cond, &pHandler->arch_cond_mutex, &ts);
		if (retval == ETIMEDOUT)  // Timed out
			std::cout << "Arch Timed out" << std::endl;
		else if (retval == 0) // Signal caught : Exit thread
		{
			std::cout << "Signal caught : exit archiver thread" << std::endl;
				
			break;
				
//					if (pHandler->remove_thread == 1) // exit thread
//					{
//						break;
//					}
		}
		else
		{
			std::cout << "in arch : exit archiver thread" << std::endl;						
		}
		

		
		/////////////////////// Archiving start /////////////////////////////////////
		//하루의 최종 StartTime(hh24mi) -> ex) 2345
		string szLastStartTime = dttm.get_lastintervalstarttime(INTERVAL);
		string szLastArchived;  // 최종 archiving된 일자+시작시간(YYYYMMDDHH24MI)
		
		time(&tCurrentTime);
		string szWorkDate = dttm.get_systemtime(TimeOption::YYYYMMDD);
		string szWorkStartTime = dttm.get_currentintervalstarttime(INTERVAL, tCurrentTime);				

		//최종 archive 실행 시간대가 현 시간대 기준 작업시각과 동일하면 다시 대기한다.
		if (szLastArchived == (szWorkDate + szWorkStartTime))
		{
			continue;
		}				
		
		
		
		
		
		// Write last archived timestamp after all archiving process done
		szLastArchived = szWorkDate + szWorkStartTime;
		
		/////////////////////// Archiving done /////////////////////////////////////
	}
	
	pthread_exit((void *) 0);
	return ((void*) 0);
};

void event_handler::WaitForJoiningMain() {
	int nRet, status;
	
	pthread_mutex_lock(&rm_mutex);
	remove_thread = 1;       // To use end thread gracefully.
	pthread_mutex_unlock(&rm_mutex);
	
	
	// Wake up archiver threads to exit from thread
	nRet = pthread_cond_signal(&arch_cond);

	
	sleep(WAIT_JOINING);
	
	
	nRet = pthread_join(th_stats, (void **)&status);
	if (status == 0) {
		std::cout << "Completed join with event handler thread:" << th_stats 
			<< " => status=" << status << std::endl;
	}
	else {
		std::cout << "ERROR return(" << nRet << ") join with thread:" << th_stats 
			<< " => status=" << status << std::endl;
	}

	nRet = pthread_join(th_log, (void **)&status);
	if (status == 0) {
		std::cout << "Completed join with event logger thread:" << th_stats 
			<< " => status=" << status << std::endl;
	}
	else {
		std::cout << "ERROR return(" << nRet << ") join with thread:" << th_stats 
			<< " => status=" << status << std::endl;
	}

	nRet = pthread_join(th_arch, (void **)&status);
	if (status == 0) {
		std::cout << "Completed join with archiver thread:" << th_arch 
			<< " => status=" << status << std::endl;
	}
	else {
		std::cout << "ERROR return(" << nRet << ") join with thread:" << th_arch 
			<< " => status=" << status << std::endl;
	}
};



event_handler* event_handler::pHandler = 0;

int main()
{
	int pid, chpid;
	event_handler evth;

	if ((pid = fork()) < 0) {
		perror("fork() error:");
		exit(0);
	}
	else if (pid != 0) {
		perror("Parent process end:");
		exit(0);
	}
	
	setsid();
	
	signal(SIGPIPE, SIG_IGN);
	Signal(SIGTERM, (void *)event_handler::FuncSIGTERM);

	evth.do_report();
}
