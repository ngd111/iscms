#include "XPRJACEDLLEx/ICSTA.h"
#include "agent_status_shm.hpp"
#include "ipc_def.hpp"

int main(int argc, char *argv[])
{
	manage_shared_memory shm_obj;
	shared_memory_t *shm;
	agent_status_shm *pAgent_shm;

	tLoggedOnEvent agentLoggedOn;
	tLoggedOffEvent agentLoggedOff;
	tNotReadyEvent agentReady;
	tReadyEvent agentNotReady;
	tWorkingAfterCallEvent agentACW;

	AGENT_STATUS_LIST_SHMKEY_t key;
	AGENT_STATUS_LIST_SHM_t data;


	if (argc < 2) {
		fprintf(stderr, "Usage: %s [LOGON | LOGOFF | RDY | NRDY | ACW]\n", argv[0]);
		exit(1);
	}

	if ( (strcmp(argv[1], "LOGON") != 0) &&
             (strcmp(argv[1], "LOGOFF") != 0) &&
             (strcmp(argv[1], "RDY") != 0) &&
             (strcmp(argv[1], "NRDY") != 0) &&
             (strcmp(argv[1], "ACW") != 0) ) {
		fprintf(stderr, "Usage: %s [LOGON | LOGOFF | RDY | NRDY | ACW]\n", argv[0]);
		exit(1);
	}

	

//	shm_obj.remove_shared_memory(REPORT_AREA_SHM);

	try {
		shm = shm_obj.create_shared_memory(REPORT_AREA_SHM, 1024 * 1024);
		//shm = shm_obj.open_shared_memory(REPORT_AREA_SHM);
	}
	catch (std::string ex)	
	{
		std::cerr << ex << std::endl;
		return -1;
	}

	pAgent_shm = new agent_status_shm(&shm_obj, shm);
	if (pAgent_shm == NULL) {
		std::cerr << "Failed to allocate Device manage object" << std::endl;
		return -1;
	}

	if (strcmp(argv[1], "LOGON") == 0) {
		memset(&agentLoggedOn, 0x00, sizeof(tLoggedOnEvent));
		memset(&key, 0x00, sizeof(AGENT_STATUS_LIST_SHMKEY_t));
		memset(&data, 0x00, sizeof(AGENT_STATUS_LIST_SHM_t));
	
		strcpy(key.agentID, "sip:8001@cube-x19.hansol.com");
		strcpy(data.extension, "sip:9001@cube-x19.hansol.com");
		strcpy(data.workmode, STATUS_AUX);
		strcpy(data.auxreason, "");
		strcpy(data.direction, "");
		strcpy(data.agstate, STATUS_AUX); 
		strcpy(data.workskill, "");	
	} else if (strcmp(argv[1], "LOGOFF") == 0) {
		memset(&agentLoggedOff, 0x00, sizeof(tLoggedOffEvent));
		memset(&key, 0x00, sizeof(AGENT_STATUS_LIST_SHMKEY_t));
		memset(&data, 0x00, sizeof(AGENT_STATUS_LIST_SHM_t));
	
		strcpy(key.agentID, "sip:8001@cube-x19.hansol.com");
		strcpy(data.extension, "");
		strcpy(data.workmode, "");
		strcpy(data.auxreason, "");
		strcpy(data.direction, "");
		strcpy(data.agstate, STATUS_UNSTAFF);
		strcpy(data.workskill, "");
	} else if (strcmp(argv[1], "NRDY") == 0) {
		memset(&agentReady, 0x00, sizeof(tNotReadyEvent));
		memset(&key, 0x00, sizeof(AGENT_STATUS_LIST_SHMKEY_t));
		memset(&data, 0x00, sizeof(AGENT_STATUS_LIST_SHM_t));
	
		strcpy(key.agentID, "sip:8001@cube-x19.hansol.com");
		strcpy(data.extension, "sip:9001@cube-x19.hansol.com");
		strcpy(data.workmode, STATUS_AUX);
		strcpy(data.auxreason, "11");
		strcpy(data.direction, "");
		strcpy(data.agstate, STATUS_AUX);
		strcpy(data.workskill, "");
	} else if (strcmp(argv[1], "RDY") == 0) {
		memset(&agentNotReady, 0x00, sizeof(tReadyEvent));
		memset(&key, 0x00, sizeof(AGENT_STATUS_LIST_SHMKEY_t));
		memset(&data, 0x00, sizeof(AGENT_STATUS_LIST_SHM_t));
	
		strcpy(key.agentID, "sip:8001@cube-x19.hansol.com");
		strcpy(data.extension, "sip:9001@cube-x19.hansol.com");
		strcpy(data.workmode, STATUS_READY);
		strcpy(data.auxreason, "");
		strcpy(data.direction, "");
		strcpy(data.agstate, STATUS_READY);
		strcpy(data.workskill, "");	
	} else if (strcmp(argv[1], "ACW") == 0) {
		memset(&agentACW, 0x00, sizeof(tWorkingAfterCallEvent));
		memset(&key, 0x00, sizeof(AGENT_STATUS_LIST_SHMKEY_t));
		memset(&data, 0x00, sizeof(AGENT_STATUS_LIST_SHM_t));
	
		strcpy(key.agentID, "sip:8001@cube-x19.hansol.com");
		strcpy(data.extension, "sip:9001@cube-x19.hansol.com");
		
		if (strcmp(data.workmode, STATUS_ACD) == 0) {
			strcpy(data.workmode, STATUS_ACW);
			strcpy(data.agstate, STATUS_ACW);
		} else if (strcmp(data.workmode, STATUS_DACD) == 0) {
			strcpy(data.workmode, STATUS_DACW);
			strcpy(data.agstate, STATUS_DACW);							
		}
		strcpy(data.auxreason, "");
		strcpy(data.direction, "");
	}
	else {
		std::cout << "Unknown found event" << std::endl;
		if (pAgent_shm != NULL) delete pAgent_shm;
		return -1;
	}

	if (pAgent_shm->register_data(key, data) == false) {
		std::cout << "Register error" << std::endl;
	}
	


//	shm_obj.remove_shared_memory(REPORT_AREA_SHM);

	if (pAgent_shm != NULL) delete pAgent_shm;

	return 0;
}
