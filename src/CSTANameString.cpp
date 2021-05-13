///////////////////////////////////////////////////////////////////////////////////////
// Copyright (C) hansol inticube. All rights reserved.
///////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************
  
      FileName : CSTANameString.h

      Author   : aro

      Contents : class CCSTANameString

      Date     : 2011/9/1 22:06:25

**************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////
// Update Log
// Date               Author                Comment
// 2011/9/1			  aro					First Create!
///////////////////////////////////////////////////////////////////////////////////////
// 
// collection class of various functions
//
//////////////////////////////////////////////////////////////////////////////////////


//#include "stdafx.h"
#include "XPRJACEDLLEx/CSTANameString.h"
#include "HUMLib/String_Formatter.h"
#include "HUMLib/Utility.h"

#include <vector>

using namespace std;
using namespace IS;

#ifndef	_WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif
    
static char * CSTA_LOCAL_CONNECTION_STATE_STRINGS[] = {
	"none",
	"null",
	"initiated",
	"alerting",
	"connected",
	"hold",
	"queued",
	"fail",

	"LocalConnectionStateEnd"
};

static	char * CSTA_PARTICIPATION_TYPE_STRINGS[] = {
	"active",
	"silent",
	"none",
};

static	char * CSTA_AGENT_STATE_STRINGS[] = {
	"agentNotReady",
	"agentNull",
	"agentReady",
	"agentBusy",
	"agentWorkingAfterCall",
};

static	char * CSTA_REQUESTED_AGENT_STATE_STRINGS[] = {
	"loggedOn",
	"loggedOff",
	"notReady",
	"ready",
	"workingAfterCall",
};

static	char * CSTA_PENDING_AGENT_STATE_STRINGS[] = {
	"agentNotReady",
	"agentNull",
	"agentReady",
	"agentWorkingAfterCall",
};

static	char * CSTA_FORWARDING_TYPE_STRINGS[] = {
	"forwardImmediate",
	"forwardBusy",
	"forwardDND",
	"forwardNoAns",
	"forwardBusyInt",
	"forwardBusyExt",
	"forwardDNDInt",
	"forwardDNDExt",
	"forwardNoAnsInt",
	"forwardNoAnsExt",
	"forwardImmInt",
	"forwardImmExt",

	"forwardingTypeEnd",
};

static char * CSTA_EVENT_CAUSE_STRINGS[] = {
	"unknown",
	"aCDBusy",				// 0
	"aCDForward",
	"aCDSaturated",
	"activation",
	"activeParticipation",
	"alertTimeExpired",
	"alternate",
	"autoWork",
	"babble",
	"badAttribute",
	"badGrammar",
	"badVoice",
	"badURI",
	"blocked",
	"busy",
	"busyOverflow",
	"calendarOverflow",
	"callBack",
	"callCancelled",
	"callForward",
	"callForwardBusy",
	"callForwardImmediate",
	"callForwardNoAnswer",
	"callNotAnswered",
	"callPickup",
	"campOn",
	"campOnTrunks",
	"capacityOverflow",
	"characterCountReached",
	"conference",
	"consultation",
	"destDetected",
	"destNotObtainable",
	"destOutOfOrder",
	"distributed",
	"distributionDelay",
	"doNotDisturb",
	"dTMFDigitDetected",
	"duplicateDTMF",
	"durationExceeded",
	"earlyStop",
	"emptyQueue",
	"endOfMessageDetected",
	"enteringDistribution",
	"forcedPause",
	"forcedTransition",
	"incompatibleDestination",
	"interdigitTimeout",		// interDigitTimeout ?
	"intrude",
	"invalidAccountCode",
	"invalidConnection",
	"invalidConnectionState",
	"invalidNumberFormat",
	"joinCall",
	"keyOperation",
	"keyOperationInUse",
	"lockout",
	"maintenance",
	"makeCall",
	"makeConnection",
	"makePredictiveCall",
	"maxTimeout",
	"messageDurationExceeded",
	"messageSizeExceeded",
	"multipleAlerting",
	"multipleQueuing",
	"networkCongestion",
	"networkDialling",
	"networkNotObtainable",
	"networkOutOfOrder",
	"networkSignal",
	"newCall",
	"nextMessage",
	"noAudioSaved",
	"noAvailableAgents",
	"noQueue",
	"normal",
	"normalClearing",
	"noRule",
	"noSpeechDetected",
	"notAvailableBearerService",
	"notSupportedBearerService",
	"numberChanged",
	"numberUnallocated",
	"outOfGrammar",
	"overflow",
	"override",
	"park",
	"pathReplacement",
	"queueCleared",
	"queueTimeOverflow",
	"recall",
	"recallBusy",
	"recallForwarded",
	"recallNoAnswer",
	"recallResourcesNotAvailable",
	"redirected",
	"remainsInQueue",
	"reorderTone",
	"reserved",
	"resourcesNotAvailable",
	"selectedTrunkBusy",
	"silentParticipation",
	"singleStepConference",
	"singleStepTransfer",
	"speechDetected",
	"suspend",
	"switchingFunctionTerminated",
	"terminationCharacterReceived",
	"timeout",
	"transfer",
	"trunksBusy",
	"unauthorizedBearerService",
	"unknownOverflow",
	// end of tEventCause
	"cSTAEventCauseEnd",
};


