#include <common.hpp>

void export_block()
{
	class_<parser_t::block_t>("Block")
		.def_readwrite("name", &parser_t::block_t::name)
		.def_readwrite("tables", &parser_t::block_t::tables)
		.def_readwrite("saves", &parser_t::block_t::saves)
	;
}

