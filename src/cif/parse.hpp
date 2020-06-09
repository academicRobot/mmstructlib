#include <cif/ostream.hpp>
#include <cif/error_handler.hpp>
#include <cif/parse_err_msg.hpp>
#include <cif/lexer.hpp>
#include <cif/parser.hpp>
#include <util/join.hpp>
#include <boost/spirit/include/qi.hpp>
#include <sstream>
#include <exception>

#include <string>

template <typename it_t>
void parseString(it_t it, it_t endIt, typename cif::parser<cif::lexer<it_t> >::file_t& fileObj){
	typedef cif::lexer<it_t> lexer_t;
	typedef cif::parser<lexer_t> parser_t;
	typedef cif::error_handler<it_t> err_t;
	typedef typename std::vector<std::string>::const_reverse_iterator strs_rev_it_t;
	it_t beginIt = it;
	lexer_t lexer;
	lexer.init_dfa(true);
	err_t errHandle;
	parser_t parser(lexer, errHandle);
	if(!tokenize_and_parse(it, endIt, lexer, parser, fileObj)){
		std::stringstream sio;
		sio
			<< "**parse failed** " << std::endl
			<< 
				util::join(
					strs_rev_it_t(errHandle.stackLocs.end()),
					strs_rev_it_t(errHandle.stackLocs.begin()),
					" -> "
				)
			<< ":" << errHandle.msg << std::endl
			<< cif::parse_err_msg(it, beginIt, endIt);
		throw std::runtime_error(sio.str());
	}
}

