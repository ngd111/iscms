//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MODULE_STAT_BASE_INCLUDED
#define MODULE_STAT_BASE_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/interprocess/sync/named_mutex.hpp>
#include "manage_umap.hpp"
#include "base_def.hpp"
#include "length_def.hpp"

using namespace boost::interprocess;


template<class TKEY, class TDATA>
class statbase : public manage_umap<TKEY, TDATA>
{
	public:
		
		statbase() {
			inner_mutex = false;
		};
		
		statbase(const char* __mutex_name) {
			create_resource_objects(__mutex_name);
		};
		
		~statbase() {
			remove_mutex();
		};
		
        bool create_resource_objects(const char *__mutex_name) {
            return create_or_open_mutex(__mutex_name);
        };		
		
        // Designate mutex created from out of the class
        void set_outer_mutex(named_mutex * __mutex, const char *__mutex_name) {
            remove_mutex();
            mtx = __mutex;
            strcpy(mutex_name, __mutex_name);
            inner_mutex = false;
        };
        
		// Get mutex pointer
		named_mutex* get_mutex_ptr() {
			return mtx;
		};
        		
		///////////// Report handling method //////////////////////////////////
		bool register_report(TKEY key, TDATA data
			, DUP_OPTION::ProcType __option = DUP_OPTION::ACCUMULATE) {
			
			mtx->lock();
			
			// manage_umap의 find함수
			if (this->check_exists(key) == false) {
				if (this->insert(key, data) == false) {
				std::cout << "in statbase register_report data inserted" << std::endl;
					mtx->unlock();
					return false;
				}
			}
			else {
				if (__option == DUP_OPTION::ACCUMULATE) {
					if (this->accumulate(key, data) == false) {
						mtx->unlock();
						return false;
					}				
				}
				else if (__option == DUP_OPTION::UPDATE) {
					if (this->update(key, data) == false) {
				std::cout << "in statbase register_report data updated" << std::endl;
						mtx->unlock();
						return false;
					}				
				}
				else if (__option == DUP_OPTION::REPLACE) {
					if (this->replace(key, data) == false) {
						mtx->unlock();
						return false;
					}				
				}
			}
			
			mtx->unlock();
			
			return true;
		};
		
		bool delete_report(TKEY key) {
			mtx->lock();
			
			if (this->erase(key) == 0) {
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

	private:
		char mutex_name [MUTEX_NAME_LEN + 1];
		bool inner_mutex; // true : innter mutex, false : outer mutex
		named_mutex *mtx;
};

#endif // MODULE_STAT_BASE_INCLUDED
