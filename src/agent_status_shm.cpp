#include "agent_status_shm.hpp"
#include "ipc_def.hpp"

agent_status_shm::~agent_status_shm() {
	if (pgroup_status_rpt != NULL) delete pgroup_status_rpt;
}

agent_status_shm::agent_status_shm(manage_shared_memory *__shmobj, shared_memory_t *__shm) {
	if (__shmobj != NULL) shmobj = __shmobj;
	if (__shm != NULL) shm = __shm;
	
	pgroup_status_rpt = new group_status_shm(__shmobj, __shm);
	if (pgroup_status_rpt == NULL) {
		std::cerr << "Failed to allocate group_status_shm memory" << std::endl;
	}			

	create_resource_objects(CONTAINER_RT_AGENT_STATUS, MTX_RT_AGENT_STATUS);
}


// Extn side event : All logical event, Originated, Established, Held, Retrieve, Delivered
// VDN side event  : Queued
bool agent_status_shm::register_data(AGENT_STATUS_LIST_SHMKEY_t __key, AGENT_STATUS_LIST_SHM_t __data) {
	GROUP_STATUS_LIST_SHMKEY_t key_group;
	GROUP_STATUS_LIST_SHM_t    data_group;
	AGENT_STATUS_LIST_SHM_t    *pData = NULL;
	
	bool bProcGroup = true;
	int  nAuxReason;
	bool bBackup_subsidiary = false;
			
	// 초기 등록이면 agent status를 read하지 않는다.
	if (__data.bInitialize == false) {
		// Read current agent status(Last status, As-Is)
		pData = read_data(__key);
		if (pData == NULL) {
			std::cout << "Couldn't find agent status with agentID => " << __key.agentID << std::endl;
			// 등록 오류처리 Agent 데이터가 없는 경우가 있어서는 안된다.
			return false;
		}
		
		// 이전에 ACD call을 받았는지 체크해서 ACW or DACW세팅
		if (__data.last_event == CSTAAgentWorkingAfterCall) {
			if ( (strcmp(pData->sub_workmode, STATUS_ACD) == 0) || (strcmp(__data.workmode, "") == 0) )
			{
				strcpy(__data.workmode, STATUS_ACW);
				strcpy(__data.agstate, STATUS_ACW);
			} else if (strcmp(pData->sub_workmode, STATUS_DACD) == 0) {
				strcpy(__data.workmode, STATUS_DACW);
				strcpy(__data.agstate, STATUS_DACW);							
			}
		}
	}
	
	// 초기 등록시(pData = NULL)에는 agent status change event만 define이 가능하므로 
	// 통화시에 발생하는 event 처리는 없다
	// 초기 등록시 가능 이벤트 : asAgentNotReady, asAgentReady, asAgentWorkingAfterCall, asAgentBusy(통화중일때)
	
	for (int i = 0; i < AGENT_GROUP_MAX; i++) {
		memset(&key_group, 0x00, sizeof(GROUP_STATUS_LIST_SHMKEY_t));
		memset(&data_group, 0x00, sizeof(GROUP_STATUS_LIST_SHM_t));			
		
		// Set key value
		// 초기 receiver startup 시 pData는 NULL
		if (pData == NULL) { // Use __data
			if (strlen(__data.group_info[i].acdGroup) == 0) break;

			strcpy(key_group.acdGroup, __data.group_info[i].acdGroup);
		}
		else { // Use pData
			if (strlen(pData->group_info[i].acdGroup) == 0) break;
				
			strcpy(key_group.acdGroup, pData->group_info[i].acdGroup);
		}
		
		// Set data value
		
		if (__data.bInitialize == true) {
			data_group.agent_count_by_status.staffed = 1;
		}
		
		// For ACD-group status
		// LoggedOn을 제외한 나머지 event처리시 이전 상담원상태를 가지고 skill별 
		// 해당 상태 수치를 감소 시켜야 한다.
		// 이전 상담원 상태가 없는 경우(pData == NULL) skill별 감소 처리는 하지 않는다
		switch(__data.last_event) {
			case CSTAAgentLoggedOn:
				data_group.agent_count_by_status.staffed = 1;
				break;
			case CSTAAgentLoggedOff:
				data_group.agent_count_by_status.staffed = -1;
				break;
			case CSTAAgentNotReady:
				data_group.agent_count_by_status.inaux = 1;
				if (strlen(__data.auxreason) > 0) {
					nAuxReason = atoi(__data.auxreason);
					if ( (nAuxReason > 0) && (nAuxReason <= INAUX_MAX) )
						data_group.agent_count_by_status.inaux_[nAuxReason-1].inaux = 1; 
				}
				break;
			case CSTAAgentReady:
				data_group.agent_count_by_status.available = 1;
				break;
			case CSTAAgentWorkingAfterCall: // workskill 세팅
				data_group.agent_count_by_status.inacw = 1;
				break;
			case CSTAAgentBusy: // 초기 등록시 other에 합산
				data_group.agent_count_by_status.other = 1;
				
				break;
			case CSTAOriginated:  // For onacdauxout && other
				// onacdauxout
				//if (pData != NULL) {
				//	if ( (pData->last_event == CSTAEstablished) && 
				//		 (pData->sub_workmode) &&
				//		 () )
				//	data_group.agent_count_by_status.onacdauxout = 1;
				//}
				
				// other
				//* 발신
				//- Originated ~ 타 event
				data_group.agent_count_by_status.other = 1;
				
				break;
			case CSTAEstablished: // workskill 세팅, 초기등록시 본 이벤트 안옴(pData 무조건 있음)
				if (pData == NULL)
					std::cerr << "bInitialize == true && pData == NULL, event = [" << CSTAEstablished << "]" << std::endl;
				else {
					if (strcmp(__data.direction, "IN") == 0) { // IB
						if (strcmp(__data.agstate, AGSTATE_ACD) == 0) {
							data_group.agent_count_by_status.onacd = 1;
						}
						else if ( (strcmp(__data.agstate, AGSTATE_ACWIN) == 0) || 
							 (strcmp(__data.agstate, AGSTATE_DACWIN) == 0) ) {
							data_group.agent_count_by_status.onacwin = 1;
						}
						else if (strcmp(__data.agstate, AGSTATE_AUXIN) == 0) {
							data_group.agent_count_by_status.onauxin = 1;
						}
						// 내선인입콜은 추후 처리(현재는 workmode 및 agstate 변화없음)
					}
					else { // OB
						if (strcmp(__data.agstate, AGSTATE_ACWOUT) == 0) {
							data_group.agent_count_by_status.onacwout = 1;
						}
						else if (strcmp(__data.agstate, AGSTATE_AUXOUT) == 0) {
							data_group.agent_count_by_status.onauxout = 1;
						}
					}
				}

				break;
			case CSTAHeld:
				data_group.onhold = 1;
				data_group.agent_count_by_status.other = 1;
				break;
			case CSTARetrieved:
				data_group.onhold = -1;
				data_group.agent_count_by_status.other = -1;
				break;
			case CSTADelivered: // VDN side event or Station side event(For other measure)
				if (strcmp(__data.sub_workmode, STATUS_RING) == 0) {
					data_group.agent_count_by_status.aginring = 1;
				}
				break;
			case CSTAConnectionCleared:
				if (strcmp(__data.workmode, STATUS_AUX) == 0)
					data_group.agent_count_by_status.inaux = 1;
				else if (strcmp(__data.workmode, STATUS_ACW) == 0)
					data_group.agent_count_by_status.inacw = 1;
			default:
				// ACD-group 갱신하지 않는다.
				bProcGroup = false;
				break;
		}
		
		if (__data.last_event == CSTAAgentLoggedOff) {
			// Reset backup fields when agent logged off
			memset(&__data.affect_directly, 0x00, sizeof(AGENT_AFFECT_TO_GROUP_t));
			memset(&__data.affect_subsidiary, 0x00, sizeof(AGENT_AFFECT_TO_GROUP_t));
		}
		else {
			// Copy ACD-group data to the backup fields of agent status
			if (strcmp(__data.workskill, key_group.acdGroup) == 0) {
				strcpy(__data.affect_directly.group_info.acdGroup, __data.workskill);
				__data.affect_directly.group_info.level = 0; // 추후처리
				memcpy(&__data.affect_directly.data, &data_group.agent_count_by_status, sizeof(GROUP_STATUS_DATA_COUNT_t));
			}
			else { // Backup data only once
				if (bBackup_subsidiary == false) {
					strcpy(__data.affect_subsidiary.group_info.acdGroup, key_group.acdGroup);
					__data.affect_subsidiary.group_info.level = 0; // 추후처리
					memcpy(&__data.affect_subsidiary.data, &data_group.agent_count_by_status, sizeof(GROUP_STATUS_DATA_COUNT_t));
			
					bBackup_subsidiary = true;
					
					// if workskill is not set, reset affect_directly_group field
					if (strlen(__data.workskill) == 0) {
						memset(&__data.affect_directly.group_info, 0x00, sizeof(AGENT_GROUP_ASSIGNED_LIST_t));
						memset(&__data.affect_directly.data, 0x00, sizeof(GROUP_STATUS_DATA_COUNT_t));
					}
				}
			}
		}
		
		// 이전상태를 이용한 skill별 상태 감소처리
		// 이전상태가 없을 경우(pData == NULL)에는 처리하지 않는다.
		if (pData != NULL) {
			//Held and Retrieved는 위에서 감소처리함
			// Logged on의 경우 이전 status가 없으므로 ACD-group 관련 감소처리를 하지 않는다.
			if ( (__data.last_event != CSTAHeld) && (__data.last_event != CSTARetrieved) 
				 && (__data.last_event != CSTAAgentLoggedOn) ) 
			{
				// 감소 처리 sub_workmode
				// Rollback grou_status data using backup data in pData
				if (strcmp(pData->affect_directly.group_info.acdGroup, key_group.acdGroup) == 0)
				{
				std::cout << "directly " << std::endl;					
					data_group.agent_count_by_status.aginring -= pData->affect_directly.data.aginring;
					data_group.agent_count_by_status.available -= pData->affect_directly.data.available;
					data_group.agent_count_by_status.inacw -= pData->affect_directly.data.inacw;
					data_group.agent_count_by_status.inaux -= pData->affect_directly.data.inaux;
					
					if (pData->affect_directly.data.inaux > 0) {
						for (int i = 0; i < INAUX_MAX; i++)
							data_group.agent_count_by_status.inaux_[i].inaux -= pData->affect_directly.data.inaux_[i].inaux;
					}
					
					data_group.agent_count_by_status.onacd -= pData->affect_directly.data.onacd;
					data_group.agent_count_by_status.onacdauxout -= pData->affect_directly.data.onacdauxout;
					data_group.agent_count_by_status.onacwin -= pData->affect_directly.data.onacwin;
					data_group.agent_count_by_status.onacwout -= pData->affect_directly.data.onacwout;
					data_group.agent_count_by_status.onauxin -= pData->affect_directly.data.onauxin;
					data_group.agent_count_by_status.onauxout -= pData->affect_directly.data.onauxout;
					data_group.agent_count_by_status.other -= pData->affect_directly.data.other;
					
					data_group.agent_count_by_status.staffed -= pData->affect_directly.data.staffed;					
				}
				else {
				std::cout << "subsidiary " << std::endl;
					data_group.agent_count_by_status.aginring -= pData->affect_subsidiary.data.aginring;
					data_group.agent_count_by_status.available -= pData->affect_subsidiary.data.available;
					data_group.agent_count_by_status.inacw -= pData->affect_subsidiary.data.inacw;
					data_group.agent_count_by_status.inaux -= pData->affect_subsidiary.data.inaux;

					if (pData->affect_subsidiary.data.inaux > 0) {
						for (int i = 0; i < INAUX_MAX; i++) {
							data_group.agent_count_by_status.inaux_[i].inaux -= pData->affect_subsidiary.data.inaux_[i].inaux;
						}
					}
					
					data_group.agent_count_by_status.onacd -= pData->affect_subsidiary.data.onacd;
					data_group.agent_count_by_status.onacdauxout -= pData->affect_subsidiary.data.onacdauxout;
					data_group.agent_count_by_status.onacwin -= pData->affect_subsidiary.data.onacwin;
					data_group.agent_count_by_status.onacwout -= pData->affect_subsidiary.data.onacwout;
					data_group.agent_count_by_status.onauxin -= pData->affect_subsidiary.data.onauxin;
					data_group.agent_count_by_status.onauxout -= pData->affect_subsidiary.data.onauxout;
					data_group.agent_count_by_status.other -= pData->affect_subsidiary.data.other;
					
					data_group.agent_count_by_status.staffed -= pData->affect_subsidiary.data.staffed;
				}
			}
		}			
		
		// Register ACD-group report
		if (bProcGroup == true) {
			if (pgroup_status_rpt->register_report(key_group, data_group, DUP_OPTION::UPDATE) == false) {
				std::cerr << "Error occured while register group_status with acdGroup => " << key_group.acdGroup << std::endl;
			}
		}
	}
	
	// Register agent status report
	if ( (__data.last_event != CSTAHeld) && (__data.last_event != CSTARetrieved) )
		return (register_report(__key, __data, DUP_OPTION::UPDATE));
	else
		return true;
}

