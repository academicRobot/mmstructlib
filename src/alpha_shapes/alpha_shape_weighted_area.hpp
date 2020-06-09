#ifndef BUSV_ALPHA_SHAPE_WEIGHTED_AREA_HPP
#define BUSV_ALPHA_SHAPE_WEIGHTED_AREA_HPP

#include "edge_data_container.hpp"
#include "process_facet.hpp"
#include "process_cells.hpp"
#include "alpha_shape_container.hpp"
#include "init_angles.hpp"
#include "evaluate_vertex_area.hpp"
#include "evaluate_edge_area.hpp"
#include "evaluate_facet_area.hpp"

#include <algorithm>
#include <iterator>

namespace busv{

template <typename T, typename U, typename V>
void alpha_shape_weighted_area(
	AlphaShapeContainer<T,U>& asc,
	const V& weight_func_obj,
	T* areas
)
{
	typedef typename AlphaShapeContainer<T,U>::Fixed_alpha_shape_3 Fixed_alpha_shape_3;
	//typedef typename Fixed_alpha_shape_3::Vertex Vertex;
	//typedef typename Fixed_alpha_shape_3::Vertex_handle Vertex_handle;
	//typedef typename Fixed_alpha_shape_3::Edge Edge;
	//typedef typename Fixed_alpha_shape_3::Facet Facet;
	//typedef typename Fixed_alpha_shape_3::Cell Cell;
	//typedef typename Fixed_alpha_shape_3::Classification_type class_t;

	Fixed_alpha_shape_3& as = *asc;

	//initialize areas
	std::fill(areas, areas+std::distance(as.finite_vertices_begin(),as.finite_vertices_end()), 0.0);

	init_angles<T,U>(as);
	process_cells<T>(as, weight_func_obj);
	evaluate_vertex_area(as, areas);
	evaluate_edge_area(as, weight_func_obj, areas);
	evaluate_facet_area(as, weight_func_obj, areas);
}	

} //namespace busv

#endif

