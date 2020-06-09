#ifndef CIF_FUSION_BLOCK_HPP
#define CIF_FUSION_BLOCK_HPP

#include <cif/block.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_TPL_STRUCT(
	(string_tp),
	(cif::block)(string_tp),
	(string_tp, name)
	(std::vector<cif::table<string_tp> >, tables)
	(std::vector<cif::save<string_tp> >, saves)
)

#endif // CIF_FUSION_BLOCK_HPP
