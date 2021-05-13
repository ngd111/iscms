#include <boost/multi_index_container.hpp> 
#include <boost/multi_index/hashed_index.hpp> 
#include <boost/multi_index/member.hpp> 
#include <iostream> 
#include <string>

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
    > 
  > 
> person_multi; 

int main() 
{ 
  person_multi persons; 

  persons.insert(person("Boris", 31)); 
  persons.insert(person("Anton", 35)); 
  persons.insert(person("Caesar", 25)); 

  std::cout << persons.count("Boris") << std::endl; 

  const person_multi::nth_index<1>::type &age_index = persons.get<1>(); 
  std::cout << age_index.count(25) << std::endl; 
} 