// Param2 : param1의 device ID가 내선#인지 AgentID인지 여부
//          true => AgentID, false => Extension#
// Return : agent status data
AGENT_STATUS_LIST_SHM_t* 
agent_status_shm::check_agentID(tDeviceID __device, tAgentID __out_agentID, bool __out_bagentID) {
	AGENT_STATUS_LIST_SHMKEY_t key;
	tAgentID agentID;
	
	memset(&key, 0x00, sizeof(AGENT_STATUS_LIST_SHMKEY_t));
	
	
	// __device 로 Agent ID조회
	if (get_agentID(__device, agentID) == false) {
		std::cout << "Couldn't find agentID with device => " << __device << std::endl;
		std::cout << "		Read agent data directly" << std::endl;

		strcpy(key.agentID, __device);
		if (check_exists(key) == false) {
			std::cout << "Data not found with device => " << __device << std::endl;
			return NULL;
		}

		// __device를 AgentID로 보고 정보조회
		__out_bagentID = true;   // __device = agentID
	}
	else // Found AgentID
	{
		strcpy(key.agentID, agentID);
		__out_bagentID = false;
	}
	
	strcpy(__out_agentID, key.agentID);

	return (statbase_shm::read_data(key));
}

bool agent_status_shm::get_agentID(tDeviceID __device, tAgentID __out_agentid)
{
	umap_offset_ptr pmap = get_umap_offset_ptr();
	named_mutex *mtx = get_mutex_ptr();
	
	memset(__out_agentid, 0x00, strlen(__out_agentid));
	
	mtx->lock();

std::cout << "get_agentID : count : " << size() << std::endl;	
	for (umap_itor_t it = pmap->begin(); it != pmap->end(); ++it) {
        AGENT_STATUS_LIST_SHMKEY_t *key = (AGENT_STATUS_LIST_SHMKEY_t*)get_key(it);
        AGENT_STATUS_LIST_SHM_t *data = (AGENT_STATUS_LIST_SHM_t*)get_data(it);
std::cout << "data->extension: " << data->extension << ", __device : " << __device << std::endl;
		if (strcmp(data->extension, __device) == 0) {
			strcpy(__out_agentid, key->agentID);
			mtx->unlock();
			return true;
		}
	}
	
	mtx->unlock();
	
	return false;
}

