#include <common.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

void export_vector_string()
{
	class_<std::vector<std::string> >("VectorString")
		.def(vector_indexing_suite<std::vector<std::string> >())
	;
}

