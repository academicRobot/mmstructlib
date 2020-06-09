#include <boost/python.hpp>
#include <boost/python/exception_translator.hpp>
#include <exception>

void export_file();
void export_save();
void export_block();
void export_table();
void export_parse();
void export_vector_block();
void export_vector_table();
void export_vector_save();
void export_vector_cell();
void export_vector_string();
void export_string_optional();

void translate(std::runtime_error const& e){
	// Use the Python 'C' API to set up an exception object
	PyErr_SetString(PyExc_RuntimeError, e.what());
	//PyErr_SetString(PyExc_UserWarning, e.what());
}

BOOST_PYTHON_MODULE(cif_parser)
{
	export_file();
	export_block();
	export_table();
	export_save();
	export_parse();
	export_vector_block();
	export_vector_table();
	export_vector_save();
	export_vector_cell();
	export_vector_string();
	export_string_optional();
	boost::python::register_exception_translator<std::runtime_error>(&translate);
}

