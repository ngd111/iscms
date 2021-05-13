#include "group_status_shm.hpp"
#include "ipc_def.hpp"

group_status_shm::~group_status_shm() {
}

group_status_shm::group_status_shm(manage_shared_memory *__shmobj, shared_memory_t *__shm) {
    if (__shmobj != NULL) shmobj = __shmobj;
    if (__shm != NULL) shm = __shm;

    create_resource_objects(CONTAINER_RT_GROUP_STATUS, MTX_RT_GROUP_STATUS);
}
