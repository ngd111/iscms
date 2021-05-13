#include "vdn_status_shm.hpp"
#include "ipc_def.hpp"

vdn_status_shm::~vdn_status_shm() {
}

vdn_status_shm::vdn_status_shm(manage_shared_memory *__shmobj, shared_memory_t *__shm, hcommon *__commobj) {
	if (__shmobj != NULL) shmobj = __shmobj;
	if (__shm != NULL) shm = __shm;
	if (__commobj != NULL) pCommObj = __commobj;

	create_resource_objects(CONTAINER_RT_VDN_STATUS, MTX_RT_VDN_STATUS);
}
/*
void vdn_status_shm::set_call_progress_status(VDN_STATUS::COLUMN_NAME __column
	                 		, VDN_STATUS_LIST_SHM_t &__data_out
	                 		, tUnsolicitedEventType __event, time_t __event_time)
{
	switch (__column) {
		case INPROGRESS:
			break;
			
		case INQUEUE:
			break;
			
		case INRING:
			break;
			
		case ATAGENT:
			break;
			
		default:
			break;
	};
	
	return;
}
*/