static char * CSTA_EVENT_TYPE_STRINGS[] = {
	"Undefined",
	"Confirmation",
	"Unsolicited",
	"ConfError"
};


static char * CONF_EVENT_TYPE_STRINGS[] = {
	"CSTAConfUndefined",
	// Sesion Services
	"CSTAOpenSession",
	"CSTACloseSession",
	"CSTAResetSessionTimer",
	// Capability Exchange Services [P3]
	"CSTAGetCSTAFeatures",
	"CSTAGetLogicalDeviceInformation",
	"CSTAGetPhysicalDeviceInformation",
	"CSTAGetSwitchingFunctionCapabilities",
	"CSTAGetSwitchingFunctionDevice",
	"CSTASwitchingFunctionDevices",
	// System Services [P3]
	"CSTAChangeSystemStatusFilter",
	"CSTASystemRegister",
	"CSTASystemRegisterAbort",
	"CSTASystemRegisterCancel",
	"CSTAGetRegistrations",
	"CSTARegistrationInfo",
	"CSTARequestSystemStatus",
	"CSTASystemStatus",
	"CSTASwitchingFunctionCapabilitiesChanged",
	"CSTASwitchingFunctionDevicesChanged",
	// Monitoring Serivces
	"CSTAChangeMonitorFilter",
	"CSTAMonitorStart",		// for "CSTA
	"CSTAMonitorCall",
	"CSTAMonitorDevice",
	"CSTAMonitorCallViaDevice",
	"CSTAMonitorStop",
	"CSTAGetMonitors",
	"CSTAMonitorInfo",
	// Snapshot Services	[P3]
	"CSTASnapshotCall",
	"CSTASnapshotCallData",
	"CSTAsnapshotDevice",
	"CSTAsnapshotDeviceData",
	// Call Control Services
	"CSTAAcceptCall",
	"CSTAAlternateCall",
	"CSTAAnswerCall",
	"CSTACallBackCallRelated",
	"CSTACallBackMessageCallRelated",
	"CSTACampOnCall",
	"CSTAClearCall",
	"CSTAClearConnection",
	"CSTAConferenceCall",
	"CSTAConsultationCall",
	"CSTADeflectCall",
	"CSTADialDigits",
	"CSTADirectedPickupCall",
	"CSTAGroupPickupCall",
	"CSTAHoldCall",
	"CSTAIntrudeCall",
	"CSTAJoinCall",
	"CSTAMakeCall",
	"CSTAMakeConnection",
	"CSTAMakePredictiveCall",
	"CSTAParkCall",
	"CSTAReconnectCall",
	"CSTARetrieveCall",
	"CSTASendMessage",
	"CSTASingleStepConferenceCall",
	"CSTASingleStepTransferCall",
	"CSTATransferCall",
	// Call Associated Features		[P3]
	"CSTAAssociateData",
	"CSTACancelTelephonyTones",
	"CSTAChangeConnectionInformation",
	"CSTAGenerateDigits",
	"CSTAGenerateTelephonyTones",
	"CSTASendUserInformation",
	// Media Attachments Services
	// Routeing Services
	// Physical Device Features
	// Logical Device Features
	"CSTACallBackNonCallRelated",
	"CSTACallBackMessageNonCallRelated",
	"CSTACancelCallBack",
	"CSTACancelCallBackMessage",
	"CSTAGetAgentState",
	"CSTAGetAutoAnswer",
	"CSTAGetAutoWorkMode",
	"CSTAGetCallBack",
	"CSTAGetCallerIDStatus",
	"CSTAGetDoNotDisturb",
	"CSTAGetForwarding",
	"CSTAGetLastNumberDialled",
	"CSTAGetRouteingMode",
	"CSTASetAgentState",
	"CSTASetAutoAnswer",
	"CSTASetAutoWorkMode",
	"CSTASetCallerIDStatus",
	"CSTASetDoNotDisturb",
	"CSTASetForwarding",
	"CSTASetRouteingMode",
	// Device Maintenance Events	[P4]
	// I/O Services
	// Data Collection Services		[P3]
	"CSTADataCollected",
	"CSTADataCollectionResumed",
	"CSTADataCollectionSuspended",
	"CSTAResumeDataCollection",
	"CSTAStartDataCollectionCall",
	"CSTAStartDataCollectionDevice",
	"CSTAStopDataCollection",
	"CSTASuspendDataCollection",
	// Voice Services
	// Call Detail Records Services
	// Location Services
	// Vendor Specific Extensions Services	[P3]
	"CSTAEscapeRegister",
	"CSTAEscapeRegisterAbort",
	"CSTAEscapeRegisterCancel",
	"CSTAEscape",
	"CSTAPrivateDataVersionSelection",
	"HUMQueryAcdGroup",

	"CSTAUniversalFailure"
};


