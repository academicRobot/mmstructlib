#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <vector>

extern "C" {
#include <nsc.h>
}

struct double_array {
	double_array(void) : data(NULL) {}
	double_array(size_t l) : data(NULL) {
		this->resize(l);
	}
	~double_array(void){
		if(this->data){
			delete [] this->data;
		}
	}
	size_t length(void){
		return this->len;
	}
	double get(size_t i){
		if(i < 0 || i >= this->len){
			throw std::runtime_error("array index out of range");
		}
		return this->data[i];
	}
	void set(size_t i, double val){
		if(i < 0 || i >= this->len){
			throw std::runtime_error("array index out of range");
		}
		this->data[i] = val;
	}
	void resize(size_t l){
		if(this->data){
			delete [] this->data;
		}
		this->data = new double[l];
		this->len = l;
	}
	double * data;
	size_t len;
};

int nsc_atom_sas(double_array& coords, double_array& radii, int dots, double_array& areas){
	double total_area;
	if(coords.len / 3 != radii.len){
		return 0;
	}
	int ret = NSC(
		coords.data,   /* atom coordinates xyz0, xyz1, ... */
		radii.data,    /* atom radii r0, r1, r2, ... */
		radii.len,     /* number of atoms */
		dots,          /* number of dots per fully accessible sphere */
		FLAG_ATOM_AREA,/* flag : dots, volume and/or area per atom */
		&total_area,   /* 1 output: overall area */
		&(areas.data), /* 2 output: pointer to list of areas per atom */
		NULL,    /* 3 output: overall volume */
		NULL,    /* 4 output: pointer to list of surface dots x0, y0, z0, ... */
		NULL     /* 5 output: number of surface dots */
	);
	areas.len = coords.len;
	return ret;
}

int nsc_total_sas(double_array& coords, double_array& radii, int dots, double_array& area){
	if(coords.len / 3 != radii.len){
		return 0;
	}
	area.resize(1);
	int ret = NSC(
		coords.data,   /* atom coordinates xyz0, xyz1, ... */
		radii.data,    /* atom radii r0, r1, r2, ... */
		radii.len,     /* number of atoms */
		dots,          /* number of dots per fully accessible sphere */
		0,             /* flag : dots, volume and/or area per atom */
		area.data,     /* 1 output: overall area */
		NULL,    /* 2 output: pointer to list of areas per atom */
		NULL,    /* 3 output: overall volume */
		NULL,    /* 4 output: pointer to list of surface dots x0, y0, z0, ... */
		NULL     /* 5 output: number of surface dots */
	);
	return ret;

}

int nsc_total_volume(double_array& coords, double_array& radii, int dots, double_array& volume){
	if(coords.len / 3 != radii.len){
		return 0;
	}
	volume.resize(1);
	int ret = NSC(
		coords.data,   /* atom coordinates xyz0, xyz1, ... */
		radii.data,    /* atom radii r0, r1, r2, ... */
		radii.len,     /* number of atoms */
		dots,          /* number of dots per fully accessible sphere */
		FLAG_VOLUME,   /* flag : dots, volume and/or area per atom */
		NULL,     /* 1 output: overall area */
		NULL,    /* 2 output: pointer to list of areas per atom */
		volume.data, /* 3 output: overall volume */
		NULL,    /* 4 output: pointer to list of surface dots x0, y0, z0, ... */
		NULL     /* 5 output: number of surface dots */
	);
	return ret;
}


BOOST_PYTHON_MODULE(nsc_ext){

	using namespace boost::python;

	class_<double_array>("DoubleArray", init<>())
		.def(init<size_t>())
		.def("__getitem__", &double_array::get)
		.def("__setitem__", &double_array::set)
		.def("__len__", &double_array::length)
		.def("resize", &double_array::resize)
	;

	def("nsc_atom_sas", nsc_atom_sas);
	def("nsc_total_sas", nsc_total_sas);
	def("nsc_total_volume", nsc_total_volume);
}

