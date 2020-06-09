#ifndef LEXER_TEST_PARSER_HPP
#define LEXER_TEST_PARSER_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace cif{

namespace detail {

template <typename func_t>
struct func_ref_wrap{
	template <typename A, typename B = void>
	struct result{ typedef typename func_t::template result<A,B>::type type; };

	func_ref_wrap(func_t& f) : f(f) {}

	template <typename A, typename B>
	typename result<A,B>::type operator()(A& a, B& b) const{
		return f(a, b);
	}

	template <typename A>
	typename result<A>::type operator()(A& a) const{
		return f(a);
	}

	func_t& f;
};

}

template <typename Iterator>
struct lexer_test_parser : boost::spirit::qi::grammar<Iterator> {

	boost::spirit::qi::rule<Iterator> start;

	template <typename token_t, typename test_func_t>
	lexer_test_parser(token_t& t, test_func_t& tFunc) : 
		lexer_test_parser::base_type(start)
	{
		using boost::spirit::qi::_1;
		using boost::spirit::qi::omit;
		detail::func_ref_wrap<test_func_t> tFuncWrap(tFunc);
		boost::phoenix::function<detail::func_ref_wrap<test_func_t> > 
			testFunc(tFuncWrap);
		start = omit[*(
			  t.data      [testFunc("data",           _1)]
			| t.loop      [testFunc("loop"              )] 
			| t.global    [testFunc("global"            )] 
			| t.save      [testFunc("save",           _1)] 
			| t.stop      [testFunc("stop"              )] 
			| t.ws        [testFunc("white space"       )] 
			| t.comment   [testFunc("comment"           )] 
			| t.label     [testFunc("label",          _1)] 
			| t.uqstr     [testFunc("str",            _1)] 
			| t.sqstr     [testFunc("sqstr",          _1)] 
			| t.dqstr     [testFunc("dqstr",          _1)] 
			| t.scqstr    [testFunc("scstr",          _1)] 
			| t.integer   [testFunc("integer",        _1)] 
			| t.fp        [testFunc("fp",             _1)] 
			| t.d         [testFunc("dot"               )] 
			| t.qm        [testFunc("qmark"             )] 
			)];
	}
};

}

#endif // LEXER_TEST_PARSER_HPP
