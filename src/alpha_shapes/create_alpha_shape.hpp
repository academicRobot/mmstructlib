#ifndef BUSV_CREATE_ALPHA_SHAPE_HPP
#define BUSV_CREATE_ALPHA_SHAPE_HPP

#include "cgal_typedefs.hpp"
#include "point_transform_function.hpp"
#include "init_edge_data.hpp"
#include "process_cells.hpp"
#include "weight_func_obj_default.hpp"
#include "alpha_shape_container.hpp"

#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>

namespace busv{

template <typename T, typename U>
AlphaShapeContainer<T,U> create_alpha_shape(
	U numPoints, T alpha, 
	const T* points, const T* radii
)
{
	typedef typename cgal_typedefs<T,U>::Triangulation_3 Triangulation_3;
	typedef typename cgal_typedefs<T,U>::Fixed_alpha_shape_3 Fixed_alpha_shape_3;

	PointTransformFunction<Triangulation_3, VertexData<T, U>, T> tfunc(points, radii);
	//T (&coords)[numPoints][3] = (T (&)[numPoints][3])points;

	//build one alpha_shape with alpha=0
	Fixed_alpha_shape_3* as_ptr = new Fixed_alpha_shape_3(
		boost::make_transform_iterator(boost::counting_iterator<U>(0), tfunc), 
		boost::make_transform_iterator(boost::counting_iterator<U>(numPoints), tfunc), 
		alpha
	);

	//process edges and setup cell edge pointers
	EdgeData<T>* edgeData = init_edge_data<T>(*as_ptr);

	//process tetrahedra
	process_cells<T>(*as_ptr, weight_func_obj_default<T,U>());

	return AlphaShapeContainer<T,U>(as_ptr, edgeData);
}

} //namespace busv

#endif

