#include "statbase_shm.hpp"
#include "manage_shared_memory.hpp"

#include <iostream>

// for VDNINFO
typedef struct __key {
	char row_date   [SZ_DATE  +1];
	char skill     [SZ_DIALPLN+1];
	short pbx_id;
	char domain     [SZ_DOMAIN+1];  // Tenant
	char logid     [SZ_DIALPLN+1];	
   
    bool operator==(const struct __key &p) const {
        std::cout << "in operator= __key" << std::endl;
        return (pbx_id == p.pbx_id &&
                strcmp(row_date, p.row_date) == 0 &&
                strcmp(skill, p.skill) == 0 &&
                strcmp(domain, p.domain) == 0 &&
                strcmp(logid, p.logid) == 0
                );
    }
} CR_HIST_AGENT_SHMKEY_SHM;

typedef struct __vdn {
	char row_date   [SZ_DATE  +1];
	char skill     [SZ_DIALPLN+1];
	short pbx_id;
	char domain     [SZ_DOMAIN+1];  // Tenant
	char logid     [SZ_DIALPLN+1];
	
    /////////////////////////// Data columns ///////////////////////
   
	size_t acdCalls;
	size_t acdTime;		
} CR_HIST_AGENT_SHM;

std::size_t hash_value(struct __key const &p)
{
    std::size_t seed = 0;
    boost::hash_combine(seed, atol(p.logid));
    return seed;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


int main()
{
	using namespace boost::interprocess;

	manage_shared_memory shm_obj;
	shared_memory_t *shm;

	typedef statbase_shm<CR_HIST_AGENT_SHMKEY_SHM, CR_HIST_AGENT_SHM>::umap_t umap_t;
	typedef statbase_shm<CR_HIST_AGENT_SHMKEY_SHM, CR_HIST_AGENT_SHM>::mapAllocator_t mapAllocator_t;
		
	CR_HIST_AGENT_SHMKEY_SHM Key;
	CR_HIST_AGENT_SHM        Data;

	umap_t *s;

	shm_obj.remove_shared_memory("winddori");

	shm = shm_obj.create_shared_memory("winddori", 100000);

	mapAllocator_t alloc_inst(shm->get_segment_manager());


	s = shm_obj.create_container_object_hash_use_default<umap_t, CR_HIST_AGENT_SHMKEY_SHM, mapAllocator_t>("VDNINFO", alloc_inst);


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
}
