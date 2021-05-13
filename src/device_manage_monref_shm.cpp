#include "device_manage_monref_shm.hpp"
#include "ipc_def.hpp"

device_manage_monref_shm::~device_manage_monref_shm() {
//	if (destroy_resource_objects() == false)
//		std::cerr << "Failed to destroy shared memory handling resource for device management" << std::endl;
}

device_manage_monref_shm::device_manage_monref_shm(manage_shared_memory *__shmobj, shared_memory_t *__shm) {
	if (__shmobj != NULL) shmobj = __shmobj;
	if (__shm != NULL) shm = __shm;

	create_resource_objects(CONTAINER_MON_REF_DEVICE, MTX_MON_REF_DEVICE);
}

bool device_manage_monref_shm::register_data(MONITOR_CROSS_REFID_KEY_t __key, MONITOR_CROSS_REFID_t __data) {
	return (register_report(__key, __data, DUP_OPTION::REPLACE));
}
