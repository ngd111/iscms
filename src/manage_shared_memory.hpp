//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
// Writer : Jin kak, Jung
// Revision history : 
// 			First released on Oct. 21, 2014
//
//////////////////////////////////////////////////////////////////////////////


#ifndef MODULE_MANAGE_SHARED_MEMORY_INCLUDED
#define MODULE_MANAGE_SHARED_MEMORY_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/interprocess/managed_shared_memory.hpp>

using namespace boost::interprocess;


namespace SHM_CONTAINER
{
    enum UsingHashType
    {
        NOHASH,
        USEHASH,
        USEDEFAULT
    };
}


typedef boost::interprocess::managed_shared_memory shared_memory_t;

class manage_shared_memory
{
	public:
		manage_shared_memory(){
			managed_shm = NULL;
		};
		~manage_shared_memory(){
			//std::cout << "destructor in manage_shared_memory" << std::endl;
			if (managed_shm != NULL)
				delete managed_shm;
		};
		
		// Erases a shared memory object
		bool remove_shared_memory(const char* name) {
			return (shared_memory_object::remove(name));
		};
		
		// Shared memory named object creation. Initializes the shared memory segment.
		// Create named object. if it's already exist, open named object.
		shared_memory_t* create_shared_memory(const char* name, 
			                                  const size_t size) throw(std::string) {               	
			try
			{
				object_name = name;
				managed_shm = new managed_shared_memory(open_or_create, name, size);
			}
			catch (boost::interprocess::bad_alloc &ex)
			{
				std::cerr << ex.what() << std::endl;
				throw ex.what();
			}
			catch (interprocess_exception &ex)
			{
				std::cerr << ex.what() << std::endl;
				throw ex.what();
			}
			
			return managed_shm;
		};
		
		// Open shared memory named object. If it's not exist, throws error 
		shared_memory_t* open_shared_memory(const char* name) throw(std::string){
		
			try
			{
				object_name = name;
				managed_shm = new managed_shared_memory(open_only, name);
			}
			catch (boost::interprocess::bad_alloc &ex)
			{
				std::cerr << "open_shared_memory error : " << ex.what() << std::endl;
				throw ex.what();
			}
			catch (interprocess_exception &ex)
			{
				std::cerr << ex.what() << std::endl;
				throw ex.what();
			}
			catch (...)
			{
				std::cerr << "generic shared memory open error" << std::endl;
			}
			
			return managed_shm;
		};

		// Create data object in shared memory.
		// T: Container type
		// TCOMPARE : comparison class
		// TALLOCATOR : allocator object
		template<class T, class TCOMPARE, class TALLOCATOR>
		T* create_container_object_nohash(const char* name, TALLOCATOR alloc){		
			try
			{
				return managed_shm->find_or_construct<T>(name)(TCOMPARE(), alloc);
			}
			catch (interprocess_exception &ex)
			{
				std::cerr << ex.what() << std::endl;
			}
		};
		
		template<class T, class THASH , class TCOMPARE, class TALLOCATOR>
		T* create_container_object_hash(const char* name, TALLOCATOR alloc){
			try
			{
				return managed_shm->find_or_construct<T>(name)(3, THASH(), TCOMPARE(), alloc);
			}
			catch (interprocess_exception &ex)
			{
				std::cerr << ex.what() << std::endl;
			}
		};
		
		template<class T, class TKEY, class TALLOCATOR>
		T* create_container_object_hash_use_default(const char* name, TALLOCATOR alloc){		
			try
			{
				return managed_shm->find_or_construct<T>(name)(3, boost::hash<TKEY>(), std::equal_to<TKEY>(), alloc);
			}
			catch (interprocess_exception &ex)
			{
				std::cerr << ex.what() << std::endl;
			}
		};
		
		// Reserved method ; Do not use this. Need more test.
		template<class T>
		bool destroy_container(const char* name){
			try {
				return (managed_shm->destroy<T>(name));
			}
			catch (interprocess_exception &ex)
			{
				std::cerr << ex.what() << std::endl;
				return false;
			}
		};
		
		// Get smart pointer for iterator 
		template<class T>
		offset_ptr<T> get_object_ptr(const char* name){
			return (managed_shm->find<T>(name).first);
		};
		
		// Return currently allocated shared memory size
		size_t get_size() const{
			return (managed_shm->get_size());
		};
		
		// Minimize the size of the segment
		void shink_shared_memory(const char* name){
			managed_shared_memory::shrink_to_fit(name);
		};
		
		// Grow extra memory
		void grow_shared_memory(const char* name, size_t add_size){
			managed_shared_memory::grow(name, add_size);
		};
		
		std::string get_shared_memory_name() const{
			return object_name;
		};
		
	private:
		shared_memory_t *managed_shm;
		std::string object_name;
};

#endif // MODULE_MANAGE_SHARED_MEMORY_INCLUDED
