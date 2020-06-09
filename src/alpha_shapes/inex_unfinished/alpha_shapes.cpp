#include "CGAL_point_transform_function.hpp"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Regular_triangulation_3.h>
#include <CGAL/Regular_triangulation_euclidean_traits_3.h>
#include <CGAL/Fixed_alpha_shape_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/Fixed_alpha_shape_vertex_base_3.h>
#include <CGAL/Fixed_alpha_shape_cell_base_3.h>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <utility>
#include <iterator>
#include <stdexcept>

#include <iostream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Regular_triangulation_euclidean_traits_3<K> Gt;
typedef CGAL::Triangulation_vertex_base_with_info_3<unsigned int, Gt> Tb;
typedef CGAL::Fixed_alpha_shape_vertex_base_3<Gt, Tb> Vb;
typedef CGAL::Fixed_alpha_shape_cell_base_3<Gt> Fb;
typedef CGAL::Triangulation_data_structure_3<Vb,Fb> Tds;
typedef CGAL::Regular_triangulation_3<Gt,Tds> Triangulation_3;
typedef CGAL::Fixed_alpha_shape_3<Triangulation_3> Fixed_alpha_shape_3;
typedef Fixed_alpha_shape_3::Cell_handle Cell_handle;
typedef Fixed_alpha_shape_3::Vertex_handle Vertex_handle;
typedef Fixed_alpha_shape_3::Facet Facet;
typedef Fixed_alpha_shape_3::Edge Edge;
typedef Gt::Weighted_point Weighted_point;
typedef Gt::Bare_point Bare_point;

template <typename T>
T* init_mem(unsigned int size, const T& init_val){
	T* mem = (T*)malloc(sizeof(T)*size);
	if(!mem){
		throw std::runtime_error("Insufficient memory");
	}
	for(unsigned int i = 0; i < size; ++i){
		mem[i] = init_val;
	}
	return mem;
}

void alpha_shapes(
	unsigned int num_points, double alpha, 
	const double* points, const double* weights, 
	unsigned int* num_edges, unsigned int** edges, 
	unsigned int* num_triangles, unsigned int** triangles, 
	unsigned int* num_tetrahedra, unsigned int** tetrahedra
){
	busv::CGALPointTransformFunction<Gt> tfunc(points, weights);
	unsigned int i;

	std::cout << "alpha " << alpha << std::endl;

	//build one alpha_shape with alpha=0
	Fixed_alpha_shape_3 as(
		boost::make_transform_iterator(boost::counting_iterator<unsigned int>(0), tfunc), 
		boost::make_transform_iterator(boost::counting_iterator<unsigned int>(num_points), tfunc), 
		alpha
	);


	i = 0;
	*num_edges = std::distance(as.finite_edges_begin(), as.finite_edges_end());
	*edges = init_mem<unsigned int>((*num_edges)*2, 0);
	for(Fixed_alpha_shape_3::Finite_edges_iterator it = as.finite_edges_begin(), eit = as.finite_edges_end(); it!= eit; ++it){
		Fixed_alpha_shape_3::Edge e = *it;
		//if(as.classify(e) == Fixed_alpha_shape_3::SINGULAR){
		if(as.classify(e) != Fixed_alpha_shape_3::EXTERIOR){
			(*edges)[i] = e.first->vertex(e.second)->info();
			(*edges)[i+1] = e.first->vertex(e.third)->info();
			i += 2;
		}
//		(*edges)[i] = it->vertex(0)->info();
//		(*edges)[i+1] = it->vertex(1)->info();
	}
	*num_edges = i/2;

	i = 0;
	*num_triangles = std::distance(as.finite_facets_begin(), as.finite_facets_end());
	*triangles = init_mem<unsigned int>((*num_triangles)*3, 0);
	for(Fixed_alpha_shape_3::Finite_facets_iterator it = as.finite_facets_begin(), eit = as.finite_facets_end(); it!= eit; ++it){
		Fixed_alpha_shape_3::Facet f = *it;
		//if(as.classify(f) == Fixed_alpha_shape_3::SINGULAR){
		if(as.classify(f) != Fixed_alpha_shape_3::EXTERIOR){
			(*triangles)[i] = f.first->vertex((f.second+1)%4)->info();
			(*triangles)[i+1] = f.first->vertex((f.second+2)%4)->info();
			(*triangles)[i+2] = f.first->vertex((f.second+3)%4)->info();
			i += 3;
		}
	}
	*num_triangles = i/3;

	i = 0;
	*num_tetrahedra = std::distance(as.finite_cells_begin(), as.finite_cells_end());
	*tetrahedra = init_mem<unsigned int>((*num_tetrahedra)*4, 0);
	for(Fixed_alpha_shape_3::Finite_cells_iterator it = as.finite_cells_begin(), eit = as.finite_cells_end(); it!= eit; ++it){
		Fixed_alpha_shape_3::Cell c = *it;
		//if(as.classify(it) == Fixed_alpha_shape_3::INTERIOR){
//		if(as.classify(it) != Fixed_alpha_shape_3::EXTERIOR){
			(*tetrahedra)[i] = c.vertex(0)->info();
			(*tetrahedra)[i+1] = c.vertex(1)->info();
			(*tetrahedra)[i+2] = c.vertex(2)->info();
			(*tetrahedra)[i+3] = c.vertex(3)->info();
			i += 4;
//		}
	}
	*num_tetrahedra = i/4;
}	


