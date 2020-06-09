#ifndef CIF_SAVE_HPP
#define CIF_SAVE_HPP

#include <cif/table.hpp>
#include <boost/optional.hpp>
#include <vector>

namespace cif{

template <typename string_tp>
struct save{
	typedef string_tp string_t;
	string_t tag;
	boost::optional<string_t> field;
	std::vector<table<string_t> > tables;
/*
	//required by boost.python vector wrapper
	bool operator==(const save<string_t>& other) const{
		return this->tag == other.tag && this->field == other.field; //testing implementation
	}
*/
};

}

#endif // CIF_SAVE_HPP
