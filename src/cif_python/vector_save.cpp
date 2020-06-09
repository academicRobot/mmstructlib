#include <common.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

void export_vector_save()
{
	class_<std::vector<parser_t::save_t> >("VectorSave")
		.def(vector_indexing_suite<std::vector<parser_t::save_t> >())
	;
}

