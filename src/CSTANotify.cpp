#include <time.h>
#include <iostream>

#include "XPRJACEDLLEx/CSTANameString.h"
#include "XPRJACEDLLEx/CSTADefineEvent.h"
#include "XPRJACEDLLEx/CSTAEvent.h"

#include "CSTANotify.h"
#include "ipc_res.hpp"
#include "device_manage_shm.hpp"


CSTANotifier::CSTANotifier(void)
: sessionID_("")
{
	linkStatus = 0;
	monitoringStatus = 0;
	
	pServiceRequested = new manage_umap<SERVICE_CROSS_REFID_KEY_t, SERVICE_CROSS_REFID_t>;
	if (pServiceRequested == NULL) {
		std::cerr << "Failed to allocate service request management memory" << std::endl;
	}
}

CSTANotifier::~CSTANotifier(void)
{
	sessionID_ = "";
	
	if (pServiceRequested != NULL) delete pServiceRequested;
}

// CSTADefineEvent.h 에 define되어 있다.
void CSTANotifier::NotifyEvent(CCSTAEvent& event)
{
	tPrivateData privData = event.PrivateData();
	
std::cout << "Inside NotifyEvent : Type   [" << event.Type() << "]" << std::endl;
std::cout << "   : ConfirmationType       [" << event.ConfirmationType() << "]" << std::endl;
std::cout << "   : UnsolicitedType        [" << event.UnsolicitedType() << "]" << std::endl;
std::cout << "   : SystemType             [" << event.SystemType() << "]" << std::endl;	
std::cout << "   : PrivateData.UCID       [" << privData.UCID << "]" << std::endl;
std::cout << "   : PrivateData.reasonCode [" << privData.reasonCode << "]" << std::endl;


	switch (event.Type())
	{
	case etUndefinedType:   // 0
		{
		}
		break;

	case etConfirmation:   // 1
		{
			OnConfirmationEvent(event.ConfirmationType(), event.ConfirmationEvent(), event.PrivateData());
		}
		break;

	case etUnsolicited:   // 2
		{
			OnUnsolicitedEvent(event.UnsolicitedType(), event.UnsolicitedEvent(), event.PrivateData());
		}
		break;

	case etSystem:  // 3
		{
			OnSystemEvent(event.SystemType(), event.SystemEvent(), event.PrivateData());
		}
		break;

	case etConfError:  // 4
		{

		}
		break;
	}
}

void CSTANotifier::SetCommonObject(hcommon *__pCommObj)
{
	pCommObj = __pCommObj;
}

