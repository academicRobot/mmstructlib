#include <cif/ostream_debug.hpp>
#include <boost/spirit/include/phoenix.hpp>
using boost::phoenix::arg_names::_1;
using boost::phoenix::val;

namespace cif{

//table field vector iterator
std::ostream& operator<<(
	std::ostream& os, 
	const boost::mpl::at_c<table,1>::type::iterator& i
)
{
	os << "FieldVectorIterator";
	return os;
}

//table field vector
std::ostream& operator<<(
	std::ostream& os, 
	const boost::mpl::at_c<table,1>::type&
)
{
	os << "FieldVector";
	return os;
}

namespace detail{

std::ostream& operator<<(std::ostream& os, const non_type&){
	os << "non_type";
	return os;
}

}

}

