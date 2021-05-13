#include "vdn_call_list_shm.hpp"
#include "ipc_def.hpp"

vdn_call_list_shm::~vdn_call_list_shm() {		 
//	if (destroy_resource_objects() == false)
//		std::cerr << "Failed to destroy shared memory handling resource for device management" << std::endl;
}

vdn_call_list_shm::vdn_call_list_shm(manage_shared_memory *__shmobj, shared_memory_t *__shm) {
	if (__shmobj != NULL) shmobj = __shmobj;
	if (__shm != NULL) shm = __shm;

	create_resource_objects(CONTAINER_RT_VDN_CALL_LIST, MTX_RT_VDN_CALL_LIST);
}

bool vdn_call_list_shm::register_data(VDN_CALL_LIST_SHMKEY_t __key, VDN_CALL_LIST_SHM_t __data) {	
	return (register_report(__key, __data, DUP_OPTION::UPDATE));
}