// 구현되지 않은 이벤트는 흘려보내도록 한다.
void CSTANotifier::OnConfirmationEvent(tConfirmationEventType conf, CSTAConfirmationEvent confEvent, tPrivateData privData)
{
	EVENTDATA_t data;
	
	SERVICE_CROSS_REFID_KEY_t  key_svcref;
	SERVICE_CROSS_REFID_t    *pData_svcref;	
	
	MONITORING_LIST_SHMKEY_t  monitor_list_key;
	MONITORING_LIST_SHM_t     *monitor_list_data;
	
	memset(&key_svcref, 0x00, sizeof(SERVICE_CROSS_REFID_KEY_t));
	memset(&monitor_list_key, 0x00, sizeof(MONITORING_LIST_SHMKEY_t));
	
	char message_text[LOG_LEN];
	

	string temp;
	vector<string> strArr;

	switch (conf)
	{	
	case CSTAConfUndefined:
		{

		}
		break;
	/* CSTAOpenSession, CSTACloseSession, CSTAResetSessionTimer : Session services */
	case CSTAOpenSession:
		{
			temp = "InvokeID=";
			temp += confEvent.serviceCrossRefID;
			strArr.push_back(temp);

			sessionID_ = confEvent.u.openSession.sessionID;

			std::cout << "sessionID  =[" << sessionID_ << "]" << std::endl;
			std::cout << "protocolVer=[" << confEvent.u.openSession.actualProtocolVersion << "]" << std::endl;
			std::cout << "sessionDuration=[" << confEvent.u.openSession.actualSessionDuration << "]" << std::endl;
		}
		
		strcpy(message_text, "CSTAOpenSession");
		pCommObj->LoggingError(__FILE__, __func__, 0, message_text);
				
		break;

	case CSTACloseSession:
		/*
		{
			strArr.Add("[CONF] CSTACloseSession");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			mainDlg_->IsLinkConnected(false);
		}
		*/
		
		std::cout << "InvokeID=[" << confEvent.serviceCrossRefID << std::endl;
			
		strcpy(message_text, "CSTACloseSession");
		pCommObj->LoggingError(__FILE__, __func__, 0, message_text);
					
		linkStatus = 0;
		break;
		
	case CSTAResetSessionTimer:
		{

		}
		break;
	
	/* Monitoring Services : CSTAMonitorCalls, CSTAMonitorDevice, CSTAMonitorCallViaDevice, CSTAMonitorStop */
	case CSTAMonitorCall: // Call의 시작시점을 알 수 있어야 추적이 가능하다.
		                  // CM에서 call 생성과 동시에 event를 자동으로 주어야 활용이 가능하다
		{
		/*
			strArr.Add("[CONF] CSTAMonitorCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			int monitorID = confEvent.u.monitorCall.monitorCrossRefID;
			int agentFilter = confEvent.u.monitorCall.monitorFilter.agent;
			int callFilter = confEvent.u.monitorCall.monitorFilter.call;
			int featureFilter = confEvent.u.monitorCall.monitorFilter.feature;

			temp.Format("monitorID=[%d]", monitorID);
			mainDlg_->AddList(temp);

			temp.Format("agentFilter=[%d] | callFilter=[%d] | featureFilter=[%d]", agentFilter, callFilter, featureFilter);
			mainDlg_->AddList(temp);
		*/
		}
		break;

	case CSTAMonitorDevice:
			/*
			strArr.Add("[CONF] CSTAMonitorDevice");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			int monitorID = confEvent.u.monitorDevice.monitorCrossRefID;
			int agentFilter = confEvent.u.monitorDevice.monitorFilter.agent;
			int callFilter = confEvent.u.monitorDevice.monitorFilter.call;
			int featureFilter = confEvent.u.monitorDevice.monitorFilter.feature;

			temp.Format("monitorID=[%d]", monitorID);
			mainDlg_->AddList(temp);

			temp.Format("agentFilter=[%d] | callFilter=[%d] | featureFilter=[%d]", agentFilter, callFilter, featureFilter);
			mainDlg_->AddList(temp);
			*/
		
		pDevice_status_shm->update_result(confEvent.serviceCrossRefID, confEvent.u.monitorDevice.monitorCrossRefID, (char *)"Y", "");

		sprintf(message_text, "CSTAMonitorDevice => monitorCrossRefID [%d]", confEvent.u.monitorDevice.monitorCrossRefID);
		pCommObj->LoggingError(__FILE__, __func__, 0, message_text);

		break;

	case CSTAMonitorCallViaDevice: // VDN Monitoring 시작시 event
		pDevice_status_shm->update_result(confEvent.serviceCrossRefID, confEvent.u.monitorDevice.monitorCrossRefID, (char *)"Y", "");
		
		sprintf(message_text, "CSTAMonitorCallViaDevice => monitorCrossRefID [%d]", confEvent.u.monitorDevice.monitorCrossRefID);
		pCommObj->LoggingError(__FILE__, __func__, 0, message_text);
				
		break;

	case CSTAMonitorStop:
		// confEvent.serviceCrossRefID는 Get Monitors를 제외하면 활용측면에서 현재로서는 큰 의미가 없다.
		//std::cout << "CSTAMonitorStop received : RefID -> [" << confEvent.serviceCrossRefID  <<  "]" << std::endl;
		
		// Issue : monitorCrossRefID로 MonitorStop을 request했으나 Stop이벤트 수신시 어떤 Extension에 대한 Monitoring 중지에 대한 
		// response인지 알 수가 없다.
		
		pDevice_status_shm->delete_data(confEvent.u.monitorDevice.monitorCrossRefID);
		pDevice_status_shm->update_result(confEvent.serviceCrossRefID, 0, (char *)"N", message_text);
		
		sprintf(message_text, "CSTAMonitorStop => monitorCrossRefID [%d]", confEvent.u.monitorDevice.monitorCrossRefID);
		pCommObj->LoggingError(__FILE__, __func__, 0, message_text);
		
		break;
		
		
/* Call Control Services AcceptCall ~ TransferCall
	case CSTAAcceptCall:
		{
			strArr.Add("[CONF] CSTAAcceptCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
		}
		break;

	case CSTAAlternateCall:
		{
			strArr.Add("[CONF] CSTAAlternateCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
		}
		break;

	case CSTAAnswerCall:
		{
			strArr.Add("[CONF] CSTAAnswerCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
		}
		break;

	case CSTAClearCall:
		{
			strArr.Add("[CONF] CSTAClearCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
		}
		break;

	case CSTAClearConnection:
		{
			strArr.Add("[CONF] CSTAClearConnection");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
		}
		break;

	case CSTAConferenceCall:
		{
			strArr.Add("[CONF] CSTAConferenceCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("conferenceCall : callID=[%d], deviceID=[%s]", 
				confEvent.u.conferenceCall.conferenceCall.callID,
				confEvent.u.conferenceCall.conferenceCall.deviceID);
			mainDlg_->AddList(temp);

			//TODO:connections
		}
		break;

	case CSTAConsultationCall:
		{
			strArr.Add("[CONF] CSTAConsultationCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("initiatedCall : callID=[%d], deviceID=[%s]", 
				confEvent.u.consultationCall.initiatedCall.callID,
				confEvent.u.consultationCall.initiatedCall.deviceID);
			mainDlg_->AddList(temp);
		}
		break;

	case CSTADeflectCall:
		{
			strArr.Add("[CONF] CSTADeflectCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
		}
		break;

	case CSTADirectedPickupCall:
		{
			strArr.Add("[CONF] CSTADirectedPickupCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("pickedCall : callID=[%d], deviceID=[%s]", 
				confEvent.u.directPickupCall.pickedCall.callID,
				confEvent.u.directPickupCall.pickedCall.deviceID);
			mainDlg_->AddList(temp);
		}
		break;

	case CSTAGroupPickupCall:
		{
			strArr.Add("[CONF] CSTAGroupPickupCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("pickedCall : callID=[%d], deviceID=[%s]", 
				confEvent.u.groupPickupCall.pickedCall.callID,
				confEvent.u.groupPickupCall.pickedCall.deviceID);
			mainDlg_->AddList(temp);
		}
		break;

	case CSTAHoldCall:
		{
			strArr.Add("[CONF] CSTAHoldCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
		}
		break;

	case CSTAIntrudeCall:
		{
			strArr.Add("[CONF] CSTAIntrudeCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("conferencedCall : callID=[%d], deviceID=[%s]", 
				confEvent.u.intrudeCall.conferencedCall.callID,
				confEvent.u.intrudeCall.conferencedCall.deviceID);
			mainDlg_->AddList(temp);
		}
		break;

	case CSTAJoinCall:
		{
			strArr.Add("[CONF] CSTAJoinCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("conferencedCall : callID=[%d], deviceID=[%s]", 
				confEvent.u.joinCall.conferencedCall.callID,
				confEvent.u.joinCall.conferencedCall.deviceID);
			mainDlg_->AddList(temp);
		}
		break;

	case CSTAMakeCall:
		{
			strArr.Add("[CONF] CSTAMakeCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("newCall : callID=[%d], deviceID=[%s]", 
				confEvent.u.makeCall.newCall.callID,
				confEvent.u.makeCall.newCall.deviceID);
			mainDlg_->AddList(temp);
		}
		break;

	case CSTAMakePredictiveCall:
		{
			strArr.Add("[CONF] CSTAMakePredictiveCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("initiatedCall : callID=[%d], deviceID=[%s]", 
				confEvent.u.makePredictiveCall.initiatedCall.callID,
				confEvent.u.makePredictiveCall.initiatedCall.deviceID);
			mainDlg_->AddList(temp);
		}
		break;

	case CSTAParkCall:
		{
			strArr.Add("[CONF] CSTAParkCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("parkedTo : callID=[%d], deviceID=[%s]", 
				confEvent.u.parkCall.parkedTo.callID,
				confEvent.u.parkCall.parkedTo.deviceID);
			mainDlg_->AddList(temp);
		}
		break;

	case CSTAReconnectCall:
		{
			strArr.Add("[CONF] CSTAReconnectCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
		}
		break;

	case CSTARetrieveCall:
		{
			strArr.Add("[CONF] CSTARetrieveCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
		}
		break;

	case CSTASendMessage:
		{
			strArr.Add("[CONF] CSTASendMessage");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			int count = confEvent.u.sendMessage.connectionList.count;
			for (int i=0; i<count; i++)
			{
				temp.Format("Conn[%d] : callID=[%d], deviceID=[%s]", i+1,
					confEvent.u.sendMessage.connectionList.connection[i].callID,
					confEvent.u.sendMessage.connectionList.connection[i].deviceID);
				mainDlg_->AddList(temp);
			}
		}
		break;

	case CSTASingleStepConferenceCall:
		{
			strArr.Add("[CONF] CSTASingleStepConferenceCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("conferencedCall : callID=[%d], deviceID=[%s]", 
				confEvent.u.singleStepConference.conferencedCall.callID,
				confEvent.u.singleStepConference.conferencedCall.deviceID);
			mainDlg_->AddList(temp);
		}
		break;

	case CSTASingleStepTransferCall:
		{
			strArr.Add("[CONF] CSTASingleStepTransferCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("transferredCall : callID=[%d], deviceID=[%s]", 
				confEvent.u.singleStepTransfer.transferredCall.callID,
				confEvent.u.singleStepTransfer.transferredCall.deviceID);
			mainDlg_->AddList(temp);
		}
		break;

	case CSTATransferCall:
		{
			strArr.Add("[CONF] CSTATransferCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("transferredCall : callID=[%d], deviceID=[%s]", 
				confEvent.u.transferCall.transferredCall.callID,
				confEvent.u.transferCall.transferredCall.deviceID);
			mainDlg_->AddList(temp);

			int count = confEvent.u.transferCall.connections.count;

			for (int i=0; i<count; i++)
			{
				temp.Format("staticDevice[%s] : callID=[%d], deviceID=[%s]", 
					confEvent.u.transferCall.connections.connection[i].staticDevice,
					confEvent.u.transferCall.connections.connection[i].party.callID,
					confEvent.u.transferCall.connections.connection[i].party.deviceID);
				mainDlg_->AddList(temp);
			}

		}
		break;
*/

/* Logical Device Features CallBackNonCallRelated ~ SetRouteingMode
	case CSTASetAgentState:
		{
			strArr.Add("[CONF] CSTASetAgentState");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
		}
		break;

	case CSTASetForwarding:
		{
			strArr.Add("[CONF] CSTASetForwarding");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
		}
		break;

	case CSTASetDoNotDisturb:
		{
			strArr.Add("[CONF] CSTASetDoNotDisturb");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
		}
		break;
		
	case CSTAGetDoNotDisturb:	
		{
			strArr.Add("[CONF] CSTAGetDoNotDisturb");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			if (true == confEvent.u.getDoNotDisturb.doNotDisturb)
				temp.Format("doNotDisturb ON");
			else
				temp.Format("doNotDisturb OFF");
			
			mainDlg_->AddList(temp);
		}
		break;		
*/
	case CSTAGetAgentState:   // DeviceID로 해당 Device에 로그인한 상담원의 현재 보유ACD-group별 
		                      // 상담원 상태를 조회
		{
			AGENT_STATUS_LIST_SHMKEY_t key_status;
			AGENT_STATUS_LIST_SHM_t    data_status;
		    tDeviceID acdGroup;
		    
		    time_t event_time;
						
			memset(&key_status, 0x00, sizeof(AGENT_STATUS_LIST_SHMKEY_t));
			memset(&data_status, 0x00, sizeof(AGENT_STATUS_LIST_SHM_t));
			
//			strArr.Add("[CONF] CSTAGetAgentState");
//			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
//			strArr.Add(temp);
//			mainDlg_->AddList(strArr, true);
//
//			if (confEvent.u.getAgentState.agentState.loggedOnState)
//			{
//				temp.Format("agentID=[%s], agentState=[%s], acdGroup=[%s], reasonCode=[%s]", 
//					confEvent.u.getAgentState.agentState.agentID,
//					CCSTANameString::AgentState2String(confEvent.u.getAgentState.agentState.agentState),
//					confEvent.u.getAgentState.agentState.acdGroup,
//					privData.reasonCode ? privData.reasonCode : "NULL");
//			}
//			else
//			{
//				temp.Format("Agent Null");		
//			}
//			mainDlg_->AddList(temp);

//confEvent.u.getAgentState.agentState.agentID
//confEvent.u.getAgentState.agentState.acdGroup
//IS::tAgentState confEvent.u.getAgentState.agentState.agentState
//bool confEvent.u.getAgentState.agentState.loggedOnState

			// ServiceCrossRefID로 extension 조회
			//serviceCrossRefID
			key_svcref.serviceCrossRefID = confEvent.serviceCrossRefID;
			pData_svcref = pServiceRequested->read_data(key_svcref);
			if (pData_svcref == NULL) {
				std::cerr << "Couldn't find extension info. with ServiceCrossRefID => " 
					<< confEvent.serviceCrossRefID << std::endl;
			}
			else {
				time(&event_time);
				
				
				hcommon::ExtractExtension(confEvent.u.getAgentState.agentState.agentID, key_status.agentID);
				//hcommon::ExtractDomain(confEvent.u.getAgentState.agentState.agentID, key_status.domain);
								
				strcpy(data_status.extension, pData_svcref->extension);
				strcpy(data_status.domain, pData_svcref->domain);
				
				// Top ACD-group 정보만 일단 세팅 추후 Multi group으로 변경
				hcommon::ExtractExtension(confEvent.u.getAgentState.agentState.acdGroup, acdGroup);
				strcpy(data_status.group_info[0].acdGroup, acdGroup);

				if (confEvent.u.getAgentState.agentState.loggedOnState == false) {
					strcpy(data_status.agstate, AGSTATE_UNSTAFFED);
				}
				else {	
					data_status.agtime = event_time;
					data_status.agduration = event_time;
					if (confEvent.u.getAgentState.agentState.agentState == asAgentNotReady) {
						strcpy(data_status.workmode, STATUS_AUX);
						strcpy(data_status.agstate, STATUS_AUX);
						data_status.last_event = CSTAAgentNotReady;
					}
					else if (confEvent.u.getAgentState.agentState.agentState == asAgentReady) {
						strcpy(data_status.workmode, STATUS_READY);
						strcpy(data_status.agstate, STATUS_READY);
						data_status.last_event = CSTAAgentReady;
					}
					else if (confEvent.u.getAgentState.agentState.agentState == asAgentWorkingAfterCall) {
						strcpy(data_status.workmode, STATUS_ACW);
						strcpy(data_status.agstate, STATUS_ACW);
						data_status.last_event = CSTAAgentWorkingAfterCall;
					}
//					else if (confEvent.u.getAgentState.agentState.agentState == asAgentNull) {
//						strcpy(data_status.agstate, STATUS_UNSTAFF);
//					}
//					else if (confEvent.u.getAgentState.agentState.agentState == asAgentBusy) {					
					else {
						// 상태 모름 
						// Busy시에는 OTHER로 처리
						strcpy(data_status.workmode, STATUS_OTHER);
						strcpy(data_status.agstate, STATUS_OTHER);
						data_status.last_event = CSTAAgentBusy;
					}
					
					data_status.tevent_time = event_time;
					data_status.bInitialize = true;   // Initializing
				
					// 일단 agent 목록을 CM에서 받기 전까지는 Unstaffed상태에서는 저장하지 않는다.
					pAgent_status_shm->register_data(key_status, data_status);
					
					pServiceRequested->erase(key_svcref);					
				}
				

			}
		}
		break;
/*
	case CSTAGetForwarding:
		{
			strArr.Add("[CONF] CSTAGetForwarding");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			int count = confEvent.u.getForwarding.forward.count;
			tForwardingInfo* forwardInfos = confEvent.u.getForwarding.forward.param;
			for (int i=0; i<count; i++) {
				temp.Format("forward#%d : deviceID=[%s], forwardingOn=[%d], forwardingType=[%s]", 
					i+1, forwardInfos[i].forwardDN, forwardInfos[i].forwardingOn, CCSTANameString::ForwardingType2String(forwardInfos[i].forwardingType));
				mainDlg_->AddList(temp);
			}
		}
		break;		
*/

/* Snapshot services
	case CSTASnapshotCall:
		{
			strArr.Add("[CONF] CSTASnapshopCall");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);	

			int count = confEvent.u.snapshotCall.snapshotData.count;
			tSnapshotCallResponseInfo*	info = confEvent.u.snapshotCall.snapshotData.info;

			for (int i=0; i<count; i++) {
				temp.Format("callinfo#%d : deviceOnCall=[%s], callID=[%d], deviceID=[%s], state=[%d]", 
					i+1, info[i].deviceOnCall, info[i].callIdentifier.callID, info[i].callIdentifier.deviceID, info[i].localConnectionState);
				mainDlg_->AddList(temp);
			}
		}
		break;

	case CSTASnapshotDevice:
		{
			strArr.Add("[CONF] CSTASnapshotDevice");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);	

			int count = confEvent.u.snapshotDevice.snapshotData.count;
			tSnapshotDeviceResponseInfo* info = confEvent.u.snapshotDevice.snapshotData.info;

			for (int i=0; i<count; i++) 
			{
				CString callState = "";
				CString	callStateSum = "";
				int callCount = info[i].localCallState.count;
				for (int j = 0; j < callCount; j++) 
				{
					callState.Format("%s;", CCSTANameString::LocalConnectionState2String(info[i].localCallState.state[j]));
					callStateSum.Append(callState);
				}
				temp.Format("deviceInfo#%d : callID=[%d], deviceID=[%s], state=[%s]\n", 
					i+1, info[i].callIdentifier.callID, info[i].callIdentifier.deviceID, callStateSum);
				mainDlg_->AddList(temp);
			}

		}
		break;
*/		


	// CSTAError시 온다.
	case CSTAUniversalFailure:
		{
/*
			strArr.Add("[CONF] CSTAUniversalFailure");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("Fail Reason=[%s]", 
				CCSTANameString::UniversalFailure2String(confEvent.u.universalFailure.error));
			mainDlg_->AddList(temp);
*/

			sprintf(message_text, "%s", CCSTANameString::UniversalFailure2String(confEvent.u.universalFailure.error));
			pDevice_status_shm->update_result(confEvent.serviceCrossRefID, 0, (char *)"N", message_text);
			
			break;
		}
/* Etc.
	// vender specific functions
	case HUMQueryAcdGroup:
		{
			strArr.Add("[CONF] HUMQueryAcdGroup");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp); 
			mainDlg_->AddList(strArr, true);

			temp.Format("callsInQueue=[%d], ewt=[%d]", confEvent.u.queryAcdGroup.callsInQueue, confEvent.u.queryAcdGroup.ewt);
			mainDlg_->AddList(temp);
			break;
		}
*/
	default:
		// case 문으로 별도 구현되지 않은 event는 message queue에 넣지 않고 그냥 흘려보낸다.
		
		break;
	}
	
	// Write message queue
	/*
	memset(&data, 0x00, sizeof(EVENTDATA_t));
	data.event_type = etConfirmation;   // Confirmation event
	data.event_type_detail = conf;
	memset(&data.u.confEvent, &confEvent, sizeof(CSTAConfirmationEvent));
	memcpy(&data.privData, &privData, sizeof(tPrivateData));
	time(&data.event_time);
	
	pMQ->message_send(data);
	*/
}


