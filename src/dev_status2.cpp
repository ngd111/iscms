#include "CSTANotify.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>

#include <boost/interprocess/sync/named_mutex.hpp>

#include "device_manage_monref_shm.hpp"
#include "manage_shared_memory.hpp"
#include "ipc_res.hpp"
#include "ipc_def.hpp"
#include "common.h"

device_manage_monref_shm *pDevice_shm;


int main()
{
	using namespace boost::interprocess;

	manage_shared_memory shm_obj;
	shared_memory_t *shm;

	MONITOR_CROSS_REFID_KEY_t Key;
	MONITOR_CROSS_REFID_t     Data;
	
	device_manage_monref_shm::umap_itor_t it;

	try {
		shm = shm_obj.open_shared_memory(REPORT_AREA_SHM);
	}
	catch (string ex) {
		std::cerr << ex << std::endl;
		return false;
	}
	//////////////////////////////////////////////////////////////////////////////////
	pDevice_shm = new device_manage_monref_shm(&shm_obj, shm);
	
	memset(&Key, 0x00, sizeof(MONITOR_CROSS_REFID_KEY_t));
	memset(&Data, 0x00, sizeof(MONITOR_CROSS_REFID_t));
		
	std::cout << "size : " << pDevice_shm->size() << std::endl;
		
	device_manage_monref_shm::umap_offset_ptr pmap = pDevice_shm->get_umap_offset_ptr();
		
	named_mutex *mtx = pDevice_shm->get_mutex_ptr();

	mtx->lock();
	
	for (it = pmap->begin(); it != pmap->end(); ++it) {
		MONITOR_CROSS_REFID_KEY_t *key = (MONITOR_CROSS_REFID_KEY_t*)pDevice_shm->get_key(it);
		MONITOR_CROSS_REFID_t *data = (MONITOR_CROSS_REFID_t*)pDevice_shm->get_data(it);
		
		std::cout << "monitorCrossRefID  : " << key->monitorCrossRefID << std::endl;
			
		std::cout << "device_type        : " << data->device_type << std::endl;
		std::cout << "extension          : " << data->extension << std::endl;
		std::cout << "=======================================================" << std::endl;
	}
	
	mtx->unlock();

    delete pDevice_shm;
}

