#include "callreport_agent_shm.hpp"
#include "statbase_shm.hpp"

#include <iostream>

int main()
{
	using namespace boost::interprocess;

	manage_shared_memory shm_obj;
	shared_memory_t *shm;

	CR_HIST_AGENT_SHMKEY_SHM Key;
	CR_HIST_AGENT_SHM        Data;
	
	callreport_agent_shm::umap_itor_t it;


	shm_obj.remove_shared_memory("winddori");
	shm = shm_obj.create_shared_memory("winddori", 100000);
	//////////////////////////////////////////////////////////////////////////////////
	callreport_agent_shm *rpt = new callreport_agent_shm(&shm_obj, shm);
	
	memset(&Key, 0x00, sizeof(CR_HIST_AGENT_SHMKEY_SHM));
	memset(&Data, 0x00, sizeof(CR_HIST_AGENT_SHM));
		
	for (int i=0; i < 10; i++) {
		strcpy(Key.row_date, "20140101");
		strcpy(Key.skill, "20");
		Key.pbx_id = i;
		strcpy(Key.domain, "hansol.com");
		strcpy(Key.logid, "1234");
		
	
		strcpy(Data.row_date, "20140101");
		strcpy(Data.skill, "20");
		Data.pbx_id = i;
		strcpy(Data.domain, "hansol.com");
		strcpy(Data.logid, "1234"); 
		Data.acdCalls = i;
		Data.acdTime = i * 10;
	
		rpt->register_report(Key, Data);
	}
	
	strcpy(Key.row_date, "20140101");
	strcpy(Key.skill, "20");
	Key.pbx_id = 5;
	strcpy(Key.domain, "hansol.com");
	strcpy(Key.logid, "1234");
	
	strcpy(Data.row_date, "20140101");
	strcpy(Data.skill, "20");
	Data.pbx_id = 5;
	strcpy(Data.domain, "hansol.com");
	strcpy(Data.logid, "1234"); 
	Data.acdCalls = 1000;
	Data.acdTime = 1000;
		
	
//	Data.acdCalls += 1;
//	Data.acdTime += 10;
	

	rpt->register_report(Key, Data);
	

	std::cout << "check exits : " << rpt->check_exists(Key) << std::endl;
	
	std::cout << "size : " << rpt->size() << std::endl;
		
	callreport_agent_shm::umap_offset_ptr pmap = rpt->get_umap_offset_ptr();
	
	for (it = pmap->begin(); it != pmap->end(); ++it) {
		CR_HIST_AGENT_SHMKEY_SHM *key = (CR_HIST_AGENT_SHMKEY_SHM*)rpt->get_key(it);
		CR_HIST_AGENT_SHM *data = (CR_HIST_AGENT_SHM*)rpt->get_data(it);
		
		std::cout << "row_date : " << key->row_date << std::endl;
		std::cout << "skilll   : " << key->skill << std::endl;
		std::cout << "pbx_id   : " << key->pbx_id << std::endl;
		std::cout << "domain   : " << key->domain << std::endl;
		std::cout << "logid    : " << key->logid << std::endl;
			
		std::cout << "acdCalls : " << data->acdCalls << std::endl;
		std::cout << "acdTime  : " << data->acdTime << std::endl;
		std::cout << "=========================================" << std::endl;
	}
	
	rpt->destroy_resource_objects();
	
    delete rpt;
    
	/////////////////////////////////////////////////
//	mapAllocator_t alloc_inst(shm->get_segment_manager());
//	s = shm_obj.create_container_object_hash_use_default<umap_t, CR_HIST_AGENT_SHMKEY_SHM, mapAllocator_t>("VDNINFO", alloc_inst);
	/////////////////////////////////////////////////
/*
	statbase_shm<CR_HIST_AGENT_SHMKEY_SHM, CR_HIST_AGENT_SHM> *pshm;
	char sMtxName[100+1];

	strcpy(sMtxName, "VDNINFO");

	pshm = new statbase_shm<CR_HIST_AGENT_SHMKEY_SHM, CR_HIST_AGENT_SHM>(sMtxName);
	
	pshm->bind(s);
	
	memset(&Key, 0x00, sizeof(CR_HIST_AGENT_SHMKEY_SHM));
	memset(&Data, 0x00, sizeof(CR_HIST_AGENT_SHM));
	
			
	strcpy(Key.row_date, "20140101");
	strcpy(Key.skill, "20140101");
	Key.pbx_id = 1;
	strcpy(Key.domain, "hansol.com");
	strcpy(Key.logid, "1234");
	

	strcpy(Data.row_date, "20140101");
	strcpy(Data.skill, "20140101");
	Data.pbx_id = 1;
	strcpy(Data.domain, "hansol.com");
	strcpy(Data.logid, "1234"); 
	Data.acdCalls = 100;
	Data.acdTime = 10050;
	
	std::cout << "size : " << pshm->size() << std::endl;
	
//	pshm->insert(Key, Data);
	pshm->register_report(Key, Data);
	
	Key.pbx_id = 2;

	std::cout << "check exits : " << pshm->check_exists(Key) << std::endl;
	
	std::cout << "size : " << pshm->size() << std::endl;

	delete pshm;
*/
}