void CSTANotifier::OnUnsolicitedEvent(tUnsolicitedEventType unsol, CSTAUnsolicitedEvent unsolEvent, tPrivateData privData)
{
	EVENTDATA_t  data;
	
	char message_text[LOG_LEN];

	switch (unsol)
	{
	case CSTAAutoAnswer:
	case CSTAAutoWorkMode:
	case CSTACallBack:
	case CSTACallBackMessage:
	case CSTACallerIDStatus:
	case CSTADoNotDisturb:
	case CSTAForwarding:
	case CSTARouteingMode:
		// 이상 이벤트는 통계 처리에 무관한 것으로 판단해서 message queuing하지 않는다.
		break;		
	//////////////// Event for queueing /////////////////////////////
	// Call Control Services & Events - CSTABridged ~ CSTATransferred 
	case CSTABridged:
	case CSTACallCleared:
	case CSTAConferenced:
	case CSTAConnectionCleared:
	case CSTADelivered:
	case CSTADigitsDialled:  // Event 발생되는지 테스트 필요
	case CSTADiverted:
	case CSTAEstablished:
	case CSTAFailed:
	case CSTAHeld:
	case CSTANetworkCapabilitiesChanged:
	case CSTANetworkReached:
	case CSTAOffered:  // ?? 설명 필요
	case CSTAOriginated:
	case CSTAQueued:
	case CSTARetrieved:
	case CSTAServiceInitiated: // 기본적으로 발신시 발생(Cause의 reason을 보고 Consultation, Transfer, Conferenece상황별 판단 필요)
	case CSTATransferred:
		
	// Call Control Services & Events - CSTAAgentBusy ~ CSTARouteingMode 
	case CSTAAgentBusy:
	case CSTAAgentLoggedOff:
	case CSTAAgentLoggedOn:
	case CSTAAgentNotReady:
	case CSTAAgentReady:
	case CSTAAgentWorkingAfterCall:

		// Write message queue
		memset(&data, 0x00, sizeof(EVENTDATA_t));
		data.event_type = etUnsolicited;
		data.event_type_detail = unsol;
		memcpy(&data.u.unsoEvent, &unsolEvent, sizeof(CSTAUnsolicitedEvent));
		memcpy(&data.privData, &privData, sizeof(tPrivateData));
		time(&data.event_time);
		
		std::cout << "message_send act" << std::endl;

		// 테스트 삼아 VDN(1001)이면 Call-Type으로 모니터링을 건다.
		// monitorCrossRefID값을 device_manage_shm객체에서 조회해 type이 2(VDN)인 경우에만
		// 모니터링 걸도록 개발해야 함
/*
		if (unsol == CSTADelivered) {
			tDeliveredEvent deliveredEvent;

			memset(&deliveredEvent, 0x00, sizeof(tDeliveredEvent));
			memcpy(&deliveredEvent, &data.u.unsoEvent.u.delivered, sizeof(tDeliveredEvent));
			
			MONITORING_LIST_SHM_t *ret_data = pDevice_status_shm->find_data(deliveredEvent.monitorCrossRefID);
			if (ret_data != NULL) {
				if (ret_data->device_type == 2) {
					pcsta_->MonitorCall(deliveredEvent.connection);
					std::cout << "monitoring_call ; with callid = " << deliveredEvent.connection.callID << std::endl;
				}
			}
		}
*/
		pMQ->message_send(&data);
		
		// Agent Log on시 agent status Query
		if (unsol == CSTAAgentLoggedOn) {
			tLoggedOnEvent agentLoggedOn;
			
			memset(&agentLoggedOn, 0x00, sizeof(tLoggedOnEvent));
			memcpy(&agentLoggedOn, &data.u.unsoEvent.u.loggedOn, sizeof(tLoggedOnEvent));
			
			GetAgentState(agentLoggedOn.agentDevice);
		}
		
		break;
			
	case CSTAUnsolUndefined:
		{
			/*
			strArr.Add("[UNSOL] CSTAUnsolUndefined");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
			*/
		}
		break;

	case CSTAMonitorEnded:
		{
			/*
			strArr.Add("[UNSOL] CSTAMonitorEnded");
			temp.Format("monitorID=[%d], cause=[%s]", unsolEvent.monitorCrossRefId, CCSTANameString::EventCause2String(unsolEvent.u.monitorEnded.cause));
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
			*/
		}
		
		pDevice_status_shm->update_result(unsolEvent.monitorCrossRefId);
		
		sprintf(message_text, "CSTAMonitorEnded => monitorCrossRefId", unsolEvent.monitorCrossRefId);
		pCommObj->LoggingError(__FILE__, __func__, -1, message_text);		
		
		break;
	}	
}


