#ifndef BUSV_PROCESS_FACET
#define BUSV_PROCESS_FACET

#include "edge_data_container.hpp"
#include "calc_angles.hpp"
#include "sqr.hpp"
#include "facet_idxs.hpp"

#define _USE_MATH_DEFINES 
#include <cmath>

#include <iostream>

namespace busv{

namespace detail{ namespace {

const unsigned char EDGE_IDXS[4][3][2] = { //order is important for edge data
/*	{ {2, 3}, {1, 3}, {1, 2} },
	{ {2, 3}, {0, 3}, {0, 2} },
	{ {1, 3}, {0, 3}, {0, 1} },
	{ {1, 2}, {0, 2}, {0, 1} }*/

	{ {1, 2}, {1, 3}, {2, 3} },
	{ {0, 2}, {0, 3}, {2, 3} },
	{ {0, 1}, {0, 3}, {1, 3} },
	{ {0, 1}, {0, 2}, {1, 2} }

/*	{ 3, 4, 5 },
	{ 1, 2, 5 },
	{ 0, 2, 4 },
	{ 0, 1, 3 }*/
};
/*
cost unsigned char VERT_EDGES = [3][2] = {
	{ 0, 1 }, { 0, 2 }, { 1, 2 }
	{ {0, 0} , {1, 0} }, { {0, 1}, {2, 0} }, { {1, 1}, {2, 1} }
}
*/


} }

template <typename T, typename Facet_t>
void processFacet(Facet_t& f, T (&areas)[3]){
	typedef typename Facet_t::first_type::value_type::Vertex_handle vh_t;

	EdgeDataContainer<T>& edc = f.first->info();
	unsigned int ov = f.second;

	//collect edge data radii
//std::cout << "facet ov " << ov << std::endl;
	const unsigned char (&edgeIdx)[3][2] = busv::detail::EDGE_IDXS[ov];
	const unsigned char (&facetIdx)[3] = busv::detail::FACET_IDXS[ov];
	EdgeData<T> edges[3];
	T facet_radii[3];
	vh_t vhs[3];
	for(unsigned int i=0; i<3; ++i){
		edges[i] = edc.edgeData(edgeIdx[i][0], edgeIdx[i][1]);
		vhs[i] = f.first->vertex(facetIdx[i]);
		facet_radii[i] = vhs[i]->info().radius;
	}

	//calculate distances (intersection corner distances are the respective radii)
	T dists[6] = {
		edges[0].dist, edges[1].dist, facet_radii[0], 
	                   edges[2].dist, facet_radii[1], 
	                                  facet_radii[2]
	};

	//calculate angles
	T angles[6];
	calcAngles(dists, angles);

/*std::cout << "facet dists";
for(int i=0; i<6; ++i){std::cout << " " << dists[i];}
std::cout << std::endl;

std::cout << "facet angles";
for(int i=0; i<6; ++i){std::cout << " " << angles[i];}
std::cout << std::endl;*/

	//calculate areas
	T 	angles_i_j[3] = { angles[0], angles[0], angles[1] },
		angles_i_k[3] = { angles[1], angles[3], angles[3] },
		angles_i_l[3] = { angles[2], angles[4], angles[5] },
		areas_i_j[3] = { edges[0].area[vhs[0]>vhs[1]], edges[0].area[vhs[0]<vhs[1]], edges[1].area[vhs[0]<vhs[2]] },
		areas_i_k[3] = { edges[1].area[vhs[0]>vhs[2]], edges[2].area[vhs[1]>vhs[2]], edges[2].area[vhs[1]<vhs[2]] };
//		areas_i_j[3] = { edges[0].area[0], edges[0].area[1], edges[1].area[1] },
//		areas_i_k[3] = { edges[1].area[0], edges[2].area[0], edges[2].area[1] };
	for(unsigned int i=0; i<3; ++i){
/*std::cout 
<< "areas (" << areas_i_j[i]
<< " " << areas_i_k[i]
<< ") angles (" << angles_i_j[i]
<< " " << angles_i_k[i]
<< " " << angles_i_l[i] << ")"
<< std::endl;*/
		areas[i] = 
			areas_i_j[i] * angles_i_j[i] + 
			areas_i_k[i] * angles_i_k[i] + 
			-2.0 * sqr(facet_radii[i]) * (
				angles_i_j[i] + angles_i_k[i] + angles_i_l[i] - M_PI
			);
	}
}

}

#endif