static char * UNSOL_EVENT_TYPE_STRINGS[] = {
	"UnsolUndefined",
	// Call Control Services
	"Bridged",
	"CallCleared",
	"Conferenced",
	"ConnectionCleared",
	"Delivered",
	"DigitsDialled",
	"Diverted",
	"Established",
	"Failed",
	"Held",
	"NetworkCapabilitiesChanged",
	"NetworkReached",
	"Offered",
	"Originated",
	"Queued",
	"Retrieved",
	"ServiceInitiated",
	"Transferred",
	// Logical Device Features
	"AgentBusy",
	"AgentLoggedOff",
	"AgentLoggedOn",
	"AgentNotReady",
	"AgentReady",
	"AgentWorkingAfterCall",
	"AutoAnswer",
	"AutoWorkMode",
	"CallBack",
	"CallBackMessage",
	"CallerIDStatus",
	"DoNotDisturb",
	"Forwarding",
	"RouteingMode",
	"MonitorEnded",
};


static char * SYSTEM_EVENT_TYPE_STRINGS[] = {
	"SystemUndefined",
	// Call Control Services
	"LinkConnected",
	"LinkDisconnected",
	"LinkSystemStatus",
};


static char * CSTA_UNIVERSAL_FAILURE_STRINGS[] = {
	// Unspecified Errors
	"genericUnspecified",							// 0
	// Operation Errors
	"atLeastOneConditionalParameterNotProvided",
	"featureAlreadySet",
	"invalidMessageID",
	"invalidParameterValue",
	"invalidAccountCode",
	"invalidAgentGroup",
	"invalidAgentIdentifier",
	"invalidAgentPassword", 
	"invalidAgentState", 
	"invalidAlertTime", 
	"invalidAllocationState", 
	"invalidAuthorizationCode", 
	"invalidAutoAnswer", 
	"invalidBitRate", 
	"invalidButtonIdentifier", 
	"invalidButtonLabel", 
	"invalidCallType", 
	"invalidConnectionRate",
	"invalidConsultPurpose", 
	"invalidCorrelatorData", 
	"invalidCrossReferenceID", 
	"invalidDelayTolerance", 
	"invalidDestination", 
	"invalidDestinationDetect", 
	"invalidDoNotDisturb", 
	"invalidEscapeCrossReferenceID", 
	"invalidFeature", 
	"invalidFile", 
	"invalidFlowDirection", 
	"invalidForwardingDestination", 
	"invalidForwardingFlag", 
	"invalidForwardingType", 
	"invalidHookswitchType", 
	"invalidHookswitchComponent", 
	"invalidLampID", 
	"invalidLampMode", 
	"invalidMessageWaitingSetting", 
	"invalidMicrophoneGain", 
	"invalidMicrophoneMute", 
	"invalidMonitorCrossReferenceID", 
	"invalidMonitorFilter", 
	"invalidMonitorObject", 
	"invalidMonitorType", 
	"invalidNumberOfChannels", 
	"invalidParticipationType",
	"invalidRemainRetry", 
	"invalidRingCount", 
	"invalidRingPattern", 
	"invalidRingVolume", 
	"invalidRouteingAlgorithm", 
	"invalidRouteingCrossReferenceID", 
	"invalidRouteRegistrationCrossReferenceID", 
	"invalidSpeakerVolume", 
	"invalidSpeakerMute", 
	"invalidSwitchingSubdomainCharsType", 
	"invalidObjectType",
	"invalidActiveCallObject",
	"invalidCalledDeviceObjectType", 
	"invalidCallingDeviceObjectType",
	"invalidCallToBePickedUpObjectType", 
	"invalidCallToDivertObjectType", 
	"invalidCallToParkObjectType", 
	"invalidDestinationDeviceObject", 
	"invalidHeldCallObject", 
	"invalidMonitorObjectType", 
	"invalidParkToObjectType",
	"messageIDRequired",
	"notDifferentDevices", 
	"notSameDevice", 
	"objectNotKnown", 
	"invalidCallID", 
	"invalidActiveCallID", 
	"invalidHeldCallID", 
	"invalidConnectionID", 
	"invalidActiveConnectionID", 
	"invalidHeldConnectionID", 
	"invalidDeviceID", 
	"invalidActiveDeviceID", 
	"invalidCalledDeviceID", 
	"invalidCallingDeviceID", 
	"invalidCallToParkDeviceID",
	"invalidDestinationDeviceID",
	"invalidDivertingDeviceID",
	"invalidHeldDeviceID", 
	"invalidParkToDeviceID", 
	"invalidPickUpDeviceID", 
	"parameterNotSupported", 
	"accountCodeNotSupported", 
	"agentGroupNotSupported", 
	"agentPasswordNotSupported", 
	"agentStateNotSupported", 
	"alertTimeNotSupported", 
	"allocationNotSupported", 
	"authorisationCodeNotSupported", 
	"autoAnswerNotSupported", 
	"bitRateNotSupported", 
	"buttonNotSupported", 
	"callTypeNotSupported", 
	"charactersToSendNotSupported", 
	"connectionRateNotSupported", 
	"connectionReservationNotSupported",
	"consultPurposeNotSupported", 
	"correlatorDataNotSupported", 
	"delayToleranceNotSupported", 
	"destinationDetectNotSupported", 
	"digitModeNotSupported", 
	"errorValueNotSupported", 
	"flowDirectionNotSupported",
	"forwardingDestinationNotSupported", 
	"lampNotSupported", 
	"monitorTypeNotSupported", 
	"numberOfChannelsNotSupported", 
	"parameterTypeNotSupported", 
	"priorityNotSupported", 
	"privateDataNotSupported", 
	"pulseDurationNotSupported",
	"pulseRateNotSupported", 
	"remainRetryNotSupported", 
	"ringCountNotSupported", 
	"routeUsedNotSupported", 
	"securityNotSupported", 
	"switchingSubDomainCCIETypeNotSupported", 
	"toneDurationNotSupported", 
	"sysStatRegIDNotSupported", 
	"userDataNotSupported", 
	"privilegeViolationSpecifiedDevice", 
	"privilegeViolationActiveDevice", 
	"privilegeViolationCalledDevice", 
	"privilegeViolationCallingDevice", 
	"privilegeViolationCallToParkDevice", 
	"privilegeViolationDestinationDevice", 
	"privilegeViolationOnDivertingDevice", 
	"privilegeViolationHeldDevice", 
	"privilegeViolationOnParkToDevice",
	"privilegeViolationPickupDevice", 
	"routeingTimerExpired", 
	"requestIncompatibleWithObject", 
	"requestIncompatibleWithConnection", 
	"requestIncompatibleWithActiveConnection", 
	"requestIncompatibleWithHeldConnection", 
	"requestIncompatibleWithDevice", 
	"requestIncompatibleWithCalledDevice", 
	"requestIncompatibleWithCallingDevice",
	"requestIncompatibleWithSubjectDevice",
	"requestIncompatibleWithActiveDevice", 
	"requestIncompatibleWithCallToParkDevice", 
	"requestIncompatibleWithDestinationDevice",
	"requestIncompatibleWithDivertingDevice",
	"requestIncompatibleWithHeldDevice", 
	"requestIncompatibleWithMedia", 
	"requestIncompatibleWithParkToDevice", 
	"requestIncompatibleWithPickupDevice", 
	"serviceNotSupported", 
	"securityViolation",
	"valueOutOfRange", 
	"agentStateOutOfRange", 
	"alertTimeOutOfRange", 
	"allocationOutOfRange",
	"autoAnswerOutOfRange",
	"bitRateOutOfRange",
	"callTypeOutOfRange",
	"connectionRateOutOfRange", 
	"connectionReservationOutOfRange", 
	"consultPurposeOutOfRange", 
	"correlatorDataOutOfRange", 
	"delayToleranceOutOfRange", 
	"destinationDetectOutOfRange", 
	"digitModeOutOfRange", 
	"doNotDisturbOutOfRange",
	"flowDirectionOutOfRange",
	"forwardingFlagOutOfRange",
	"forwardingTypeOutOfRange",
	"hookswitchComponentOutOfRange", 
	"hookSwithTypeOutOfRange", 
	"lampModeOutOfRange", 
	"messageWaitingSettingOutOfRange", 
	"micGainOutOfRange", 
	"micMuteOutOfRange", 
	"monitorTypeOutOfRange", 
	"numberOfChannelsOutOfRange", 
	"participationTypeOutOfRange",
	"pulseDurationOutOfRange",
	"pulseRateOutOfRange", 
	"ringCountOutOfRange", 
	"ringPatternOutOfRange",
	"ringVolumnOutOfRange",
	"routingAlgorithmOutOfRange",
	"speakerMuteOutOfRange",
	"speakerVolumeOutOfRange",
	"switchingCcittType", 
	"systemStatusOutOfRange", 
	"toneCharacterOutOfRange",
	"toneDurationOutOfRange", 
	// Security Errors
	"sequenceNumberViolated", 
	"timeStampViolated", 
	"securityInfoViolated",
	// State Incompatibility Errors
	"invalidObjectState", 
	"invalidDeviceState", 
	"connectedCallExists", 
	"invalidActiveDeviceState", 
	"invalidCalledDeviceState", 
	"invalidCallingDeviceState", 
	"invalidCallToParkDeviceState", 
	"invalidDestinationDeviceState", 
	"invalidDivertingDeviceState", 
	"invalidHeldDeviceState", 
	"invalidParkToDeviceState", 
	"invalidConnectionState", 
	"invalidActiveConnectionState", 
	"invalidConnectionIDForActiveCall", 
	"invalidHeldConnectionState", 
	"noActiveCall", 
	"noCallToAnswer",
	"noCallToClear", 
	"noCallToComplete", 
	"noConnectionToClear", 
	"noHeldCall", 
	"incorrectMessageState", 
	"beginningOfMessage", 
	"endOfMessage", 
	"messageSuspended", 
	"notAbleToPlay", 
	"notAbleToResume",
	// System Resource Availability Errors
	"resourceBusy", 
	"internalResourceBusy", 
	"classifierBusy", 
	"noMediaChannelsAvailable", 
	"channelsInUseForBridgedDevices", 
	"channelsInUseForData", 
	"toneDetectorBusy", 
	"toneGeneratorBusy", 
	"networkBusy", 
	"resourceOutOfService", 
	"deviceOutOfService", 
	"activeDeviceOutOfService", 
	"calledDeviceOutOfService", 
	"callingDeviceOutOfService", 
	"callToParkDeviceOutOfService", 
	"destinationDeviceOutOfService", 
	"divertingDeviceOutOfService", 
	"heldDeviceOutOfService",
	"parkToDeviceOutOfService", 
	"pickupDeviceOutOfService", 
	"networkOutOfService", 
	"otherResourceOutOfService", 
	"resourceLimitExceeded", 
	"overallMonitorLimitExceeded", 
	"conferenceMemberLimitExceeded", 
	"registrationLimitExceeded", 
	"conferenceResourceCannotBeSatisfied", 
	"maxDevicesExceeded",
	"startTimeOutOfRange",
	"durationExceeded", 
	// Subscribed Resource Availability Errors
	"objectMonitorLimitExceeded", 
	"trunkLimitExceeded", 
	"outstandingRequestsLimitExceeded", 
	"objectRegistrationLimitExceeded", 
	// Performance Management Errors
	"performanceLimitExceeded",
	// Private Data Information Errors
	"cSTAPrivateDataInfoError",
	// Private Error Code
	"requestTimeout",							// request timeout [1/27/2012 aro]
	"sipErrorClient",							// SIP error client (4XX) [2/15/2012 aro]
	"sipErrorServer",							// SIP error server (5XX) [2/15/2012 aro]
	"badRequest",								// 'bad request' response from IP-PBX [2/15/2012 aro]
	"callLegDoesNotExist",						// 'Call Leg/Transaction Does not Exist' response from IP-PBX [2/15/2012 aro]
	"accessForbidden",							// '403 Forbidden'
	"licenseDoesNotExist",						// license does not exist [2012/12/11 aro]
	"licenseLimitExceeded",						// license limit exceeded [2012/12/11 aro]

	"CSTAUniversalFailureEnd"
};