void CSTANotifier::OnSystemEvent(tSystemEventType sys, CSTASystemEvent sysEvent, tPrivateData privData)
{
	char message_text[LOG_LEN];
	
	EVENTDATA_t  data;

	switch ((int)sys)
	{
	case CSTALinkConnected:
		/*
		{
			strArr.Add("[SYSE] CSTALinkConnected");
			temp.Format("PBX-IP=[%s], PBX-PORT=[%d], ID=[%s], Pass=[%s]", 
				sysEvent.u.linkConnected.pbxIP, sysEvent.u.linkConnected.pbxPort,
				sysEvent.u.linkConnected.id, sysEvent.u.linkConnected.password);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("LinkID=[%s]", sysEvent.u.linkConnected.linkID);
			mainDlg_->AddList(temp);

			temp.Format("Client-IP=[%s], Client-PORT=[%d]", 
				sysEvent.u.linkConnected.clientIP, sysEvent.u.linkConnected.clientPort);
			mainDlg_->AddList(temp);
			mainDlg_->IsLinkConnected(true);
		}
		*/
		linkStatus = 1;
		
		// Re-monitoring start
		
		std::cout << "CSTALinkConnected " << std::endl;

		strcpy(message_text, "CSTALinkConnected");
		pCommObj->LoggingError(__FILE__, __func__, -1, message_text);
			
		break;

	case CSTALinkDisconnected:
		/*
		{
			strArr.Add("[SYSE] CSTALinkDisconnected");
			temp.Format("LinkID=[%s]", sysEvent.u.linkDisconnected.linkID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
			mainDlg_->IsLinkConnected(false);
		}
		*/
		linkStatus = 0;
		monitoringStatus = 0;
		
		// Clear monitoring device list data
		pDevice_status_shm->truncate();
		
		strcpy(message_text, "CSTALinkDisconnected");
		pCommObj->LoggingError(__FILE__, __func__, -1, message_text);
		
		std::cout << "CSTALinkDisconnected " << std::endl;
		break;

	case CSTALinkSystemStatus: 
		/*
		{
			strArr.Add("[SYSE] CSTALinkSystemStatus");
			temp.Format("SystemStatus=[%s]", CCSTANameString::SystemStauts2String(sysEvent.u.linkSystemStatus.systemStatus));
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
		}
		*/
//		linkStatus = 1;
				
		std::cout << "CSTALinkSystemStatus : " << CCSTANameString::SystemStauts2String(sysEvent.u.linkSystemStatus.systemStatus) << std::endl;
		break;
	}
	
	// Write message queue
	memset(&data, 0x00, sizeof(EVENTDATA_t));
	data.event_type = etSystem;
	data.event_type_detail = sys;
	memcpy(&data.u.systEvent, &sysEvent, sizeof(CSTASystemEvent));
	memcpy(&data.privData, &privData, sizeof(tPrivateData));
	time(&data.event_time);	
	
	std::cout << "message_send act" << std::endl;
	
	pMQ->message_send(&data);	
	
}

