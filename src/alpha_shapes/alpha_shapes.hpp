#include "point_transform_function.hpp"
#include "cgal_typedefs.hpp"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Regular_triangulation_3.h>
//#include <CGAL/Regular_triangulation_euclidean_traits_3.h>
#include <CGAL/Fixed_alpha_shape_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/Fixed_alpha_shape_vertex_base_3.h>
#include <CGAL/Fixed_alpha_shape_cell_base_3.h>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <utility>
#include <iterator>
#include <stdexcept>

namespace busv{

template <typename T>
T* init_mem(size_t size, const T& init_val){
	T* mem = (T*)malloc(sizeof(T)*size);
	if(!mem){
		throw std::runtime_error("Insufficient memory");
	}
	for(size_t i = 0; i < size; ++i){
		mem[i] = init_val;
	}
	return mem;
}

template <typename it_t, typename AS, typename T>
size_t countNotType(it_t it, it_t end, const AS& as, T type){
	size_t num = 0;
	for(;it!=end;++it){
		if(as.classify(*it) != type){
			++num;
		}
	}
	return num;
}

template <typename it_t, typename AS, typename T>
size_t countNotType2(it_t it, it_t end, const AS& as, T type){
	size_t num = 0;
	for(;it!=end;++it){
		if(as.classify(it) != type){
			++num;
		}
	}
	return num;
}

template <typename T, typename U>
void alpha_shapes(
	U num_points, T alpha, 
	const T* points, const T* weights, 
	U* num_edges, U** edges, 
	U* num_triangles, U** triangles, 
	U* num_tetrahedra, U** tetrahedra
){
	//typedef cgal_typedefs<T, U> cgt;
	//typedef typename cgt::Fixed_alpha_shape_3 Fixed_alpha_shape_3;
	//typedef typename cgt::Triangulation_3 Triangulation_3;
	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

	//fixed weighted example from docs
	/*typedef CGAL::Regular_triangulation_vertex_base_3<K>      Vbb;
	typedef CGAL::Fixed_alpha_shape_vertex_base_3<K,Vbb>      Vb;
	typedef CGAL::Regular_triangulation_cell_base_3<K>        Rcb;
	typedef CGAL::Fixed_alpha_shape_cell_base_3<K,Rcb>        Cb;
	typedef CGAL::Triangulation_data_structure_3<Vb,Cb>       Tds;
	typedef CGAL::Regular_triangulation_3<K,Tds>              Triangulation_3;
	typedef CGAL::Fixed_alpha_shape_3<Triangulation_3>        Fixed_alpha_shape_3;*/

	typedef CGAL::Regular_triangulation_vertex_base_3<K> Vbb;
	typedef CGAL::Triangulation_vertex_base_with_info_3<U, K, Vbb> Tb;
	typedef CGAL::Fixed_alpha_shape_vertex_base_3<K, Tb> Vb;

	typedef CGAL::Regular_triangulation_cell_base_3<K> Rcb;
	typedef CGAL::Fixed_alpha_shape_cell_base_3<K, Rcb> Fb;

	typedef CGAL::Triangulation_data_structure_3<Vb,Fb> Tds;
	typedef CGAL::Regular_triangulation_3<K,Tds> Triangulation_3;
	typedef CGAL::Fixed_alpha_shape_3<Triangulation_3> Fixed_alpha_shape_3;
	//typedef typename Fixed_alpha_shape_3::Cell_handle Cell_handle;
	//typedef typename Fixed_alpha_shape_3::Vertex_handle Vertex_handle;
	typedef typename Fixed_alpha_shape_3::Cell Cell;
	typedef typename Fixed_alpha_shape_3::Facet Facet;
	typedef typename Fixed_alpha_shape_3::Edge Edge;
	//typedef typename Triangulation_3::Weighted_point Weighted_point;
	//typedef typename Triangulation_3::Bare_point Bare_point;

	busv::PointTransformFunction<Triangulation_3, U, T> tfunc(points, weights);
	//unsigned int i;

	//build one alpha_shape with alpha=0
	Fixed_alpha_shape_3 as(
		boost::make_transform_iterator(boost::counting_iterator<U>(0), tfunc), 
		boost::make_transform_iterator(boost::counting_iterator<U>(num_points), tfunc), 
		alpha
	);


	*num_edges = countNotType(as.finite_edges_begin(), as.finite_edges_end(), as, Fixed_alpha_shape_3::EXTERIOR);
	U* t = *edges = init_mem<U>((*num_edges)*2, 0);
	typedef typename Fixed_alpha_shape_3::Finite_edges_iterator edge_it_t;
	for(edge_it_t it = as.finite_edges_begin(), eit = as.finite_edges_end(); it!= eit; ++it){
		const Edge& e = *it;
		if(as.classify(e) != Fixed_alpha_shape_3::EXTERIOR){
			t[0] = e.first->vertex(e.second)->info();
			t[1] = e.first->vertex(e.third)->info();
			t += 2;
		}
	}

	*num_triangles = countNotType(as.finite_facets_begin(), as.finite_facets_end(), as, Fixed_alpha_shape_3::EXTERIOR);
	t = *triangles = init_mem<U>((*num_triangles)*3, 0);
	typedef typename Fixed_alpha_shape_3::Finite_facets_iterator facet_it_t;
	for(facet_it_t it = as.finite_facets_begin(), eit = as.finite_facets_end(); it!= eit; ++it){
		const Facet& f = *it;
		//if(as.classify(f) == Fixed_alpha_shape_3::SINGULAR){
		if(as.classify(f) != Fixed_alpha_shape_3::EXTERIOR){
			t[0] = f.first->vertex((f.second+1)%4)->info();
			t[1] = f.first->vertex((f.second+2)%4)->info();
			t[2] = f.first->vertex((f.second+3)%4)->info();
			t += 3;
		}
	}

	*num_tetrahedra = countNotType2(as.finite_cells_begin(), as.finite_cells_end(), as, Fixed_alpha_shape_3::EXTERIOR);
	t = *tetrahedra = init_mem<U>((*num_tetrahedra)*4, 0);
	typedef typename Fixed_alpha_shape_3::Finite_cells_iterator cell_it_t;
	for(cell_it_t it = as.finite_cells_begin(), eit = as.finite_cells_end(); it!= eit; ++it){
		const Cell& c = *it;
//		if(as.classify(it) == Fixed_alpha_shape_3::INTERIOR){
		if(as.classify(it) != Fixed_alpha_shape_3::EXTERIOR){
			t[0] = c.vertex(0)->info();
			t[1] = c.vertex(1)->info();
			t[2] = c.vertex(2)->info();
			t[3] = c.vertex(3)->info();
			t += 4;
		}
	}
}	

} //namespace busv


