#include "CSTANotifier.h"

#include "XPRJACEDLLEx/CSTANameString.h"
#include "XPRJACEDLLEx/CSTADefineEvent.h"
#include "XPRJACEDLLEx/CSTAEvent.h"
#include <iostream>
#include <vector>


CSTANotifier::CSTANotifier(void)
: sessionID_("")
{
}

CSTANotifier::~CSTANotifier(void)
{
	sessionID_ = "";
}


// CSTADefineEvent.h 에 define되어 있다.
void	CSTANotifier::NotifyEvent(CCSTAEvent& event)
{
std::cout << "Inside NotifyEvent : Type   [" << event.Type() << "]" << std::endl;
std::cout << "                   : Ctype  [" << event.ConfirmationType() << "]" << std::endl;
//std::cout << "                   : Event  [" << event.ConfirmationEvent() << "]" << std::endl;
//std::cout << "                   : Private[" << event.PrivateData() << "]" << std::endl;


	switch (event.Type())
	{
	case etUndefinedType:
		{
		}
		break;

	case etConfirmation:
		{
			OnConfirmationEvent(event.ConfirmationType(), event.ConfirmationEvent(), event.PrivateData());
		}
		break;

	case etUnsolicited:
		{
			OnUnsolicitedEvent(event.UnsolicitedType(), event.UnsolicitedEvent(), event.PrivateData());
		}
		break;

	case etSystem:
		{
//			OnSystemEvent(event.SystemType(), event.SystemEvent(), event.PrivateData());
		}
		break;

	case etConfError:
		{

		}
		break;
	}
}


// 구현되지 않은 이벤트는 흘려보내도록 한다.
void	CSTANotifier::OnConfirmationEvent(tConfirmationEventType conf, CSTAConfirmationEvent confEvent, tPrivateData privData)
{
//	CString				temp		= "";
//	CStringArray		strArr;
//	XPRJExerciserDlg*	mainDlg_	= (XPRJExerciserDlg*)AfxGetApp()->m_pMainWnd;

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
			/*
			strArr.Add("[CONF] CSTAOpenSession");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
			
			sessionID_ = confEvent.u.openSession.sessionID;
			
			temp.Format("sessionID=[%s]", sessionID_.c_str());
			mainDlg_->AddList(temp);

			temp.Format("protocolVer=[%s]", confEvent.u.openSession.actualProtocolVersion);
			mainDlg_->AddList(temp);

			temp.Format("sessionDuration=[%d]", confEvent.u.openSession.actualSessionDuration);
			mainDlg_->AddList(temp);

			mainDlg_->IsLinkConnected(true);			
			*/
		
			temp = "InvokeID=";
			temp += confEvent.serviceCrossRefID;
			strArr.push_back(temp);

			sessionID_ = confEvent.u.openSession.sessionID;
			
			SetSessionStatus(1);

			std::cout << "sessionID  =[" << sessionID_ << "]" << std::endl;
			std::cout << "protocolVer=[" << confEvent.u.openSession.actualProtocolVersion << "]" << std::endl;
			std::cout << "sessionDuration=[" << confEvent.u.openSession.actualSessionDuration << "]" << std::endl;
		}
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
		
		SetSessionStatus(0);
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
		{
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
			vmonlist.push_back(confEvent.u.monitorDevice.monitorCrossRefID);
		}
		break;

	case CSTAMonitorCallViaDevice: // VDN Monitoring 시작시 event?
		{
			/*
			strArr.Add("[CONF] CSTAMonitorCallViaDevice");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			int monitorID = confEvent.u.monitorCallViaDevice.monitorCrossRefID;
			int agentFilter = confEvent.u.monitorCallViaDevice.monitorFilter.agent;
			int callFilter = confEvent.u.monitorCallViaDevice.monitorFilter.call;
			int featureFilter = confEvent.u.monitorCallViaDevice.monitorFilter.feature;

			temp.Format("monitorID=[%d]", monitorID);
			mainDlg_->AddList(temp);

			temp.Format("agentFilter=[%d] | callFilter=[%d] | featureFilter=[%d]", agentFilter, callFilter, featureFilter);
			mainDlg_->AddList(temp);
			*/
			
			vmonlist.push_back(confEvent.u.monitorCallViaDevice.monitorCrossRefID);	
		}
		break;

	case CSTAMonitorStop:
		{
			/*
			strArr.Add("[CONF] CSTAMonitorStop");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
			*/
			
		}
		// confEvent.serviceCrossRefID는 Get Monitors를 제외하면 활용측면에서 현재로서는 큰 의미가 없다.
		//std::cout << "CSTAMonitorStop received : RefID -> [" << confEvent.serviceCrossRefID  <<  "]" << std::endl;
		
		// Issue : monitorCrossRefID로 MonitorStop을 request했으나 Stop이벤트 수신시 어떤 Extension에 대한 Monitoring 중지에 대한 
		// response인지 알 수가 없다.
	
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

			strArr.Add("[CONF] CSTAGetAgentState");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			if (confEvent.u.getAgentState.agentState.loggedOnState)
			{
				temp.Format("agentID=[%s], agentState=[%s], acdGroup=[%s], reasonCode=[%s]", 
					confEvent.u.getAgentState.agentState.agentID,
					CCSTANameString::AgentState2String(confEvent.u.getAgentState.agentState.agentState),
					confEvent.u.getAgentState.agentState.acdGroup,
					privData.reasonCode ? privData.reasonCode : "NULL");
			}
			else
			{
				temp.Format("Agent Null");
			}
			mainDlg_->AddList(temp);
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


