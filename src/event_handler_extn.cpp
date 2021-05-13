#include "event_handler_extn.hpp"
#include "ipc_def.hpp"


event_handler_extn::~event_handler_extn() {
}


// Handle event agent statuts change
// Handle event non-VDN thru call like direct ACD call, extension call
// Handle event agent device make call like outbound call and extension call
event_handler_extn::event_handler_extn(manage_shared_memory *__shmobj
			, shared_memory_t *__shm, hcommon *__commobj) : event_handler_base(__shmobj, __shm, __commobj)
{
}

// Event가 발생 시킨 device와 일치하는지 확인해서 일치할 경우에만 처리
// 향후 device가 일치하지 않는 이벤트는 false로 반환해서 호출자(cms_anl)에서
// message queue 에 전송하기 위한 queueing작업을 하지 않도록 변경한다.
// Agent 통계 생성(상담원 상태시간등의 시간값....). 그외의 콜 관련 통계는 call log를 기준으로 생성한다.
bool event_handler_extn::register_data(tUnsolicitedEventType __event, void* __event_data
			, tPrivateData __private_data, time_t __event_time
			, MONITOR_CROSS_REFID_t __monitored_dev_info)
{
	AGENT_STATUS_LIST_SHMKEY_t key_status;
	AGENT_STATUS_LIST_SHM_t    data_status;
	AGENT_STATUS_LIST_SHM_t*   pdata_agent_status;
	TRACKING_CALL_KEY_t key_call_log;
	TRACKING_CALL_t     data_call_log, data_current_call_log, data_first_call_log;
//	CALL_PROCESS_INFO_KEY_t key_call_info;
//	CALL_PROCESS_INFO_t     data_call_info;
	
	
	AGENT_STATUS_LIST_SHM_t*   pData_staus;
	
	bool bNewCallLog = false;
	bool bMakeCallLog = false;   // Call tracking log 생성할지 여부
	bool bComplete_calllog = false; // Call log 마감처리 여부
	bool bIsDataFound = false;   // Current call log data found 여부
	bool bIsAgentID;

	tDeviceID extension;
	tAgentID agentID;
	short segment;		
	
	memset(&key_status, 0x00, sizeof(AGENT_STATUS_LIST_SHMKEY_t));
	memset(&data_status, 0x00, sizeof(AGENT_STATUS_LIST_SHM_t));
	memset(&key_call_log, 0x00, sizeof(TRACKING_CALL_KEY_t));
	memset(&data_call_log, 0x00, sizeof(TRACKING_CALL_t));
	memset(&data_current_call_log, 0x00, sizeof(TRACKING_CALL_t));
	memset(&data_first_call_log, 0x00, sizeof(TRACKING_CALL_t));
	
//	memset(&key_call_info, 0x00, sizeof(CALL_PROCESS_INFO_KEY_t));
//	memset(&data_call_info, 0x00, sizeof(CALL_PROCESS_INFO_t));
	
	switch(__event) {
		case CSTAAgentBusy:  // 일단 처리하지 않는다.(추후 활용도가 있으면 처리) 
			return true;

		case CSTAAgentLoggedOn:
			tLoggedOnEvent *agentLoggedOn;
			agentLoggedOn = (tLoggedOnEvent *)__event_data;

			strcpy(key_status.agentID, agentLoggedOn->agentID);
			strcpy(data_status.extension, agentLoggedOn->agentDevice);
			strcpy(data_status.workmode, STATUS_AUX);
			strcpy(data_status.auxreason, "");
			strcpy(data_status.direction, "");
			strcpy(data_status.agstate, AGSTATE_AUX);
			strcpy(data_status.workskill, "");
			break;
			
		case CSTAAgentLoggedOff:
			tLoggedOffEvent *agentLoggedOff;
			agentLoggedOff = (tLoggedOffEvent *)__event_data;
			
			// cause 가 ecNormal밖에 오지 않는다
			strcpy(key_status.agentID, agentLoggedOff->agentID);
			strcpy(data_status.extension, "");
			strcpy(data_status.workmode, "");
			strcpy(data_status.auxreason, "");
			strcpy(data_status.direction, "");
			strcpy(data_status.agstate, AGSTATE_UNSTAFFED);
			strcpy(data_status.workskill, "");
			break;
			
		case CSTAAgentNotReady:
			tNotReadyEvent *agentNotReady;
			agentNotReady = (tNotReadyEvent *)__event_data;
			
			// cause 체크 하지 않고 무조건 통계 처리
			strcpy(key_status.agentID, agentNotReady->agentID);
			strcpy(data_status.extension, agentNotReady->agentDevice);
			strcpy(data_status.workmode, STATUS_AUX);
			strcpy(data_status.auxreason, __private_data.reasonCode);
			strcpy(data_status.direction, "");
			strcpy(data_status.agstate, AGSTATE_AUX);
			strcpy(data_status.workskill, "");
			break;
			
		case CSTAAgentReady:
			tReadyEvent *agentReady;
			agentReady = (tReadyEvent *)__event_data;

			// cause 체크 하지 않고 무조건 통계 처리
			// cause ; Normal(ecNormal) or Autowork(ecAutoWork)
			strcpy(key_status.agentID, agentReady->agentID);
			strcpy(data_status.extension, agentReady->agentDevice);
			strcpy(data_status.workmode, STATUS_READY);
			strcpy(data_status.auxreason, "");
			strcpy(data_status.direction, "");
			strcpy(data_status.agstate, AGSTATE_READY);
			strcpy(data_status.workskill, "");						
			
			break;
		case CSTAAgentWorkingAfterCall:
			tWorkingAfterCallEvent *agentACW;
			agentACW = (tWorkingAfterCallEvent *)__event_data;
			
			//////// Agent status /////////////////////////////////////////////
			
			// cause 체크 하지 않고 무조건 통계 처리
			// cause ; Normal(ecNormal) or Autowork(ecAutoWork)
			strcpy(key_status.agentID, agentACW->agentID);
			strcpy(data_status.extension, agentACW->agentDevice);

			strcpy(data_status.workmode, STATUS_ACW);
			strcpy(data_status.agstate, AGSTATE_ACW);
			
			strcpy(data_status.auxreason, "");
			strcpy(data_status.direction, "");
			// workskill 값세팅 : IB call이후 CM에서 바로 event 발생(acdGroup정보 포함)
			// Agent가 수동으로 ACW 발생시 acdGroup정보 미포함
			strcpy(data_status.workskill, agentACW->acdGroup);
			
			//////// Tracking call log ////////////////////////////////////////	
			// acwtime 보류
			
			break;
			
		case CSTADelivered:
			tDeliveredEvent *delivered;
			delivered = (tDeliveredEvent *) __event_data;
			
			// Event가 발생 시킨 device와 일치하는지 확인
			// -> 기본적으로 VDN에서만 중복 이벤트가 발생된다.
			// -> 추후 비교문 삭제 고려
//			if (strcmp(delivered->calledDevice, __monitored_dev_info.extension) != 0)
//				return false;
			
			
			// Direction 구분
			if (strcmp(delivered->callingDevice, __monitored_dev_info.extension) == 0) {
				strcpy(data_status.direction, "OUT");
			}
			else {
				strcpy(data_status.direction, "IN");
				
				//////// Agent status /////////////////////////////////////////////
				strcpy(data_status.sub_workmode, STATUS_RING);
				strcpy(data_status.agstate, STATUS_RING);
			}
	
			// DeviceID로 Agent 현 status 조회
			if (strcmp(data_status.direction, "OUT") == 0) // OUT -> 발신 Device에서는 bIsAgentID는 무조건 false이다.
				pdata_agent_status = pagent_status_rpt->check_agentID(delivered->callingDevice, agentID, bIsAgentID);
			else // IN
				pdata_agent_status = pagent_status_rpt->check_agentID(delivered->alertingDevice, agentID, bIsAgentID);
				
			if (pdata_agent_status == NULL) {
				std::cerr << "Agent status data not found with extension : " 
						<< delivered->connection.deviceID << std::endl;
				return false;
			}
			
			// Set key structure
			strcpy(key_status.agentID, agentID);
			
//			strcpy(data_status.extension, __monitored_dev_info.extension);
				
			// 원칙적으로 ecNewCall 경우에만 call log가 추가(bNewCallLog = true 되나 
			// cause 체크 없이 처리할 방안이 있는지 체크해보자
			
//			if (strcmp(data_status.direction, "IN") == 0) {
//				// 처리 않는다.
//			}
//			else {
//				if (strcmp(pdata_agent_status->workmode, STATUS_DACW) == 0)
//					strcpy(data_status.agstate, AGSTATE_DACWOUT);
//				else if (strcmp(pdata_agent_status->workmode, STATUS_ACW) == 0)
//					strcpy(data_status.agstate, AGSTATE_ACWOUT);
//			}			
			

			//////// Tracking Call log //////////////////////////////////////////////
			bMakeCallLog = pcall_tracking->decide_make_call_log(delivered->connection.callID
								, __monitored_dev_info.device_type, __monitored_dev_info.extension
			                    , delivered->callingDevice, delivered->calledDevice
			                    , data_current_call_log, bIsDataFound);
			
			if (bMakeCallLog == true) {
				data_call_log.event_handle = STATION;

				if (bIsDataFound == false)
					bNewCallLog = true;
				else {
					// NewCall이거나 다른 device에서 넘어온 cause는 모두 call log 추가
					if ( (delivered->cause == ecNewCall) ||
						 (delivered->cause == ecRedirected) ||
						 (delivered->cause == ecSingleStepTransfer) ||
						 (delivered->cause == ecSingleStepConference)
						 ) 
					{
						// Device 발신 이후라면 현 call log 에 갱신처리한다.
						if (data_current_call_log.last_callcontrol_event == CSTAOriginated)
							bNewCallLog = false;
						else
							bNewCallLog = true;
					}
				}
				
				key_call_log.c.callID = delivered->connection.callID;
				strcpy(data_call_log.c.domain, __monitored_dev_info.domain);
				strcpy(data_call_log.c.ucid,  __private_data.UCID);
				
				// Call log는 발신 device 측면의 event로 처리한다.(내선발신은 별도 테스트 필요)
				// 내부 DA콜(Agent try to connect to Agent dialed AgentID) 제외하고는 IB event는 패스					
				// 발신 Device 측면 event
				strcpy(data_call_log.c.calling_pty, delivered->callingDevice);
				strcpy(data_call_log.c.dialed_num, delivered->calledDevice);
				//origlogin callingDevice가 Agent가 아니면 null 세팅
				if (pagent_status_rpt->get_agentID(data_call_log.c.calling_pty, agentID) == true) {
					strcpy(data_call_log.c.origlogin, agentID);
				}

				pcall_tracking->set_column_switch(RINGTIME, ON, data_call_log, __event_time);
				
				pcall_tracking->set_column_switch(SEGMENT, ON, data_call_log, __event_time);
			}
			
			// cause Redirected, Transfer, Conference시에는 newcall에 준하는 
			
			//////// Agent current interval stat. /////////////////////////////
			// callreport_agent_shm.hpp 에 상담원 시간 관련 데이터 누적처리
			// 이전 agent_status_shm 정보를 비교해서 적합한 상담원 시간 데이터 
			// 생성 후 현 interval의 통계 데이터를 생성/갱신한다.
						
			break;
		case CSTAEstablished:
			tEstablishedEvent *established;
			established = (tEstablishedEvent *) __event_data;
			
			// 발신 Device 측면 event일 경우에만 call log처리(중복처리 방지)
//			if (strcmp(established->callingDevice, __monitored_dev_info.extension) == 0)
//				bMakeCallLog  = true;

			bMakeCallLog = pcall_tracking->decide_make_call_log(established->establishedConnection.callID
									, __monitored_dev_info.device_type, __monitored_dev_info.extension
				                    , established->callingDevice, established->calledDevice
				                    , data_current_call_log, bIsDataFound);
			
			//////// Agent status /////////////////////////////////////////////
			
			// Direction 구분
			if (strcmp(established->answeringDevice, __monitored_dev_info.extension) == 0)
				strcpy(data_status.direction, "IN");
			else
				strcpy(data_status.direction, "OUT");

			// DeviceID로 Agent 현 status 조회
			if (strcmp(data_status.direction, "IN") == 0) // IN
				pdata_agent_status = pagent_status_rpt->check_agentID(established->answeringDevice, agentID, bIsAgentID);
			else // OUT -> 발신 Device에서는 bIsAgentID는 무조건 false이다.
				pdata_agent_status = pagent_status_rpt->check_agentID(established->callingDevice, agentID, bIsAgentID);
				
			if (pdata_agent_status == NULL) {
				std::cerr << "Agent status data not found with extension : " 
						<< established->establishedConnection.deviceID << std::endl;
				return false;
			}
			
			// Set key structure
			strcpy(key_status.agentID, agentID);

			// Agstate, Workmode
			if (strcmp(data_status.direction, "IN") == 0) {  // IB
				if (strlen(data_current_call_log.c.dispsplit) > 0) {  // ACD
					strcpy(data_status.sub_workmode, STATUS_ACD);
					strcpy(data_status.agstate, AGSTATE_ACD);
					
					std::cout << "Workskill : " << data_current_call_log.c.dispsplit << std::endl;
					strcpy(data_status.workskill, data_current_call_log.c.dispsplit);
				}
				else {
					// DACD인지 내선통화인지 확인 (수신device#가 agentID인지 체크)
					if (bIsAgentID == true) {
						strcpy(data_status.sub_workmode, STATUS_DACD);
						strcpy(data_status.agstate, AGSTATE_DACD);						
					}
					else {
						// 내선콜 수신시에는 workmode는 변화없다.
						if (strcmp(pdata_agent_status->workmode, STATUS_ACW) == 0)
							strcpy(data_status.agstate, AGSTATE_ACWIN);
						else if (strcmp(pdata_agent_status->workmode, STATUS_AUX) == 0)
							strcpy(data_status.agstate, AGSTATE_AUXIN);
						else if (strcmp(pdata_agent_status->workmode, STATUS_DACW) == 0)
							strcpy(data_status.agstate, AGSTATE_DACWIN);							
					}		
				}
			}
			else {  // OB
				// Set agstate
				strcpy(data_status.sub_workmode, "");
				if (strcmp(pdata_agent_status->workmode, STATUS_ACW) == 0) {
					strcpy(data_status.agstate, AGSTATE_ACWOUT);
				}
				else if (strcmp(pdata_agent_status->workmode, STATUS_AUX) == 0) {
					strcpy(data_status.agstate, AGSTATE_AUXOUT);
				}
				else if (strcmp(pdata_agent_status->sub_workmode, STATUS_DACW) == 0) {
					strcpy(data_status.agstate, AGSTATE_DACWOUT);
				}
			}
			
			//////// Tracking call log ////////////////////////////////////////
			if (bMakeCallLog == true) {
				key_call_log.c.callID = established->establishedConnection.callID;
				
				AGENT_STATUS_LIST_SHMKEY_t key_status_tmp_c;
				
				memset(&key_status_tmp_c, 0x00, sizeof(AGENT_STATUS_LIST_SHMKEY_t));
				
				strcpy(key_status_tmp_c.agentID, established->calledDevice);
				
				if (bIsDataFound == false)
					bNewCallLog = true;
				
				// Agent ID검색
				if (pagent_status_rpt->check_exists(key_status_tmp_c) == true) {
					strcpy(data_call_log.c.anslogin, key_status_tmp_c.agentID);
				}
				else {
					if (pagent_status_rpt->get_agentID(established->calledDevice, agentID) == true) {
						strcpy(data_call_log.c.anslogin, agentID);
					}
				}
				
				// DACD여부 판별
				if (strcmp(established->answeringDevice, established->calledDevice) != 0) {
					if (strlen(data_call_log.c.anslogin) > 0)
						data_call_log.c.call_disp = TRACKING_CALL::ANSWERED;
					else // OB로 판별 -> 추후 추가되는 Event data field로 체크
						data_call_log.c.call_disp = TRACKING_CALL::OTHER;
				}
				else {
					data_call_log.c.call_disp = TRACKING_CALL::CONNECTED;
				}
				
				pcall_tracking->set_column_switch(RINGTIME, OFF, data_call_log, __event_time);
				pcall_tracking->set_column_switch(TALKTIME, ON, data_call_log, __event_time);				
			}
			
			break;
		case CSTAConnectionCleared:
			tConnectionClearedEvent *connCleared;
			connCleared = (tConnectionClearedEvent *) __event_data;
			
			// Event가 발생 시킨 device와 일치하는지 확인(Event 중복 처리 방지)
//			if (strcmp(connCleared->droppedConnection.deviceID, __monitored_dev_info.extension) != 0)
//				return true;
			// 중복 Event 제거
			if (connCleared->localConnectionInfo == csConnected)
				return true;
				
			bMakeCallLog = pcall_tracking->decide_make_call_log(connCleared->droppedConnection.callID
									, __monitored_dev_info.device_type, __monitored_dev_info.extension
				                    , 0, 0
				                    , data_current_call_log, bIsDataFound);				                    
			if (bMakeCallLog == true) {
				bComplete_calllog = true;
			}


			key_call_log.c.callID = connCleared->droppedConnection.callID;
			
			//////// Agent status /////////////////////////////////////////////			
			// Agstate -> Copy previous workmode to agstate
			pdata_agent_status = pagent_status_rpt->check_agentID(connCleared->releasingDevice, agentID, bIsAgentID);
			if (pdata_agent_status == NULL) {
				std::cerr << "Agent status data not found with extension : " 
						<< connCleared->releasingDevice << std::endl;
				return false;
			}
			
			// Set key structure
			strcpy(key_status.agentID, agentID);
			
			strcpy(data_status.sub_workmode, "");

			// Rollback agstate status value
			strcpy(data_status.agstate, pdata_agent_status->workmode);

			// Direction -> Blank
			strcpy(data_status.direction, "");
			
			// Workmode will be changed when server receives CSTAAgentWorkingAfterCall event
			
			//////// Tracking call log ////////////////////////////////////////
			key_call_log.c.callID = connCleared->droppedConnection.callID;
			
			// Held상태에서 끊어졌는지 확인(일반적으로 VDN을 경유한 IB처리시)
			if (data_current_call_log.last_callcontrol_sub_event == CSTAHeld) {
				// call log의 origlogin정보가 존재하면 상담원 발신콜
				//			  origlogin정보가 없고 anslogin이 존재하면 상담원 수신콜
				// 발신콜시 발신상담원(origlogin)의 device에서 발생한 event로 처리
				// 수신콜시 수신상담원(anslogin)의 device에서 발생한 event로 처리
				if (strlen(data_current_call_log.c.origlogin) == 0) { // IB
					if (strcmp(data_current_call_log.c.anslogin, agentID) == 0) {
						bMakeCallLog = true;
					}
				}
				else { // OB
					if (strcmp(data_current_call_log.c.origlogin, agentID) == 0) {
						bMakeCallLog = true;
					}
				}

				data_call_log.c.ansholdtime = __event_time - data_current_call_log.last_callcontrol_sub_event_time;
				data_call_log.c.holdabn = 1;

				pcall_tracking->set_column_switch(HELD, OFF, data_call_log, 0);
			}
			else {
				// Established 이후 종료된 콜인지 체크
				if (bMakeCallLog == true) {
					if (data_current_call_log.last_callcontrol_event == CSTAEstablished) {
						pcall_tracking->set_column_switch(TALKTIME, OFF, data_call_log, __event_time);
						
						if (data_current_call_log.first_releasing == TRACKING_CALL::NONE) {							
							set_agt_released(connCleared->droppedConnection.callID, connCleared->releasingDevice
												, data_current_call_log, data_call_log);
						}					
					}
					// 연결 못하고 종료된 케이스
					else {
						data_call_log.c.call_disp = TRACKING_CALL::ABANDONED;
						// Failed등 Delivered Event 누락후 종료
//						if (data_current_call_log.sw_ringtime == ON) {
							pcall_tracking->set_column_switch(RINGTIME, OFF, data_call_log, __event_time);
//						}
					}

					pcall_tracking->set_column_switch(SEGMENT, OFF, data_call_log, __event_time);
				}
			}
		
			break;
		case CSTAConferenced:
			tConferencedEvent *conferenced;
			conferenced = (tConferencedEvent *) __event_data;
			
//			if (strcmp(conferenced->conferencingDevice, __monitored_dev_info.extension) != 0)
//				return false;
			
			//////// Tracking call log ////////////////////////////////////////
			key_call_log.c.callID = conferenced->primaryOldCall.callID;			
			
			if (data_current_call_log.last_callcontrol_sub_event == CSTAHeld) {
				data_call_log.c.ansholdtime = __event_time - data_current_call_log.last_callcontrol_sub_event_time;
				// Sub event 초기화
				pcall_tracking->set_column_switch(HELD, OFF, data_call_log, __event_time);
			}			

			break;

		case CSTATransferred:
			tTransferredEvent *transferred;
			transferred = (tTransferredEvent *) __event_data;
			
//			if (strcmp(transferred->transferringDevice, __monitored_dev_info.extension) != 0)
//				return false;				

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
				// Sub event 초기화
				pcall_tracking->set_column_switch(HELD, OFF, data_call_log, __event_time);
			}
						
			break;
			
		case CSTAServiceInitiated:  // VDN에서는 발생 안되는 event(OriginateEvent부터 call log 처리 시작)
			// 발신번호가 나와야 VDN으로 발신을 하는지 외선발신인지, 내선발신인지 알 수 있기때문에
			// 본 이벤트는 처리않고 넘긴다.
			return true;  // Pass

			break;
		case CSTAOriginated:
			tOriginatedEvent *originated;
			originated = (tOriginatedEvent *) __event_data;
			
//			if (strcmp(originated->callingDevice, __monitored_dev_info.extension) != 0)
//				return false;

			//////// Agent status /////////////////////////////////////////////
			pdata_agent_status = pagent_status_rpt->check_agentID(originated->originatedConnection.deviceID, agentID, bIsAgentID);
			if (pdata_agent_status == NULL) {
				std::cerr << "Agent status data not found with extension : "
						<< originated->originatedConnection.deviceID << std::endl;
				return false;
			}

			// Set key structure
			strcpy(key_status.agentID, agentID);

			// Workmode is not changed
			strcpy(data_status.direction, "OUT");
			strcpy(data_status.sub_workmode, STATUS_OTHER);
			strcpy(data_status.agstate, STATUS_OTHER);
			
			// workskill 데이터 존재시 그대로 유지
			// ex) Auto ACW 상태일 경우
			strcpy(data_status.workskill, pdata_agent_status->workskill);
			
			//////// Tracking call log ////////////////////////////////////////
			bMakeCallLog = true;
			bNewCallLog = true;
			data_call_log.event_handle = STATION;	
			
			if (bMakeCallLog == true) {
				key_call_log.c.callID = originated->originatedConnection.callID;
				
				strcpy(data_call_log.c.domain, __monitored_dev_info.domain);
				data_call_log.c.tenant = 1;   // Value fix to 1 currently   
				                                                            
				                                                            
				strcpy(data_call_log.c.ucid,  __private_data.UCID);
				
				pcall_tracking->set_column_switch(SEGMENT, ON, data_call_log, __event_time);
				
				strcpy(data_call_log.c.calling_pty, originated->callingDevice);
				strcpy(data_call_log.c.dialed_num, originated->calledDevice);
				strcpy(data_call_log.c.origlogin, agentID);
				strcpy(data_call_log.c.dispsplit, pdata_agent_status->workskill);
			}
			
			break;

		case CSTAHeld:
			tHeldEvent *held;
			held = (tHeldEvent *) __event_data;
			
			if (strcmp(held->holdingDevice, __monitored_dev_info.extension) != 0)
				return true;

			if (pcall_tracking->read_current_call_log(held->heldConnection.callID
					, data_current_call_log) == true) {
					bMakeCallLog = true;
			}
			// If couldn't find call log, do not update call log
//			else 
//				bMakeCallLog = true;

			bNewCallLog = false;
			
			//////// Agent status /////////////////////////////////////////////
			pdata_agent_status = pagent_status_rpt->check_agentID(held->heldConnection.deviceID, agentID, bIsAgentID);
			if (pdata_agent_status == NULL) {
				std::cerr << "Agent status data not found with extension : "
						<< originated->originatedConnection.deviceID << std::endl;
				return false;
			}
						
			// Set key structure - for ACD-group status
			strcpy(key_status.agentID, agentID);
			

			//////// Tracking call log ////////////////////////////////////////
			key_call_log.c.callID = held->heldConnection.callID;
			
			pcall_tracking->set_column_switch(HELD, ON, data_call_log, __event_time);
			
			break;		
		case CSTARetrieved:
			tRetrievedEvent *retrieved;
			retrieved = (tRetrievedEvent *) __event_data;

			if (strcmp(retrieved->retrievingDevice, __monitored_dev_info.extension) != 0)
				return true;
				
			if (pcall_tracking->read_current_call_log(retrieved->retrievedConnection.callID
					, data_current_call_log) == true) {
				bMakeCallLog = true;
			}
			
			bNewCallLog = false;		
			
			//////// Agent status /////////////////////////////////////////////
			pdata_agent_status = pagent_status_rpt->check_agentID(retrieved->retrievedConnection.deviceID, agentID, bIsAgentID);
			if (pdata_agent_status == NULL) {
				std::cerr << "Agent status data not found with extension : "
						<< originated->originatedConnection.deviceID << std::endl;
				return false;
			}
						
			// Set key structure - for ACD-group status
			strcpy(key_status.agentID, agentID);
						
			//////// Tracking call log ////////////////////////////////////////
			key_call_log.c.callID = retrieved->retrievedConnection.callID;
			
			if (data_current_call_log.last_callcontrol_sub_event == CSTAHeld) {
				data_call_log.c.ansholdtime 
						= __event_time - data_current_call_log.last_callcontrol_sub_event_time;

				pcall_tracking->set_column_switch(HELD, OFF, data_call_log, 0);
			}
			
			break;	

		case CSTADiverted:	  // RONA에 발생할 가능성 있음(테스트 후 적용)
			return true;
			break;		
		case CSTAOffered:	  // 이하 5개 Event 사용하지 않음
		case CSTADigitsDialled:
		case CSTAFailed:	
		case CSTACallCleared: // Device type monitoring 에서는 절대 발생하지 않는 event
		case CSTAQueued:	  // VDN이 아니므로 이벤트 발생할 일이 없음			
			return true;
			break;					
		default:
			return false;
			break;
	}

	// agtime과 agduration은 Insert 시에만 사용됨
	data_status.agtime = __event_time;
	data_status.agduration = __event_time;
	// Common ; timestamp with event creation time
	data_status.last_event = __event;
	data_status.tevent_time = __event_time;
	
	if ( (__event != CSTAHeld) && (__event != CSTARetrieved) ) {
		data_call_log.last_callcontrol_event_time = __event_time;
		data_call_log.last_callcontrol_event = __event;
	}
		
	if (strlen(key_status.agentID) > 0) {
std::cout << "pagent_status_rpt->register_data : agentID [" << key_status.agentID << "]" << std::endl;
		// Agent status
		if (pagent_status_rpt->register_data(key_status, data_status) == false) {
			std::cout << "Register failed with agent_status_rpt" << std::endl;
			return false;
		}
	}
	
	// Call tracking log
	if (bMakeCallLog == true) {
		if (pcall_tracking->register_data(key_call_log, data_call_log, bNewCallLog) == false) {
			std::cerr << "Call tracking insert/update failed!!!" << std::endl;
			return false;
		}
		
		if (bComplete_calllog == true) {
			complete_call_log(key_call_log.c.callID);	
		}
	}
	
	return true;
}
