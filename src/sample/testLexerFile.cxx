
#include <cif/lexer.hpp>
#include "lexer_test_parser.hpp"
#include "lexer_file_keywords.hpp"

#include <boost/spirit/include/support_istream_iterator.hpp>
//#include <boost/spirit/include/support_multi_pass.hpp>

#include <iostream>
#include <fstream>
#include <streambuf>

typedef cif::lexer<std::string::const_iterator> lexer_t;
typedef cif::lexer_test_parser<lexer_t::iterator_type> parser_t;

//overload output for non_type
namespace std{
std::ostream& operator<<(
	std::ostream& os, 
	const lexer_t::base_iterator_range_t& range
)
{
	os << std::string(range.begin(), range.end());
	return os;
}
std::ostream& operator<<(std::ostream& os, const lexer_t::qmark_t&){return os;}
std::ostream& operator<<(std::ostream& os, const lexer_t::dot_t&){return os;}
}

class VariantPrintFunc {
	public:
		template <typename A, typename B = void> 
		struct result{ typedef void type; };

		VariantPrintFunc(std::ostream& os) : 
			os(os), 
			typeMap(cif::make_lexer_type_map<lexer_t>()) 
		{}

		template <typename type_t>
		void operator()(const std::string& tag, const type_t& value) const{
			os << cif::LEXER_TAG_STR      << std::endl
			   << tag                     << std::endl
			   << cif::LEXER_TYPE_STR     << std::endl 
			   << boost::fusion::at_key<type_t>(typeMap) << std::endl
			   << cif::LEXER_VALUE_STR    << std::endl
			   << value                   << std::endl;
		}

		void operator()(const std::string& tag) const{
			os << cif::LEXER_TAG_STR      << std::endl
			   << tag                     << std::endl
			   << cif::LEXER_TYPE_STR     << std::endl 
			   << "none"                  << std::endl;
		}
	private:
		std::ostream& os;
		cif::lexer_type_map<lexer_t>::type typeMap;
};

int main(int argc, char* argv[]){
	if(argc != 2){
		std::cerr << "usage: cifLexerTestFile <cif file>" << std::endl;
		return 1;
	}

	std::ifstream inFH(argv[1]);
	if(!inFH.is_open()){
		std::cerr << "Could not open file '" << argv[1] << "'" << std::endl;
		return 1;
	}
	//inFH.unsetf(std::ios::skipws);
	std::string fileStr;
	
	inFH.seekg(0, std::ios::end);
	fileStr.reserve(inFH.tellg());
	inFH.seekg(0, std::ios::beg);

	fileStr.assign((std::istreambuf_iterator<char>(inFH)), std::istreambuf_iterator<char>());

	std::string::const_iterator it = fileStr.begin(), endIt = fileStr.end();

	lexer_t lexer;
	lexer.init_dfa(true); //minimize the dfa
	VariantPrintFunc testFunc(std::cout);
	parser_t parser(lexer, testFunc);

	bool r = tokenize_and_parse(it, endIt, lexer, parser);

	if(!r || it != endIt){
		std::string str;
		std::getline(inFH, str);
		std::cerr << "**parse failed**" << std::endl << ">>>"<< str << "<<<" << std::endl;
		return 1;
	}

	return 0;
}


