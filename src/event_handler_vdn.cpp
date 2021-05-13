#include "event_handler_vdn.hpp"
#include "ipc_def.hpp"


event_handler_vdn::~event_handler_vdn() {
	if (pcall_list != NULL) delete pcall_list;
	if (pstatus != NULL) delete pstatus;
}

event_handler_vdn::event_handler_vdn(manage_shared_memory *__shmobj
			, shared_memory_t *__shm, hcommon *__commobj) : event_handler_base(__shmobj, __shm, __commobj)
{
	if ( (pstatus = new vdn_status_shm(__shmobj, __shm, __commobj)) == NULL) {
		std::cerr << "Failed to allocate vdn_status_shm memory" << std::endl;
	}
	
	if ( (pcall_list = new vdn_call_list_shm(__shmobj, __shm)) == NULL) {
		std::cerr << "Failed to allocate vdn_call_list memory" << std::endl;
	}
}

// Event가 발생 시킨 device와 일치하는지 확인해서 일치할 경우에만 처리
// 향후 device가 일치하지 않는 이벤트는 false로 반환해서 호출자(cms_anl)에서
// message queue 에 전송하기 위한 queueing작업을 하지 않도록 변경한다.
bool event_handler_vdn::register_data(tUnsolicitedEventType __event, void* __event_data
			, tPrivateData __private_data, time_t __event_time
			, MONITOR_CROSS_REFID_t __monitored_dev_info)
{
	VDN_STATUS_LIST_SHMKEY_t key_status;
	VDN_STATUS_LIST_SHM_t    data_status;					
	VDN_CALL_LIST_SHMKEY_t key_call_list;
	VDN_CALL_LIST_SHM_t    data_call_list;
	TRACKING_CALL_KEY_t key_call_log;
	TRACKING_CALL_t     data_call_log, data_current_call_log, data_first_call_log;
	VDN_CALL_LIST_SHM_t *pdata;
	
	AGENT_STATUS_LIST_SHMKEY_t key_agent_status;
	AGENT_STATUS_LIST_SHM_t*   pdata_agent_status;
	
	bool bMakeCallLog = false;
	bool bNewCallLog = false;
	bool bIsDataFound = false;

	char extension[EXTENSION_LEN];
	char agentID[EXTENSION_LEN];
	short segment;
	
	
	memset(&key_status, 0x00, sizeof(VDN_STATUS_LIST_SHMKEY_t));
	memset(&data_status, 0x00, sizeof(VDN_STATUS_LIST_SHM_t));
	memset(&key_call_list, 0x00, sizeof(VDN_CALL_LIST_SHMKEY_t));
	memset(&data_call_list, 0x00, sizeof(VDN_CALL_LIST_SHM_t));
	memset(&key_call_log, 0x00, sizeof(TRACKING_CALL_KEY_t));
	memset(&data_call_log, 0x00, sizeof(TRACKING_CALL_t));
	memset(&data_current_call_log, 0x00, sizeof(TRACKING_CALL_t));
	memset(&key_agent_status, 0x00, sizeof(AGENT_STATUS_LIST_SHMKEY_t));
	
	switch(__event) {
		case CSTADelivered:
			// 가능 Cause
			// 상담연결 VDN ; newcall, enter distribution, distributed, ACD forward, No available agents
			// non-상담연결 VDN ; redirected, call forward 
			
			tDeliveredEvent *delivered;
			delivered = (tDeliveredEvent *) __event_data;
						
			// VDN인입시의 delivered와 VDN에서 Agent Device로 forwarding하는 delivered event가 있다
			// 두 case를 비교해서 이중 처리필요(cause로 분류)
			
			key_call_list.callID = delivered->connection.callID;
			
			key_call_log.c.callID = delivered->connection.callID;
			
			
			bMakeCallLog = pcall_tracking->decide_make_call_log(delivered->connection.callID
									, __monitored_dev_info.device_type, __monitored_dev_info.extension
				                    , delivered->callingDevice, delivered->alertingDevice
				                    , data_current_call_log, bIsDataFound);
				                    
			if (bMakeCallLog == true) {
				data_call_log.event_handle = VDN;
std::cout << "bMakeCallLog = true" << std::endl;
				if (bIsDataFound == false)
					bNewCallLog = true;
				else {
					// NewCall이거나 다른 device에서 넘어온 cause는 모두 call log 추가
					if ( (delivered->cause == ecNewCall) ||
						 (delivered->cause == ecRedirected) ||
						 (delivered->cause == ecSingleStepTransfer) ||
						 (delivered->cause == ecSingleStepConference) ) 
					{
	///CAUSE 반영시까지만 존속하는 코드					
	// 원래 무조건 위의 4개 cause코드는 bNewCallLog = true이다.
						if (data_current_call_log.event_handle == STATION) {
							// VDN으로 진입했으므로 기존 station에서 잡은 ringtime을 닫는다.
							pcall_tracking->set_column_switch(RINGTIME, OFF, data_call_log, __event_time);
						}
	
						if ( (data_current_call_log.event_handle == STATION) &&
						   (data_current_call_log.last_callcontrol_event == CSTADelivered) )
							bNewCallLog = false;
						else
							bNewCallLog = true;
					}
				}
			}
			else 
				std::cout << "bMakeCallLog = false" << std::endl;
			
			// 기처리된 상황이면 패스 중복제거
			if ( (data_current_call_log.event_handle == VDN) &&
				 (data_current_call_log.last_callcontrol_event == CSTADelivered) )
				return true;
			
			// 추후 IS-CM쪽에서 cause 제대로 잡히면 아래 로직 수정
			if (delivered->cause == ecNewCall) {

				// Event가 발생 시킨 device와 일치하는지 확인
//				if (strcmp(delivered->alertingDevice, __monitored_dev_info.extension) != 0)
//					return false;
									
std::cout << "Delivered VDN NewCall " << std::endl;
				//////// VDN call list ////////////////////////////////////////////				
				strcpy(data_call_list.vdn, delivered->alertingDevice);
				strcpy(data_call_list.calling_device, delivered->callingDevice);
				
				data_call_list.tentry_time = __event_time;
				data_call_list.tlast_event_time = __event_time;
				
				//////// VDN status ///////////////////////////////////////////////
				// vector 설정에 의해 redirected되어져 들어온 콜도 있을 수 있기 때문에 
				// alertingDevice를 사용 해야 한다.
				strcpy(key_status.vdn, delivered->alertingDevice);
								
				data_status.inprogress = 1;
				
				//////// Tracking call log ////////////////////////////////////////
//				// VDN 측면 delivered event 가 수신되면 기존 call log 존재 여부 검색 후 newcall 처리 여부 결정
//				key_call_info.callID = delivered->connection.callID;				
//				memset(&data_call_info, 0x00, sizeof(CALL_PROCESS_INFO_t));
//				if (pcall_tracking->get_call_info(key_call_info, data_call_info) == false) {
//					bNewCallLog = true;
//					std::cout << "not found call_info " << std::endl;
//				}
//				else {
//					std::cout << "found call_info callID => " << data_call_info.device_type << std::endl;
//						bNewCallLog = false;
//					if (data_call_info.device_type == TRACKING_CALL::EXTENSION) {
//						data_call_info.device_type = TRACKING_CALL::VDN;
//	
//						pcall_tracking->set_call_info(key_call_info, data_call_info);
//					}
//				}
	
				// Domain 명 event전문에서 pumping해서 올리도록 하자
				strcpy(data_call_log.c.domain, __monitored_dev_info.domain);
				data_call_log.c.tenant = 1;   // Value fix to 1 currently
				strcpy(data_call_log.c.calling_pty, delivered->callingDevice);
				strcpy(data_call_log.c.dialed_num, delivered->calledDevice);
				strcpy(data_call_log.c.dispvdn, delivered->connection.deviceID);
				//origlogin callingDevice가 Agent가 아니면 null 세팅
				if (pagent_status_rpt->get_agentID(data_call_log.c.calling_pty, agentID) == true) {
					strcpy(data_call_log.c.origlogin, agentID);
					std::cout << "DeliveredEvent. origlogin : " << agentID << std::endl;
				}
				
				strcpy(data_call_log.c.ucid,  __private_data.UCID);
				data_call_log.first_releasing = TRACKING_CALL::NONE;
	
				pcall_tracking->set_column_switch(SEGMENT, ON, data_call_log, __event_time);
											
				pcall_tracking->set_column_switch(DISPTIME, ON, data_call_log, __event_time);											
///SW				data_call_log.c.segstart = __event_time;		// Init time
///SW				data_call_log.c.disptime = __event_time;		// Init time
			}
			else if (delivered->cause == ecCallForward) {
				// VDN -> 상담원 연결시 - 수정필요
std::cout << "Delivered VDN CallForward " << std::endl;
								
				//////// VDN call list ////////////////////////////////////////////
				strcpy(data_call_list.vdn, NOUPDATE);
				strcpy(data_call_list.calling_device, NOUPDATE);
				strcpy(data_call_list.skill, NOUPDATE);
				strcpy(data_call_list.agent_device, delivered->alertingDevice);
				data_call_list.tentry_time = 0; // No update
				
				//////// VDN status ///////////////////////////////////////////////
				strcpy(key_status.vdn, delivered->calledDevice);
								
				data_status.inring = 1;
				data_status.inqueue = -1;
				
				//////// Tracking Call log ///////////////////
				if (pagent_status_rpt->get_agentID(delivered->alertingDevice, agentID) == true) {
					strcpy(data_call_log.c.anslogin, agentID);
				}				
				
				pcall_tracking->set_column_switch(QUEUETIME, OFF, data_call_log, __event_time);
													
				pcall_tracking->set_column_switch(RINGTIME, ON, data_call_log, __event_time);
///SW				data_call_log.c.queuetime = __event_time;
///SW				data_call_log.c.ringtime = __event_time;   // Init time

			}		
			
			// 이하 cause에 따른 처리는 newCall에 준하여 처리되어야 함
			// VDN간의 callforward 및 Transfer 테스트 해야함
			// 타 VDN에서 Transfered 되어 인입되는 DevliveredEvent
//			else if (delivered.cause == ecTransfer) {
//			}
			// 타 VDN에서 Redirected되어 인입되는 DeliveredEvent
//			else if (delivered.cause == ecRedirected) {
//			}
			
			break;
			
		case CSTAQueued:
			tQueuedEvent *queued;
			queued = (tQueuedEvent *) __event_data;
			
			// Event가 발생 시킨 device와 일치하는지 확인
//			if (strcmp(queued->calledDevice, __monitored_dev_info.extension) != 0)
//				return false;
			
			//////// VDN call list ////////////////////////////////////////////
			key_call_list.callID = queued->queuedConnection.callID;
			
			strcpy(data_call_list.vdn, NOUPDATE);
			strcpy(data_call_list.calling_device, NOUPDATE);
			strcpy(data_call_list.skill, queued->queuedConnection.deviceID);
			
			data_call_list.tentry_time = 0; // No update
			data_call_list.tlast_event_time = __event_time;
			
			//////// VDN status ///////////////////////////////////////////////
			strcpy(key_status.vdn, queued->calledDevice);
			
			data_status.inqueue = 1;

			//////// Tracking call log ////////////////////////////////////////
			key_call_log.c.callID = queued->queuedConnection.callID;
			strcpy(data_call_log.c.dispsplit, queued->queuedConnection.deviceID);
			
			pcall_tracking->set_column_switch(QUEUETIME, ON, data_call_log, __event_time);

			break;
						
		case CSTAEstablished:  
			tEstablishedEvent *established;
			established = (tEstablishedEvent *) __event_data;
			
			// Event가 발생 시킨 device와 일치하는지 확인 -> // 중복event 없음
//			if (strcmp(established->calledDevice, __monitored_dev_info.extension) != 0)
//				return false;
			
			//////// VDN call list ////////////////////////////////////////////
			key_call_list.callID = established->establishedConnection.callID;
			
			strcpy(data_call_list.vdn, NOUPDATE);
			strcpy(data_call_list.calling_device, NOUPDATE);
			// skill은 데이터가 QueuedEvent에서만 catch되니 변경하지 않는다.
			strcpy(data_call_list.skill, NOUPDATE);
			strcpy(data_call_list.agent_device, established->answeringDevice);
			
			data_call_list.tentry_time = 0; // No update
			
			//////// VDN status ///////////////////////////////////////////////
			strcpy(key_status.vdn, established->calledDevice);
						
			data_status.atagent = 1;
			data_status.inring = -1;
			
			//////// Tracking call log ////////////////////////////////////////
			key_call_log.c.callID = established->establishedConnection.callID;
			

			// answeringDevice가 상담원의 device인지 check해서 true면 call_disp = answered
			// 그렇지 않으면 call_disp = connected
			if (pagent_status_rpt->get_agentID(established->answeringDevice, agentID) == true)
				data_call_log.c.call_disp = TRACKING_CALL::ANSWERED;
			else
				data_call_log.c.call_disp = TRACKING_CALL::CONNECTED;
					
			strcpy(data_call_log.c.anslogin, agentID);
			
			pcall_tracking->set_column_switch(RINGTIME, OFF, data_call_log, __event_time);
										
			pcall_tracking->set_column_switch(DISPTIME, OFF, data_call_log, __event_time);
										
			pcall_tracking->set_column_switch(TALKTIME, ON, data_call_log, __event_time);

			break;
					
		case CSTAConnectionCleared:
			tConnectionClearedEvent *connCleared;
			connCleared = (tConnectionClearedEvent *) __event_data;
			
			if (pcall_tracking->read_current_call_log(connCleared->droppedConnection.callID,
							data_current_call_log) == false) {
				std::cout << "read_current_call_log error with callID : " << 
								connCleared->droppedConnection.callID << std::endl;
				return false;
			}
			
			// 기처리된 상황이면 패스 중복제거
			if (data_current_call_log.last_callcontrol_event == CSTAConnectionCleared)
				return true;		
			
			// 수신측면 device event만 이용(중복제거)
//			if (strcmp(connCleared->calledDevice, __monitored_dev_info.extension) != 0)
//				return false;
			
			//////// VDN call list ////////////////////////////////////////////
			// 삭제 처리
			key_call_list.callID = connCleared->droppedConnection.callID;
			
			// call list 데이터 check후 없거나 event처리여부가 true로 확인되면 후속 작업 패스
			// VDN은 관련된 device 숫자만큼 이벤트가 발생해서 최초 한건에 대해서만 처리하기 위함
			// -> ConnCleared event 처리여부를 marking해서 bit로 관리 최초 한번에 대해서만 처리 후
			//    CallCleared event 에서 delete 처리
			if (pcall_list->check_exists(key_call_list) == false)
				return true;
			
			//////// VDN status  //////////////////////////////////////////////
			//tConnectionClearedEvent 구조체에 VDN이 없어서 파라메터로 넘겨진 값 사용
			strcpy(key_status.vdn, __monitored_dev_info.extension);

			data_status.inprogress = -1;
			// Established event 이후의 이벤트이면 atagent 수치감소
			if (data_current_call_log.last_callcontrol_event == CSTAEstablished) {
				data_status.atagent = -1;
			}
			else if (data_current_call_log.last_callcontrol_event == CSTAQueued) {
					data_status.inqueue = -1;
			}
			else if (data_current_call_log.last_callcontrol_event == CSTADelivered) {
				if (strlen(data_current_call_log.c.anslogin) > 0)
					data_status.inring = -1;
			}
						
			//////// Tracking call log ////////////////////////////////////////
			key_call_log.c.callID = connCleared->droppedConnection.callID;
	
			if (data_current_call_log.c.call_disp == TRACKING_CALL::NIL)
				data_call_log.c.call_disp = TRACKING_CALL::ABANDONED;

			if (data_current_call_log.first_releasing == TRACKING_CALL::NONE) {
				set_agt_released(connCleared->droppedConnection.callID, connCleared->releasingDevice
									, data_current_call_log, data_call_log);
			}
					
			// Hold 관련 call log 항목은 non-VDN device측면의 event분석

			// Queuing상태에서 발생된 이벤트인지
			if (data_current_call_log.last_callcontrol_event == CSTAQueued) {
				pcall_tracking->set_column_switch(QUEUETIME, OFF, data_call_log, __event_time);
				pcall_tracking->set_column_switch(DISPTIME, OFF, data_call_log, __event_time);
			}
			// Delivered 상태의 이벤트인지
			else if (data_current_call_log.last_callcontrol_event == CSTADelivered) {
				// 상담원 Device로의 Delivered(Ring) 상태 이후 인지 판단
				if (strlen(data_current_call_log.c.dispsplit) > 0) {
					pcall_tracking->set_column_switch(RINGTIME, OFF, data_call_log, __event_time);
					pcall_tracking->set_column_switch(DISPTIME, OFF, data_call_log, __event_time);
				}
				// VDN 으로의 초기진입후의 이벤트인지 판단(dispsplit check -> 추후)
				else {
					pcall_tracking->set_column_switch(DISPTIME, OFF, data_call_log, __event_time);	
				}
			}
			else // 연결이후의 이벤트로 간주(Established 이후인지 판단)
			{
				pcall_tracking->set_column_switch(TALKTIME, OFF, data_call_log, __event_time);
			}

			pcall_tracking->set_column_switch(SEGMENT, OFF, data_call_log, __event_time);

			break;
			
		case CSTAConferenced:  // 테스트 전
			tConferencedEvent *conferenced;
			conferenced = (tConferencedEvent *) __event_data;
			
			if (strcmp(conferenced->conferencingDevice, __monitored_dev_info.extension) != 0)
				return false;			
			
			//////// VDN call list ////////////////////////////////////////////
			key_call_list.callID = conferenced->primaryOldCall.callID;
			
			//////// VDN status  //////////////////////////////////////////////
			// 별도의 처리를 하지 않는다
			
			//////// Tracking call log ////////////////////////////////////////
			key_call_log.c.callID = conferenced->primaryOldCall.callID;
			
			if (pcall_tracking->read_current_call_log(conferenced->primaryOldCall.callID,
							data_current_call_log) == false) {
				std::cout << "read_current_call_log error with callID : " << 
								conferenced->primaryOldCall.callID << std::endl;
				return false;
			}
		
//			if (data_current_call_log.last_callcontrol_sub_event == CSTAHeld) {
//				data_call_log.c.ansholdtime = __event_time - data_current_call_log.last_callcontrol_sub_event_time;
//				// Sub event 초기화
//				data_call_log.last_callcontrol_sub_event_time = 0;
//				data_call_log.last_callcontrol_sub_event = CSTAUnsolUndefined;
//			}
			
			break;			
			
		case CSTADiverted:  // 테스트 전
			tDivertedEvent *diverted;
			diverted = (tDivertedEvent *) __event_data;

			if (strcmp(diverted->divertingDevice, __monitored_dev_info.extension) != 0)
				return false;
							
			//////// VDN call list ////////////////////////////////////////////
			// 삭제처리
			key_call_list.callID = diverted->connection.callID;	
			
			//////// VDN status  //////////////////////////////////////////////
			strcpy(key_status.vdn, diverted->divertingDevice);
			data_status.inprogress = -1;
			
			// Read current call list to define call progress state
			pdata = pcall_list->read_data(key_call_list);
			if (pdata == NULL) {	// Call already established to agent device
				data_status.atagent = -1;
			}
			else {
				if (strlen(pdata->agent_device) > 0) // Call ringing agent device
					data_status.inring = -1;
				else if (strlen(pdata->skill) > 0)   // Call queued
					data_status.inqueue = -1;
				else {
					std::cerr << "Unknown state" << std::endl;
				}
			}
			
			//////// Tracking call log ////////////////////////////////////////
			key_call_log.c.callID = diverted->connection.callID;
			
			pcall_tracking->set_column_switch(SEGMENT, OFF, data_call_log, __event_time);
										
			pcall_tracking->set_column_switch(DISPTIME, OFF, data_call_log, __event_time);								
			
///SW			data_call_log.c.segstop = __event_time;
///SW			data_call_log.c.disptime = __event_time;

			break;
			
		case CSTATransferred:   // 테스트 전
			tTransferredEvent *transferred;
			transferred = (tTransferredEvent *) __event_data;
			
			if (strcmp(transferred->transferringDevice, __monitored_dev_info.extension) != 0)
				return false;			
			//////// VDN call list ////////////////////////////////////////////
			// 삭제처리
			key_call_list.callID = transferred->primaryOldCall.callID;
			
			//////// VDN status  //////////////////////////////////////////////
			//tConnectionClearedEvent 구조체에 VDN이 없어서 파라메터로 넘겨진 값 사용
			strcpy(key_status.vdn, __monitored_dev_info.extension);
									
			data_status.inprogress = -1;
			// Established된 상태에서 호전환되므로 atagent를 감한다.
			data_status.atagent = -1;

			//////// Tracking call log ////////////////////////////////////////
			key_call_log.c.callID = transferred->primaryOldCall.callID;
			
			if (pcall_tracking->read_current_call_log(transferred->primaryOldCall.callID,
							data_current_call_log) == false) {
				std::cout << "read_current_call_log error with callID : " << 
								transferred->primaryOldCall.callID << std::endl;
				return false;
			}			
			
			if (data_current_call_log.last_callcontrol_sub_event == CSTAHeld) {
				data_call_log.c.ansholdtime = __event_time - data_current_call_log.last_callcontrol_sub_event_time;
				
				pcall_tracking->set_column_switch(HELD, OFF, data_call_log, __event_time);
				// Sub event 초기화
///SW				data_call_log.last_callcontrol_sub_event_time = 0;
///SW				data_call_log.last_callcontrol_sub_event = CSTAUnsolUndefined;
			}
			
			pcall_tracking->set_column_switch(SEGMENT, OFF, data_call_log, __event_time);			
			
///SW			data_call_log.c.segstop = __event_time;	

			break;
			
		// 테스트 전
		// Held와 Retrieved는 Holding의 주체 device에서 처리. 
		// 즉, non-VDN device이므로 event_handler_extn에서 처리함
		case CSTAHeld:
		case CSTARetrieved:
			return true;
			break;
						
		case CSTAServiceInitiated: // Pass
			return true;
			break;
			
		case CSTAOffered:
		case CSTADigitsDialled:
		case CSTAFailed:
			// 추후 구현되면 event상황 종합해서 사용여부 결정
			return true;
						
		case CSTAOriginated:   // VDN으로 Dial했을시의 event-> 발생하면 안됨
			return true;
			break;		

		case CSTACallCleared:
			tCallClearedEvent *callCleared;
			callCleared = (tCallClearedEvent *) __event_data;
			
			//////// Tracking call log ////////////////////////////////////////
			key_call_log.c.callID = callCleared->clearedCall.callID;
			
			
			// 기처리된 Event 인지 체크 (Event 중복처리 방지)
			key_call_log.c.segment = 1;
			if (pcall_tracking->check_exists(key_call_log) == false)
				return true;			
			
			std::cout << "clearedCall.callID : " << callCleared->clearedCall.callID << std::endl;
			
			break;
						
		default:
			std::cerr << "VDN status processing ; Unknown event!!!" << std::endl;
			return false;
			break;
	}
	
	data_call_list.tlast_event_time = __event_time;  // Last event occurred time
	data_call_log.last_callcontrol_event = __event;  // Last received event
	data_call_log.last_callcontrol_event_time = __event_time;
	
	
	// When ConnectionClearedEvent event received, send call log data to logger process via message queue(QUE_LOGGER)
	// then delete data from call log list.
	// Event will change to CSTACallCleared, when the event develop on IS-CM later on.
	if (__event == CSTACallCleared) {
		if (complete_call_log(key_call_log.c.callID) == true) {
			// Create historical data using call log(hvdn, hsplit, hagent)
				// call_report->create_callreport
				//              callreport_agent, callreport_vdn, callreport_skill
			// Delete call log(move to event_handler_base)
//			if (pcall_tracking->delete_data(key_call_log.c.callID) == false) {
//				std::cerr << "pcall_tracking->delete_data failed!!!" << std::endl;
//				return false;
//			}
		}
	}	
	else {
		// End of segment; delete vdn call list
		if ( (__event == CSTAConnectionCleared) ||
			 (__event == CSTATransferred) ||
		     (__event == CSTADiverted) ) 
		{
	std::cout << "key_call_list.callID -> " << key_call_list.callID << std::endl;		
			if (pcall_list->delete_report(key_call_list) == false) {
				std::cerr << "VDN call list delete failed!!!" << std::endl;
				return false;
			}
			
			std::cout << "after delete_report vdn_call_list , size : " << pcall_list->size() << std::endl;
		} 
		else {
			// Insert call list
			if (key_call_list.callID > 0) {
				std::cout << "key_call_list.callID=>[" << key_call_list.callID << "]" << std::endl;
				if (pcall_list->register_report(key_call_list, data_call_list, DUP_OPTION::UPDATE) == false) {
					std::cerr << "VDN call list insert/update failed!!!" << std::endl;
					return false;
				}
				std::cout << "after register_report vdn_call_list , size : " << pcall_list->size() << std::endl;
			}
		}
	std::cout << "__newcall : " << bNewCallLog << std::endl;
		// Call tracking log
		if (pcall_tracking->register_data(key_call_log, data_call_log, bNewCallLog) == false) {
			std::cerr << "Call tracking insert/update failed!!!" << std::endl;
			return false;
		}
	std::cout << "call log count   : " << pcall_tracking->size() << std::endl;
		
		if (strlen(key_status.vdn) > 0) {
			pstatus->register_report(key_status, data_status, DUP_OPTION::ACCUMULATE);
			std::cout << "vdn status count : " << pstatus->size() << std::endl;
		}
	}
	
	// VDN[Skill] status -> Skill status를 따로 처리할지 VDN status 처리와 동시에 생성할지 고민
	return true;
}
