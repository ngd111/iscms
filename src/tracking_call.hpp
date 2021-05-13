//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TRACKING_CALL_INCLUDED
#define TRACKING_CALL_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "XPRJACEDLLEx/CSTADefineBaseType.h"
#include "XPRJACEDLLEx/CSTADefineEvent.h"
#include "statbase.hpp"
#include "manage_umap.hpp"
#include "length_def.hpp"
#include "ipc_res.hpp"
#include "base_def.hpp"

using namespace IS;
//using namespace TRACKING_CALL;

using namespace TRACKING_CALL::COLUMN_SWITCH_SET;

// In memory call tracking log
// If call cleared, tracking log will be wrote to DB table by logger process

// For VDN realtime status list management
typedef struct _tracking_call_key {
	CALLLOG_KEY_t c;
	
    bool operator==(const struct _tracking_call_key &p) const {
    	if ((c.callID == p.c.callID) && (c.segment == p.c.segment)) return true;
    	else return false;
    }
} TRACKING_CALL_KEY_t;

typedef struct _tracking_call_data {
	CALLLOG_t c;
	
	///////////// for management ////////////////////////////////////////////////////
	TRACKING_CALL::RELEASING_SIDE first_releasing;     // For mark releasing side
	DEVICE_TYPE event_handle;   // Mark event handle side(Prohibit handling event dup.)
	bool bRedirected;           // Mark bit for call was redirected
	
	tUnsolicitedEventType last_callcontrol_event;     // Main call control event
	time_t last_callcontrol_event_time;
	
	tUnsolicitedEventType last_callcontrol_sub_event; // Sub call control event(Held, Retrieved~)
	time_t last_callcontrol_sub_event_time;	
	
	tUnsolicitedEventType last_logical_event;		  // Reserved(For agent device)
	time_t last_logical_event_time;				      // Reserved(For agent device)
	
	//////////// for data field switch management ///////////////////////////////////
	TRACKING_CALL::COLUMN_SWITCH_SET::COLUMN_SWITCH sw_held;
	
	TRACKING_CALL::COLUMN_SWITCH_SET::COLUMN_SWITCH sw_segment;
		
	TRACKING_CALL::COLUMN_SWITCH_SET::COLUMN_SWITCH sw_talktime;
	TRACKING_CALL::COLUMN_SWITCH_SET::COLUMN_SWITCH sw_ringtime;
	TRACKING_CALL::COLUMN_SWITCH_SET::COLUMN_SWITCH sw_queuetime;
	TRACKING_CALL::COLUMN_SWITCH_SET::COLUMN_SWITCH sw_disptime;
	TRACKING_CALL::COLUMN_SWITCH_SET::COLUMN_SWITCH sw_consulttime;
				

	_tracking_call_data & operator=(_tracking_call_data& __data) {
		std::cout << "inside tracking_call_data structure" << std::endl;
			
		if (__data.c.tenant > 0) c.tenant = __data.c.tenant;
		if (__data.c.acwtime > 0) c.acwtime = __data.c.acwtime;
		
		// SWITCH 가 동일하면 처리않고 넘어간다.(중복처리 방지)
		if (sw_held != __data.sw_held) {
			if (__data.sw_held == TRACKING_CALL::COLUMN_SWITCH_SET::ON) {
				c.held += __data.c.held;
			}
			else {
				if (__data.c.ansholdtime > 0) {
					std::cout << "ansholdtime : " << __data.c.ansholdtime << std::endl;
					c.ansholdtime += __data.c.ansholdtime;
				}
				c.holdabn += __data.c.holdabn;
			}
			
			sw_held = __data.sw_held;
		}	
			
		if (strlen(__data.c.anslogin) > 0) strcpy(c.anslogin, __data.c.anslogin);
		if (__data.c.agt_released != TRACKING_CALL::NO) c.agt_released = __data.c.agt_released;
		if (__data.c.call_disp > 0) {
			// 값이 이미 세팅 되어 있으면 갱신없이 지나간다.
			if (c.call_disp == TRACKING_CALL::NIL)
				c.call_disp = __data.c.call_disp;
		}
		if (strlen(__data.c.calling_pty) > 0) strcpy(c.calling_pty, __data.c.calling_pty);
		if (__data.c.conference != TRACKING_CALL::NO) c.conference = __data.c.conference;
		if (__data.c.consulttime > 0) {
			// SWITCH 가 동일하면 처리 않고 넘어간다.(중복처리 방지)
			if (sw_consulttime != __data.sw_consulttime) {
				if (__data.sw_consulttime == TRACKING_CALL::COLUMN_SWITCH_SET::ON)
					c.consulttime = __data.c.consulttime;
				else { // OFF
					if (c.consulttime > 0)
						c.consulttime = __data.c.consulttime - c.consulttime;
				}
					
				sw_consulttime = __data.sw_consulttime;
			}
		}
		
		if (strlen(__data.c.dialed_num) > 0) strcpy(c.dialed_num, __data.c.dialed_num);
		if (__data.c.dispsklevel > 0) c.dispsklevel = __data.c.dispsklevel;
		if (strlen(__data.c.dispsplit) > 0) strcpy(c.dispsplit, __data.c.dispsplit);
		
		if (__data.c.disptime > 0) {
			// SWITCH 가 동일하면 처리 않고 넘어간다.(중복처리 방지)
			if (sw_disptime != __data.sw_disptime) {
				if (__data.sw_disptime == TRACKING_CALL::COLUMN_SWITCH_SET::ON)
					c.disptime = __data.c.disptime;
				else {
					if (c.disptime > 0)
						c.disptime = __data.c.disptime - c.disptime;
				}
					
				sw_disptime = __data.sw_disptime;
			}
		}
		
		if (strlen(__data.c.dispvdn) > 0) strcpy(c.dispvdn, __data.c.dispvdn);
		if (strlen(__data.c.origlogin) > 0) strcpy(c.origlogin, __data.c.origlogin);
		
		if (__data.c.queuetime > 0) {
			// SWITCH 가 동일하면 처리 않고 넘어간다.(중복처리 방지)
			if (sw_queuetime != __data.sw_queuetime) {
				if (__data.sw_queuetime == TRACKING_CALL::COLUMN_SWITCH_SET::ON)
					c.queuetime = __data.c.queuetime;
				else {
					if (c.queuetime > 0)
						c.queuetime = __data.c.queuetime - c.queuetime;
				}
					
				sw_queuetime = __data.sw_queuetime;
			}
		}		
		
		if (__data.c.ringtime > 0) {
			// SWITCH 가 동일하면 처리 않고 넘어간다.(중복처리 방지)
			if (sw_ringtime != __data.sw_ringtime) {
				if (__data.sw_ringtime == TRACKING_CALL::COLUMN_SWITCH_SET::ON)
					c.ringtime = __data.c.ringtime;
				else {
					if (c.ringtime > 0)
						c.ringtime = __data.c.ringtime - c.ringtime;
				}
					
				sw_ringtime = __data.sw_ringtime;
			}
		}
		
		// SWITCH 가 동일하면 처리 않고 넘어간다.(중복처리 방지)
		if (sw_segment != __data.sw_segment) {
			if (__data.sw_segment == TRACKING_CALL::COLUMN_SWITCH_SET::ON)
				c.segstart = __data.c.segstart;
			else
				c.segstop = __data.c.segstop;
				
			sw_segment = __data.sw_segment;
		}

		if (__data.c.talktime > 0) {
			// SWITCH 가 동일하면 처리 않고 넘어간다.(중복처리 방지)
			if (sw_talktime != __data.sw_talktime) {
				if (__data.sw_talktime == TRACKING_CALL::COLUMN_SWITCH_SET::ON)
					c.talktime = __data.c.talktime;
				else {
					if (c.talktime > 0)
						c.talktime = (__data.c.talktime - c.talktime) - c.ansholdtime;
				}
					
				sw_talktime = __data.sw_talktime;
			}
		}
					
		if (__data.c.transferred != TRACKING_CALL::NO) c.transferred = __data.c.transferred;
		
		if (__data.last_callcontrol_event_time > 0) {
			last_callcontrol_event_time = __data.last_callcontrol_event_time;
			last_callcontrol_event = __data.last_callcontrol_event;
		}
		else if (__data.last_logical_event_time > 0) {
			last_logical_event_time = __data.last_logical_event_time;
			last_logical_event = __data.last_logical_event;
		}
		
		if (__data.last_callcontrol_sub_event_time > 0) {
			last_callcontrol_sub_event_time = __data.last_callcontrol_sub_event_time;
			last_callcontrol_sub_event = __data.last_callcontrol_sub_event;
		}
		
		if (__data.event_handle > 0)
			event_handle = __data.event_handle;
			
		return *this;
	}

	_tracking_call_data & operator+=(_tracking_call_data& __data) {
		// Not used ; Don't write any code
		return *this;
	}
		
} TRACKING_CALL_t;

