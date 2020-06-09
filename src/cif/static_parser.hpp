#ifndef CIF_STATIC_PARSER_HPP
#define CIF_STATIC_PARSER_HPP

#include <cif/fusion/parse_file.hpp>
#include <cif/fusion/parse_block.hpp>
#include <cif/fusion/save.hpp>
#include <cif/fusion/table.hpp>
#include <cif/fusion/field.hpp>

#ifdef BOOST_SPIRIT_DEBUG
#include <cif/ostream.hpp>
#include <cif/ostream_debug.hpp>
#endif // BOOST_SPIRIT_DEBUG

#include <boost/spirit/include/qi.hpp>
//qi_subrule fixes
namespace boost { namespace spirit { namespace repository { namespace qi { 
using ::boost::spirit::qi::domain;
}}}}
//end qi_subrule fixes
#include <boost/spirit/repository/include/qi_subrule.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace cif{

template<typename Iterator>
struct static_parser : boost::spirit::qi::grammar<Iterator, parse_file(void)>{

	template <typename TokenDef>
	static_parser(TokenDef const& tok);

	boost::spirit::qi::rule<Iterator, parse_file(void)> file_r;
	boost::spirit::repository::qi::subrule<
			0
		,	parse_file(void)
		>	file_sr
		;
	boost::spirit::repository::qi::subrule<
			1
		,	parse_block(void)
		>	block_sr
		;
	boost::spirit::repository::qi::subrule<
			2
		,	save(void)
		>	save_sr
		;
	boost::spirit::repository::qi::subrule<
			3
		,	table(void)
		>	table_sr
		;
	boost::spirit::repository::qi::subrule<
			4
		,	table(size_t)
		,	boost::spirit::qi::locals<std::string> 
		>	table_single_sr
		;
	boost::spirit::repository::qi::subrule<
			5
		,	table(void)
		,	boost::spirit::qi::locals<
					//table field vector iterator type
					boost::mpl::at_c<table,1>::type::iterator
				,	boost::mpl::at_c<table,1>::type::iterator
				,	boost::mpl::at_c<table,1>::type::iterator
				> 
		>	table_multi_sr
		;
	boost::spirit::repository::qi::subrule<
			6
			//table field vector type
		,	boost::mpl::at_c<table,1>::type(size_t, std::string) 
		>	fields_sr
		;
	boost::spirit::repository::qi::subrule<
			7
		,	table::field_t(size_t)
		>	first_field_sr
		;
	boost::spirit::repository::qi::subrule<
			8
		,	table::field_t(size_t, std::string)
		>	dep_tag_field_sr
		;
	boost::spirit::repository::qi::subrule<
			9
		,	table::field_t::value_t(void)
		>	value_sr
		;
	boost::spirit::repository::qi::subrule<
			10
		,	std::string(void)
		>	string_sr
		;
};

//Implementation----------------------------------------------------------------
template <typename Iterator>
template <typename TokenDef>
static_parser<Iterator>::static_parser(TokenDef const& t) : 
	static_parser::base_type(file_r)
{
	//local namespace imports----------------------------------------------------
	using boost::spirit::ascii::string;
	using boost::spirit::qi::eoi;
	using boost::spirit::qi::omit;
	using boost::spirit::qi::on_error;
	using boost::spirit::qi::fail;
	using boost::spirit::qi::debug;
//	using boost::spirit::_val;
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
	using boost::phoenix::begin;
	using boost::phoenix::end;
	using boost::phoenix::val;
	using boost::phoenix::construct;

	//rules----------------------------------------------------------------------
	file_r %= (
		file_sr 
			%=	(*block_sr)
			>	eoi
			,

		block_sr 
			%=	omit[t.data]
			>	t.data_name
			>	*(save_sr | table_sr)
			,

		save_sr 
			%=	omit[t.save]
			>	t.tag_SAVE
			>	-t.field_SAVE
			>	*table_sr
			>	omit[t.save]
			,

		table_sr 
			%=	table_single_sr(1) 
			|	table_multi_sr
			,

		table_single_sr //_r1 = number of values per field
			%=	t.tag[_a = _1] 
			>	fields_sr(_r1, _a)
			,

		//table_multi_sr rule does not ensure correct number of values in a table 
		//   (a multiple of the number of fields).  Left for validation.
		//semantic actions:
		//   _a = _c = fields.begin, _b = fields.end
		//   for value in *value_sr:
		//      if(_a == _b){
		//         _a = _c;
		//      }
		//      _a->push_back(value);
		//      ++_a;
		table_multi_sr
			%=	omit[t.loop]
			>	table_single_sr(0)[
					_a=begin(at_c<1>(_1)), 
					_c=begin(at_c<1>(_1)), 
					_b=end(at_c<1>(_1))
				]
			>	omit[*(value_sr[
					if_(_a==_b)[_a=_c],
					push_back(boost::phoenix::at_c<1>(*_a),_1), 
					++_a
				])]
			,
	
		fields_sr //_r1 = number of values, _r2 = name of tag
			%=	first_field_sr(_r1)
			>	*dep_tag_field_sr(_r1,_r2)
			,
	
		first_field_sr //_r1 = number of values
			%=	t.field 
			>	repeat(_r1)[value_sr]
			,
	
		//TODO: replace _pass action with something less hacky
		dep_tag_field_sr //_r1 = number of values, _r2 = name of tag
			%=	omit[t.tag[if_(_1!=_r2)[_pass=false]]]
//			%=	omit[string(_r2)]
			>	t.field
			>	repeat(_r1)[value_sr]
			,
	
		value_sr
			%=	(t.qmark | t.dot | t.fp | t.integer | string_sr)
			,
	
		string_sr
			%=	(t.str | t.sqstr | t.dqstr | t.scstr)
	);

	//error handling-------------------------------------------------------------
	//define readable names
	file_r.name("file_r");
	file_sr.name("file");
	block_sr.name("block");
	save_sr.name("save");
	table_sr.name("table");
	table_single_sr.name("table_single");
	table_multi_sr.name("table_multi");
	fields_sr.name("fields");
	first_field_sr.name("first_field");
	dep_tag_field_sr.name("dep_tag_field");
	value_sr.name("value");
	string_sr.name("string");

	//error function
	on_error<fail>(
		file_r,
		std::cerr 
			<< val("Parsing error, expecting ")
			<< _4
			<< val(" here: \"")
			//<< construct<std::string>(_3, _2)
			<< construct<std::string>(_1, _3)
			//<< _3
			<< val("\"")
			<< std::endl
	);

	//debugging------------------------------------------------------------------
	#ifdef BOOST_SPIRIT_DEBUG
	debug(file_r);
	debug(file_sr);
	debug(block_sr);
	debug(save_sr);
	debug(table_sr);
	debug(table_single_sr);
	debug(table_multi_sr);
	debug(fields_sr);
	debug(first_field_sr);
	debug(dep_tag_field_sr);
	debug(value_sr);
	debug(string_sr);
	#endif // BOOST_SPIRIT_DEBUG
}

}

#endif // CIF_PARSER_HPP

