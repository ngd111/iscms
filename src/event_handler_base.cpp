#include "event_handler_base.hpp"
#include "ipc_def.hpp"


event_handler_base::~event_handler_base() {
//	if (pgroup_status_rpt != NULL) delete pgroup_status_rpt;
	if (pagent_status_rpt != NULL) delete pagent_status_rpt;
	if (pDevice_shm != NULL) delete pDevice_shm;
}

event_handler_base::event_handler_base(manage_shared_memory *__shmobj
								, shared_memory_t *__shm, hcommon *__commobj) 
{
	if (__commobj != NULL) pCommObj = __commobj;

//	pgroup_status_rpt = new group_status_shm(__shmobj, __shm, __commobj);
//	if (pgroup_status_rpt == NULL) {
//		std::cerr << "Failed to allocate group_status_shm memory" << std::endl;
//	}
	
	pagent_status_rpt = new agent_status_shm(__shmobj, __shm);
	if (pagent_status_rpt == NULL) {
		std::cerr << "Failed to allocate agent_status_shm memory" << std::endl;
	}
	
	pDevice_shm = new device_manage_shm(__shmobj, __shm);
	if (pDevice_shm == NULL) {
		std::cerr << "Failed to allocate device_manage_shm memory" << std::endl;
	}
}

void event_handler_base::set_shared_report_objects(tracking_call *__pcall_tracking)
{
	pcall_tracking = __pcall_tracking;
}

void event_handler_base::set_log_queue_objects(queue<LOGGINGDATA_t> *__logqueue
					, pthread_mutex_t* __mut, pthread_cond_t* __cond)
{
	logqueue = __logqueue;
	log_mutex = __mut;
	log_cond  = __cond;
}

// Message queue로 전송하기 위해 queue에 등록한다.
//  ==> 각종 SWITCH를 검사해 OFF되지 않고 ON상태로 남아 있는 것은 닫음처리 한다.
// 등록이 완료되면 해당 callID를 가진 call log를 삭제처리한다.
// 삭제처리 후 call segment 정보도 삭제 처리 한다.
bool event_handler_base::complete_call_log(long __callID) {
	TRACKING_CALL_KEY_t key_call_log;
	TRACKING_CALL_t*    pdata_call_log;
	short last_segment;
	
	pthread_mutex_lock(log_mutex);
	
	last_segment = pcall_tracking->get_last_segment(__callID);
	if (last_segment == -1) {
		// 조회된 값이 없을 경우 전체 call log를 대상으로 iterator를 scan해 처리한다.
		/* 추후 테스트 상황 봐서 코드 오픈
		tracking_call::mapAlt_t *pmap = pcall_tracking->get_map_ptr();
		for (tracking_call::mapAlt_t::iterator it = pmap->begin(); it != pmap->end(); ++it) {
			TRACKING_CALL_KEY_t key_call_log_tmp = it->first;
			TRACKING_CALL_t     data_call_log_tmp = it->second;
			
			if (key_call_log_tmp.c.callID == __callID) {
				std::cout << "key.callID  : " << key_call_log_tmp.c.callID << std::endl;
				std::cout << "key.segment : " << key_call_log_tmp.c.segment << std::endl;
					
				push_queue(key_call_log_tmp, data_call_log_tmp);
			}
		}			
		*/
		pthread_mutex_unlock(log_mutex);
		return false;
	}
	
	memset(&key_call_log, 0x00, sizeof(TRACKING_CALL_KEY_t));
	key_call_log.c.callID = __callID;
			
	for (int i = 0; i < last_segment; i++) {
		key_call_log.c.segment = ++i;
	
		pdata_call_log = pcall_tracking->read_data(key_call_log);
		if (pdata_call_log == NULL) {
			std::cout << __FILE__ << ":" << __func__ << " ; " << "read_data return NULL" << std::endl;
			pthread_mutex_unlock(log_mutex);
			return false;
		}
		
		// pdata_call_log의 SWITCH 가 ON 상태에 있는 것은 현재 시간기준으로 막음처리한다.
		// => Event가 누락되는 등으 비정상적인 상태 처리
				
		push_queue(key_call_log, *pdata_call_log);
	}
	
	pthread_mutex_unlock(log_mutex);
	// Send wake up signal to cms_anl logger thread that owned loggerMain function.
	pthread_cond_signal(log_cond);
	
	// Delete call log
	if (pcall_tracking->delete_data(key_call_log.c.callID) == false) {
		std::cerr << "pcall_tracking->delete_data failed!!!" << std::endl;
//		return false;
	}
	
	return true;
}

