#include "CSTANotify.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>

#include "group_status_shm.hpp"
#include "manage_shared_memory.hpp"
#include "ipc_res.hpp"
#include "ipc_def.hpp"
#include "common.h"

group_status_shm *pGroup_shm;


int main()
{
	using namespace boost::interprocess;

	manage_shared_memory shm_obj;
	shared_memory_t *shm;

	GROUP_STATUS_LIST_SHMKEY_t Key;
	GROUP_STATUS_LIST_SHM_t    Data;
	
	group_status_shm::umap_itor_t it;

	try {
		shm = shm_obj.open_shared_memory(REPORT_AREA_SHM);
	}
	catch (string ex) {
		std::cerr << ex << std::endl;
		return false;
	}
	//////////////////////////////////////////////////////////////////////////////////
	pGroup_shm = new group_status_shm(&shm_obj, shm);
	
	memset(&Key, 0x00, sizeof(GROUP_STATUS_LIST_SHMKEY_t));
	memset(&Data, 0x00, sizeof(GROUP_STATUS_LIST_SHM_t));
		
	std::cout << "size : " << pGroup_shm->size() << std::endl;
		
	group_status_shm::umap_offset_ptr pmap = pGroup_shm->get_umap_offset_ptr();

	time_t tcurtime;

	time(&tcurtime);


	for (it = pmap->begin(); it != pmap->end(); ++it) {
		GROUP_STATUS_LIST_SHMKEY_t *key = (GROUP_STATUS_LIST_SHMKEY_t*)pGroup_shm->get_key(it);
		GROUP_STATUS_LIST_SHM_t *data = (GROUP_STATUS_LIST_SHM_t*)pGroup_shm->get_data(it);

		std::cout << "acdGroup   : " << key->acdGroup << std::endl;

		std::cout << "inqueue    : " << data->inqueue << std::endl;
		std::cout << "inring     : " << data->inring << std::endl;
		std::cout << "onhold     : " << data->onhold << std::endl;
		std::cout << "oldestcall : " << data->oldestcall << std::endl;
			
		std::cout << "aginring   : " << data->agent_count_by_status.aginring << std::endl;
		std::cout << "available  : " << data->agent_count_by_status.available << std::endl;
		std::cout << "inacw      : " << data->agent_count_by_status.inacw << std::endl;
		std::cout << "inaux      : " << data->agent_count_by_status.inaux << std::endl;
		for (int i = 0; i < INAUX_MAX; i++) {
			if (data->agent_count_by_status.inaux_[i].inaux > 0)
				std::cout << "	inaux" << i+1 << " : " << data->agent_count_by_status.inaux_[i].inaux << std::endl;
		}	
		std::cout << "onacd      : " << data->agent_count_by_status.onacd << std::endl;
		std::cout << "onacdauxout: " << data->agent_count_by_status.onacdauxout << std::endl;
		std::cout << "onacwin    : " << data->agent_count_by_status.onacwin << std::endl;
		std::cout << "onacwout   : " << data->agent_count_by_status.onacwout << std::endl;
		std::cout << "onauxin    : " << data->agent_count_by_status.onauxin << std::endl;
		std::cout << "onauxout   : " << data->agent_count_by_status.onauxout << std::endl;
		std::cout << "other      : " << data->agent_count_by_status.other << std::endl;
		std::cout << "staffed    : " << data->agent_count_by_status.staffed << std::endl;

		std::cout << "=======================================================" << std::endl;
	}

    delete pGroup_shm;
}