static char * CSTA_SYSTEM_STATUS_STRINGS[] = {
	"initializing",
	"enabled",
	"normal",
	"messagesLost",
	"disabled",
	"partiallyDisabled",
	"overloadImminent",
	"overloadReached",
	"overloadRelieved",
	"tSystemStatusEnd"
};


CCSTANameString::CCSTANameString(void)
{
}

CCSTANameString::~CCSTANameString(void)
{
}

const char*		CCSTANameString::UniversalFailure2String(tUniversalFailure fail)
{
	if ( tUniversalFailureEnd <= fail )	fail = genericUnspecified;
	return CSTA_UNIVERSAL_FAILURE_STRINGS[(int)fail];
}

tUniversalFailure	CCSTANameString::UniversalFailureFromString(const char* fail)
{
	for ( int i=0; i<(int)tUniversalFailureEnd; i++ )
	{
		if ( !strcmp(fail, CSTA_UNIVERSAL_FAILURE_STRINGS[i]) )		return (tUniversalFailure)i;
//		if ( !_stricmp(fail, CSTA_UNIVERSAL_FAILURE_STRINGS[i]) )	return (tUniversalFailure)i;
	}
	return genericUnspecified;
}

const char*		CCSTANameString::EventCause2String(tEventCause ec)
{
	if ( tEventCauseEnd <= ec )	ec = ecUnknown;
	return CSTA_EVENT_CAUSE_STRINGS[(int)ec];
}

