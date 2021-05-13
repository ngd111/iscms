//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
// Writer : Jin kak, Jung
// Revision history : 
// 			First released on Oct. 21, 2014
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MODULE_STATBASE_SHM_INCLUDED
#define MODULE_STATBASE_SHM_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/interprocess/sync/named_mutex.hpp>
#include "manage_shared_memory.hpp"
#include "manage_shared_umap.hpp"
#include "length_def.hpp"
#include "base_def.hpp"

#include <iostream>


using namespace boost::interprocess;


template<class TKEY, class TDATA>
class statbase_shm : public manage_shared_umap<TKEY, TDATA>
{
	public:
		
    	typedef typename manage_shared_umap<TKEY, TDATA>::mapAlt_t umap_t;
    	typedef typename manage_shared_umap<TKEY, TDATA>::mapAllocator_t mapAllocator_t;
    	typedef typename umap_t::iterator umap_itor_t;
    	typedef offset_ptr<umap_t> umap_offset_ptr;

	public:
		
		statbase_shm() {
			inner_mutex = false;
		};

		statbase_shm(const char* __mutex_name) {
			create_or_open_mutex(__mutex_name);
		};

		~statbase_shm(){		
			//std::cout << "destructor statbase_shm" << std::endl;
			destroy_resource_objects();
		};

		///////////// Object management method //////////////////////////////////
		
		bool create_resource_objects(const char *__container_name, 
									 const char *__mutex_name) {
			if (create_container(__container_name) == false)
				return false;

			if (create_or_open_mutex(__mutex_name) == false)
				return false;
				
			strcpy(container_name, __container_name);

			return true;
		};
		
		bool destroy_resource_objects() {
			remove_mutex();
			return true;
//			return (shmobj->destroy_container<umap_t>(container_name));
		};

		// Designate mutex pointer created from out of the class
		void set_outer_mutex(named_mutex * __mutex, const char *__mutex_name) {
			remove_mutex();
			mtx = __mutex;
			strcpy(mutex_name, __mutex_name);
			inner_mutex = false;
		};
		
		
		// Get smart pointer to the container
		umap_offset_ptr get_umap_offset_ptr() {
			return shm->find<umap_t>(container_name).first;
		};
		
		// Get mutex pointer
		named_mutex* get_mutex_ptr() {
			return mtx;
		};

		///////////// Report handling method //////////////////////////////////
		bool register_report(TKEY __key, TDATA __data
			, DUP_OPTION::ProcType __option = DUP_OPTION::ACCUMULATE) {
			mtx->lock();
	
			// manage_umap의 find함수
			if (this->check_exists(__key) == false) {
				if (this->insert(__key, __data) == false) {
					mtx->unlock();
					return false;
				}
			}
			else {
				if (__option == DUP_OPTION::ACCUMULATE) {
					if (this->accumulate(__key, __data) == false) { // += operator
						mtx->unlock();
						return false;				
					}
				}
				else if (__option == DUP_OPTION::UPDATE) {          // == operator
					if (this->update(__key, __data) == false) {
						mtx->unlock();
						return false;				
					}
				}
				else if (__option == DUP_OPTION::REPLACE) {         // No operator
					if (this->replace(__key, __data) == false) {
						mtx->unlock();
						return false;
					}
				}
			}
			
			mtx->unlock();
			
			return true;
		};
		
		bool delete_report(TKEY __key) {
			mtx->lock();
			
			if (this->erase(__key) == 0) {
				mtx->unlock();
				return false;
			}
			
			mtx->unlock();
			
			return true;
		};

		void truncate() {
			mtx->lock();
			
			this->clear();
			
			mtx->unlock();
		};

	private:
		bool create_or_open_mutex(const char *__mutex_name) {
			if ( (strlen(__mutex_name) > MUTEX_NAME_LEN) ||
				 (strlen(__mutex_name) == 0) )
				 return false;
			
			strcpy(mutex_name, __mutex_name);
			
			mtx = new named_mutex(open_or_create, mutex_name);
			inner_mutex = true;
			
			return true;
		};
		
		// Delete inner mutex object only
		void remove_mutex() {
			if (strlen(mutex_name) > 0) {
				named_mutex::remove(mutex_name);
				memset(mutex_name, 0x00, sizeof(mutex_name));
			}
			
			if (mtx != NULL) {
				if (inner_mutex == true) {
					delete mtx; mtx = NULL;
				}
			}
		};		

		// Only SHM_CONTAINER::USEDEFAULT type supported currently
		bool create_container(const char *__container_name,
			SHM_CONTAINER::UsingHashType __type = SHM_CONTAINER::USEDEFAULT) {
				
			try {
				if (__type == SHM_CONTAINER::NOHASH) {
					//create_container_object_nohash(__container_name, ~~
				}
				else if (__type == SHM_CONTAINER::USEHASH) {
				}
				else if (__type == SHM_CONTAINER::USEDEFAULT) {
					pmap = shmobj->create_container_object_hash_use_default<umap_t, TKEY, mapAllocator_t>(__container_name, shm->get_segment_manager());
				}
				else // Unknown container creation type
					return false;
			}
			catch (interprocess_exception &ex) {
				std::cerr << ex.what() << std::endl;
				return false;
			}

			this->bind(pmap);

			return true;
			
		};		
		
	protected:
		shared_memory_t *shm;
		manage_shared_memory *shmobj;

		umap_t *pmap;
		umap_itor_t itor; // unordered map iterator
	private:
		char mutex_name [MUTEX_NAME_LEN + 1];
		char container_name [CONTAINER_NAME_LEN + 1];
		bool inner_mutex; // true : inner mutex, false : outer mutex
		named_mutex *mtx;
};

#endif // MODULE_STATBASE_SHM_INCLUDED
