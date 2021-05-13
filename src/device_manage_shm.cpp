#include "device_manage_shm.hpp"
#include "ipc_def.hpp"

device_manage_shm::~device_manage_shm() {
	if (pTempCrossRef != NULL) delete pTempCrossRef;
	if (pDeviceList_byRefID != NULL) delete pDeviceList_byRefID;
		 
//	if (destroy_resource_objects() == false)
//		std::cerr << "Failed to destroy shared memory handling resource for device management" << std::endl;
}

device_manage_shm::device_manage_shm(manage_shared_memory *__shmobj, shared_memory_t *__shm) {
	if (__shmobj != NULL) shmobj = __shmobj;
	if (__shm != NULL) shm = __shm;

	create_resource_objects(CONTAINER_MON_DEVICE, MTX_MON_DEVICE);

	// Allocate temporary object
	pTempCrossRef = new manage_umap<SERVICE_CROSS_REFID_KEY_t, SERVICE_CROSS_REFID_t>;
	if (pTempCrossRef == NULL) {
		std::cerr << "Failed to allocate temporary memory(crossRef)" << std::endl;
	}
	
	pDeviceList_byRefID = new device_manage_monref_shm(__shmobj, __shm);
	if (pDeviceList_byRefID == NULL) {
		std::cerr << "Failed to allocate monref memory" << std::endl;
	}
}

bool device_manage_shm::register_data(MONITORING_LIST_SHMKEY_t __key, MONITORING_LIST_SHM_t __data) {
	SERVICE_CROSS_REFID_KEY_t key_refid_temp;
	SERVICE_CROSS_REFID_t     data_refid_temp;
	
	memset(&key_refid_temp, 0x00, sizeof(SERVICE_CROSS_REFID_KEY_t));
	memset(&data_refid_temp, 0x00, sizeof(SERVICE_CROSS_REFID_t));
	
	if (register_report(__key, __data, DUP_OPTION::REPLACE) == false) {
		return false;
	}
	
	key_refid_temp.serviceCrossRefID = __data.serviceCrossRefID;
	
	data_refid_temp.device_type = __data.device_type;
	strcpy(data_refid_temp.extension, __data.extension);
	strcpy(data_refid_temp.domain, __key.domain);

	// Add serviceCrossRefID(temporarily used for update monitoring result
	return (pTempCrossRef->insert(key_refid_temp, data_refid_temp));
}

bool device_manage_shm::update_result(tServiceCrossRefID __serviceCrossRefID
	, tMonitorCrossRefID __monitorCrossRefID, char *__result_yn, const char* __err_msg) {
	SERVICE_CROSS_REFID_KEY_t service_refid_key;
	SERVICE_CROSS_REFID_t     *service_refid_data;
	MONITORING_LIST_SHMKEY_t monitor_list_key;
	MONITORING_LIST_SHM_t    *monitor_list_data;
	MONITOR_CROSS_REFID_KEY_t key_mon_refid;
	MONITOR_CROSS_REFID_t     data_mon_refid;	
	
	named_mutex *mtx = get_mutex_ptr();
	
	memset(&service_refid_key, 0x00, sizeof(SERVICE_CROSS_REFID_KEY_t));
	memset(&monitor_list_key, 0x00, sizeof(MONITORING_LIST_SHMKEY_t));
	memset(&key_mon_refid, 0x00, sizeof(MONITOR_CROSS_REFID_KEY_t));
	memset(&data_mon_refid, 0x00, sizeof(MONITOR_CROSS_REFID_t));	
	

	service_refid_key.serviceCrossRefID = __serviceCrossRefID;	

	mtx->lock();

	service_refid_data = pTempCrossRef->read_data(service_refid_key);
	if (service_refid_data == NULL) {
		std::cout << "No data found with key(serviceCrossRefID) ; " 
			<< service_refid_key.serviceCrossRefID << std::endl;
		mtx->unlock();
		return false;
	}
	else {
		////////////////////////////////////////////////////////////////////////////////////
		key_mon_refid.monitorCrossRefID = __monitorCrossRefID;
		data_mon_refid.device_type = service_refid_data->device_type;
		strcpy(data_mon_refid.extension, service_refid_data->extension);
		strcpy(data_mon_refid.domain, service_refid_data->domain);
		
		if (pDeviceList_byRefID->register_data(key_mon_refid, data_mon_refid) == false) {
			std::cerr << "Data register failed with key by monitor reference ID" << std::endl;
			mtx->unlock();
			return false;
		}

		////////////////////////////////////////////////////////////////////////////////////
				
		strcpy(monitor_list_key.extension, service_refid_data->extension);
		strcpy(monitor_list_key.domain, service_refid_data->domain);
		
		monitor_list_data = read_data(monitor_list_key);
		if (monitor_list_data == NULL) {
			std::cout << "No data found with key(device_type, extension) ; "
				<< monitor_list_data->device_type << ", " 
				<< monitor_list_data->extension << std::endl;
			mtx->unlock();
			return false;
		}

		// Update monitoring status to success
		monitor_list_data->monitorCrossRefID = __monitorCrossRefID;
		strcpy(monitor_list_data->monitor_yn, __result_yn);
		if (strlen(__err_msg) > 0)
			strcpy(monitor_list_data->message, __err_msg);
			
		if (update(monitor_list_key, *monitor_list_data) == false) {
			std::cout << "Failed to update monitoring status with key(device_type, extension, domain) ; "
				<< monitor_list_data->device_type << ", " 
				<< monitor_list_data->extension << ", "
				<< monitor_list_key.domain << std::endl;
			mtx->unlock();
			return false;
		}
		else {
			std::cout << "Update monitoring status finished with key(device_type, extension, domain) ; " 
				<< monitor_list_data->device_type << ", " 
				<< monitor_list_data->extension << ", "
				<< monitor_list_key.domain << std::endl;
		}			
	}
	
	// Delete exist monitoring request
	if (pTempCrossRef->erase(service_refid_key) == false) {
		std::cout << "Failed to delete serviceCrossRefID ; [" 
			<< service_refid_key.serviceCrossRefID << "]" << "from temporary object" << std::endl;
	}	
	
	mtx->unlock();
	
	return true;
}