void	CSTANotifier::AddLocalConnState2List(tLocalConnectionState localstate)
{
	/*
	CString temp = "";
	string	state = CCSTANameString::LocalConnectionState2String(localstate);
	temp.Format("localConnectionInfo=[%s]", state.c_str());

	XPRJExerciserDlg*	mainDlg_	= (XPRJExerciserDlg*)AfxGetApp()->m_pMainWnd;
	mainDlg_->AddList(temp);
	*/
	string	temp = CCSTANameString::LocalConnectionState2String(localstate);
	std::cout << "LocalConnectionState = " << temp << std::endl;
	
}

void CSTANotifier::AddCause2List(tEventCause cause)
{
	/*
	CString temp = "";
	temp.Format("cause=[%s]", CCSTANameString::EventCause2String(cause));

	XPRJExerciserDlg*	mainDlg_	= (XPRJExerciserDlg*)AfxGetApp()->m_pMainWnd;
	mainDlg_->AddList(temp);
	*/
	string	temp = CCSTANameString::EventCause2String(cause);
	std::cout << "cause = " << temp << std::endl;	
}

void CSTANotifier::AddLastRedirectDevice2List(tDeviceID lastRedirectDevice)
{
	/*
	CString temp = "";
	temp.Format("LastRedirectDevice=[%s]", lastRedirectDevice);

	XPRJExerciserDlg*	mainDlg_	= (XPRJExerciserDlg*)AfxGetApp()->m_pMainWnd;
	mainDlg_->AddList(temp);
	*/
	std::cout << "LastRedirectDevice = " << lastRedirectDevice << std::endl;	
}

