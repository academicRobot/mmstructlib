#ifndef CIF_TABLE_HPP
#define CIF_TABLE_HPP

#include <vector>
#include <boost/optional.hpp>

namespace cif{

template <typename string_tp>
struct table{
	typedef string_tp string_t;
	string_t name;
	std::vector<string_t> cellNames;
	std::vector<boost::optional<string_t> > cells;
/*
	//required by boost.python vector wrapper
	bool operator==(const table<string_t>& other) const{
		return this->name == other.name; //testing implementation
	}
*/
};

}

#endif // CIF_TABLE_HPP
