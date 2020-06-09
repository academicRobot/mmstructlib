#include <boost/test/parameterized_test.hpp>
#include "lexer_test.hpp"

#include <boost/assign/list_of.hpp>

#include <vector>
#include <string>

//DATA_DIR=command line compiler flag
const std::string D = std::string(LEXER_DATA_DIR) + "/"; 

strPair p(const std::string& f1, const std::string& f2){
	return strPair(D + f1, D + f2);
}

test_suite* init_unit_test_suite(int argc, char* argv[]){
	std::vector<strPair> test_files;
	test_files = boost::assign::list_of
		(p("lexer_1_input.txt", "lexer_1_output.txt")),
		(p("lexer_2_input.txt", "lexer_2_output.txt"));

//test_suite* ts1 = BOOST_TEST_SUITE( "test_suite1" );
//ts1->add( BOOST_TEST_CASE( &test_case1 ) )
	framework::master_test_suite().add( 
		BOOST_PARAM_TEST_CASE( 
			&lexer_test, 
			test_files.begin(), 
			test_files.end() 
		) 
	);

	return 0;
}

