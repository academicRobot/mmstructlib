#ifndef LEXER_FILE_KEYWORDS_HPP
#define LEXER_FILE_KEYWORDS_HPP

#include <boost/fusion/container/map.hpp>
#include <string>

namespace cif{

namespace{

const std::string LEXER_TAG_STR("<<<<<<<<token>>>>>>>>");
const std::string LEXER_TYPE_STR("<<<type>>>");
const std::string LEXER_VALUE_STR("<<<value>>>");

}

//typedef struct{} non_type;

template <typename lexer_t>
struct lexer_type_map{
	typedef boost::fusion::map<
//		boost::fusion::pair<non_type, std::string>,
		boost::fusion::pair<typename lexer_t::dot_t, std::string>,
		boost::fusion::pair<typename lexer_t::qmark_t, std::string>,
		boost::fusion::pair<typename lexer_t::base_iterator_range_t,std::string>
	> type;
};
template <typename lexer_t>
typename lexer_type_map<lexer_t>::type make_lexer_type_map(void){
	return typename lexer_type_map<lexer_t>::type(
//		boost::fusion::make_pair<none_type>("none"),
		boost::fusion::make_pair<typename lexer_t::dot_t   >("dot"),
		boost::fusion::make_pair<typename lexer_t::qmark_t >("qmark"),
		boost::fusion::make_pair<typename lexer_t::base_iterator_range_t>("range")
	);
}

}

#endif // LEXER_FILE_KEYWORDS_HPP
