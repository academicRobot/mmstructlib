#ifndef CIF_PARSE_ERR_MSG_HPP
#define CIF_PARSE_ERR_MSG_HPP

#include <boost/spirit/include/phoenix.hpp>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

namespace cif{

template <typename It>
std::string parse_err_msg(It it, It beginIt, It endIt){
	std::stringstream ss;
	const char eol[] = "\n";
	It bolIt = std::find_end(beginIt, it, eol, eol+1),
	   eolIt = std::search(it, endIt, eol, eol+1);
	if(*bolIt == *eol){ //advance bolIt to first char of line
		++bolIt;
	}
	if(bolIt == it){ //true for it on first line
		bolIt = beginIt;
	}
	ss	<< "Parsing failed at or before line " 
		<< (std::count_if(beginIt, it, boost::phoenix::arg_names::_1 == *eol)+1)
		<< ", column " << (std::distance(bolIt, it)+1) << ":" << std::endl
		<< "      '" << std::string(bolIt, eolIt) << "'" << std::endl
		<< "before" << std::string(std::distance(bolIt, it), '-') 
		<< "^" << std::endl;
	return ss.str();
}

}

#endif //CIF_PARSE_ERR_MSG_HPP

