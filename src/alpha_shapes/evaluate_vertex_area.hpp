#ifndef BUSV_EVALUATE_VERTEX_AREA_HPP
#define BUSV_EVALUATE_VERTEX_AREA_HPP

#define _USE_MATH_DEFINES // for C++
#include <cmath>

namespace busv{

template <typename float_t, typename index_t>
void evaluate_vertex_area(
	const AlphaShapeContainer<float_t,index_t>& asc, 
	float_t* areas
){
	evaluate_vertex_area(*asc, areas);
}

template <typename alpha_shape_t, typename float_t>
void evaluate_vertex_area(const alpha_shape_t& as, float_t* areas){
	typedef typename alpha_shape_t::Vertex Vertex;
	typedef typename alpha_shape_t::Finite_vertices_iterator vert_it_t;
//	typedef typename Fixed_alpha_shape_3::Classification_type class_t;
	const float_t four_pi = M_PI*4.0;
	for(
		vert_it_t 
			it = as.finite_vertices_begin(), 
			eit = as.finite_vertices_end(); 
		it != eit; 
		++it
	){
//Note: needed to get exact 0 for internal vertices, but incompatible with weights
//		class_t cls = as.classify(it);
//		if(cls == Fixed_alpha_shape_3::REGULAR || cls == Fixed_alpha_shape_3::SINGULAR){
			Vertex& v = *it;
			float_t r = v.info().radius;
			areas[v.info().index] = (four_pi - v.info().omega) * r * r;
#ifdef RES_DETAIL
if(v.info().index == RES_DETAIL){
std::cout << "v: " << v.info().omega << " " << (four_pi - v.info().omega) * r * r << std::endl;
}
#endif
//		}
	}
}

}

#endif //BUSV_EVALUATE_VERTEX_AREA_HPP

