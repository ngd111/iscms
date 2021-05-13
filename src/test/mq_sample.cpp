#include "mqueue.hpp"

#define WAIT_SECOND 5                 // used in timed_receive

int main()
{
	mqueue *pMQ = new mqueue;
	event_data data;
	
	memset(&data, 0x00, sizeof(event_data));
/* 
	try {
		pMQ->cr_message_queue("iscms_event1", 200, sizeof(__events));
	}
	catch (std::string err_msg)
	{
std::cout << "err:" << err_msg << std::endl;
	}
	
std::cout << "end of mq_sample" << std::endl;
	
*/	
 

std::cout << "step : " << std::endl;	
	try {
		pMQ->create_message_queue("iscms_event1", 200, sizeof(__events));
std::cout << "step1 : " << std::endl;		
	}
	catch (std::string err_msg)
	{
std::cout << "err:" << err_msg << std::endl;
	}
/*	
std::cout << "step5_0-=========================" << std::endl;	
//	int received_size = pMQ->message_recv(&data, sizeof(__events));
	int received_size = pMQ->message_recv_timed(&data, sizeof(__events), WAIT_SECOND);
std::cout << "step5_1-=========================" << std::endl;
*/	
	pMQ->remove_message_queue("iscms_event1");

	delete pMQ;
}
