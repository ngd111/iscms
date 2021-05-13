#include <boost/multi_index_container.hpp> 
#include <boost/multi_index/hashed_index.hpp> 
#include <boost/multi_index/member.hpp> 
#include <boost/multi_index/composite_key.hpp>
#include <iostream>
#include <string>
#include <boost/tuple/tuple.hpp>
//#include <boost/tuple/tuple_io.hpp>

using namespace boost::multi_index;

struct person 
{ 
  std::string name; 
  int age;

  person(const std::string &n, int a) 
    : name(n), age(a) 
  { 
  } 
}; 

typedef multi_index_container< 
  person, 
  indexed_by< 
    hashed_non_unique< 
      member< 
        person, std::string, &person::name 
      > 
    >, 
    hashed_non_unique< 
      member< 
        person, int, &person::age 
      > 
    >,
    hashed_non_unique<
    	composite_key<
    		person,
    		BOOST_MULTI_INDEX_MEMBER(person, std::string, person::name),
    		BOOST_MULTI_INDEX_MEMBER(person, int, person::age)
    	>
    >    
  > 
> person_multi; 

typedef boost::tuple<std::string, int> mtuple_t;
	
typedef nth_index<person_multi, 2>::type person_multi_by_composite;
	
static person_multi pm;
static person_multi_by_composite& pm_by_composite=pm;

int main() 
{ 
  person_multi persons; 
  
  

  persons.insert(person("Boris", 31)); 
  persons.insert(person("Anton", 35)); 
  persons.insert(person("Caesar", 25)); 

  std::cout << persons.count("Boris") << std::endl;
  	


//  const person_multi::nth_index<1>::type &age_index = persons.get<1>(); 
//  std::cout << age_index.count(25) << std::endl; 
  	
 // std::string name_ = "Boris";
 // int age_ = 2;
 
 // mtuple_t tup_srch("Boris", 2);
  
  mtuple_t tup_srch;
  tup_srch = boost::make_tuple("Boris", 2);
person_multi_by_composite::iterator it = pm.find(tup_srch);  
//  std::cout << persons.count(tup_srch) << std::endl;
//  person_multi::iterator it = persons.find(tup_srch);
 

  	
//  std::cout << persons.count(std::make_tuple(&name_, &age_)) << std::endl;
} 