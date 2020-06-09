#ifndef CIF_OSTREAM_DEBUG_HPP
#define CIF_OSTREAM_DEBUG_HPP

#include <cif/common.hpp>
#include <cif/fusion/table.hpp>
#include <boost/fusion/sequence/intrinsic.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/mpl/at.hpp>

namespace cif{

//table field vector iterator
std::ostream& operator<<(
	std::ostream& os, 
	const boost::mpl::at_c<table,1>::type::iterator&
);

//table field vector
std::ostream& operator<<(
	std::ostream& os, 
	const boost::mpl::at_c<table,1>::type&
);

namespace detail{

std::ostream& operator<<(std::ostream& os, const non_type&);

}

}

#endif // CIF_OSTREAM_DEBUG_HPP

