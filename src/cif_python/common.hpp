#include <boost/python.hpp>
#include <cif/lexer.hpp>
#include <cif/parser.hpp>
#include <cif/ostream.hpp>
#include <cif/error_handler.hpp>
#include <cif/parse_err_msg.hpp>
#include <util/join.hpp>
//#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/spirit/include/qi.hpp>
#include <sstream>
#include <exception>
#include <string>

typedef std::string::const_iterator str_iterator;
typedef cif::lexer<str_iterator> lexer_t;
typedef cif::parser<lexer_t> parser_t;
typedef cif::error_handler<str_iterator> err_t;

typedef std::vector<std::string> strs_t;
typedef strs_t::const_iterator strs_it_t;
typedef std::reverse_iterator<strs_it_t> strs_rev_it_t;

using namespace boost::python;

