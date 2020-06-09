#ifndef CIF_FUSION_TABLE_HPP
#define CIF_FUSION_TABLE_HPP

#include <cif/table.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_TPL_STRUCT(
	(string_tp),
	(cif::table)(string_tp),
	(string_tp, name)
	(std::vector<string_tp>, cellNames)
	(std::vector<boost::optional<string_tp> >, cells)
)

#endif // CIF_FUSION_TABLE_HPP
