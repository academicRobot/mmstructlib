#ifndef CIF_FUSION_FILE_HPP
#define CIF_FUSION_FILE_HPP

#include <cif/file.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_TPL_STRUCT(
	(string_tp),
	(cif::file)(string_tp),
	(std::vector<cif::block<string_tp> >, blocks)
)

#endif // CIF_FUSION_FILE_HPP