/* Etc.
	case CSTAUniversalFailure:
		{
			strArr.Add("[CONF] CSTAUniversalFailure");
			temp.Format("InvokeID=[%d]", confEvent.serviceCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("Fail Reason=[%s]", 
				CCSTANameString::UniversalFailure2String(confEvent.u.universalFailure.error));
			mainDlg_->AddList(temp);
			break;
		}

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
}


void	CSTANotifier::OnUnsolicitedEvent(tUnsolicitedEventType unsol, CSTAUnsolicitedEvent unsolEvent, tPrivateData privData)
{
//	CStringArray		strArr;
//	XPRJExerciserDlg*	mainDlg_	= (XPRJExerciserDlg*)AfxGetApp()->m_pMainWnd;
//	CString				temp		= "";

	switch (unsol)
	{
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

		// Call Control Services & Events - CSTABridged ~ CSTATransferred 
	case CSTABridged:
		{
			/*
			strArr.Add("[UNSOL] CSTABridged");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("bridgedConnection : callID=[%d], deviceID=[%s]", 
				unsolEvent.u.bridged.bridgedConnection.callID, unsolEvent.u.bridged.bridgedConnection.deviceID);
			mainDlg_->AddList(temp);

			temp.Format("bridgedAppearance=[%s]", unsolEvent.u.bridged.bridgedAppearance);
			mainDlg_->AddList(temp);

			temp.Format("UCID=[%s]", privData.UCID);
			mainDlg_->AddList(temp);

			AddLocalConnState2List(unsolEvent.u.bridged.localConnectionInfo);
			AddCause2List(unsolEvent.u.bridged.cause);
			*/
		}
		break;

	case CSTACallCleared:
		{
			/*
			strArr.Add("[UNSOL] CSTACallCleared");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("clearedCall : callID=[%d], deviceID=[%s]", 
				unsolEvent.u.callCleared.clearedCall.callID, unsolEvent.u.callCleared.clearedCall.deviceID);
			mainDlg_->AddList(temp);

			temp.Format("UCID=[%s]", privData.UCID);
			mainDlg_->AddList(temp);

			AddCause2List(unsolEvent.u.callCleared.cause);
			*/
		}
		break;

	case CSTAConferenced:
		{
			/*
			strArr.Add("[UNSOL] CSTAConferenced");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("primaryOldCall : callID=[%d], deviceID=[%s]", 
				unsolEvent.u.conferenced.primaryOldCall.callID, unsolEvent.u.conferenced.primaryOldCall.deviceID);
			mainDlg_->AddList(temp);

			temp.Format("secondaryOldCall : callID=[%d], deviceID=[%s]", 
				unsolEvent.u.conferenced.secondaryOldCall.callID, unsolEvent.u.conferenced.secondaryOldCall.deviceID);
			mainDlg_->AddList(temp);

			temp.Format("conferencingDevice=[%s], addedParty=[%s]", 
				unsolEvent.u.conferenced.conferencingDevice, unsolEvent.u.conferenced.addedParty);
			mainDlg_->AddList(temp);

			int count = unsolEvent.u.conferenced.conferenceConnections.count;
			for (int i=0; i<count; i++)
			{
				temp.Format("Conn[%d] : callID=[%d], deviceID=[%s]", i+1,
					unsolEvent.u.conferenced.conferenceConnections.connection[i].callID,
					unsolEvent.u.conferenced.conferenceConnections.connection[i].deviceID);
				mainDlg_->AddList(temp);
			}

			temp.Format("UCID=[%s]", privData.UCID);
			mainDlg_->AddList(temp);

			AddLocalConnState2List(unsolEvent.u.conferenced.localConnectionInfo);
			AddCause2List(unsolEvent.u.conferenced.cause);
			*/
		}
		break;

	case CSTAConnectionCleared:
		{
			/*
			strArr.Add("[UNSOL] CSTAConnectionCleared");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("droppedConnection : callID=[%d], deviceID=[%s]", 
				unsolEvent.u.connectionCleared.droppedConnection.callID, unsolEvent.u.connectionCleared.droppedConnection.deviceID);
			mainDlg_->AddList(temp);

			temp.Format("releasingDevice=[%s]", unsolEvent.u.connectionCleared.releasingDevice);
			mainDlg_->AddList(temp);

			temp.Format("UCID=[%s]", privData.UCID);
			mainDlg_->AddList(temp);

			AddLocalConnState2List(unsolEvent.u.connectionCleared.localConnectionInfo);
			AddCause2List(unsolEvent.u.connectionCleared.cause);
			*/
		}
		break;

	case CSTADelivered:
		{
			/*
			strArr.Add("[UNSOL] CSTADelivered");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("connection : callID=[%d], deviceID=[%s]", 
				unsolEvent.u.delivered.connection.callID, unsolEvent.u.delivered.connection.deviceID);
			mainDlg_->AddList(temp);

			temp.Format("alertingDevice=[%s], callingDevice=[%s], calledDevice=[%s]", 
				unsolEvent.u.delivered.alertingDevice,
				unsolEvent.u.delivered.callingDevice,
				unsolEvent.u.delivered.calledDevice);
			mainDlg_->AddList(temp);

			temp.Format("UCID=[%s]", privData.UCID);
			mainDlg_->AddList(temp);

			AddLastRedirectDevice2List(unsolEvent.u.delivered.lastRedirectionDevice);
			AddLocalConnState2List(unsolEvent.u.delivered.localConnectionInfo);
			AddCause2List(unsolEvent.u.delivered.cause);
			*/
		}
		break;

	case CSTADigitsDialled:  // Event 발생되는지 테스트 필요
		{
			/*
			strArr.Add("[UNSOL] CSTADigitsDialled");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("diallingConnection : callID=[%d], deviceID=[%s]", 
				unsolEvent.u.digitsDialled.diallingConnection.callID, unsolEvent.u.digitsDialled.diallingConnection.deviceID);
			mainDlg_->AddList(temp);

			temp.Format("diallingDevice=[%s], diallingSequence=[%s]", 
				unsolEvent.u.digitsDialled.diallingDevice,
				unsolEvent.u.digitsDialled.diallingSequence);
			mainDlg_->AddList(temp);

			temp.Format("UCID=[%s]", privData.UCID);
			mainDlg_->AddList(temp);

			AddLocalConnState2List(unsolEvent.u.digitsDialled.localConnectionInfo);
			AddCause2List(unsolEvent.u.digitsDialled.cause);
			*/
		}
		break;

	case CSTADiverted:
		{
			/*
			strArr.Add("[UNSOL] CSTADiverted");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("connection : callID=[%d], deviceID=[%s]", 
				unsolEvent.u.diverted.connection.callID, unsolEvent.u.diverted.connection.deviceID);
			mainDlg_->AddList(temp);

			temp.Format("divertingDevice=[%s], newDestination=[%s]", 
				unsolEvent.u.diverted.divertingDevice,
				unsolEvent.u.diverted.newDestination);
			mainDlg_->AddList(temp);

			temp.Format("UCID=[%s]", privData.UCID);
			mainDlg_->AddList(temp);

			AddLastRedirectDevice2List(unsolEvent.u.diverted.lastRedirectionDevice);
			AddLocalConnState2List(unsolEvent.u.diverted.localConnectionInfo);
			AddCause2List(unsolEvent.u.diverted.cause);
			*/
		}
		break;

	case CSTAEstablished:
		{
			/*
			strArr.Add("[UNSOL] CSTAEstablished");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("establishedConnection : callID=[%d], deviceID=[%s]", 
				unsolEvent.u.established.establishedConnection.callID, unsolEvent.u.established.establishedConnection.deviceID);
			mainDlg_->AddList(temp);

			temp.Format("answeringDevice=[%s], callingDevice=[%s], calledDevice=[%s]", 
				unsolEvent.u.established.answeringDevice,
				unsolEvent.u.established.callingDevice,
				unsolEvent.u.established.calledDevice);
			mainDlg_->AddList(temp);

			temp.Format("UCID=[%s]", privData.UCID);
			mainDlg_->AddList(temp);

			AddLastRedirectDevice2List(unsolEvent.u.established.lastRedirectionDevice);
			AddLocalConnState2List(unsolEvent.u.established.localConnectionInfo);
			AddCause2List(unsolEvent.u.established.cause);
			*/
		}
		break;

	case CSTAFailed:
		{
			/*
			strArr.Add("[UNSOL] CSTAFailed");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("failedConnection : callID=[%d], deviceID=[%s]", 
				unsolEvent.u.failed.failedConnection.callID, unsolEvent.u.failed.failedConnection.deviceID);
			mainDlg_->AddList(temp);

			temp.Format("faillingDevice=[%s], callingDevice=[%s], calledDevice=[%s]", 
				unsolEvent.u.failed.faillingDevice,
				unsolEvent.u.failed.callingDevice,
				unsolEvent.u.failed.calledDevice);
			mainDlg_->AddList(temp);

			temp.Format("UCID=[%s]", privData.UCID);
			mainDlg_->AddList(temp);

			AddLastRedirectDevice2List(unsolEvent.u.failed.lastRedirectionDevice);
			AddLocalConnState2List(unsolEvent.u.failed.localConnectionInfo);
			AddCause2List(unsolEvent.u.failed.cause);
			*/
		}
		break;

	case CSTAHeld:
		{
			/*
			strArr.Add("[UNSOL] CSTAHeld");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("heldConnection : callID=[%d], deviceID=[%s]", 
				unsolEvent.u.held.heldConnection.callID, unsolEvent.u.held.heldConnection.deviceID);
			mainDlg_->AddList(temp);

			temp.Format("holdingDevice=[%s]", unsolEvent.u.held.holdingDevice);
			mainDlg_->AddList(temp);

			temp.Format("UCID=[%s]", privData.UCID);
			mainDlg_->AddList(temp);

			AddLocalConnState2List(unsolEvent.u.held.localConnectionInfo);
			AddCause2List(unsolEvent.u.held.cause);
			*/
		}
		break;

	case CSTANetworkCapabilitiesChanged:
		{
			/*
			strArr.Add("[UNSOL] CSTANetworkCapabilitiesChanged");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
			*/
		}
		break;

	case CSTANetworkReached:
		{
			/*
			strArr.Add("[UNSOL] CSTANetworkReached");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("outboundConnection : callID=[%d], deviceID=[%s]", 
				unsolEvent.u.networkReached.outboundConnection.callID, unsolEvent.u.networkReached.outboundConnection.deviceID);
			mainDlg_->AddList(temp);

			temp.Format("networkInterfaceUsed=[%s], callingDevice=[%s], calledDevice=[%s]", 
				unsolEvent.u.networkReached.networkInterfaceUsed,
				unsolEvent.u.networkReached.callingDevice,
				unsolEvent.u.networkReached.calledDevice);
			mainDlg_->AddList(temp);

			temp.Format("UCID=[%s]", privData.UCID);
			mainDlg_->AddList(temp);

			AddLastRedirectDevice2List(unsolEvent.u.networkReached.lastRedirectionDevice);
			AddLocalConnState2List(unsolEvent.u.networkReached.localConnectionInfo);
			AddCause2List(unsolEvent.u.networkReached.cause);
			*/
		}
		break;

	case CSTAOffered:  // ?? 설명 필요
		{
			/*
			strArr.Add("[UNSOL] CSTAOffered");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("offeredConnection : callID=[%d], deviceID=[%s]", 
				unsolEvent.u.offered.offeredConnection.callID, unsolEvent.u.offered.offeredConnection.deviceID);
			mainDlg_->AddList(temp);

			temp.Format("offeredDevice=[%s], callingDevice=[%s], calledDevice=[%s]", 
				unsolEvent.u.offered.offeredDevice,
				unsolEvent.u.offered.callingDevice,
				unsolEvent.u.offered.calledDevice);
			mainDlg_->AddList(temp);

			temp.Format("UCID=[%s]", privData.UCID);
			mainDlg_->AddList(temp);

			AddLastRedirectDevice2List(unsolEvent.u.offered.lastRedirectionDevice);
			AddLocalConnState2List(unsolEvent.u.offered.localConnectionInfo);
			AddCause2List(unsolEvent.u.offered.cause);
			*/
		}
		break;

	case CSTAOriginated:
		{
			/*
			strArr.Add("[UNSOL] CSTAOriginated");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("originatedConnection : callID=[%d], deviceID=[%s]", 
				unsolEvent.u.originated.originatedConnection.callID, unsolEvent.u.originated.originatedConnection.deviceID);
			mainDlg_->AddList(temp);

			temp.Format("callingDevice=[%s], calledDevice=[%s]", 
				unsolEvent.u.originated.callingDevice,
				unsolEvent.u.originated.calledDevice);
			mainDlg_->AddList(temp);

			temp.Format("UCID=[%s]", privData.UCID);
			mainDlg_->AddList(temp);

			AddLocalConnState2List(unsolEvent.u.originated.localConnectionInfo);
			AddCause2List(unsolEvent.u.originated.cause);
			*/
		}
		break;

	case CSTAQueued:
		{
			/*
			strArr.Add("[UNSOL] CSTAQueued");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("queuedConnection : callID=[%d], deviceID=[%s]", 
				unsolEvent.u.queued.queuedConnection.callID, unsolEvent.u.queued.queuedConnection.deviceID);
			mainDlg_->AddList(temp);

			temp.Format("queue=[%s], callingDevice=[%s], calledDevice=[%s]", 
				unsolEvent.u.queued.queue,
				unsolEvent.u.queued.callingDevice,
				unsolEvent.u.queued.calledDevice);
			mainDlg_->AddList(temp);

			temp.Format("UCID=[%s]", privData.UCID);
			mainDlg_->AddList(temp);

			AddLastRedirectDevice2List(unsolEvent.u.queued.lastRedirectionDevice);
			AddLocalConnState2List(unsolEvent.u.queued.localConnectionInfo);
			AddCause2List(unsolEvent.u.queued.cause);
			*/
		}
		break;

	case CSTARetrieved:
		{
			/*
			strArr.Add("[UNSOL] CSTARetrieved");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("retrievedConnection : callID=[%d], deviceID=[%s]", 
				unsolEvent.u.retrieved.retrievedConnection.callID, unsolEvent.u.retrieved.retrievedConnection.deviceID);
			mainDlg_->AddList(temp);

			temp.Format("retrievingDevice=[%s]", unsolEvent.u.retrieved.retrievingDevice);
			mainDlg_->AddList(temp);

			temp.Format("UCID=[%s]", privData.UCID);
			mainDlg_->AddList(temp);

			AddLocalConnState2List(unsolEvent.u.retrieved.localConnectionInfo);
			AddCause2List(unsolEvent.u.retrieved.cause);
			*/
		}
		break;

	case CSTAServiceInitiated: // 기본적으로 발신시 발생(Cause의 reason을 보고 Consultation, Transfer, Conferenece상황별 판단 필요)
		{
			/*
			strArr.Add("[UNSOL] CSTAServiceInitiated");
//			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			temp.Format("monitorID=[%d]", unsolEvent.u.serviceInitiated.monitorCrossRefID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("initiatedConnection : callID=[%d], deviceID=[%s]", 
				unsolEvent.u.serviceInitiated.initiatedConnection.callID, unsolEvent.u.serviceInitiated.initiatedConnection.deviceID);
			mainDlg_->AddList(temp);

			temp.Format("initiatingDevice=[%s]", unsolEvent.u.serviceInitiated.initiatingDevice);
			mainDlg_->AddList(temp);

			temp.Format("UCID=[%s]", privData.UCID);
			mainDlg_->AddList(temp);

			AddLocalConnState2List(unsolEvent.u.serviceInitiated.localConnectionInfo);
			AddCause2List(unsolEvent.u.serviceInitiated.cause);
			*/
		}
		break;

	case CSTATransferred:
		{
			/*
			strArr.Add("[UNSOL] CSTATransferred");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("primaryOldCall : callID=[%d], deviceID=[%s]", 
				unsolEvent.u.transferred.primaryOldCall.callID, unsolEvent.u.transferred.primaryOldCall.deviceID);
			mainDlg_->AddList(temp);

			temp.Format("secondaryOldCall : callID=[%d], deviceID=[%s]", 
				unsolEvent.u.transferred.secondaryOldCall.callID, unsolEvent.u.transferred.secondaryOldCall.deviceID);
			mainDlg_->AddList(temp);

			temp.Format("transferringDevice=[%s], transferredToDevice=[%s]", 
				unsolEvent.u.transferred.transferringDevice, unsolEvent.u.transferred.transferredToDevice);
			mainDlg_->AddList(temp);

			int count = unsolEvent.u.transferred.transferredConnections.count;
			for (int i=0; i<count; i++)
			{
				temp.Format("Conn[%d] : callID=[%d], deviceID=[%s]", i+1,
					unsolEvent.u.transferred.transferredConnections.connection[i].callID,
					unsolEvent.u.transferred.transferredConnections.connection[i].deviceID);
				mainDlg_->AddList(temp);
			}

			temp.Format("UCID=[%s]", privData.UCID);
			mainDlg_->AddList(temp);

			AddLocalConnState2List(unsolEvent.u.transferred.localConnectionInfo);
			AddCause2List(unsolEvent.u.transferred.cause);
			*/
		}
		break;

	case CSTAAgentBusy:
		{
			/*
			strArr.Add("[UNSOL] CSTAAgentBusy");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("agentID=[%s], agentDevice=[%s], acdGroup=[%s]", 
				unsolEvent.u.agentBusy.agentID, unsolEvent.u.agentBusy.agentDevice, unsolEvent.u.agentBusy.acdGroup);
			mainDlg_->AddList(temp);

			temp.Format("pendingAgentState=[%d]", unsolEvent.u.agentBusy.pendingAgentState);
			mainDlg_->AddList(temp);

			AddCause2List(unsolEvent.u.agentBusy.cause);
			*/
		}
		break;

	case CSTAAgentLoggedOff:
		{
			/*
			strArr.Add("[UNSOL] CSTAAgentLoggedOff");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("agentID=[%s], agentDevice=[%s], acdGroup=[%s]", 
				unsolEvent.u.loggedOff.agentID, unsolEvent.u.loggedOff.agentDevice, unsolEvent.u.loggedOff.acdGroup);
			mainDlg_->AddList(temp);

			AddCause2List(unsolEvent.u.loggedOff.cause);
			*/
		}
		break;

	case CSTAAgentLoggedOn:
		{
			/*
			strArr.Add("[UNSOL] CSTAAgentLoggedOn");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("agentID=[%s], agentDevice=[%s], acdGroup=[%s]", 
				unsolEvent.u.loggedOn.agentID, unsolEvent.u.loggedOn.agentDevice, unsolEvent.u.loggedOn.acdGroup);
			mainDlg_->AddList(temp);

			AddCause2List(unsolEvent.u.loggedOn.cause);
			*/
		}
		break;

	case CSTAAgentNotReady:
		{
			/*
			strArr.Add("[UNSOL] CSTAAgentNotReady");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("agentID=[%s], agentDevice=[%s], acdGroup=[%s], reason=[%s]", 
				unsolEvent.u.notReady.agentID, unsolEvent.u.notReady.agentDevice, unsolEvent.u.notReady.acdGroup, privData.reasonCode);
			mainDlg_->AddList(temp);

			AddCause2List(unsolEvent.u.notReady.cause);
			*/
		}
		break;

	case CSTAAgentReady:
		{
			/*
			strArr.Add("[UNSOL] CSTAAgentReady");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("agentID=[%s], agentDevice=[%s], acdGroup=[%s]", 
				unsolEvent.u.ready.agentID, unsolEvent.u.ready.agentDevice, unsolEvent.u.ready.acdGroup);
			mainDlg_->AddList(temp);

			AddCause2List(unsolEvent.u.ready.cause);
			*/
		}
		break;

	case CSTAAgentWorkingAfterCall:
		{
			/*
			strArr.Add("[UNSOL] CSTAAgentWorkingAfterCall");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("agentID=[%s], agentDevice=[%s], acdGroup=[%s]", 
				unsolEvent.u.workingAfterCall.agentID, unsolEvent.u.workingAfterCall.agentDevice, unsolEvent.u.workingAfterCall.acdGroup);
			mainDlg_->AddList(temp);

			temp.Format("pendingAgentState=[%d]", unsolEvent.u.workingAfterCall.pendingAgentState);
			mainDlg_->AddList(temp);

			AddCause2List(unsolEvent.u.workingAfterCall.cause);
			*/
		}
		break;

	case CSTADoNotDisturb:
		{
			/*
			strArr.Add("[UNSOL] CSTADoNotDisturb");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("device=[%s], doNotDisturbOn=[%d], callOrigination(internal)=[%d], callOrigination(external)=[%d]", 
				unsolEvent.u.doNotDisturb.device, unsolEvent.u.doNotDisturb.doNotDisturbOn,
				unsolEvent.u.doNotDisturb.callOrigination.internal,
				unsolEvent.u.doNotDisturb.callOrigination.external);
			mainDlg_->AddList(temp);
			*/
		}
		break;

	case CSTAForwarding:
		{
			/*
			strArr.Add("[UNSOL] CSTAForwarding");
			temp.Format("monitorID=[%d]", unsolEvent.monitorCrossRefId);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);

			temp.Format("device=[%s], forwardTo=[%s], forwardStatus=[%d]", 
				unsolEvent.u.forwarding.device, unsolEvent.u.forwarding.forwardTo, unsolEvent.u.forwarding.forwardStatus);
			mainDlg_->AddList(temp);

			temp.Format("forwardingType=[%s], forwardDefault=[%d]", 
				CCSTANameString::ForwardingType2String(unsolEvent.u.forwarding.forwardingType),
				unsolEvent.u.forwarding.forwardDefault);
			mainDlg_->AddList(temp);

			temp.Format("ringCount=[%d], ringDuration=[%d]", 
				unsolEvent.u.forwarding.ringCount, unsolEvent.u.forwarding.ringDuration);
			mainDlg_->AddList(temp);
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
		break;
	}
}

/*
void	CSTANotifier::OnSystemEvent(tSystemEventType sys, CSTASystemEvent sysEvent, tPrivateData privData)
{
	CStringArray		strArr;
	XPRJExerciserDlg*	mainDlg_	= (XPRJExerciserDlg*)AfxGetApp()->m_pMainWnd;
	CString				temp		= "";

	switch ((int)sys)
	{
	case CSTALinkConnected:
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
		break;

	case CSTALinkDisconnected:
		{
			strArr.Add("[SYSE] CSTALinkDisconnected");
			temp.Format("LinkID=[%s]", sysEvent.u.linkDisconnected.linkID);
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
			mainDlg_->IsLinkConnected(false);
		}
		break;

	case CSTALinkSystemStatus: 
		{
			strArr.Add("[SYSE] CSTALinkSystemStatus");
			temp.Format("SystemStatus=[%s]", CCSTANameString::SystemStauts2String(sysEvent.u.linkSystemStatus.systemStatus));
			strArr.Add(temp);
			mainDlg_->AddList(strArr, true);
		}
		break;
	}
}
*/
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

void	CSTANotifier::AddCause2List(tEventCause cause)
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

void	CSTANotifier::AddLastRedirectDevice2List(tDeviceID lastRedirectDevice)
{
	/*
	CString temp = "";
	temp.Format("LastRedirectDevice=[%s]", lastRedirectDevice);

	XPRJExerciserDlg*	mainDlg_	= (XPRJExerciserDlg*)AfxGetApp()->m_pMainWnd;
	mainDlg_->AddList(temp);
	*/
	std::cout << "LastRedirectDevice = " << lastRedirectDevice << std::endl;	
}
