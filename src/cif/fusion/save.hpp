#ifndef CIF_PARSER_SAVE_HPP
#define CIF_PARSER_SAVE_HPP

#include <cif/save.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_TPL_STRUCT(
	(string_tp),
	(cif::save)(string_tp),
	(string_tp, tag)
	(boost::optional<string_tp>, field)
	(std::vector<cif::table<string_tp> >, tables)
)

#endif // CIF_PARSER_SAVE_HPP
