#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <vector>
#include "sg_info.hpp"

BOOST_PYTHON_MODULE(sginfo_ext){

	using namespace boost::python;

	class_<std::vector<double> >("VectorDouble")
		.def(vector_indexing_suite<std::vector<double> >())
	;

	def("sgToTransforms", sgToTransforms);
}

