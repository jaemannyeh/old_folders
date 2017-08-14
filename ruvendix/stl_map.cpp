// https://msdn.microsoft.com/en-us/library/s44w4h2s.aspx

#include <stdio.h>
#include <iostream>
#include <map>

//////// //////// //////// ////////

#if 0
template <class Key,   
    class Type,   
    class Traits = less<Key>,   
    class Allocator=allocator<pair <const Key, Type>>>  
class map; 
#endif

//////// //////// //////// ////////

int main_010(void) {
    std::cout << __FUNCTION__ << std::endl;
    
    std::map<char,int> m;
    
    m['a'] = __LINE__;
    m.insert(std::map<char, int>::value_type('b',__LINE__));

    std::cout << m['a'] << std::endl;
    std::cout << m.at('b') << std::endl;
    
    return 0;
}

//////// //////// //////// ////////

int main(void) {
    main_010();
}
