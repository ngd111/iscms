#include "CSTANotify.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>

#include <boost/interprocess/sync/named_mutex.hpp>

#include "vdn_status_shm.hpp"
#include "vdn_call_list_shm.hpp"
#include "manage_shared_memory.hpp"
#include "ipc_res.hpp"
#include "ipc_def.hpp"
#include "common.h"

vdn_status_shm *pVDN_shm;
vdn_call_list_shm *pCall_list_shm;

int main(int argc, char **argv)
{
	using namespace boost::interprocess;

	manage_shared_memory shm_obj;
	shared_memory_t *shm;

	vdn_status_shm::umap_itor_t it;
	vdn_call_list_shm::umap_itor_t it_list;
		
	short reset_flag = 0;
		
	if (argc == 2) {
		if (strcmp(argv[1], "-r") == 0) {
			std::cout << "Data reset start" << std::endl;
			reset_flag = 1;
		}
		else {
			std::cout << "Unknown option " << std::endl;
			std::cout << "Option : -r -> Reset report data" << std::endl;
			exit(-1);
		}
	}
		
	char message[1024+1];
	hcommon *pCommObj = new hcommon;

	try {
		shm = shm_obj.open_shared_memory(REPORT_AREA_SHM);
	}
	catch (string ex) {
		std::cerr << ex << std::endl;
		return false;
	}
	//////////////////////////////////////////////////////////////////////////////////
	pVDN_shm = new vdn_status_shm(&shm_obj, shm, pCommObj);
	
	std::cout << "vdn status size : " << pVDN_shm->size() << std::endl;
		
	vdn_status_shm::umap_offset_ptr pmap = pVDN_shm->get_umap_offset_ptr();
		
	named_mutex *mtx = pVDN_shm->get_mutex_ptr();
	
	//////////////////////////////////////////////////////////////////////////////////	
	pCall_list_shm = new vdn_call_list_shm(&shm_obj, shm);
	
	std::cout << "call list size : " << pCall_list_shm->size() << std::endl;
		
	vdn_call_list_shm::umap_offset_ptr pmap_list = pCall_list_shm->get_umap_offset_ptr();
		

	
	if (reset_flag == 1) {
		
//		pVDN_shm->clear();
//		pCall_list_shm->clear();
		pVDN_shm->truncate();
		pCall_list_shm->truncate();
		
	    delete pCall_list_shm;
	    delete pVDN_shm;
		delete pCommObj;
		
		return 0;
	}
	

	named_mutex *mtx_list = pCall_list_shm->get_mutex_ptr();
	
	mtx->lock();
	
	for (it = pmap->begin(); it != pmap->end(); ++it) {
		VDN_STATUS_LIST_SHMKEY_t *key = (VDN_STATUS_LIST_SHMKEY_t*)pVDN_shm->get_key(it);
		VDN_STATUS_LIST_SHM_t *data = (VDN_STATUS_LIST_SHM_t*)pVDN_shm->get_data(it);
		
		std::cout << "VDN         : " << key->vdn << std::endl;
			
		std::cout << "inprogress : " << data->inprogress << std::endl;
		std::cout << "inqueue    : " << data->inqueue << std::endl;
		std::cout << "inring     : " << data->inring << std::endl;
		std::cout << "atagent    : " << data->atagent << std::endl;
		std::cout << "oldestcall : " << data->oldestcall << std::endl;
		std::cout << "=======================================================================" << std::endl;
		std::cout << "    callID          VDN          ANI         skill    agent_device     " << std::endl;
		std::cout << " --------------------------------------------------------------------- " << std::endl;
			          
		mtx_list->lock();
		
		for (it_list = pmap_list->begin(); it_list != pmap_list->end(); ++it_list) {
			VDN_CALL_LIST_SHMKEY_t *key_list = (VDN_CALL_LIST_SHMKEY_t*)pCall_list_shm->get_key(it_list);
			VDN_CALL_LIST_SHM_t *data_list = (VDN_CALL_LIST_SHM_t*)pCall_list_shm->get_data(it_list);
			
			memset(message, 0x00, sizeof(message));
			
//			if (strcmp(data_list->vdn, key->vdn) == 0) {
				sprintf(message, "%10d %12s %12s %12s %13s"
							, key_list->callID
							, data_list->vdn
							, data_list->calling_device
							, data_list->skill
							, data_list->agent_device);
				std::cout << message << std::endl;
//			}
		}
		
		mtx_list->unlock();
		
	}	
	
	mtx->unlock();
	
	
    delete pCall_list_shm;
    delete pVDN_shm;
	delete pCommObj;
	
	
	return 0;
}
