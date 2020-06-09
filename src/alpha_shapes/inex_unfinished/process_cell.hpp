#ifndef BUSV_PROCESS_CELL
#define BUSV_PROCESS_CELL

#include "angles.hpp"

namespace busv{

template <typename T, typename Cell_t>
void processCell(Cell_t& cell, T (&areas)[4]){
	EdgeData<T>* (&es)[6] = cell.info()->edges;
	FacetData<T>* (&fs)[4] = cell.info()->facets;

	T dists[6] = {
		es[0]->dist, es[1]->dist, es[2]->dist,
		             es[3]->dist, es[4]->dist,
		                          es[5]->dist
	};

	T angles[6];
	calcAngles4(dists, angles);

	//Note: angles and es arrays follow the upper triangular convention:
	//                j
	//         0 1 2 3
	//         -------
	//       0|  0 1 2
	//       1|    3 4
	//       2|      5
	//       3|
	//      i
	//
	//The fs array follows the facet definition, e.g. all facets that are 
	//  incedent on vertex 0 are facets 1, 2, and 3; for vertex 1 the facets
	//  are 0, 2, and 3.
	//For es[i,j].area[x], x = 0 if i<j, x = 1 if i>j
	//For fs[x].area[y] = {Ai, Aj, Ak}, x != i, j, k and i < j < k
	T
		angle_i_j[4] = { angles[0], angles[0], angles[1], angles[2] },
		angle_i_k[4] = { angles[1], angles[3], angles[3], angles[4] },
		angle_i_l[4] = { angles[2], angles[4], angles[5], angles[5] },

		area_i_j[4] = {es[0]->area[0], es[0]->area[1], es[1]->area[1], es[2]->area[1]},
		area_i_k[4] = {es[1]->area[0], es[3]->area[0], es[3]->area[1], es[4]->area[1]},
		area_i_l[4] = {es[2]->area[0], es[4]->area[0], es[5]->area[0], es[5]->area[1]},

		area_i_jk[4]= {fs[1]->area[0], fs[0]->area[0], fs[0]->area[1], fs[0]->area[2]},
		area_i_jl[4]= {fs[2]->area[0], fs[2]->area[1], fs[1]->area[1], fs[1]->area[2]},
		area_i_kl[4]= {fs[3]->area[0], fs[3]->area[1], fs[3]->area[2], fs[2]->area[2]};

	for(unsigned int i=0; i<4; ++i){
		areas[i] = 
			0.5 * (
				(angle_i_j[i] + angle_i_k[i] + angle_i_l[i] - M_PI) * sqr(radii[i]) -
				angle_i_j[i] * area_i_j[i] -
				angle_i_k[i] * area_i_k[i] -
				angle_i_l[i] * area_i_l[i] +
				area_i_jk[i] +
				area_i_jl[i] +
				area_i_kl[i]
			);
	}
}

}

#endif

