#ifndef UTIL_JOIN_HPP
#define UTIL_JOIN_HPP

#include <string>

namespace util{
	
template <typename It>
std::string join(It it, It endIt, const std::string del){
	std::string ret = *it;
	for(++it; it != endIt; ++it){
		ret += del;
		ret += *it;
	}
	return ret;
}

}

#endif //UTIL_JOIN_HPP
