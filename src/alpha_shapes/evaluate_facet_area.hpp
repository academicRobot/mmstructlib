#ifndef BUSV_EVALUATE_FACET_AREA_HPP
#define BUSV_EVALUATE_FACET_AREA_HPP

#include "facet_idxs.hpp"
#include "find_vertex_index.hpp"

#include <iostream>

namespace busv{

namespace detail{

//Calculation of facet factors is complicated by three factors:
//	1) Cells with infinite vertices cannot be passed to weight_func_obj
//		because infinite vertices do not have valid indices into weight
//		data.  Since these cells are exterior by definition, do not call
//		weight_func_object on exterior cells
//	2) in the case that the AS has less than 3 dimensions, mirror_facet is 
//	   undefined.
//	3) vertex-wise locality
//Also could streamline this by caching cell weights in cell data

template <typename float_t, typename alpha_shape_t, typename weight_func_obj_t>
void calc_facet_factor_half(
	const alpha_shape_t& as,
	const typename alpha_shape_t::Facet& f,
	const weight_func_obj_t& weight_func_obj,
	float_t factors[3]
){
	typedef typename alpha_shape_t::Cell_handle Cell_handle;
	const Cell_handle& c = f.first;
	if(as.classify(c) == alpha_shape_t::INTERIOR){
		const unsigned char (&facetIdx)[3] = busv::detail::FACET_IDXS[f.second];
		float_t weights[4];
		weight_func_obj(*c, weights);
		for(int i=0; i<3; ++i){
			factors[i] += weights[facetIdx[i]];
		}
	}
}

template <typename float_t, typename alpha_shape_t, typename weight_func_obj_t>
void calc_facet_factor(
	const alpha_shape_t& as,
	const typename alpha_shape_t::Facet& f,
	const weight_func_obj_t& weight_func_obj,
	float_t factors[3]
){
	typedef typename alpha_shape_t::Cell Cell;
	typedef typename alpha_shape_t::Facet Facet;
	for(int i=0; i<3; ++i){
		factors[i] = 0;
	}
	calc_facet_factor_half(as, f, weight_func_obj, factors);
	if(as.number_of_cells()){ //for the less than 3D case
		const Facet& mf = as.mirror_facet(f);
		float_t mfactors[3] = {0.0, 0.0, 0.0};
		calc_facet_factor_half(as, mf, weight_func_obj, mfactors);
		const unsigned char (&facetIdx)[3] = busv::detail::FACET_IDXS[mf.second];
		const Cell& c = *(f.first);
		const Cell& mc = *(mf.first);
		for(int i=0; i<3; ++i){
			//reverse cross index mapped arcane magics
			int mappedIndex = find_vertex_index(c, mc.vertex(facetIdx[i]));
			if(mappedIndex > f.second){
				--mappedIndex;
			}
			factors[mappedIndex] += mfactors[i];
		}
	}
}

}

template <typename float_t, typename index_t, typename weight_func_obj_t>
void evaluate_facet_area(
	AlphaShapeContainer<float_t, index_t>& asc, 
	const weight_func_obj_t& wfo,
	float_t* areas
){
	evaluate_facet_area(*asc, wfo, areas);
}

template <typename alpha_shape_t, typename weight_func_obj_t, typename float_t>
void evaluate_facet_area(
	const alpha_shape_t& as,
	const weight_func_obj_t& weight_func_obj,
	float_t* areas
){
	//fringe facet
	typedef typename alpha_shape_t::Cell_handle Cell_handle;
	typedef typename alpha_shape_t::Facet Facet;
	typedef typename alpha_shape_t::Finite_facets_iterator face_it_t;
	for(
		face_it_t 
			it=as.finite_facets_begin(), 
			eit=as.finite_facets_end(); 
		it!=eit; 
		++it
	){
		const Facet& f = *it;
		//class_t cls = as.classify(f);
//all edges, by weight
		if(as.classify(f) != alpha_shape_t::EXTERIOR){
		//if(cls == Fixed_alpha_shape_3::REGULAR || cls == Fixed_alpha_shape_3::SINGULAR){
			float_t factors[3];
			::busv::detail::calc_facet_factor<float_t>(as, f, weight_func_obj, factors);

			float_t weights[3];
			weight_func_obj(f, weights);

			float_t area[3];
			processFacet(f, area);

			const Cell_handle& c = f.first;
			const unsigned char (&facetIdx)[3] = busv::detail::FACET_IDXS[f.second];
			for(size_t i=0; i<3; ++i){
				const size_t index = c->vertex(facetIdx[i])->info().index;
				areas[index] += (weights[i]-0.5*factors[i])*area[i];
#ifdef RES_DETAIL
if(index == RES_DETAIL){
const Facet& mf = as.mirror_facet(f);
std::cout << "f (" <<  c->vertex(facetIdx[0])->info().index << ", " << c->vertex(facetIdx[1])->info().index << ", " <<  c->vertex(facetIdx[2])->info().index << ", (" << c->vertex(f.second)->info().index << ", " << mf.first->vertex(mf.second)->info().index << ")) " << i << " : " << weights[i] << " " << factors[i] << " " << area[i]  << " " << areas[index] << std::endl;
std::cout << "\tfactors " << factors[0] << " " << factors[1] << " " << factors[2] << std::endl; 
std::cout << "\tweights " << weights[0] << " " << weights[1] << " " << weights[2] << std::endl; 

float_t tfactors[3] = {0.0, 0.0, 0.0};
::busv::detail::calc_facet_factor_half<float_t>(as, f, weight_func_obj, tfactors);
std::cout << "\tcffh " << tfactors[0] << " " << tfactors[1] << " " << tfactors[2] << std::endl;
tfactors[0] = tfactors[1] = tfactors[2] = 0.0;
::busv::detail::calc_facet_factor_half<float_t>(as, mf, weight_func_obj, tfactors);
std::cout << "\tcffhm " << tfactors[0] << " " << tfactors[1] << " " << tfactors[2] << std::endl;
}
#endif
//				areas[c->vertex(facetIdx[i])->info().index] += (weight-0.5*factor)*area[i];
			}
//
		}
	}
}

}

#endif //BUSV_EVALUATE_FACET_AREA_HPP

