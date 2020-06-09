#ifndef CIF_PARSER_HPP 
#define CIF_PARSER_HPP

#include <cif/fusion/file.hpp>
#include <cif/fusion/block.hpp>
#include <cif/fusion/save.hpp>
#include <cif/fusion/table.hpp>

#ifdef BOOST_SPIRIT_DEBUG
#include <cif/ostream.hpp>
#include <cif/ostream_debug.hpp>
#endif // BOOST_SPIRIT_DEBUG

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

//#include <boost/spirit/home/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include <iterator>
#include <string>

//void swap_test(const char& attr){}
//template <typename Attrib>
//void swap_test(Attrib& attr){}
//template <typename Attrib, typename Context>
//void swap_test(Attrib& attr, Context& con){}
//	std::swap(boost::fusion::at_c<0>(con.attributes), attr);
//}

namespace cif{

template<typename lexer_tp, typename string_tp = std::string>
struct parser : 
	boost::spirit::qi::grammar<
		typename lexer_tp::iterator_type, 
		file<string_tp>(void)
	>
{
	//primary types
	typedef lexer_tp lexer_t;
	typedef typename lexer_t::iterator_type iterator_t;
	typedef typename lexer_t::base_iterator_range_t base_iterator_range_t;
	typedef typename lexer_t::base_iterator_range_t::iterator base_iterator_t;
	typedef string_tp string_t;
	typedef string_t cell_t;
	typedef file<cell_t> file_t;
	typedef block<cell_t> block_t;
	typedef save<cell_t> save_t;
	typedef table<cell_t> table_t;

	//only random access iterators are usable with this parser
	BOOST_STATIC_ASSERT((
		boost::is_convertible<
			typename std::iterator_traits<base_iterator_t>::iterator_category,
			const std::random_access_iterator_tag&
		>::value
	));

	template <typename error_handler_t>
	parser(lexer_t const& t, error_handler_t& errFunc);

	boost::spirit::qi::rule<iterator_t, file_t(void)> file_r;
	boost::spirit::qi::rule<iterator_t, block_t(void)> block_r;
	boost::spirit::qi::rule<
			iterator_t
		,	save_t(void)
		,	boost::spirit::qi::locals<base_iterator_t> 
		> save_r;
	boost::spirit::qi::rule<iterator_t, table_t(void)> table_r;
	boost::spirit::qi::rule<
			iterator_t
		,	table_t(void)
		,	boost::spirit::qi::locals<std::string> 
		> table_single_r;
	boost::spirit::qi::rule<
			iterator_t
		,	table_t(void)
		,	boost::spirit::qi::locals<std::string> 
		> table_multi_r;
	boost::spirit::qi::rule<
			iterator_t
		,	boost::fusion::vector<std::string, std::string>(void)
		,	boost::spirit::qi::locals<base_iterator_t, bool> 
		> label_r;
	boost::spirit::qi::rule<
			iterator_t
		,	std::string(std::string)
		,	boost::spirit::qi::locals<base_iterator_t, bool> 
		> dep_label_r;
	boost::spirit::qi::rule<
			iterator_t
		,	boost::optional<cell_t>(void)
		> value_r;
	boost::spirit::qi::rule<
			iterator_t
		,	string_t(void)
		> string_r;

	private:
};

//Implementation----------------------------------------------------------------

//helper funcitons----------------------------------------------------------
namespace detail{
struct mod_it_impl{
	template <typename It, typename Diff>
	struct result{
		typedef It type;	
	};
	template <typename It, typename Diff>
	It operator()(It& it, Diff diff) const{
		return it + diff;
	}
};
struct make_iterator_range_impl{
	template <typename It, typename It2>
	struct result{
		typedef boost::iterator_range<It> type;	
	};
	template <typename It, typename It2>
	boost::iterator_range<It> operator()(It f, It2 l) const{
		return boost::iterator_range<It>(f,l);
	}
};
} //detail

namespace{
boost::phoenix::function<detail::mod_it_impl> mod_it;
boost::phoenix::function<detail::make_iterator_range_impl> make_ir;
}

template <typename lexer_tp, typename string_tp>
template <typename error_handler_t>
parser<lexer_tp, string_tp>::parser(lexer_tp const& t, error_handler_t& errFunc) : 
	parser::base_type(file_r)
{
	//local namespace imports----------------------------------------------------
	using boost::spirit::ascii::string;
	using boost::spirit::qi::eoi;
	using boost::spirit::qi::omit;
	using boost::spirit::qi::lit;
	using boost::spirit::qi::eps;
	using boost::spirit::qi::expectation_failure;
	using boost::spirit::info;
	using boost::spirit::_val;
	using boost::spirit::repeat;
	using boost::spirit::_pass;
	using boost::spirit::_1;
	using boost::spirit::_2;
	using boost::spirit::_3;
	using boost::spirit::_4;
	using boost::spirit::_r1;
	using boost::spirit::_r2;
	using boost::spirit::_a;
	using boost::spirit::_b;
	using boost::spirit::_c;
	using boost::phoenix::if_;
	using boost::phoenix::at_c;
	using boost::phoenix::push_back;
	using boost::phoenix::distance;
	using boost::phoenix::equal;
	using boost::phoenix::assign;
	using boost::phoenix::swap;
	using boost::phoenix::find;
	using boost::phoenix::begin;
	using boost::phoenix::end;
	using boost::phoenix::val;
	using boost::phoenix::construct;
	using boost::phoenix::cref;
	using boost::phoenix::throw_;



	//rules----------------------------------------------------------------------
	file_r 
		%=	-t.comment
		>	-t.ws
		>	(*block_r)
		>	-t.ws
		>	eoi
		;

	block_r 
		=	t.data	[ at_c<0>(_val) = construct<string_t>(mod_it(begin(_1), 5), end(_1)) ]
		>	*(
					t.ws
				>>	(
							table_r	[ push_back(at_c<1>(_val), _1) ]
						|	save_r	[ push_back(at_c<2>(_val), _1) ]
					)
			)
		;

	save_r 
		=	t.save [
				_a = find(_1, '.'),
				if_(_a != end(_1))[
					at_c<1>(_val) = construct<string_t>(mod_it(_a,1),end(_1))
				],
				at_c<0>(_val) = construct<string_t>(mod_it(begin(_1), 5), _a) 
			]
		>	t.ws
		>	(table_r % t.ws) [ swap(_1, at_c<2>(_val)) ]
		>	t.ws
		>	t.save
		;

	table_r 
		%=	table_single_r 
		|	table_multi_r
		;

	table_single_r
		=	label_r [ 
				_a = at_c<0>(_1), 
				at_c<0>(_val) = at_c<0>(_1), 
				push_back(at_c<1>(_val), at_c<1>(_1)) 
			] 
		>	t.ws
		>	value_r [ push_back(at_c<2>(_val), _1) ]
		>	*(
					t.ws
				>>	dep_label_r(_a) [ push_back(at_c<1>(_val), _1) ]
				>	t.ws
				>	value_r [ push_back(at_c<2>(_val), _1) ]
			)
		;

	//table_multi_r rule does not ensure correct number of values in a table (a 
	//   multiple of the number of fields).  Left for validation.
	table_multi_r
		=	t.loop
		>	t.ws
		>	label_r[ 
				_a = at_c<0>(_1),  
				at_c<0>(_val) = at_c<0>(_1), 
				push_back(at_c<1>(_val), at_c<1>(_1)) 
			]
		>	*(t.ws >> dep_label_r(_a)[ push_back(at_c<1>(_val), _1) ])
		>	+(t.ws >> value_r[push_back(at_c<2>(_val), _1)])
		;

	label_r 
		=	t.label [
				_a = find(_1, '.'),
				if_(_a == end(_1)) [ 
					_b = false
//					_pass = false
				]
				.else_ [
					assign(at_c<0>(_val), mod_it(begin(_1), 1), _a),
					assign(at_c<1>(_val), mod_it(_a, 1), end(_1)),
					_b = true
				]
			]
		>	eps[ _pass = _b ] //emit expectation failure if bad label format
		;

	dep_label_r 
		=	t.label [
				_a = find(_1, '.'),
				if_(_a == end(_1))[ 
					_b = false
				]
				.else_ [
					if_(distance(_r1) == (distance(make_ir(begin(_1), _a))-1) &&
					    equal(_r1, mod_it(begin(_1), 1)))
					[ 
						assign(_val, mod_it(_a, 1), end(_1)),
						_b = true
					]
					.else_ [
						_pass = false
					]
				]
			]
		>	eps[ _pass = _b ] //emit expectation failure if bad label format
		;

	value_r %=	omit[t.qm] | omit[t.d] | t.fp | t.integer | string_r;

	string_r
		=	t.uqstr [ _val = construct<string_t>(begin(_1), end(_1)) ]
		|	( t.sqstr | t.dqstr ) [ 
				_val = construct<string_t>(
					mod_it(begin(_1), 1),
					mod_it(end(_1), -1)
				) 
			]
		|	t.scqstr [ 
				_val = construct<string_t>(
					mod_it(begin(_1), 1),
					mod_it(
						end(_1),
						if_else(*mod_it(end(_1), -3) == '\r', -3, -2)
					)
				) 
			]
			
		;

	//error handling-------------------------------------------------------------
	//define readable names
	file_r.name("file");
	block_r.name("block");
	save_r.name("save");
	table_r.name("table");
	table_single_r.name("table_single");
	table_multi_r.name("table_multi");
	label_r.name("label");
	dep_label_r.name("dep_label");
	value_r.name("value");
	string_r.name("string");

	//error functions
	using boost::spirit::qi::on_error;
	using boost::spirit::qi::fail;
	using boost::spirit::qi::rethrow;
	namespace pho = boost::phoenix;
	//using boost::phoenix::ref;

/*	using boost::phoenix::arg_names::arg1;
	using boost::phoenix::arg_names::arg3;
	using boost::phoenix::arg_names::arg4;
	on_error<fail>(file_r, 
	               bind(&error_handler_t::operator(), &errFunc, arg1, arg3, file_r.name(), arg4));
	on_error<rethrow>(block_r, 
	                  bind(&error_handler_t::operator(), ref(errFunc), _1, _3, block_r.name(), _4));
	on_error<rethrow>(save_r, 
	                  bind(&error_handler_t::operator(), ref(errFunc), _1, _3, save_r.name(), _4));
	on_error<rethrow>(table_r, 
	                  bind(&error_handler_t::operator(), ref(errFunc), _1, _3, table_r.name(), _4));
	on_error<rethrow>(table_multi_r, 
	                  bind(&error_handler_t::operator(), ref(errFunc), _1, _3, table_multi_r.name(),
	                       _4));
	on_error<rethrow>(table_single_r, 
	                  bind(&error_handler_t::operator(), ref(errFunc), _1, _3, table_single_r.name(),
	                       _4));
	on_error<rethrow>(label_r, 
	                  bind(&error_handler_t::operator(), ref(errFunc), _1, _3, label_r.name(), _4));
	on_error<rethrow>(dep_label_r, 
	                  bind(&error_handler_t::operator(), ref(errFunc), _1, _3, dep_label_r.name(), 
	                       _4));*/
/*	boost::phoenix::function<error_handler_t> errFuncLazy;
	on_error<fail>(file_r, 
	               errFuncLazy(_1, _3, file_r.name(), _4));
	on_error<rethrow>(block_r, 
	                  errFuncLazy(_1, _3, block_r.name(), _4));
	on_error<rethrow>(save_r, 
	                  errFuncLazy(_1, _3, save_r.name(), _4));
	on_error<rethrow>(table_r, 
	                  errFuncLazy(_1, _3, table_r.name(), _4));
	on_error<rethrow>(table_multi_r, 
	                  errFuncLazy(_1, _3, table_multi_r.name(), _4));
	on_error<rethrow>(table_single_r, 
	                  errFuncLazy(_1, _3, table_single_r.name(), _4));
	on_error<rethrow>(label_r, 
	                  errFuncLazy(_1, _3, label_r.name(), _4));
	on_error<rethrow>(dep_label_r, 
	                  errFuncLazy(_1, _3, dep_label_r.name(), _4));*/
	//using boost::phoenix::cref;
	on_error<fail>(file_r, 
	               pho::bind(pho::ref(errFunc), _1, _3, file_r.name(), _4));
	on_error<rethrow>(block_r, 
	                  pho::bind(pho::ref(errFunc), _1, _3, block_r.name(), _4));
	on_error<rethrow>(save_r, 
	                  pho::bind(pho::ref(errFunc), _1, _3, save_r.name(), _4));
	on_error<rethrow>(table_r, 
	                  pho::bind(pho::ref(errFunc), _1, _3, table_r.name(), _4));
	on_error<rethrow>(table_multi_r, 
	                  pho::bind(pho::ref(errFunc), _1, _3, table_multi_r.name(),
	                       _4));
	on_error<rethrow>(table_single_r, 
	                  pho::bind(pho::ref(errFunc), _1, _3, table_single_r.name(),
	                       _4));
	on_error<rethrow>(label_r, 
	                  pho::bind(pho::ref(errFunc), _1, _3, label_r.name(), _4));
	on_error<rethrow>(dep_label_r, 
	                  pho::bind(pho::ref(errFunc), _1, _3, dep_label_r.name(), 
	                       _4));

	//debugging----------------------------------------------------------------
	#ifdef BOOST_SPIRIT_DEBUG
	using boost::spirit::qi::debug;
	debug(file_r);
	debug(block_r);
	debug(save_r);
	debug(table_r);
	debug(table_single_r);
	debug(table_multi_r);
	debug(fields_r);
	debug(first_field_r);
	debug(dep_tag_field_r);
	debug(value_r);
	debug(string_r);
	#endif // BOOST_SPIRIT_DEBUG
}

}//cif

#endif // CIF_PARSER_HPP

