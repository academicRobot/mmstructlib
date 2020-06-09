#include <common.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

void export_vector_table()
{
	class_<std::vector<parser_t::table_t> >("VectorTable")
		.def(vector_indexing_suite<std::vector<parser_t::table_t> >())
	;
}

