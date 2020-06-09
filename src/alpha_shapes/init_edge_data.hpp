#ifndef BUSV_INIT_EDGE_DATA_HPP
#define BUSV_INIT_EDGE_DATA_HPP

#include "find_vertex_index.hpp"
#include "process_edge.hpp"

//#include <iostream>

namespace busv {

//note: caller is responsible for "delete [] ret_edge_data_array;"
template <typename float_t, typename alpha_shape_t>
EdgeData<float_t>* init_edge_data(alpha_shape_t& as){
	//process edges and setup cell edge pointers
	typedef typename alpha_shape_t::Edge Edge;
	typedef typename alpha_shape_t::Finite_edges_iterator edge_it_t;
	typedef typename alpha_shape_t::Cell_circulator cc_t;
	typedef typename alpha_shape_t::Vertex_handle vh_t;
	//EdgeData<float_t>* edgeData = new EdgeData<float_t>[as.number_of_edges()]; //Includes infinite edges. Could loop through edges twice to avoid over allocation, but meh
	EdgeData<float_t>* edgeData = new EdgeData<float_t>[as.number_of_finite_edges()];
	EdgeData<float_t>* ed_it = edgeData;
	for(
		edge_it_t 
			it=as.finite_edges_begin(), 
			eit=as.finite_edges_end();
		it!= eit; 
		++it, ++ed_it
	){
		const Edge& e = *it;
		EdgeData<float_t>& ed = *ed_it;
		processEdge(e, ed.area, ed.dist);
		vh_t
			vh1 = e.first->vertex(e.second), 
			vh2 = e.first->vertex(e.third);
		cc_t cc = as.incident_cells(*it);
		cc_t ecc = cc;
//std::cout << "cells " << as.number_of_cells() << std::endl;
		if(cc != 0){
			do{
				size_t 
					i = find_vertex_index(*cc, vh1),
					j = find_vertex_index(*cc, vh2);
				cc->info().setEdgeData(i, j, ed);
				++cc;
			}while(cc != 0 && cc != ecc);
		}
	}
	return edgeData;
}

}

#endif

