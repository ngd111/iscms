#include <boost/multi_index_container.hpp> 
#include <boost/multi_index/hashed_index.hpp> 
#include <boost/multi_index/member.hpp> 
#include <iostream> 
#include <string>

using namespace boost::multi_index;



typedef struct _agent_info_key {
	int locale_pos;
	int agentid;
	
	bool operator==(const struct _agent_info_key &p) const {
		if ( (locale_pos == p.locale_pos) &&
			 (agentid == p.agentid) )
			return true;
		else
			return false;
	}
} AGENT_INFO_KEY_t;

typedef struct _agent_info_data {
	char name[128];
	
	_agent_info_data & operator=(_agent_info_data& __data) {
		strcpy(name, __data.name);
		
		return *this;
	}
	
	_agent_info_data & operator+=(_agent_info_data& __data) {
		return *this;
	}
} AGENT_INFO_DATA_t;

typedef struct _agent_info {
	AGENT_INFO_KEY_t  key;
	AGENT_INFO_DATA_t data;
	
//	_agent_info & operator*(_agent_info& __data) {
//		return *this;
//	}

} AGENT_INFO_t;

inline std::size_t hash_value(struct _agent_info_key const &p)
{
    std::size_t seed = 0;
    boost::hash_combine(seed, p.locale_pos);
    boost::hash_combine(seed, p.agentid);
    return seed;
};


typedef multi_index_container< 
  AGENT_INFO_t, 
  indexed_by< 
    hashed_non_unique< 
      member< 
        AGENT_INFO_KEY_t, int, &AGENT_INFO_KEY_t::locale_pos 
      > 
    > ,  
    hashed_non_unique< 
      member< 
        AGENT_INFO_t, AGENT_INFO_KEY_t, &AGENT_INFO_t::key 
      > 
    >
  > 
> agent_multi;

int main() 
{ 
  agent_multi agents;
  
  
  AGENT_INFO_t     data_;
  AGENT_INFO_KEY_t key;
  
  memset(&data_, 0x00, sizeof(AGENT_INFO_t));
  memset(&key, 0x00, sizeof(AGENT_INFO_KEY_t));
  
  data_.key.locale_pos = 1;
  data_.key.agentid = 100;
  strcpy(data_.data.name, "Boris");
  
  agents.insert(data_);
  

//  agents.insert(AGENT_INFO_t(1, 100, "Boris")); 
//  agents.insert(AGENT_INFO_t(1, 101, "Anton")); 
//  agents.insert(AGENT_INFO_t(2, 100, "Caesar"));
//  agents.insert(AGENT_INFO_t(2, 103, "John"));

//	std::cout << agents.count(2) << std::endl;  // Counting agent using locale_pos
  
//  key.locale_pos = 2;
//  key.agentid = 103;
//  std::cout << agents.count(key) << std::endl;  // Counting agent using AGENT_INFO_KEY_t structure

 // const agent_multi::nth_index<1>::type &age_index = persons.get<1>(); 
 // std::cout << age_index.count(25) << std::endl;

}
