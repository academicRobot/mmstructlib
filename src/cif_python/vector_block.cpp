#include <common.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

void export_vector_block()
{
	class_<std::vector<parser_t::block_t> >("VectorBlock")
		.def(vector_indexing_suite<std::vector<parser_t::block_t> >())
	;
}

