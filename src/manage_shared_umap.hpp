//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
// Author : Jin kak, Jung
// 
//////////////////////////////////////////////////////////////////////////////


#ifndef MODULE_MANAGE_SHARED_UMAP_INCLUDED
#define MODULE_MANAGE_SHARED_UMAP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/unordered_map.hpp>



using namespace boost::interprocess;

typedef boost::interprocess::managed_shared_memory shared_memory_t;

// TKEY : Key
// TDATA: Value
template <class TKEY, class TDATA>
class manage_shared_umap
{
	public:
		typedef std::pair<const TKEY, TDATA> map_pair_t;
		typedef boost::interprocess::allocator<map_pair_t, managed_shared_memory::segment_manager> mapAllocator_t;
		typedef boost::unordered_map<TKEY, TDATA, boost::hash<TKEY>, std::equal_to<TKEY>, mapAllocator_t> mapAlt_t;
		typedef std::pair<typename mapAlt_t::iterator, bool> iter_b_pair_t;
		
	public:
		
//		manage_shared_umap(mapAlt_t *__map_ptr) : map_ptr(__map_ptr){};

		// Insertion with key, value
		bool insert(TKEY __key, TDATA __data) {
			return (map_ptr->insert(std::make_pair(__key, __data)).second);
		};
		
		// Insertion with pair data
		iter_b_pair_t insert(map_pair_t __p) {
			return (map_ptr->insert(__p));
		};


		// Find data with key
		// Return value : iterator
		iter_b_pair_t find(TKEY __key) {
			typename mapAlt_t::iterator it = map_ptr->find(__key);
			return (std::make_pair(it, it == map_ptr->end()?false:true));
		};
		
		// Find data with key
		// Return value : value
		TDATA* read_data(TKEY __key){
			typename mapAlt_t::iterator it = map_ptr->find(__key);
			return (it == map_ptr->end()?NULL:&(it->second));
		};
		
		// clear memory
		void clear(){
			map_ptr->clear();
		};
		
		// get key
		const TKEY* get_key(typename mapAlt_t::iterator __it){
			return &(__it->first);
		};
		
		// get data
		TDATA* get_data(typename mapAlt_t::iterator __it){
			return &(__it->second);
		};

		// Update data
		bool update(TKEY __key, TDATA __data){
			iter_b_pair_t r = find(__key);
			if (check_pair_it_bool(r) == true) {
				typename mapAlt_t::iterator it = r.first;
				TDATA* r_data = &(it->second);
				*r_data = __data;
				return true;
			}
			else
				return false;
		};
		
		// Replace data
		bool replace(TKEY __key, TDATA __data){
			iter_b_pair_t r = find(__key);
			
			if (check_pair_it_bool(r) == true) {
				typename mapAlt_t::iterator it = r.first;
				TDATA* r_data = &(it->second);
				memcpy(r_data, &__data, sizeof(TDATA));
				return true;
			}
			else
				return false;
		};
				
		// Accumulate data
		bool accumulate(TKEY __key, TDATA __data){
			iter_b_pair_t r = find(__key);
			
			if (check_pair_it_bool(r) == true) {
				typename mapAlt_t::iterator it = r.first;
				TDATA* r_data = &(it->second);
				*r_data += __data;
				return true;
			}
			else
				return false;
		};		
		
		// Delete data with key
		size_t erase(TKEY __key){
			return map_ptr->erase(__key);
		};
		
		// Return data count
		size_t size() const {
			return map_ptr->size();
		};

		// Check data existence with key		
		bool check_exists(TKEY __key) {
			typename mapAlt_t::iterator it = map_ptr->find(__key);
			return (it == map_ptr->end()?false:true);
		};
		
		bool check_pair_it_bool(iter_b_pair_t __p){
			return (__p.second);
		};

	protected:

		void bind(mapAlt_t *__map_ptr) {
			map_ptr = __map_ptr;
		};

	private:
		mapAlt_t *map_ptr;
};

#endif // MODULE_MANAGE_SHARED_UMAP_INCLUDED