inline std::size_t hash_value(struct _tracking_call_key const &p)
{
    std::size_t seed = 0;
    boost::hash_combine(seed, p.c.callID);
    boost::hash_combine(seed, p.c.segment);
    return seed;
};


typedef struct _segment_info_key {
	long       callID;
	
    bool operator==(const struct _segment_info_key &p) const {
    	if (callID == p.callID) return true;
    	else return false;
    }
} CALL_SEGMENT_KEY_t;

typedef struct _segment_info_data {
	short      segment;
	
	_segment_info_data & operator=(_segment_info_data& __data) {
	
		segment = __data.segment;
		return *this;
	}

	_segment_info_data & operator+=(_segment_info_data& __data) {
	
		return *this;
	}    
} CALL_SEGMENT_t;

inline std::size_t hash_value(struct _segment_info_key const &p)
{
    std::size_t seed = 0;
    boost::hash_combine(seed, p.callID);
    return seed;
};


class tracking_call
	 : public statbase<TRACKING_CALL_KEY_t, TRACKING_CALL_t>
{
public:

	~tracking_call();
	tracking_call();

	bool register_data(TRACKING_CALL_KEY_t __key, TRACKING_CALL_t __data, bool __newcall);
	bool read_current_call_log(long __callID, TRACKING_CALL_t& __data_out);
	bool read_first_call_log(long __callID, TRACKING_CALL_t& __data_out);
	
	short get_last_segment(long __callID);

	bool delete_data(long __callID);
	bool delete_current_call_log(long __callID);
	
	bool decide_make_call_log(long __callID
	                        , DEVICE_TYPE __monitored_device_type, tDeviceID __monitored_extension
							, tDeviceID __callingDevice, tDeviceID __calledDevice
							, TRACKING_CALL_t &__data_current_call_log
							, bool &_bIsDataFound);
	
	void set_column_switch(TRACKING_CALL::COLUMN_SWITCH_SET::COLUMN_NAME __column
		                 , TRACKING_CALL::COLUMN_SWITCH_SET::COLUMN_SWITCH __switch
		                 , TRACKING_CALL_t &__data_out
		                 , time_t __event_time);

private:
	manage_umap<CALL_SEGMENT_KEY_t, CALL_SEGMENT_t> *pManageSegmentInfo;
};

#endif // TRACKING_CALL_INCLUDED
