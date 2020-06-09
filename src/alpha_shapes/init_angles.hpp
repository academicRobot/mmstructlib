#ifndef BUSV_INIT_ANGLES_HPP
#define BUSV_INIT_ANGLES_HPP

#include "cgal_typedefs.hpp"
#include "alpha_shape_container.hpp"

namespace busv{

template <typename T, typename U>
void init_angles(AlphaShapeContainer<T,U>& asc){
	init_angles(*asc);
}

template <typename T, typename U>
void init_angles(typename cgal_typedefs<T,U>::Fixed_alpha_shape_3& as){
	typedef typename cgal_typedefs<T,U>::Fixed_alpha_shape_3 Fixed_alpha_shape_3;

	//set phi on edges
	typedef typename Fixed_alpha_shape_3::Finite_edges_iterator edge_it_t;
	typedef typename Fixed_alpha_shape_3::Edge Edge;
	for(
		edge_it_t
			it=as.finite_edges_begin(),
			eit=as.finite_edges_end();
		it!= eit;
		++it
	){
		const Edge& e = *it;
		T (&phi)[2] = e.first->info().edgeData(e.second, e.third).phi;
		phi[0] = phi[1] = 0.0;
	}

	//set omega on vertices
	typedef typename Fixed_alpha_shape_3::Finite_vertices_iterator vert_it_t;
	//typedef typename Fixed_alpha_shape_3::Vertex Vertex;
	for(
		vert_it_t
			it=as.finite_vertices_begin(),
			eit=as.finite_vertices_end();
		it!= eit;
		++it
	){
		it->info().omega = 0.0;
		it->info().edge_count = 0;
	}
}

} //namespace busv

#endif