tEventCause	CCSTANameString::EventCauseFromString(const char* ec)
{
	for ( int i=0; i<(int)tEventCauseEnd; i++ )
	{
		if ( !strcmp(ec, CSTA_EVENT_CAUSE_STRINGS[i]) )		return (tEventCause)i;
//		if ( !_stricmp(ec, CSTA_EVENT_CAUSE_STRINGS[i]) )	return (tEventCause)i;
	}
	return ecUnknown;
}

const char*		CCSTANameString::ForwardingType2String(tForwardingType ft)
{
	if ( tForwardingTypeEnd <= ft )	ft = ftForwardImmediate;
	return CSTA_FORWARDING_TYPE_STRINGS[(int)ft];
}

tForwardingType	CCSTANameString::ForwardingTypeFromString(const char* ft)
{
	for ( int i=0; i<(int)tForwardingTypeEnd; i++ )
	{
		if ( !strcmp(ft, CSTA_FORWARDING_TYPE_STRINGS[i]) )		return (tForwardingType)i;
//		if ( !_stricmp(ft, CSTA_FORWARDING_TYPE_STRINGS[i]) )	return (tForwardingType)i;
	}
	return ftForwardImmediate;
}

const char*		CCSTANameString::LocalConnectionState2String(tLocalConnectionState lcs)
{
	if ( tLocalConnectionStateEnd <= lcs )		lcs = csNone;
	return CSTA_LOCAL_CONNECTION_STATE_STRINGS[(int)lcs+1];
}

