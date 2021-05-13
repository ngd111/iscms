#include "tracking_call.hpp"
#include "ipc_def.hpp"

tracking_call::~tracking_call() {
	if (pManageSegmentInfo != NULL) {
		delete pManageSegmentInfo;	pManageSegmentInfo = NULL;
	}
	
///	if (pManageCallInfo != NULL) {
///		delete pManageCallInfo;	pManageCallInfo = NULL;
///	}	
}

tracking_call::tracking_call() {
	create_resource_objects(MTX_TRACKING_CALL);
	
	pManageSegmentInfo = new manage_umap<CALL_SEGMENT_KEY_t, CALL_SEGMENT_t>;
	if (pManageSegmentInfo == NULL) {
		std::cerr << "Failed to allocate memory(Call Segment Info.)" << std::endl;
	}
	
///	pManageCallInfo = new manage_umap<CALL_PROCESS_INFO_KEY_t, CALL_PROCESS_INFO_t>;
///	if (pManageCallInfo == NULL) {
///		std::cerr << "Failed to allocate memory(Call Info.)" << std::endl;
///	}	
}

bool tracking_call::register_data(TRACKING_CALL_KEY_t __key, TRACKING_CALL_t __data
									, bool __NewCallLog) {
	CALL_SEGMENT_KEY_t key_segment;
	CALL_SEGMENT_t     data_segment;
	
	short segment;
	
	memset(&key_segment, 0x00, sizeof(CALL_SEGMENT_KEY_t));
	memset(&data_segment, 0x00, sizeof(CALL_SEGMENT_t));
	
	key_segment.callID = __key.c.callID;
	
	// VDN에 콜이 최초 Delivered 된 경우
	// 기존 데이터 조회 후 데이터가 있으면 어떻게 현존하는 콜인지 확인할지 검토해서 
	// 존재하지 않는 call data는 삭제처리(IS-CM은 callID를 3000콜마다 rotation시킴)
	if (__NewCallLog == true) { // Add call log with callID
		//__key.c.segment = 1;
//      현 segment에 콜로그가 있더라도 갱신처리하거나 해당 segment 데이터만 지우고 등록 처리해야 함
//      -> 갱신처리로 변경

		// Brand new call -> Create first call log segment
		if (pManageSegmentInfo->check_exists(key_segment) == false) {
			data_segment.segment = 1;
			__key.c.segment = data_segment.segment;
			
std::cout << "tracking_call::register_data; check_exits = false" << std::endl;
		}
		else {
std::cout << "tracking_call::register_data; check_exits = true" << std::endl;			
			// Add call log. Segment should be increased.
			segment = get_last_segment(__key.c.callID);
			if (segment == -1) {
				std::cout << "Call segment read error with callID : " << __key.c.callID << std::endl;
				return false;
			}
			else {
				data_segment.segment = segment + 1;
				__key.c.segment = data_segment.segment;
			}
		}
		
		if (data_segment.segment == 1) {
			// pManageSegmentInfo 등록
			if (pManageSegmentInfo->insert(key_segment, data_segment) == false) {
				std::cout << "Call segment info insertion failed with callID : " << __key.c.callID << std::endl;
				return false;
			}
		}
		else {
			if (pManageSegmentInfo->replace(key_segment, data_segment) == false) {
				std::cout << "Call segment info replace failed with callID : " << __key.c.callID << std::endl;
				return false;
			}			
		}
			

		// Register call log
		return (register_report(__key, __data, DUP_OPTION::REPLACE));
	}
	else // Update to current data segment call log 
	{
		// Mutex 처리할지 고민...
		segment = get_last_segment(__key.c.callID);
		if (segment == -1) {
			std::cout << "Call segment read error with callID : " << __key.c.callID << std::endl;
			return false;
		}
		else {
			__key.c.segment = segment;
		}
		
std::cout << "update callID/segment  : " << __key.c.callID << "/" << __key.c.segment << std::endl;

		// Register call log
//		return (register_report(__key, __data, DUP_OPTION::UPDATE));
		if (register_report(__key, __data, DUP_OPTION::UPDATE) == false) {
			return false;
		}
std::cout << "after update size : " << size() << std::endl;
		
		return true;
	}
}


