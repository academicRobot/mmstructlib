#ifndef CIF_LEXER_HPP
#define CIF_LEXER_HPP

//#define BOOST_SPIRIT_LEXERTL_DEBUG
//#define BOOST_SPIRIT_DEBUG

#include <cif/common.hpp>

//#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include <iterator>
#include <string>

namespace cif{

namespace detail{

template <typename string_t>
struct trim{
	trim(size_t back, size_t state) : back(back), state(state) {}
	template <typename it_t, typename pf_t, typename id_t, typename context_t>
	void operator()(it_t& start, it_t& end, pf_t&, id_t&, context_t& ctx) const{
		++start;
		string_t str(start, end);
		ctx.set_value(str.erase(str.size()-back));
		ctx.set_state(state);
	}
	private:
		size_t back, state;
};

}

template <typename base_iterator_tp>
struct lexer : 
	boost::spirit::lex::lexer<
		boost::spirit::lex::lexertl::actor_lexer<
			boost::spirit::lex::lexertl::token<
				base_iterator_tp,
				boost::mpl::vector0<>, //construct token attributes on demand
				boost::mpl::false_
			>
		>
	>
{
	//only at least a forward iterator is usable with this lexer
	BOOST_STATIC_ASSERT((
		boost::is_convertible<
			typename std::iterator_traits<base_iterator_tp>::iterator_category,
			const std::forward_iterator_tag&
		>::value
	));

	typedef base_iterator_tp   base_iterator_t;
	typedef boost::iterator_range<base_iterator_t> base_iterator_range_t;
	typedef qmark              qmark_t;
	typedef dot                dot_t; 

	//token types---------------------------------------------------------------
	typedef boost::spirit::lex::token_def<boost::spirit::lex::omit> 
		consumed_token_t;
	typedef boost::spirit::lex::token_def<base_iterator_range_t> 
		range_token_t;
	typedef boost::spirit::lex::token_def<dot_t> dot_token_t;
	typedef boost::spirit::lex::token_def<qmark_t> qmark_token_t;

	//tokens--------------------------------------------------------------------
	//white space
	consumed_token_t ws, comment;

	//reserved words
	range_token_t data;
	consumed_token_t loop, global;
	range_token_t save;
	consumed_token_t stop;

	//identifiers
	range_token_t label;

	//values
	range_token_t uqstr, sqstr, dqstr, scqstr;
	range_token_t integer;
	range_token_t fp;
	dot_token_t d;
	qmark_token_t qm;

	//initialize at construction------------------------------------------------
	lexer(void);
};

template <typename base_iterator_t>
lexer<base_iterator_t>::lexer(void) :
	//pattern definitions--------------------------------------------------------
	//white space as defined in cif spec, includes comments
	ws         ("{WS}+"),
	comment    ("{COMMENT}+"),

	//reserved words
	data       ("[Dd][Aa][Tt][Aa]_{NBC}+"),
	loop       ("[Ll][Oo][Oo][Pp]_"),
	global     ("[Gg][Ll][Oo][Bb][Aa][Ll]_"),
	save       ("[Ss][Aa][Vv][Ee]_{NBC}*"),
	stop       ("[Ss][Tt][Oo][Pp]_"),

	//identifiers
	label      ("_{NBC}+"),

	//strings: un-, double-, and single-quoted and semicolon (uq/str,sq,dq,sc)
	//Note sq and dq refactored from reference to avoid lookahead (trailing ws)
	uqstr      ("({OC}|;){NBC}*"),
	sqstr      ("{SQ}(({SQ}*{APC_NO_SQ_WS})|[ \t])*{SQ}+"),  
	dqstr      ("{DQ}(({DQ}*{APC_NO_DQ_WS})|[ \t])*{DQ}+"), 
	scqstr     ("^{SC}{APC}*{EOL}(({TLC}{APC}*)?{EOL})*{SC}"),

	//numerical types - int and floating point
	integer    ("{INT}"),
	fp("({INT}[eE]{INT})|([\\+\\-]?((\\d*\\.\\d+)|(\\d+\\.))([eE]{INT})?)"),

	//missing info tokens, single char tokens
	d          ("{DOT}"),
	qm         ("\\?")
{
	//helper functions
	struct{
		static std::string rmStr(std::string str, const std::string& rmStr){
			size_t pos = str.find(rmStr);
			if(pos != std::string::npos){
				str.erase(pos, rmStr.size());
			}
			return str;
		}
	} h;

	//character classes----------------------------------------------------------
	const std::string ORDINARY_CHAR(
		"!%&\\(\\)\\*\\+,\\-\\.\\/0-9:<=>\\?@A-Z\\\\\\\\^`a-z\\{\\|\\}~"
	);
	const std::string NON_BLANK_CHAR = ORDINARY_CHAR + "\\\"#\\$'_;\\[\\]";
	const std::string TEXT_LEAD_CHAR = ORDINARY_CHAR + "\\\"#\\$'_ \t\\[\\]";
	const std::string ANY_PRINT_CHAR = ORDINARY_CHAR + "\\\"#\\$'_ \t;\\[\\]";
	this->self.add_pattern
		("EOL",        "\r?\n")
		("OC",         "[" + ORDINARY_CHAR + "]")
		("NBC",        "[" + NON_BLANK_CHAR + "]")
		("TLC",        "[" + TEXT_LEAD_CHAR + "]")
		("APC",        "[" + ANY_PRINT_CHAR + "]")
		("NBC_NO_DOT", "[" + h.rmStr(NON_BLANK_CHAR, "\\.") + "]")
		(
			"APC_NO_SQ_WS",
			"["+h.rmStr(h.rmStr(h.rmStr(ANY_PRINT_CHAR,"'")," "),"\t")+"]"
		)
		(
			"APC_NO_DQ_WS",
			"["+h.rmStr(h.rmStr(h.rmStr(ANY_PRINT_CHAR,"\\\"")," "),"\t")+"]"
		)
		("INT",        "[\\+\\-]?\\d+")
		("WS",         "(\\s(#[^\\n]*\\n)*)")
		("COMMENT",    "(#[^\\n]*\\n)")
		("SQ",         "'")
		("DQ",         "\\\"")
		("SC",         ";")
		("DOT",        "\\.");

/*
	//semantic action to trim quotes from quoted strings by initializing the 
	//   tokens string attributes
	detail::trim<string_t> trim1(1, this->map_state("WS")), 
	                       trim2(2, this->map_state("WS"));
*/
	this->self.add
		(ws)
		(comment)
		(data)
		(loop)
		(global)
		(save)
		(stop)
		(label)
		(integer)
		(fp)
		(d)
		(qm)
		(uqstr)
		(sqstr)
		(dqstr)
		(scqstr)
	;

}

}

//specialization for getting empty type attributes
namespace boost { namespace spirit { namespace traits {
/*
//non-type
template <typename Iterator>
struct assign_to_attribute_from_iterators< ::cif::detail::non_type, Iterator>{
	static void call(Iterator const&, Iterator const&, ::cif::detail::non_type&)
	{}
};
*/
//qmark
template <typename Iterator>
struct assign_to_attribute_from_iterators< ::cif::qmark, Iterator>{
	static void call(Iterator const&, Iterator const&, ::cif::qmark&)
	{}
};

//dot
template <typename Iterator>
struct assign_to_attribute_from_iterators< ::cif::dot, Iterator>{
	static void call(Iterator const&, Iterator const&, ::cif::dot&)
	{}
};

}}}

#endif // CIF_LEXER_HPP

