#ifndef CIF_BLOCK_HPP
#define CIF_BLOCK_HPP

#include <cif/table.hpp>
#include <cif/save.hpp>
#include <vector>

namespace cif{

template <typename string_tp>
struct block{
	typedef string_tp string_t;
	string_t name;
	std::vector<table<string_t> > tables;
	std::vector<save<string_t> >  saves;
/*
	//required by boost.python vector wrapper
	bool operator==(const block<string_t>& other) const{
		return this->name == other.name; //testing implementation
	}
*/
};

}

#endif //CIF_BLOCK_HPP