// delete_call_log
bool tracking_call::delete_data(long __callID) {
	TRACKING_CALL_KEY_t key;
	CALL_SEGMENT_KEY_t key_segment;
	
	short last_segment;
	
	memset(&key, 0x00, sizeof(TRACKING_CALL_KEY_t));	
	memset(&key_segment, 0x00, sizeof(CALL_SEGMENT_KEY_t));
	
	key.c.callID = __callID;
	key_segment.callID = __callID;
	
	CALL_SEGMENT_t *pdata_segment = pManageSegmentInfo->read_data(key_segment);
	if (pdata_segment == NULL) {
		std::cout << "Couldn't find call segment info. with callID : " << __callID << std::endl;
		return false;
	}
	
	last_segment = pdata_segment->segment;
	
	pManageSegmentInfo->erase(key_segment); // ignore result	
	
	// segment info.에서 segment 정보 조회해서 
	// 1~조회된 정보값까지 반복실행해서 data 삭제처리
	for (int i = 0; i < last_segment; i++) {
		key.c.segment = i + 1;
		// queue에 send후 delete처리
		if (delete_report(key) == false) {
			std::cout << "Call log deletion failed with callID : " << key.c.callID
				      << ", segment : " << key.c.segment << std::endl;
			return false;
		}
	}

	return true;
}

bool tracking_call::delete_current_call_log(long __callID) {
	TRACKING_CALL_KEY_t key;
	CALL_SEGMENT_KEY_t key_segment;
	CALL_SEGMENT_t     data_segment;
	
	short last_segment;
	
	memset(&key, 0x00, sizeof(TRACKING_CALL_KEY_t));	
	memset(&key_segment, 0x00, sizeof(CALL_SEGMENT_KEY_t));
	memset(&data_segment, 0x00, sizeof(CALL_SEGMENT_t));
	
	key_segment.callID = __callID;
	
	CALL_SEGMENT_t *pdata_segment = pManageSegmentInfo->read_data(key_segment);
	if (pdata_segment == NULL) {
		std::cout << "Couldn't find call segment info. with callID : " << __callID << std::endl;
		return false;
	}
	
	last_segment = pdata_segment->segment;
	
	key.c.callID = __callID;
	key.c.segment = last_segment;
	// queue에 send후 delete처리
	if (delete_report(key) == false) {
		std::cout << "Call log deletion failed with callID : " << key.c.callID
			      << ", segment : " << key.c.segment << std::endl;
		return false;
	}
	
	last_segment -= 1;
	
	// Update 
	if (last_segment == 0)
		pManageSegmentInfo->erase(key_segment);
	else {
		data_segment.segment = last_segment;
		
		pManageSegmentInfo->update(key_segment, data_segment);
	}

	return true;			
}

bool tracking_call::read_first_call_log(long __callID, TRACKING_CALL_t& __data_out) {
	TRACKING_CALL_KEY_t key_call_log;
	TRACKING_CALL_t*    pdata_call_log;
	short last_segment;
	
	memset(&key_call_log, 0x00, sizeof(TRACKING_CALL_KEY_t));
	
	key_call_log.c.callID = __callID;
	key_call_log.c.segment = 1;
	
	pdata_call_log = read_data(key_call_log);
	if (pdata_call_log == NULL) {
		std::cout << "Call log not found with callID : " << __callID << std::endl;
		return false;
	}
	
	memset(&__data_out, 0x00, sizeof(TRACKING_CALL_t));
	memcpy(&__data_out, pdata_call_log, sizeof(TRACKING_CALL_t));
	
	return true;	
}

