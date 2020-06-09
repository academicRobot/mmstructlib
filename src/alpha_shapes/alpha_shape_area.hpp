#ifndef BUSV_ALPHA_SHAPE_AREA_HPP
#define BUSV_ALPHA_SHAPE_AREA_HPP

#include "edge_data_container.hpp"
#include "process_facet.hpp"
#include "create_alpha_shape.hpp"
#include "evaluate_vertex_area.hpp"
#include "evaluate_edge_area.hpp"
#include "evaluate_facet_area.hpp"

#include <algorithm>

namespace busv{

template <typename T, typename U>
void alpha_shape_area(
	U numPoints, T alpha, 
	const T* points, const T* radii, 
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

	AlphaShapeContainer<T,U> asc(create_alpha_shape(numPoints, alpha, points, radii));
	Fixed_alpha_shape_3& as = *asc;

	//initialize areas
	std::fill(areas, areas+numPoints, 0.0);

	//weight_func_obj_default<T,Fixed_alpha_shape_3> wfod;
	weight_func_obj_default<T,U> wfod;
	evaluate_vertex_area(as, areas);
	evaluate_edge_area(as, wfod, areas);
	evaluate_facet_area(as, wfod, areas);
}	

} //namespace busv

#endif

