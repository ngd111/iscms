#include "CSTANotify.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>

#include "agent_status_shm.hpp"
#include "manage_shared_memory.hpp"
#include "ipc_res.hpp"
#include "ipc_def.hpp"
#include "common.h"

agent_status_shm *pAgent_shm;


int main()
{
	using namespace boost::interprocess;

	manage_shared_memory shm_obj;
	shared_memory_t *shm;

	AGENT_STATUS_LIST_SHMKEY_t Key;
	AGENT_STATUS_LIST_SHM_t    Data;
	
	agent_status_shm::umap_itor_t it;

	try {
		shm = shm_obj.open_shared_memory(REPORT_AREA_SHM);
	}
	catch (string ex) {
		std::cerr << ex << std::endl;
		return false;
	}
	//////////////////////////////////////////////////////////////////////////////////
	pAgent_shm = new agent_status_shm(&shm_obj, shm);
	
	memset(&Key, 0x00, sizeof(AGENT_STATUS_LIST_SHMKEY_t));
	memset(&Data, 0x00, sizeof(AGENT_STATUS_LIST_SHM_t));
		
	std::cout << "size : " << pAgent_shm->size() << std::endl;
		
	agent_status_shm::umap_offset_ptr pmap = pAgent_shm->get_umap_offset_ptr();

	time_t tcurtime;

	time(&tcurtime);


	for (it = pmap->begin(); it != pmap->end(); ++it) {
		AGENT_STATUS_LIST_SHMKEY_t *key = (AGENT_STATUS_LIST_SHMKEY_t*)pAgent_shm->get_key(it);
		AGENT_STATUS_LIST_SHM_t *data = (AGENT_STATUS_LIST_SHM_t*)pAgent_shm->get_data(it);

		std::cout << "agent ID   : " << key->agentID << std::endl;

		std::cout << "extension  : " << data->extension << std::endl;
		if (strlen(data->sub_workmode) > 0)
			std::cout << "workmode   : " << data->sub_workmode << std::endl;
		else
			std::cout << "workmode   : " << data->workmode << std::endl;
		std::cout << "auxreason  : " << data->auxreason << std::endl;
		std::cout << "direction  : " << data->direction << std::endl;
		std::cout << "agstate    : " << data->agstate << std::endl;
		std::cout << "workskill  : " << data->workskill << std::endl;
		if (strcmp(data->agstate, AGSTATE_UNSTAFFED) != 0) {
			std::cout << "agtime     : " << (tcurtime - data->agtime) << std::endl;
			std::cout << "agduration : " << (tcurtime - data->agduration) << std::endl;
		}
		else {
			std::cout << "agtime     : " << std::endl;
			std::cout << "agduration : " << std::endl;			
		}
		
		for (int i = 0; i < 20; i++) {
			if (data->affect_subsidiary.data.inaux_[i].inaux > 0)
				std::cout << "aux[" << i << "] => " << data->affect_subsidiary.data.inaux_[i].inaux << std::endl;
		}
		
				
		std::cout << "--- Logged ACD group --------------------------" << std::endl;
			
		for (int i = 0; i < AGENT_GROUP_MAX; i++) {
			if ( (data->group_info[i].acdGroup == NULL) ||
				 (strlen(data->group_info[i].acdGroup) == 0) )
				 break;

			std::cout << "Group[" << i << "] => " << data->group_info[i].acdGroup << std::endl;
		}
		std::cout << "=======================================================" << std::endl;
	}

    delete pAgent_shm;
}
