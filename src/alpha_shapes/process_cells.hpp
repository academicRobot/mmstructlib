#ifndef BUSV_PROCESS_CELLS_HPP
#define BUSV_PROCESS_CELLS_HPP

#include "process_cell.hpp"
#include "edge_data_container.hpp"
#include "alpha_shape_container.hpp"

namespace busv {

template <typename float_t, typename index_t, typename weight_func_obj_t>
void process_cells(
	AlphaShapeContainer<float_t, index_t>& asc, 
	const weight_func_obj_t& wfo
){
	process_cells(*asc, wfo);
}

template <typename float_t, typename alpha_shape_t, typename weight_func_obj_t>
void process_cells(
	alpha_shape_t& as, 
	const weight_func_obj_t& weight_func_obj
){
	typedef typename alpha_shape_t::Cell Cell;
	typedef typename alpha_shape_t::Finite_cells_iterator cell_it_t;
	for(
		cell_it_t 
			it=as.finite_cells_begin(), 
			eit=as.finite_cells_end(); 
		it!=eit; 
		++it
	){
//all cells, by weight
		if(as.classify(it) == alpha_shape_t::INTERIOR){
			Cell& c = *it;

			//for(size_t i=0; i<4; ++i){
			//	indices[i] = c.vertex(i)->info().index;
			//}

			float_t weights[4];
			weight_func_obj(c, weights);

			float_t angles[6];
			processCell(c, angles);

			//collect edge-wise angles
			EdgeDataContainer<float_t>& edc = c.info();
			for(size_t i=0, pos=0; i<3; ++i){
				for(size_t j=i+1; j<4; ++j){
					size_t ii = i, jj = j;
					if(c.vertex(ii) > c.vertex(jj)){
						std::swap(ii, jj);
					}
					float_t (&phi)[2] = edc.edgeData(i,j).phi;
					phi[0] += weights[ii] * angles[pos];
					phi[1] += weights[jj] * angles[pos];
					++pos;
				}
			}

			//collect vertex incident edge dihedral angles
			//    0 1 2 3
			//  0   0 1 2
			//  1     3 4
			//  2       5
			//  3
			const unsigned char vertAngles[4][3] = {
				{ 0, 1, 2 },
				{ 0, 3, 4 },
				{ 1, 3, 5 },
				{ 2, 4, 5 }
			};
			for(size_t i=0; i<4; ++i){
				const unsigned char (&vertAngles_i)[3] = vertAngles[i];
				float_t sum = -M_PI;
				for(size_t j=0; j<3; ++j){
					sum += angles[vertAngles_i[j]];
				}
				c.vertex(i)->info().omega += weights[i] * sum;
			}
//
		}
	}
}

}

#endif
