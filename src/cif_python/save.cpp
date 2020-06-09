#include <common.hpp>

void export_save()
{
	class_<parser_t::save_t>("Save")
		.def_readwrite("tag", &parser_t::save_t::tag)
		.def_readwrite("field", &parser_t::save_t::field)
	;
}

