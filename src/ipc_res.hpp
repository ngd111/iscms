//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
//
//////////////////////////////////////////////////////////////////////////////

#ifndef IPC_RESOURCE_INCLUDED
#define IPC_RESOURCE_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "XPRJACEDLLEx/CSTADefineEvent.h"

using namespace IS;

// Used for queueing
struct __events {
    tCSTAEventType event_type;  // tUnsolicitedEventType  : 1
                                // tConfirmationEventType : 2
                                // tSystemEventType       : 3
    short event_type_detail;    // ex) confEvent.CSTAMonitorDevice
                                //     unsoEvent.CSTAHeld
    union
    {
        CSTAConfirmationEvent confEvent;
        CSTAUnsolicitedEvent  unsoEvent;
        CSTASystemEvent       systEvent;
    } u;

    tPrivateData privData;

    time_t event_time;
};

typedef struct __events EVENTDATA_t;



namespace TRACKING_CALL
{
	enum CALL_DISP
	{
		NIL = 0,
		CONNECTED = 1,
		ANSWERED,
		ABANDONED,
		INTERFLOWED,
		FBUSY,
		FDISCONNECTED,
		OTHER
	};

	enum NO_YES
	{
		NO = 0,
		YES = 1
	};

	enum RELEASING_SIDE
	{
		NONE = 0, 
		CALLED_PTY  = 1, // Called(ex, Agent)
		CALLING_PTY = 2  // Caller
	};

	namespace COLUMN_SWITCH_SET
	{
		enum COLUMN_NAME
		{
			HELD = 1,
			//HELDABN, -> Consolidated to HELD
			//ANSHOLDTIME, -> Consolidated to HELD
//			SEGSTART,  -> Consolidated to SEGMENT
//			SEGSTOP,   -> Consolidated to SEGMENT
			SEGMENT,
			TALKTIME,
			RINGTIME,
			QUEUETIME,
			DISPTIME,
			CONSULTTIME
		};

		enum COLUMN_SWITCH
		{
			ON = 1,
			OFF = 2
		};
	};
};

// Type defined when call was made(Using first received event)
/*
enum LOGICAL_CALL_TYPE
{
	ACD = 1,   // Event handle -> VDN event
	DACD,      //              -> Called Device event
	OB,        //              -> Calling Device event
	EXTENSION  //              -> Calling Device event
	// TR
};
*/

struct __calllog_key {
	long  callID;
	short segment;
};

typedef struct __calllog_key CALLLOG_KEY_t;

struct __calllog {
	int        tenant;       // Tenant number(reserved for tenant)
	time_t     acwtime;      // Reserved
	time_t     ansholdtime;
	tAgentID   anslogin;
	TRACKING_CALL::NO_YES agt_released; // Connection cleared by agent(0=No, 1=Yes)
	TRACKING_CALL::CALL_DISP call_disp; // Call disposition(1~7 ; Read operation manual)
	tDeviceID  calling_pty;  // Calling party(ANI or SID(extension)), Trunk #(if possible~)
	TRACKING_CALL::NO_YES conference;   // Conference call(0=No, 1=Yes)
	time_t     consulttime;
	tDeviceID  dialed_num;   // Dialed number by caller. 
	                         // In case inbound ACD call set VDN#,
	                         //         DA call set null value, 
	                         //         outbound call set the dialed number
	short      dispsklevel;  // Agent Skill level(reserved)
	tDeviceID  dispsplit;    // Skill number for call segment(Non-queued call set null value)
	time_t     disptime;     // Inbound call via VDN lead time to the agent.
	                         // Call via VDN but non-queued must be calculated time.
	                         // Extension call not via VDN. Set zero.
	                         // Vectoring + Queueing + Ringing time
	tDeviceID  dispvdn;      // VDN for call segment(The call not related to the VDN, set null value)
	short      held;         // Hold count by agent in the segment
	short      holdabn;      // Mark call abandoned in held by called device.
	tAgentID   origlogin;    // The agent-id who originate the call
	time_t     queuetime;
	time_t     ringtime;
	time_t     segstart;     // Start time of the call segment
	time_t     segstop;      // End time of the call segment
	time_t     talktime;     // Agent talk time. Only inbound call measured.
	                         // IS-CM version => Outbound call can measured time
	TRACKING_CALL::NO_YES transferred;  // Transfered call(0=No, 1=Yes)
	char       ucid[UCID_LENGTH];

//	LOGICAL_CALL_TYPE call_type;    // Call type
	
	tDeviceID  domain;       // Domain name	
};

typedef struct __calllog CALLLOG_t;

struct __calllog_set {
	CALLLOG_KEY_t key;
	CALLLOG_t     data;
};

typedef struct __calllog_set CALLLOG_SET_t;

// Event data 와 call log 를 Union으로 합친다.
// For Logger process
struct __logging_data {
	int data_type;     // 1 or 2
	
	union
	{
		EVENTDATA_t      eventData;  // data_type : 1
		CALLLOG_SET_t    callData;   // data_type : 2
	} u;
};

typedef struct __logging_data  LOGGINGDATA_t;

#endif // IPC_RESOURCE_INCLUDED