//ConnectionClearedEvent의 last segment의 releasingDevice가 anslogin의 상담원의 extension 와 동일하면 상담원이 drop함
//ConnectionClearedEvent의 first segment의 releasingDevice가 calling_pty와 동일하면 고객이 drop함
bool event_handler_base::set_agt_released(long __callID, tDeviceID __releasingDevice
			, TRACKING_CALL_t __current_call_log, TRACKING_CALL_t &__call_log_data) {
	AGENT_STATUS_LIST_SHMKEY_t key_agent_status;
	AGENT_STATUS_LIST_SHM_t*   pdata_agent_status;
	TRACKING_CALL_t data_first_call_log;
	
	memset(&data_first_call_log, 0x00, sizeof(TRACKING_CALL_t));
	memset(&key_agent_status, 0x00, sizeof(AGENT_STATUS_LIST_SHMKEY_t));
	
	if (pcall_tracking->read_first_call_log(__callID, data_first_call_log) == false) {
		std::cout << "read_first_call_log error with callID : " << __callID << std::endl;
		return false;
	}
	
	if (strcmp(data_first_call_log.c.calling_pty, __releasingDevice) == 0) {
		__call_log_data.first_releasing = TRACKING_CALL::CALLING_PTY; // Caller	
		__call_log_data.c.agt_released = TRACKING_CALL::NO;
	}
	else {
		// Check if releasing device is equal to agent device ~
		if (strlen(__current_call_log.c.anslogin) > 0) {
			strcpy(key_agent_status.agentID, __current_call_log.c.anslogin);
			
			pdata_agent_status = pagent_status_rpt->statbase_shm::read_data(key_agent_status);
			if (pdata_agent_status == NULL) {
				std::cout << "Couldn't find agent status with => " << key_agent_status.agentID << std::endl;
				return false;
			}
			else {
				if (strcmp(__releasingDevice, pdata_agent_status->extension) == 0) {
					__call_log_data.first_releasing = TRACKING_CALL::CALLED_PTY; // Agent
					__call_log_data.c.agt_released = TRACKING_CALL::YES;
				}
			}
		}
	}

	return true;	
}

void event_handler_base::push_queue(TRACKING_CALL_KEY_t __key_call_log, TRACKING_CALL_t __call_log_data)
{
	LOGGINGDATA_t log_data;
	CALLLOG_SET_t calllog_set;
		
	memset(&log_data, 0x00, sizeof(LOGGINGDATA_t));
	memset(&calllog_set, 0x00, sizeof(CALLLOG_SET_t));
	
	log_data.data_type = 2;  // call log
	
	calllog_set.key.callID =  __key_call_log.c.callID;
	calllog_set.key.segment =  __key_call_log.c.segment;
	memcpy(&calllog_set.data, &__call_log_data.c, sizeof(CALLLOG_t));	
	
	memcpy(&log_data.u.callData, &calllog_set, sizeof(CALLLOG_SET_t));
	
//std::cout << "log_data.u.callData.key.callID  : " << log_data.u.callData.key.callID << std::endl;
//std::cout << "log_data.u.callData.key.segment : " << log_data.u.callData.key.segment << std::endl;
			
	logqueue->push(log_data);
}
