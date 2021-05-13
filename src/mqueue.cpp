#include "mqueue.hpp"


mqueue::mqueue() {
}

mqueue::~mqueue() {
	if (message_que != NULL)
		delete message_que;
}
	
//message_queue *
void
mqueue::create_message_queue(const char* __name,
                             const size_t __max_size,
                             const size_t __message_size) throw(std::string)
{
	try
	{
		object_name = __name;
		message_size = __message_size;
		
		remove_message_queue(__name);
//		if (remove_message_queue(__name) == false)
//			return NULL;
	
		message_que = new message_queue(open_or_create, __name, __max_size, __message_size);
		/*	// manage_shared_memory.hpp
		message_queue mq
		(
		create_only        //only create
		, "message_queue"  //name
		, 100              //max message number
		, sizeof(packet)   //max message size
		);
		*/
	}
	catch (boost::interprocess::bad_alloc &ex)
	{
		std::cerr << ex.what() << std::endl;
		throw ex.what();				
	}
	catch (boost::interprocess::interprocess_exception &ex)
	{
		std::cerr << ex.what() << std::endl;
		throw ex.what();
	}
	
	return;
	//return message_que;
}

message_queue *
mqueue::open_message_queue(const char* __name) throw(std::string) 
{
	try
	{
		object_name = __name;
		message_que = new message_queue(open_only, __name);
	}
	catch (boost::interprocess::bad_alloc &ex)
	{
		std::cerr << ex.what() << std::endl;
		throw ex.what();				
	}
	catch (boost::interprocess::interprocess_exception &ex)
	{
		std::cerr << ex.what() << std::endl;
		throw ex.what();
	}
	
	return message_que;
}

bool mqueue::remove_message_queue(const char* __name) {
	return (message_queue::remove(__name));
}

// Maximun number of messages allowed by the queue
size_t mqueue::message_queue_max() {
	return (message_que->get_max_msg());
}

// Maximun size of message allowed by the queue
size_t mqueue::message_queue_max_msg_size() {
	return (message_que->get_max_msg_size());
}

// Number of messages currently stored
size_t mqueue::message_queue_number_stored() {
	return (message_que->get_num_msg());
}

void mqueue::set_message_size(size_t __message_size) {
    message_size = __message_size;
}

// Send message
void mqueue::message_send(void const  *__buffer) {
	// Default priority(3rd parameter) -> 0
	message_que->send(__buffer, message_size, 0);
}

// Receive message
size_t mqueue::message_recv(void *__buffer, size_t __message_size) {
	size_t received_size;
	unsigned int priority;

//std::cout << "in message_recv : before" << std::endl;
	// Default priority(4th parameter)
	try {
		message_que->receive(__buffer, __message_size, received_size, priority);
	}
	catch (boost::interprocess::interprocess_exception &ex) {
		std::cout << "ex : " << ex.what() << std::endl;
	}
//std::cout << "in message_recv : after" << std::endl;
	
	return received_size;
}

bool mqueue::message_recv_timed(void *__buffer, size_t __message_size
	, size_t __waitseconds) {
	size_t received_size;
	unsigned int priority;
	bool bRet;

//std::cout << "in message_recv_timed : before" << std::endl;	
	// Default priority(4th parameter)
	try {
		bRet = message_que->timed_receive(__buffer, __message_size, received_size, 
		     priority, ptime(second_clock::universal_time()) + seconds(__waitseconds) );
	}
	catch (boost::interprocess::interprocess_exception &ex) {
		std::cout << "ex : " << ex.what() << std::endl;
	}
//std::cout << "in message_recv_timed : after " << std::endl;
	//mq.receive(&number, sizeof(number), recvd_size, priority);
	
	return bRet;
}
