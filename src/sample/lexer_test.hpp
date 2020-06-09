#ifndef LEXER_TEST_HPP
#define LEXER_TEST_HPP

#include <boost/test/included/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <cif/lexer.hpp>
#include "lexer_test_parser.hpp"
#include "lexer_file_keywords.hpp"
#include "lexer_file_iterator.hpp"

#include <boost/spirit/include/support_istream_iterator.hpp>
//#include <boost/spirit/include/support_multi_pass.hpp>

#include <boost/lexical_cast.hpp>

#include <limits>
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>

using namespace boost::unit_test;

typedef std::pair<std::string,std::string> strPair;

//convert function with overload to accomodate non_type
template <typename type_t>
type_t convertVal(const std::string& str){
	return boost::lexical_cast<type_t>(str);
}
template <>
cif::detail::non_type 
convertVal<cif::detail::non_type>(const std::string& str){ 
	return cif::detail::non_type(); 
}

template <typename lexer_tp>
class	TestFunc {
	public:
		template <typename A, typename B> 
		struct result{ typedef void type; };
		//template <typename A> struct result<A,A>{ typedef void type; };

		typedef lexer_tp lexer_t;
		typedef typename lexer_t::non_t non_t; 
		typedef typename lexer_t::int_t int_t; 
		typedef typename lexer_t::float_t float_t; 
		typedef typename lexer_t::string_t string_t; 

		TestFunc(std::istream& is) : 
			it(is), 
			endIt(),
			typeMap(cif::make_lexer_type_map<lexer_t>()) 
		{}

		template <typename type_t>
		void operator()(const std::string& name, const type_t& val){
			if(is_end()){
				BOOST_CHECK(false);
				return;
			}

			//name
			BOOST_CHECK_EQUAL(name, it->name);

			//type
			BOOST_CHECK_EQUAL(boost::fusion::at_key<type_t>(typeMap), it->type);

			//value
			bool goodCast = true;
			type_t outVal;
			try{
				outVal = convertVal<type_t>(it->value);
			}
			catch(...){
				goodCast = false;
				BOOST_ERROR("Error casting '" + it->value + "' to " +
				            boost::fusion::at_key<type_t>(typeMap));
			}
			if(goodCast){
				comp(val, outVal);
			}

			++it;
		}

		bool is_end(void) const { return it == endIt; }

	private:
		//value comparisons w/ overload template to catch bad cases.
		static void comp(const int_t& v1, const int_t& v2){
			BOOST_CHECK_EQUAL(v1, v2);
		}
		static void comp(const float_t& v1, const float_t& v2){
			BOOST_CHECK_CLOSE(v1, v2, std::numeric_limits<float_t>::epsilon());
		}
		static void comp(const string_t& v1, const string_t& v2){
			BOOST_CHECK_EQUAL(v1, v2);
		}
		static void comp(const non_t&, const non_t&){
			BOOST_CHECK(true);
		}
		template <typename t1, typename t2>
		static void comp(const t1&, const t2&){
			BOOST_CHECK(false);
		}

		cif::lexer_file_iterator it, endIt;
		typename cif::lexer_type_map<lexer_t>::type typeMap;
};

void lexer_test(const strPair& sp){
	typedef cif::lexer<boost::spirit::istream_iterator> lexer_t;
	typedef cif::lexer_test_parser<lexer_t::iterator_type> parser_t;

	std::ifstream outFH(sp.second.c_str());
	if(!outFH.is_open()){
		BOOST_FAIL("Could not open test file '" + sp.second + "'");
		return;
	}
	TestFunc<lexer_t> testFunc(outFH);

	std::ifstream inFH(sp.first.c_str());
	if(!inFH.is_open()){
		BOOST_FAIL("Could not open test file '" + sp.first + "'");
		return;
	}
	inFH.unsetf(std::ios::skipws);

	lexer_t lexer;
	parser_t parser(lexer, testFunc);

	boost::spirit::istream_iterator it(inFH), endIt;
	bool r = tokenize_and_parse(it, endIt, lexer, parser);

	if(!r || it != endIt){
		BOOST_ERROR("Test case '"+sp.first+"' vs. '"+sp.second+
		            "'gave a truncated output");
	}
}

#endif // LEXER_TEST_HPP

