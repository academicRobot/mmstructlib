#include <common.hpp>

void export_file()
{
	class_<parser_t::file_t>("File")
		.def_readwrite("blocks", &parser_t::file_t::blocks)
	;
}

