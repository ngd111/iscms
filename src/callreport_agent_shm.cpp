#include "callreport_agent_shm.hpp"
#include "ipc_def.hpp"

/*
callreport_agent_shm::callreport_agent_shm() {
}
*/
callreport_agent_shm::~callreport_agent_shm() {
}

//callreport_agent_shm::callreport_agent_shm(shared_memory_t *__shm) {
callreport_agent_shm::callreport_agent_shm(manage_shared_memory *__shmobj, shared_memory_t *__shm) {
	std::cout << "constructor callreport_agent_shm(__shmobj, __shm)" << std::endl;
	if (__shmobj != NULL) shmobj = __shmobj;
	if (__shm != NULL) shm = __shm;

	create_resource_objects(MTX_HR_AGENT, CONTAINER_HR_AGENT);

//	create_or_open_mutex(MTX_HR_AGENT);
//	create_container(CONTAINER_HR_AGENT);

//	s = shmobj->create_container_object_hash_use_default<umap_t, CR_HIST_AGENT_SHMKEY_SHM, mapAllocator_t>(CONTAINER_HR_AGENT, shm->get_segment_manager());
//	bind(s);  // bind shared memory block
}