tLocalConnectionState	CCSTANameString::LocalConnectionStateFromString(const char* str)
{
	for ( int i=0; i<=(int)tLocalConnectionStateEnd; i++ )
	{
		if ( !strcmp(str, CSTA_LOCAL_CONNECTION_STATE_STRINGS[i+1]) )	return (tLocalConnectionState)i;
//		if ( !_stricmp(str, CSTA_LOCAL_CONNECTION_STATE_STRINGS[i+1]) )	return (tLocalConnectionState)i;
	}
	return csNone;
}

const char*		CCSTANameString::ParticipationType2String(tParticipationType lcs)
{
	if ( tParticipationTypeEnd <= lcs )	lcs = ptNone;
	return CSTA_PARTICIPATION_TYPE_STRINGS[(int)lcs];
}

tParticipationType	CCSTANameString::ParticipationTypeFromString(const char* str)
{
	for ( int i=0; i<(int)tParticipationTypeEnd; i++ )
	{
		if ( !strcmp(str, CSTA_PARTICIPATION_TYPE_STRINGS[i]) )		return (tParticipationType)i;
//		if ( !_stricmp(str, CSTA_PARTICIPATION_TYPE_STRINGS[i]) )	return (tParticipationType)i;
	}
	return ptNone;
}

const char*		CCSTANameString::RequestedAgentState2String(tRequestedAgentState rast)
{
	if ( tRequestedAgentStateEnd <= rast )	rast = rasLoggedOff;
	return CSTA_REQUESTED_AGENT_STATE_STRINGS[(int)rast];
}

