#ifndef BUSV_CGAL_TYPEDEFS_HPP
#define BUSV_CGAL_TYPEDEFS_HPP

#include "edge_data_container.hpp"
#include "vertex_data.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Regular_triangulation_3.h>
#include <CGAL/Regular_triangulation_vertex_base_3.h>
//#include <CGAL/Regular_triangulation_euclidean_traits_3.h>
#include <CGAL/Fixed_alpha_shape_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/Fixed_alpha_shape_vertex_base_3.h>
#include <CGAL/Triangulation_cell_base_with_info_3.h>
#include <CGAL/Fixed_alpha_shape_cell_base_3.h>

namespace busv{

template <typename T, typename U>
struct cgal_typedefs{
	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

	typedef CGAL::Regular_triangulation_vertex_base_3<K> Vbb;
	typedef CGAL::Triangulation_vertex_base_with_info_3<VertexData<T, U>, K, Vbb> Tb;
	typedef CGAL::Fixed_alpha_shape_vertex_base_3<K, Tb> Vb;

	typedef CGAL::Regular_triangulation_cell_base_3<K> Rcb;
	typedef CGAL::Triangulation_cell_base_with_info_3<EdgeDataContainer<T>, K, Rcb> Cb;
	typedef CGAL::Fixed_alpha_shape_cell_base_3<K, Cb> Fb;

	typedef CGAL::Triangulation_data_structure_3<Vb,Fb> Tds;
	typedef CGAL::Regular_triangulation_3<K,Tds> Triangulation_3;
	typedef CGAL::Fixed_alpha_shape_3<Triangulation_3> Fixed_alpha_shape_3;
	
	typedef typename Fixed_alpha_shape_3::Cell Cell;
	typedef typename Fixed_alpha_shape_3::Facet Facet;
	typedef typename Fixed_alpha_shape_3::Edge Edge;
};

} //namespace busv

#endif

