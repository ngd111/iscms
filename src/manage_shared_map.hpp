//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
//
//////////////////////////////////////////////////////////////////////////////


#ifndef MODULE_MANAGE_SHARED_MAP_INCLUDED
#define MODULE_MANAGE_SHARED_MAP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>


using namespace boost::interprocess;
	
	
// T: Container type
// TKEY : 
// TDATA:
template <class T, class TKEY, class TDATA>
class manage_shared_map
{
	public:
//		typedef std::pair<const typename KeyType, typename MappedType> ValueType;
//		typedef allocator<ValueType, managed_shared_memory::segment_manager> mapAllocator;
//		typedef map<typename KeyType, typename MappedType, TCOMPARE(), mapAllocator> map_ref;  

		typedef std::pair<TKEY, TDATA> map_pair_t;
		typedef std::pair<typename T::iterator, bool> iter_b_pair_t;
		
	public:
		manage_shared_map(T *__map_ptr) : map_ptr(__map_ptr){};
		~manage_shared_map(){};
		
		//insert
		bool insert(TKEY __key, TDATA __data) {
			//std::pair<typename T::iterator, bool> result;
			
//			result = map_ptr->insert(std::make_pair(__key, __data));
//			return (result.second);
			return (map_ptr->insert(std::make_pair(__key, __data)).second);
		};
		
		iter_b_pair_t insert(map_pair_t __p) {
			return (map_ptr->insert(__p));
		};

		iter_b_pair_t find(TKEY __key) {
			typename T::iterator it = map_ptr->find(__key);
			return (std::make_pair(it, it == map_ptr->end()?false:true));
		};
		
		TDATA* read_data(TKEY __key) {
			typename T::iterator it = map_ptr->find(__key);
			return (it == map_ptr->end()?NULL:&(it->second));
		};
		
		// clear memory
		void clear() { map_ptr->clear(); };

		// update
		bool update(TKEY __key, TDATA __data) {
			iter_b_pair_t r = find(__key);
			if (check_pair_it_bool(r) == true) {
				typename T::iterator it = r.first;
				TDATA* r_data = &(it->second);
				memcpy(r_data, &__data, sizeof(TDATA));
				return true;
			}
			else
				return false;
		};
		
		// delete
		size_t erase(TKEY __key) {
			return map_ptr->erase(__key);
		};
		
		// count
		size_t size() const {
			return map_ptr->size();
		};

		// check data using key		
		bool check_exists(TKEY __key) {
			typename T::iterator it = map_ptr->find(__key);
			return (it == map_ptr->end()?false:true);
		};
		
		// check pair type data
		bool check_pair_it_bool(iter_b_pair_t __p) {
			return (__p.second);
		};
		
		/*
		typename T::iterator get_start_position() {
			return map_ptr->begin();
		};
		*/
		
	private:
		T *map_ptr;
};

#endif // MODULE_MANAGE_SHARED_MAP_INCLUDED