void CSTANotifier::SetICSTA(ICSTA *__pcsta_) {
	pcsta_ = __pcsta_;
}

void CSTANotifier::SetMessageQueue(mqueue *__pMQ) {
	pMQ = __pMQ;
}

void CSTANotifier::SetSharedMemory(device_manage_shm *__pDevice_status_shm
						, agent_status_shm *__pAgent_status_shm)
{
	pDevice_status_shm = __pDevice_status_shm;
	pAgent_status_shm = __pAgent_status_shm;
}

int  CSTANotifier::GetAgentState(string __extension) {
	SERVICE_CROSS_REFID_KEY_t key;
	SERVICE_CROSS_REFID_t     data;
	

	char extension[EXTENSION_LEN];
	char domain   [DOMAIN_LEN];
	///////////////////////////////////////////////////////
	
	memset(&key, 0x00, sizeof(SERVICE_CROSS_REFID_KEY_t));
	memset(&data, 0x00, sizeof(SERVICE_CROSS_REFID_t));	
	
	///////////////////////////////////////////////////////
	tAgentID		agentID;		
	hstrcpy(agentID, __extension.c_str());
	
	memset(&extension, 0x00, sizeof(extension));
	memset(&domain, 0x00, sizeof(domain));
	
	hcommon::ExtractExtension(agentID, extension);
	hcommon::ExtractDomain(agentID, domain);

	data.device_type = STATION;
	strcpy(data.extension, extension);	
	strcpy(data.domain, domain);

	key.serviceCrossRefID = pcsta_->GetAgentState(agentID);
	
	if (pServiceRequested->insert(key, data) == false) {
		std::cout << "Failed to insert serviceRequest data" << std::endl;
		return -1;
	}
	
	return key.serviceCrossRefID;	
}


//void CSTANotifier::SetServiceRequetObject(
//	manage_umap<SERVICE_CROSS_REFID_KEY_t, SERVICE_CROSS_REFID_t> *__pServiceRequested)
//{
//	pServiceRequested = __pServiceRequested;
//}