bool device_manage_shm::delete_data(tMonitorCrossRefID __monitorCrossRefID) {
	MONITORING_LIST_SHMKEY_t monitor_list_key;
	MONITORING_LIST_SHM_t*   monitor_list_data;
	MONITOR_CROSS_REFID_KEY_t monitor_cross_refid_key;
	MONITOR_CROSS_REFID_t*    monitor_cross_refid_data;
	
	memset(&monitor_list_key, 0x00, sizeof(MONITORING_LIST_SHMKEY_t));
	memset(&monitor_cross_refid_key, 0x00, sizeof(MONITOR_CROSS_REFID_KEY_t));
	
	monitor_cross_refid_key.monitorCrossRefID = __monitorCrossRefID;

	monitor_cross_refid_data = pDeviceList_byRefID->read_data(monitor_cross_refid_key);
	if (monitor_list_data == NULL) {
		std::cerr << "Couldn't found data ; delete_data failed" << std::endl;
		return false;
	}

	strcpy(monitor_list_key.extension, monitor_cross_refid_data->extension);
	strcpy(monitor_list_key.domain, monitor_cross_refid_data->domain);
	
	if (delete_report(monitor_list_key) == false) {
		std::cerr << "delete_report failed" << std::endl;
		return false;
	}
	
	monitor_cross_refid_key.monitorCrossRefID = __monitorCrossRefID;
	
	if (pDeviceList_byRefID->delete_report(monitor_cross_refid_key) == false) {
		std::cerr << "delete_report failed with reference ID" << std::endl;		
		return false;
	}
	
	return true;
}

void device_manage_shm::truncate() {
	statbase_shm::truncate();
	pTempCrossRef->clear();
}

// Multimap 구현시까지 한시적으로 사용
void device_manage_shm::update_result(tMonitorCrossRefID __monitorCrossRefID) {
	named_mutex *mtx = get_mutex_ptr();

	mtx->lock();
	
	umap_offset_ptr pmap = get_umap_offset_ptr();
	for (umap_itor_t it = pmap->begin(); it != pmap->end(); ++it) {
		MONITORING_LIST_SHMKEY_t *key = (MONITORING_LIST_SHMKEY_t*) get_key(it);
		MONITORING_LIST_SHM_t *data = (MONITORING_LIST_SHM_t*) get_data(it);
		
		if (data->monitorCrossRefID == __monitorCrossRefID) {
			//Update monitor flag
			strcpy(data->monitor_yn, "N");
			break;
		}
	}
	
	mtx->unlock();
}

MONITORING_LIST_SHM_t* device_manage_shm::find_data(tMonitorCrossRefID __monitorCrossRefID) {
	named_mutex *mtx = get_mutex_ptr();
	static MONITORING_LIST_SHM_t *data = NULL;
	
	mtx->lock();
	
	umap_offset_ptr pmap = get_umap_offset_ptr();
	for (umap_itor_t it = pmap->begin(); it != pmap->end(); ++it) {
		MONITORING_LIST_SHMKEY_t *key = (MONITORING_LIST_SHMKEY_t*) get_key(it);
		data = (MONITORING_LIST_SHM_t*) get_data(it);
		
		if (data->monitorCrossRefID == __monitorCrossRefID) {
			break;
		}
	}
	
	mtx->unlock();
	
	return data;
}