bool tracking_call::read_current_call_log(long __callID, TRACKING_CALL_t& __data_out) {
	TRACKING_CALL_KEY_t key_call_log;
	TRACKING_CALL_t*    pdata_call_log;
	short last_segment;
	
	memset(&key_call_log, 0x00, sizeof(TRACKING_CALL_KEY_t));
	
	last_segment = get_last_segment(__callID);
	if (last_segment == -1) {
		return false;
	}
		
std::cout << "last_segment with callID : " << __callID << " -> " << last_segment << std::endl;
	
	key_call_log.c.callID = __callID;
	key_call_log.c.segment = last_segment;
	
std::cout << "size : " << size() << std::endl;
	
mapAlt_t *pmap = get_map_ptr();
for (mapAlt_t::iterator it = pmap->begin(); it != pmap->end(); ++it) {
	TRACKING_CALL_KEY_t pkey_call_log_tmp = it->first;
	TRACKING_CALL_t     pdata_call_log_tmp = it->second;
	
	std::cout << "key.callID  : " << pkey_call_log_tmp.c.callID << std::endl;
	std::cout << "key.segment : " << pkey_call_log_tmp.c.segment << std::endl;
}

	
	pdata_call_log = read_data(key_call_log);
	if (pdata_call_log == NULL) {
		std::cout << "Call log not found with callID : " << __callID << std::endl;
		return false;
	}
	
	memset(&__data_out, 0x00, sizeof(TRACKING_CALL_t));
	memcpy(&__data_out, pdata_call_log, sizeof(TRACKING_CALL_t));
	
	return true;
}

short tracking_call::get_last_segment(long __callID) {
	CALL_SEGMENT_KEY_t key_segment;
	short last_segment;
	
	memset(&key_segment, 0x00, sizeof(CALL_SEGMENT_KEY_t));
	
	key_segment.callID = __callID;
	
	CALL_SEGMENT_t *pdata_segment = pManageSegmentInfo->read_data(key_segment);
	if (pdata_segment == NULL) {
		std::cout << "Couldn't find call segment info. with callID : " << __callID << std::endl;
		return -1;
	}
	
	return pdata_segment->segment;
}


// 일단 event 관련 데이터는 포함시키지 말고 진행
// sub_event 처리만 함
void tracking_call::set_column_switch(
		TRACKING_CALL::COLUMN_SWITCH_SET::COLUMN_NAME __column, 
		TRACKING_CALL::COLUMN_SWITCH_SET::COLUMN_SWITCH __switch, 
		TRACKING_CALL_t &__data_out,
		time_t __event_time)
{
	switch (__column) {
		case HELD:   // held, retrieve, ansholdtime
			__data_out.sw_held = __switch;
			if (__switch == ON) {
				__data_out.c.held = 1;
				
				__data_out.last_callcontrol_sub_event = CSTAHeld;
				__data_out.last_callcontrol_sub_event_time = __event_time;				
			}
			else {
				__data_out.last_callcontrol_sub_event = CSTAUnsolUndefined;
				__data_out.last_callcontrol_sub_event_time = 0;
			}

			break;

		case SEGMENT:
			__data_out.sw_segment = __switch;
			if (__switch == ON)
				__data_out.c.segstart = __event_time;
			else
				__data_out.c.segstop = __event_time;
			break;
			
		case TALKTIME:
			__data_out.sw_talktime =__switch;
			__data_out.c.talktime = __event_time;
			break;
			
		case RINGTIME:
			__data_out.sw_ringtime =__switch;
			__data_out.c.ringtime = __event_time;
			break;
			
		case QUEUETIME:
			__data_out.sw_queuetime =__switch;
			__data_out.c.queuetime = __event_time;
			break;
			
		case DISPTIME:
			__data_out.sw_disptime =__switch;
			__data_out.c.disptime = __event_time;			
			break;
			
		case CONSULTTIME:
			__data_out.sw_consulttime =__switch;
			__data_out.c.consulttime = __event_time;			
			break;
			
		default:
			break;
	}
	
	return;
}

bool tracking_call::decide_make_call_log(long __callID, DEVICE_TYPE __monitored_device_type, tDeviceID __monitored_extension
	, tDeviceID __callingDevice, tDeviceID __calledDevice, TRACKING_CALL_t &__data_current_call_log, bool &__bIsDataFound)
{
	TRACKING_CALL_KEY_t key_call_log;
	bool bMakeCallLog;
	
	memset(&__data_current_call_log, 0x00, sizeof(TRACKING_CALL_t));

	if (read_current_call_log(__callID, __data_current_call_log) == false) {
		std::cout << "Current call log not found with callID : " << __callID << std::endl;
		bMakeCallLog = true;
		__bIsDataFound = false;
	}
	else {
		std::cout << "event_handle_side :  " << __data_current_call_log.event_handle << std::endl;
			
		__bIsDataFound = true;
		
		if (__monitored_device_type == VDN) // VDN
		{
			if (strcmp(__calledDevice, __monitored_extension) == 0)
				bMakeCallLog = true;
			else
				bMakeCallLog = false;
		}
		else { // EXTENSION
			if (__data_current_call_log.event_handle == STATION) 
				bMakeCallLog = true;
			else  // event_handle == VDN
				bMakeCallLog = false;
		}
	}

	return bMakeCallLog;
}

