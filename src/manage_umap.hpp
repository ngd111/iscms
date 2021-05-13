//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
//
//////////////////////////////////////////////////////////////////////////////


#ifndef MODULE_MANAGE_UMAP_INCLUDED
#define MODULE_MANAGE_UMAP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif


#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/unordered_map.hpp>


using namespace boost::interprocess;


// TKEY  : Key
// TDATA : Value
template <class TKEY, class TDATA>
class manage_umap
{
	public:
		typedef std::pair<const TKEY, TDATA> map_pair_t;
		typedef boost::unordered_map<TKEY, TDATA> mapAlt_t;
		typedef std::pair<typename mapAlt_t::iterator, bool> iter_b_pair_t;
			
	public:
		//manage_umap(mapAlt_t *__map_ptr) : map_ptr(__map_ptr){};
		manage_umap(){
			map_ptr = new mapAlt_t;
		};
		~manage_umap(){
			if (map_ptr != NULL) {
				clear();
				delete map_ptr;
			}
		};
		
		//insert
		bool insert(TKEY __key, TDATA __data) {
			return (map_ptr->insert(std::make_pair(__key, __data)).second);
		};
		
		iter_b_pair_t insert(map_pair_t __p){
			return (map_ptr->insert(__p));
		};
		
		iter_b_pair_t find(TKEY __key){
			typename mapAlt_t::iterator it = map_ptr->find(__key);
			return (std::make_pair(it, it == map_ptr->end()?false:true));
		};
	
		// Read value
		TDATA* read_data(TKEY __key){
			typename mapAlt_t::iterator it = map_ptr->find(__key);
			return (it == map_ptr->end()?NULL:&(it->second));
		};
		
		// Clear memory
		void clear(){
			map_ptr->clear();
		};
		
		// Update data
		bool update(TKEY __key, TDATA __data){
			iter_b_pair_t r = find(__key);
			
			if (check_pair_it_bool(r) == true) {
				typename mapAlt_t::iterator it = r.first;
				TDATA* r_data = &(it->second);
				*r_data = __data;
				//memcpy(r_data, &__data, sizeof(TDATA));
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
		
		// Delete data
		size_t erase(TKEY __key){
			return map_ptr->erase(__key);
		};
		
		// Count
		size_t size() const{
			return map_ptr->size();
		};
		
		// Check data using key
		bool check_exists(TKEY __key){
			typename mapAlt_t::iterator it = map_ptr->find(__key);
			return (it == map_ptr->end()?false:true);
		};
		
		// Check pair type data
		bool check_pair_it_bool(iter_b_pair_t __p){
			return (__p.second);
		};

		// Get memory ptr
		mapAlt_t* get_map_ptr(){
			return map_ptr;
		};	
		
	private:
		mapAlt_t *map_ptr;
};


#endif // MODULE_MANAGE_UMAP_INCLUDED
