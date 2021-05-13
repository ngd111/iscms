#include "CSTANotify.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>

#include <boost/interprocess/sync/named_mutex.hpp>

#include "device_manage_shm.hpp"
#include "manage_shared_memory.hpp"
#include "ipc_res.hpp"
#include "ipc_def.hpp"
#include "common.h"

device_manage_shm *pDevice_shm;


int main()
{
	using namespace boost::interprocess;

	manage_shared_memory shm_obj;
	shared_memory_t *shm;

	MONITORING_LIST_SHMKEY_t Key;
	MONITORING_LIST_SHM_t        Data;
	
	device_manage_shm::umap_itor_t it;

	try {
		shm = shm_obj.open_shared_memory(REPORT_AREA_SHM);
	}
	catch (string ex) {
		std::cerr << ex << std::endl;
		return false;
	}
	//////////////////////////////////////////////////////////////////////////////////
	pDevice_shm = new device_manage_shm(&shm_obj, shm);
	
	memset(&Key, 0x00, sizeof(MONITORING_LIST_SHMKEY_t));
	memset(&Data, 0x00, sizeof(MONITORING_LIST_SHM_t));
		
	std::cout << "size : " << pDevice_shm->size() << std::endl;
		
	device_manage_shm::umap_offset_ptr pmap = pDevice_shm->get_umap_offset_ptr();
		
	named_mutex *mtx = pDevice_shm->get_mutex_ptr();
/*	time_t tcurtime;

	time(&tcurtime);
	std::cout << "wait for cms_ircv process unlock mutex object " << tcurtime << std::endl;	
	mtx->lock();

	time(&tcurtime);
	std::cout << "mtx locking 10 seconds; " << tcurtime << std::endl;
	sleep(10);
	mtx->unlock();
	
	time(&tcurtime);
	std::cout << "mtx unlocking         ; " << tcurtime << std::endl;
*/
	mtx->lock();
	
	for (it = pmap->begin(); it != pmap->end(); ++it) {
		MONITORING_LIST_SHMKEY_t *key = (MONITORING_LIST_SHMKEY_t*)pDevice_shm->get_key(it);
		MONITORING_LIST_SHM_t *data = (MONITORING_LIST_SHM_t*)pDevice_shm->get_data(it);
		
		std::cout << "extension   : " << key->extension << std::endl;
		std::cout << "domain      : " << key->domain << std::endl;

		std::cout << "device_type : " << data->device_type << std::endl;
		std::cout << "monitor_yn  : " << data->monitor_yn << std::endl;
		if (strlen(data->message) > 0)
			std::cout << "message     : " << data->message << std::endl;
//		std::cout << "svccrossRef : " << data->serviceCrossRefID << std::endl;
		std::cout << "moncrossRef : " << data->monitorCrossRefID << std::endl;
		std::cout << "=======================================================" << std::endl;
	}
	
	mtx->unlock();

    delete pDevice_shm;
}