tRequestedAgentState	CCSTANameString::RequestedAgentStateFromString(const char* str)
{
	for ( int i=0; i<(int)tRequestedAgentStateEnd; i++ )
	{
		if ( !strcmp(str, CSTA_REQUESTED_AGENT_STATE_STRINGS[i]) )		return (tRequestedAgentState)i;
//		if ( !_stricmp(str, CSTA_REQUESTED_AGENT_STATE_STRINGS[i]) )	return (tRequestedAgentState)i;
	}
	return rasLoggedOff;
}

const char*		CCSTANameString::AgentState2String(tAgentState ast)
{
	if ( tAgentStateEnd <= ast )	ast = asAgentNull;
	return CSTA_AGENT_STATE_STRINGS[(int)ast];
}

tAgentState	CCSTANameString::AgentStateFromString(const char* str)
{
	for ( int i=0; i<(int)tAgentStateEnd; i++ )
	{
		if ( !strcmp(str, CSTA_AGENT_STATE_STRINGS[i]) )	return (tAgentState)i;
//		if ( !_stricmp(str, CSTA_AGENT_STATE_STRINGS[i]) )	return (tAgentState)i;
	}
	return asAgentNull;
}

const char*		CCSTANameString::PendingAgentState2String(tPendingAgentState pas)
{
	return CSTA_PENDING_AGENT_STATE_STRINGS[(int)pas];
}

tPendingAgentState	CCSTANameString::PendingAgentStateFromString(const char* lppas)
{
	for ( int i=0; i<(int)tPendingAgentStateEnd; i++ )
	{
		if ( !strcmp(lppas, CSTA_PENDING_AGENT_STATE_STRINGS[i]) )		return (tPendingAgentState)i;
//		if ( !_stricmp(lppas, CSTA_PENDING_AGENT_STATE_STRINGS[i]) )	return (tPendingAgentState)i;
	}
	return pasAgentNull;
}


const char*		CCSTANameString::CSTAEventType2String(tCSTAEventType cet)
{
	return CSTA_EVENT_TYPE_STRINGS[(int)cet];
}

const char*		CCSTANameString::CSTAUnsolicitedEventType2String(tUnsolicitedEventType uet)
{
	return UNSOL_EVENT_TYPE_STRINGS[(int)uet];
}

const char*		CCSTANameString::CSTAConfirmationEventType2String(tConfirmationEventType cet)
{
	return CONF_EVENT_TYPE_STRINGS[(int)cet];
}

const char*		CCSTANameString::CSTASystemEventType2String(tSystemEventType set)
{
	return SYSTEM_EVENT_TYPE_STRINGS[(int)set];
}

const char*		CCSTANameString::SystemStauts2String(tSystemStatus ss)
{
	return CSTA_SYSTEM_STATUS_STRINGS[(int)ss];
}

tSystemStatus	CCSTANameString::SystemStatusFromString(const char* str)
{
	for ( int i=0; i<(int)tSystemStatusEnd; i++ )
	{
		if ( !strcmp(str, CSTA_SYSTEM_STATUS_STRINGS[i]) )		return (tSystemStatus)i;
//		if ( !_stricmp(str, CSTA_SYSTEM_STATUS_STRINGS[i]) )	return (tSystemStatus)i;
	}
	return ssInitializing;
}

#ifndef	_WIN32
#pragma GCC diagnostic pop
#endif
