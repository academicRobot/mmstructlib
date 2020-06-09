#include <common.hpp>

void export_table()
{
	class_<parser_t::table_t>("Table")
		.def_readwrite("name", &parser_t::table_t::name)
		.def_readwrite("cell_names", &parser_t::table_t::cellNames)
		.def_readwrite("cells", &parser_t::table_t::cells)
	;
}