bool agent_status_shm::update_column(AGENT_STATUS_LIST_SHMKEY_t __key
	, AGENT_RT_ITEMS::Item __item, void* __value){
	AGENT_STATUS_LIST_SHM_t *data;
	umap_itor_t it;
	named_mutex *mtx = get_mutex_ptr();
	
	mtx->lock();
	
	it = pmap->find(__key);
	if (it == pmap->end()) {
		std::cout << "Data not found; with AgentID : " << __key.agentID << std::endl;
		mtx->unlock();
		return false;
	}
	else {
		data = &(it->second);
		switch (__item) {
			case AGENT_RT_ITEMS::WORKMODE:
				strcpy(data->workmode, (char *)__value);
				break;
			case AGENT_RT_ITEMS::AUXREASON:
				strcpy(data->auxreason, (char *)__value);
				break;
			case AGENT_RT_ITEMS::DIRECTION:
				strcpy(data->direction, (char *)__value);
				break;
			case AGENT_RT_ITEMS::AGSTATE:
				strcpy(data->agstate, (char *)__value);
				break;
			case AGENT_RT_ITEMS::WORKSKILL:
				strcpy(data->workskill, (char *)__value);
				break;
			default:
				break;
		}
	}
	
	mtx->unlock();
	
	return true;
}
