#include <common.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

void export_vector_cell()
{
	class_<std::vector<parser_t::cell_t> >("VectorCell")
		.def(vector_indexing_suite<std::vector<parser_t::cell_t> >())
	;
}

