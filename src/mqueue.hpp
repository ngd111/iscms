//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MESSAGE_QUEUE_INCLUDED
#define MESSAGE_QUEUE_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/interprocess/ipc/message_queue.hpp>

using namespace boost::interprocess;
using namespace boost::posix_time;


class mqueue {
	public:
		mqueue();
		~mqueue();

		//void
		//message_queue *
		void
		create_message_queue(const char* name,           // message queue name
		                     const size_t max_size,      // max message number
		                     const size_t message_size   // max message size
		                    ) throw(std::string);
		
		message_queue *
		open_message_queue(const char* name) throw(std::string);

		bool remove_message_queue(const char* name);
		
		// Maximun number of messages allowed by the queue
		size_t message_queue_max();
		
		// Maximun size of message allowed by the queue
		size_t message_queue_max_msg_size();
		
		// Number of messages currently stored
		size_t message_queue_number_stored();
		
		// Send message
		void message_send(void const *buffer);
		
		// Receive message
		// Return : received bytes -> 
		size_t message_recv(void *buffer, size_t message_size);
		
		// Receive message with timeout
		// Return : received bytes
		bool message_recv_timed(void *buffer, size_t message_size, size_t waitseconds);
		
		void set_message_size(size_t message_size);

	private:
		message_queue *message_que = NULL;
		size_t message_size;
		std::string object_name;		
};

#endif // MESSAGE_QUEUE_INCLUDED
