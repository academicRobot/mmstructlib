//#define BOOST_SPIRIT_DEBUG 
#include <cif/lexer.hpp>
//#include <cif/static_parser.hpp>
#include <cif/parser.hpp>
//#include <cif/parse_file.hpp>
#include <cif/ostream.hpp>
#include <cif/error_handler.hpp>
#include <cif/parse_err_msg.hpp>
#include <util/join.hpp>
//#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/spirit/include/qi.hpp>
#include <iostream>
#include <fstream>
#include <iterator>

typedef const char* mmap_iterator;

//typedef cif::lexer<boost::spirit::istream_iterator> lexer_t;
typedef cif::lexer<mmap_iterator> lexer_t;
typedef cif::parser<lexer_t> parser_t;
typedef cif::error_handler<mmap_iterator> err_t;
//typedef cif::static_parser<lexer_t::iterator_type> parser_t;

typedef std::vector<std::string> strs_t;
typedef strs_t::const_iterator strs_it_t;
typedef std::reverse_iterator<strs_it_t> strs_rev_it_t;

int main(int argc, char* argv[]){
	if(argc != 2){
		std::cerr << "usage: testParserFile <cif file>" << std::endl;
		return 1;
	}

	boost::iostreams::mapped_file_source mapfile;
	mapfile.open(argv[1]);
	if(!mapfile.is_open()){
		std::cerr << "Could not open file '" << argv[1] << "'" << std::endl;
		return 1;
	}

	mmap_iterator it(mapfile.data()), endIt(mapfile.data()+mapfile.size());

	lexer_t lexer;
	lexer.init_dfa(true); //minimize the dfa
	err_t errHandle;
	parser_t parser(lexer, errHandle);

	parser_t::file_t file;
	bool r = tokenize_and_parse(it, endIt, lexer, parser, file);
	if(!r){
		std::cerr 
			<< "**parse failed** " << std::endl
			<< util::join(strs_rev_it_t(errHandle.stackLocs.end()),
			              strs_rev_it_t(errHandle.stackLocs.begin()), 
			              " -> ")
			<< ":" << errHandle.msg << std::endl
			//<< cif::parse_err_msg(errHandle.startIt, mapfile.data(), endIt);
			<< cif::parse_err_msg(it, mapfile.data(), endIt);
		return 1;
	}

	std::cout << file;

	return 0;
}

