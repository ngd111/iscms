#include "CSTANotify.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>

#include "device_manage_shm.hpp"
#include "manage_shared_memory.hpp"
#include "ipc_res.hpp"
#include "common.h"

device_manage_shm *pDevice_shm;

int MonitorDevice(string device, int __i) {
	MONITORING_LIST_SHMKEY_t Key;
	MONITORING_LIST_SHM_t    Data;
	
	SERVICE_CROSS_REFID_KEY_t Key_temp;
	SERVICE_CROSS_REFID_t     Data_temp;
	/////////////////////////////////////////////////////
	
	memset(&Key, 0x00, sizeof(MONITORING_LIST_SHMKEY_t));
	memset(&Data, 0x00, sizeof(MONITORING_LIST_SHM_t));	
	
	////////////////////////////////////////////		
	tDeviceID		deviceID;
	strcpy(deviceID, device.c_str());

	////////////////////////////////////////////

//	Key.device_type = 1;
	strcpy(Key.extension, deviceID);
	
//	Data.device_type = Key.device_type;
	Data.device_type = 1;
	strcpy(Data.extension, Key.extension);

	Data.serviceCrossRefID = __i;

	if (pDevice_shm->register_data(Key, Data) == false) {
		std::cout << "Failed to insert monitoring device data to the shared memory with "
					<< Data.extension << std::endl;
		return -1;
	}

	return Data.serviceCrossRefID;
}


int main()
{
	using namespace boost::interprocess;

	manage_shared_memory shm_obj;
	shared_memory_t *shm;

	MONITORING_LIST_SHMKEY_t Key;
	MONITORING_LIST_SHM_t        Data;
	
	device_manage_shm::umap_itor_t it;


	shm_obj.remove_shared_memory("CMS_MANAGEMENT");
	shm = shm_obj.create_shared_memory("CMS_MANAGEMENT", 1024 * 1024);
	//////////////////////////////////////////////////////////////////////////////////
	pDevice_shm = new device_manage_shm(&shm_obj, shm);

	MonitorDevice("sip:9001@cube-x19.hansol.com", 1);
/*	MonitorDevice("sip:9002@cube-x19.hansol.com", 2);
	
	memset(&Key, 0x00, sizeof(MONITORING_LIST_SHMKEY_t));
	memset(&Data, 0x00, sizeof(MONITORING_LIST_SHM_t));
		

	std::cout << "check exits : " << pDevice_shm->check_exists(Key) << std::endl;

	std::cout << "size : " << pDevice_shm->size() << std::endl;
		
	device_manage_shm::umap_offset_ptr pmap = pDevice_shm->get_umap_offset_ptr();
*/
	/*
	for (it = pmap->begin(); it != pmap->end(); ++it) {
		MONITORING_LIST_SHMKEY_t *key = (MONITORING_LIST_SHMKEY_t*)pDevice_shm->get_key(it);
		MONITORING_LIST_SHM_t *data = (MONITORING_LIST_SHM_t*)pDevice_shm->get_data(it);
		
		std::cout << "device_type : " << key->device_type << std::endl;
		std::cout << "extension   : " << key->extension << std::endl;
			
		std::cout << "device_type : " << data->device_type << std::endl;
		std::cout << "extension   : " << data->extension << std::endl;
		std::cout << "svccrossRef : " << data->serviceCrossRefID << std::endl;
		std::cout << "moncrossRef : " << data->monitorCrossRefID << std::endl;
		std::cout << "=========================================" << std::endl;
	}
	*/
    delete pDevice_shm;
}