///REF bool tracking_call::decide_make_call_log(long __callID, MONITOR_CROSS_REFID_t __monitored_dev_info
///REF 	, tDevice __callingDevice, tDevice __calledDevice, TRACKING_CALL_t &__data_current_call_log)
///REF {
///REF 	TRACKING_CALL_KEY_t key_call_log;
///REF 	bool bMakeCallLog;
///REF 	
///REF 	memset(&__data_current_call_log, 0x00, sizeof(TRACKING_CALL_t));
///REF 
///REF 	if (pcall_tracking->read_current_call_log(__callID, __data_current_call_log) == false) {
///REF 		std::cout << "Current call log not found with callID : " << __callID << std::endl;
///REF 		bMakeCallLog = true;
///REF 	}
///REF 	else {
///REF 		std::cout << "event_handle_side :  " << data_current_call_log.event_handle << std::endl;
///REF 		
///REF 		
///REF 		if ( (__monitored_dev_info.device_type == 1) && (__data_current_call_log.event_handle == TRACKING_CALL::VDN) )
///REF 			bMakeCallLog = false;
///REF 			
///REF 			
///REF 			
///REF 		if (__data_current_call_log.event_handle == TRACKING_CALL::VDN)
///REF 			bMakeCallLog = false;
///REF 		else {
///REF 
///REF 			if ( (__data_current_call_log.c.call_type == OB) || 
///REF 				 (__data_current_call_log.c.call_type == EXTENSION) ) 
///REF 			{
///REF 				// Calling Device의 event handling
///REF 				if (strcmp(__callingDevice, __monitored_dev_info.extension) == 0)
///REF 					bMakeCallLog = true;
///REF 				else
///REF 					bMakeCallLog = false;
///REF 			}
///REF 			else {
///REF 				// DACD -> Called Device의 event handling
///REF 				if (strcmp(__calledDevice, __monitored_dev_info.extension) == 0)
///REF 					bMakeCallLog = true;
///REF 				else
///REF 					bMakeCallLog = false;						
///REF 			}
///REF 		}
///REF 	}
///REF 
///REF 	return bMakeCallLog;
///REF }


/*
bool tracking_call::set_call_info(CALL_PROCESS_INFO_KEY_t __key, CALL_PROCESS_INFO_t __data) {
	if (pManageCallInfo->check_exists(__key) == false) {
		if (pManageCallInfo->insert(__key, __data) == false) {
			std::cout << "call info insert failed with " << __key.callID << std::endl;
		}
		else
			std::cout << "call info insert success with " << __key.callID << std::endl;
	}
	else {
		if (pManageCallInfo->update(__key, __data) == false) {
			std::cout << "call info update failed with " << __key.callID << std::endl;
		}
		else
			std::cout << "call info update success with " << __key.callID << std::endl;
	}
}

bool tracking_call::get_call_info(CALL_PROCESS_INFO_KEY_t __key, CALL_PROCESS_INFO_t &__data_out) {
	CALL_PROCESS_INFO_t *pData;
	
	pData = pManageCallInfo->read_data(__key);
	if (pData == NULL) {
		return false;
	}
	else {
		memset(&__data_out, 0x00, sizeof(CALL_PROCESS_INFO_t));
		memcpy(&__data_out, pData, sizeof(CALL_PROCESS_INFO_t));
		return true;
	}
}

// delete_call_info
bool tracking_call::delete_call_info(long __callID) {
	CALL_PROCESS_INFO_KEY_t key_call_info;
		
	memset(&key_call_info, 0x00, sizeof(CALL_PROCESS_INFO_KEY_t));
	
	key_call_info.callID = __callID;
	
	return pManageCallInfo->erase(key_call_info);
}
*/
