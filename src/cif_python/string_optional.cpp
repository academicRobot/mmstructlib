#include <common.hpp>
#include <cif_python/expose_optional.hpp>
#include <string>

void export_string_optional()
{
	python_wrap::exposeOptional<std::string>("StringOptional");
}

