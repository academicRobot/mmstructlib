#ifndef CIF_OSTREAM_HPP
#define CIF_OSTREAM_HPP

#include <cif/file.hpp>

namespace cif{

template <typename cell_tp>
std::ostream& operator<<(std::ostream& os, const file<cell_tp>& f);

template <typename cell_tp>
std::ostream& operator<<(std::ostream& os, const block<cell_tp>& b);

template <typename cell_tp>
std::ostream& operator<<(std::ostream& os, const save<cell_tp>& s);

template <typename cell_tp>
std::ostream& operator<<(std::ostream& os, const table<cell_tp>& t);

template <typename val_t>
std::ostream& operator<<(std::ostream& os, const boost::optional<val_t>& v);

struct printOptional_impl{
	template <typename os_t, typename arg_t>
	struct result { typedef void type; };

	template <typename os_t, typename arg_t>
	void operator()(os_t& os, const arg_t& arg) const{
		cif::operator<<(os, arg);
	}
};

}

#include <cif/ostream.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <algorithm>
//using boost::phoenix::arg_names::_1;

namespace cif{

template <typename cell_tp>
std::ostream& operator<<(std::ostream& os, const file<cell_tp>& f){
	std::for_each(f.blocks.begin(), f.blocks.end(), 
		os << boost::phoenix::arg_names::_1
	);
	return os;
}

template <typename cell_tp>
std::ostream& operator<<(std::ostream& os, const block<cell_tp>& b){
	os << "BLOCK_TABLES: " << b.name << std::endl;
	std::for_each(b.tables.begin(), b.tables.end(), 
		os << boost::phoenix::arg_names::_1
	);
	os << "END_BLOCK_TABLES: " << b.name << std::endl;
	os << "BLOCK_SAVES: " << b.name << std::endl;
	std::for_each(b.saves.begin(), b.saves.end(), 
		os << boost::phoenix::arg_names::_1
	);
	os << "END_BLOCK_SAVES: " << b.name << std::endl;
	return os;
}

template <typename cell_tp>
std::ostream& operator<<(std::ostream& os, const save<cell_tp>& s){
	os << " SAVE: " << s.tag << " " << s.field << std::endl;
	std::for_each(s.tables.begin(), s.tables.end(), 
		os << boost::phoenix::arg_names::_1
	);
	os << " END_SAVE: " << s.tag << " " << s.field << std::endl;
	return os;
}

template <typename cell_tp>
std::ostream& operator<<(std::ostream& os, const table<cell_tp>& t){
	boost::phoenix::function<printOptional_impl> printOptional;
	os << "   TABLE: " << t.name << std::endl << "    " << *(t.cellNames.begin());
	std::for_each(++(t.cellNames.begin()), t.cellNames.end(), 
		os << boost::phoenix::cref(", ") << boost::phoenix::arg_names::_1
	);
	os << std::endl << "    " << *(t.cells.begin());
	std::for_each(++(t.cells.begin()), t.cells.end(), 
		(os << boost::phoenix::cref(", "),
		printOptional(boost::phoenix::ref(os), boost::phoenix::arg_names::_1))
	);
	os << std::endl << "   END_TABLE: " << t.name << std::endl;
	return os;
}

template <typename val_t>
std::ostream& operator<<(std::ostream& os, const boost::optional<val_t>& v){
	if(v){
		os << *v;
	}
	else{
		os << "NA";
	}
	return os;
}

}

#endif // CIF_OSTREAM_HPP

