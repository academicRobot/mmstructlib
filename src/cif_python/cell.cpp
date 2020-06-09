#include <common.hpp>
#include <python/expose_optional.hpp>

void export_cell()
{
	python_wrap::exposeOptional<parser_t::cell_t::type_t> ("OptionalCell") 
}

