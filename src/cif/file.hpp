#ifndef CIF_FILE_HPP
#define CIF_FILE_HPP

#include <cif/block.hpp>
#include <vector>

namespace cif{

template <typename string_tp>
struct file{
	typedef string_tp string_t;
	std::vector<block<string_t> > blocks;
};

}

#endif // CIF_FILE_HPP
