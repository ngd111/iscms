///////////////////////////////////////////////////////////////////////////////////////
// Copyright (C) hansol inticube. All rights reserved.
///////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************
  
      FileName : ICSTA.h

      Author   : aro

      Contents : class ICSTA

      Date     : 2011/9/22 13:28:08

**************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////
// Update Log
// Date               Author                Comment
// 21/11/1998	      aro               First Create!
///////////////////////////////////////////////////////////////////////////////////////
// 
// Interface of CSTA object
// All CSTA children is derived from it !
//
//////////////////////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "XPRJACEDLLEx/ICSTA.h"
//#include "XPRJACEDLLEx/XLog.h"

#ifndef	_WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"
#endif	//_WIN32


ICSTA::ICSTA(ICSTANotifier& notee)
{
}

ICSTA::~ICSTA(void)
{
}

// Session Services
tRetCode	ICSTA::OpenCSTASession(string applicationInfo, int requestedSessionDuration)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::CloseCSTASession(string sessionID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::ResetCSTASessionTimer(void)
{
	return eReturnInvalidInstance;
}

// Capability Exchange Services
tRetCode	ICSTA::GetCSTAFeatures()
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::GetLogicalDeviceInformation(tDeviceID deviceID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::GetPhysicalDeviceInformation(tDeviceID deviceID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::GetSwitchingFunctionCapabilities()
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::GetSwitchingFunctionDevice()
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SwitchingFunctionDevices()
{
	return eReturnInvalidInstance;
}

// System Services
tRetCode	ICSTA::ChangeSystemStatusFilter(tSysStatRegisterID sysStatRegisterID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SystemRegister(int requestTypes)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SystemRegisterAbort(tSysStatRegisterID sysStatRegisterID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SystemRegisterCancel(tSysStatRegisterID sysStatRegisterID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::GetRegistrations()
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::RegistrationInfo(tServiceCrossRefID serviceCrossRefID, bool lastSegment, string registrationList)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::RequestSystemStatus(tSysStatRegisterID sysStatRegisterID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SystemStatus(tSysStatRegisterID sysStatRegisterID, tSystemStatus systemStatus)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SwitchingFunctionCapabilitiesChanged(tSysStatRegisterID sysStatRegisterID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SwitchingFunctionDevicesChanged(tSysStatRegisterID sysStatRegisterID)
{
	return eReturnInvalidInstance;
}

// Monitoring Services
tRetCode	ICSTA::ChangeMonitorFilter(tMonitorCrossRefID crossRefIdentifier, int requestedFilterList)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::MonitorCall(tConnectionID monitoringConnID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::MonitorDevice(tDeviceID monitoringDeviceID, tMonitorFilter monitorFilter)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::MonitorCallViaDevice(tDeviceID monitoringDeviceID, tMonitorFilter monitorFilter)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::MonitorStop(tMonitorCrossRefID monitorID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::GetMonitors()
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::MonitorInfo(tServiceCrossRefID serviceCrossRefID, bool lastSegment, string monitorList)
{
	return eReturnInvalidInstance;
}

// Snapshot Services
tRetCode	ICSTA::SnapshotCall(tConnectionID connID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SnapshotCallData(tServiceCrossRefID serviceCrossRefID, bool lastSegment, string snapshotData)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SnapshotDevice(tDeviceID deviceID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SnapshotDeviceData(tServiceCrossRefID serviceCrossRefID, bool lastSegment, string snapshotData)
{
	return eReturnInvalidInstance;
}

// Call Control Services
tRetCode	ICSTA::AcceptCall(tConnectionID callToBeAccepted)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::AlternateCall(tConnectionID heldCall, tConnectionID activeCall)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::AnswerCall(tConnectionID callToBeAnswered)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::CallBackCallRelated(tConnectionID callbackConnection)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::CallBackMessageCallRelated(tConnectionID callbackMessageConnection)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::CampOnCall(tConnectionID camponConnection)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::ClearCall(tConnectionID callToBeCleared)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::ClearConnection(tConnectionID connectionToBeCleared)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::ConferenceCall(tConnectionID heldCall, tConnectionID activeCall)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::ConsultationCall(tConnectionID existingCall, tDeviceID consultedDevice)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::DeflectCall(tConnectionID callToBeDiverted, tDeviceID newDestination)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::DialDigits(tConnectionID diallingConnection, string diallingSequence)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::DirectedPickupCall(tConnectionID callToBePickedUp, tDeviceID requestingDevice)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::GroupPickupCall(tDeviceID newDestination)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::HoldCall(tConnectionID callToBeHeld, bool partyPlay)		// added for hold party play [5/21/2012 aro])
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::IntrudeCall(tConnectionID intrude)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::JoinCall(tConnectionID activeCall, tDeviceID joiningDevice)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::MakeCall(tDeviceID callingDevice, tDeviceID calledDirectoryNumber, int autoOriginated, int callCharacteristics)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::MakeConnection(tDeviceID initiatingDevice)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::MakePredictiveCall(tDeviceID callingDevice, tDeviceID calledDirectoryNumber)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::ParkCall(tConnectionID parking, tDeviceID parkTo)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::ReconnectCall(tConnectionID heldCall, tConnectionID activeCall)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::RetrieveCall(tConnectionID callToBeRetrieved, bool partyPlay)		// added for hold party play [5/21/2012 aro])
{
	return eReturnInvalidInstance;
}

//tRetCode	ICSTA::SendMessage(tDeviceID callingDevice, tDeviceList destinationDevices, tMessageInfoItemList messageInfo)
tRetCode	ICSTA::SendMessage(tDeviceID callingDevice, tDeviceID destinationDevice, tMessageInfoItem messageInfo)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SingleStepConferenceCall(tConnectionID activeCall, tDeviceID deviceToJoin, tParticipationType participationType)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SingleStepTransferCall(tConnectionID activeCall, tDeviceID transferredTo, int callCharacteristics)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::TransferCall(tConnectionID heldCall, tConnectionID activeCall)
{
	return eReturnInvalidInstance;
}

// Call Associated Features
tRetCode	ICSTA::AssociateData(tConnectionID existingCall)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::CancelTelephonyTones(tConnectionID connectionToStopTone)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::ChangeConnectionInformation(tConnectionID connectionToBeChanged, string requestedConnectionInfo)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::GenerateDigits(tConnectionID connectionToSendDigits, string charactersToSend)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::GenerateTelephonyTones(tConnectionID connectionToSendTone, string toneToSend)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SendUserInformation(tConnectionID existingCall, string userData)
{
	return eReturnInvalidInstance;
}

// Media Attachments Services

// Routeing Services

// Physical Device Features

// Logical Device Features
tRetCode	ICSTA::CallBackNonCallRelated(tDeviceID originatingDevice, tDeviceID targetDevice)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::CallBackMessageNonCallRelated(tDeviceID originatingDevice, tDeviceID targetDevice)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::CancelCallBack(tDeviceID originatingDevice, tDeviceID targetDevice)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::CancelCallBackMessage(tDeviceID originatingDevice, tDeviceID targetDevice)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::GetAgentState(tDeviceID deviceID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::GetAutoAnswer(tDeviceID deviceID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::GetAutoWorkMode(tDeviceID deviceID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::GetCallBack(tDeviceID deviceID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::GetCallerIDStatus(tDeviceID deviceID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::GetDoNotDisturb(tDeviceID deviceID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::GetForwarding(tDeviceID deviceID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::GetLastNumberDialled(tDeviceID deviceID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::GetRouteingMode(tDeviceID deviceID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SetAgentState(tDeviceID deviceID, tRequestedAgentState requestedAgentState, tAgentID agentID, char* password, tAgentID group, int reasonCode)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SetAutoAnswer(tDeviceID deviceID, bool autoAnswerOn)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SetAutoWorkMode(tDeviceID deviceID, bool autoWorkOn)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SetCallerIDStatus(tDeviceID deviceID, bool callerIDProvided)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SetDoNotDisturb(tDeviceID deviceID, bool doNotDisturbOn)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SetForwarding(tDeviceID deviceID, tForwardingType forwardingType, bool activateForward, tDeviceID forwardDN, int ringDuration)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SetRouteingMode(tDeviceID deviceID, bool routeingMode)
{
	return eReturnInvalidInstance;
}

// I/O Services

// Data Collection Services
tRetCode	ICSTA::DataCollected(tDcollCrossRefID dcollCrossRefID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::DataCollectionResumed(tDcollCrossRefID dcollCrossRefID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::DataCollectionSuspended(tDcollCrossRefID dcollCrossRefID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::ResumeDataCollection(tDcollCrossRefID dcollCrossRefID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::StartDataCollectionCall(tConnectionID connID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::StartDataCollectionDevice(tDeviceID deviceID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::StopDataCollection(tDcollCrossRefID dcollCrossRefID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::SuspendDataCollection(tDcollCrossRefID dcollCrossRefID)
{
	return eReturnInvalidInstance;
}

// Voice Services

// Call Detail Records Services

// Location Services

// Vendor Specific Extensions Services
tRetCode	ICSTA::EscapeRegister()
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::EscapeRegisterAbort(tEscapeRegisterID escapeRegisterID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::EscapeRegisterCancel(tEscapeRegisterID escapeRegisterID)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::Escape(string privateData)
{
	return eReturnInvalidInstance;
}

tRetCode	ICSTA::PrivateDataVersionSelection(string privateDataVersion)
{
	return eReturnInvalidInstance;
}

// vendor specific services [2014/03/12 aro]
// private Service
tRetCode	ICSTA::QueryAcdGroup(tDeviceID acdID)
{
	return eReturnInvalidInstance;
}


void	ICSTA::SetNotifier(ICSTANotifier* pnotee)
{
	HLOGE("[%s] abnormal routine... it should be not happen !", __FUNCTION__);
}

void	ICSTA::Release()
{
	HLOGE("[%s] abnormal routine... it should be not happen !", __FUNCTION__);
	delete this;
}

//void	ICSTA::SetConfig(string serverIP, int serverPort, string loginID, string loginPassword, string serverDomain)
//void	ICSTA::SetConfig(CCSTAConfig config)
void	ICSTA::SetPBXInfo(CPBXConfig& conf)
{
	HLOGE("[%s] abnormal routine... it should be not happen !", __FUNCTION__);
	return;
}

string	ICSTA::GetVersion()
{
	HLOGE("[%s] abnormal routine... it should be not happen !", __FUNCTION__);
	return "0.0.0";
}

#ifndef	_WIN32
#pragma GCC diagnostic pop
#endif	//_WIN32
