#ifndef BUSV_EVALUATE_EDGE_AREA_HPP
#define BUSV_EVALUATE_EDGE_AREA_HPP

#include <iostream>

namespace busv{

template <typename float_t, typename index_t, typename weight_func_obj_t>
void evaluate_edge_area(
	const AlphaShapeContainer<float_t, index_t>& asc,
	const weight_func_obj_t& weight_func_obj,
	float_t* areas
){
	evaluate_edge_area(*asc, weight_func_obj, areas);
}

template <typename alpha_shape_t, typename weight_func_obj_t, typename float_t>
void evaluate_edge_area(
	const alpha_shape_t& as,
	const weight_func_obj_t& weight_func_obj,
	float_t* areas
){
	typedef typename alpha_shape_t::Edge Edge;
	typedef typename alpha_shape_t::Cell Cell;
	typedef typename alpha_shape_t::Vertex_handle Vertex_handle;
	typedef typename alpha_shape_t::Finite_edges_iterator edge_it_t;
	//typedef typename Fixed_alpha_shape_3::Classification_type class_t;
	const float_t two_pi = 2.0*M_PI;
	for(
		edge_it_t 
			it = as.finite_edges_begin(), 
			eit = as.finite_edges_end(); 
		it!= eit; 
		++it
	){
		const Edge& e = *it;
		//class_t cls = as.classify(e);
		//if(cls == Fixed_alpha_shape_3::REGULAR || cls == Fixed_alpha_shape_3::SINGULAR){
//all edges, by weight
//std::cout << "e" << std::endl;
		if(as.classify(e) != alpha_shape_t::EXTERIOR){
			const float_t weight = weight_func_obj(e);
			Cell& c = *e.first;
			Vertex_handle vh1 = c.vertex(e.second),
			              vh2 = c.vertex(e.third);
			if(vh1 > vh2){
				std::swap(vh1, vh2);
			}
			const size_t v1 = vh1->info().index, 
			             v2 = vh2->info().index;
			const EdgeData<float_t>& ed = c.info().edgeData(e);
//std::cout << "   edge " << weight << " " << ed.phi << std::endl;
			const float_t temp = two_pi * weight;
			areas[v1] -= (temp - ed.phi[0]) * 0.5 * ed.area[0];
			areas[v2] -= (temp - ed.phi[1]) * 0.5 * ed.area[1];
#ifdef RES_DETAIL
if(v1 == RES_DETAIL){
std::cout << "e (" << v2 << "): " << weight << " " << ed.phi[0] << " " << ed.area[0] << std::endl;
}
if(v2 == RES_DETAIL){
std::cout << "e (" << v1 << "): " << weight << " " << ed.phi[1] << " " << ed.area[1] << std::endl;
}
#endif
if(weight > 0.0){
vh1->info().edge_count++;
vh2->info().edge_count++;
}
//
		}
	}
}

}

#endif ///BUSV_EVALUATE_EDGE_AREA_HPP

