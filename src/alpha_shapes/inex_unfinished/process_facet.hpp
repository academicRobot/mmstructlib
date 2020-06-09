#ifndef BUSV_PROCESS_FACET
#define BUSV_PROCESS_FACET

#define _USE_MATH_DEFINES 
#include <cmath>

namespace busv{

typedef <typename T>
inline T sqr(T a){
	return a*a;
}

namespace detail{ namespace {

//process facet
//Note on storing facet intersection data
//  from: http://doc.cgal.org/latest/TDS_3/index.html#fig__TDS3figrepres
//  The four vertices of a cell are indexed with 0, 1, 2 and 3. The neighbors 
//  of a cell are also indexed with 0, 1, 2, 3 in such a way that the neighbor 
//  indexed by i is opposite to the vertex with the same index
//A pointer to the intersection data structure is stored in the cells info
// array using the opposite vertex index as the info index, with the same
// index for both neigboring cells.
const unsigned char FACET_IDXS[4][3] = {
	{1,2,3},
	{0,2,3},
	{0,1,3},
	{0,1,2}
};
const unsigned char EDGE_IDXS[4][3][2] = { //order is important for edge data
/*	{ {2, 3}, {1, 3}, {1, 2} },
	{ {2, 3}, {0, 3}, {0, 2} },
	{ {1, 3}, {0, 3}, {0, 1} },
	{ {1, 2}, {0, 2}, {0, 1} }

	{ {1, 2}, {1, 3}, {2, 3} },
	{ {0, 2}, {0, 3}, {2, 3} },
	{ {0, 1}, {0, 3}, {1, 3} },
	{ {0, 1}, {0, 2}, {1, 2} }*/

	{ 3, 4, 5 },
	{ 1, 2, 5 },
	{ 0, 2, 4 },
	{ 0, 1, 3 }
};
/*
cost unsigned char VERT_EDGES = [3][2] = {
	{ 0, 1 }, { 0, 2 }, { 1, 2 }
	{ {0, 0} , {1, 0} }, { {0, 1}, {2, 0} }, { {1, 1}, {2, 1} }
}
*/


} }

template <typename T, typename Facet_t>
void processFacet(Facet_t& facet, T (&areas)[3]){
	CellData<T>& cellData = facet.first->info();
	unsigned int ov = facet.second

	//collect edge data radii
	const unsigned char (&edgeIdx)[3] = detail::EDGE_IDXS[ov];
	const unsigned char (&facetIdx)[3] = detail::FACET_IDXS[ov];
	EdgeData<T> edges[3];
	T facet_radii[3];
	for(unsigned int i=0; i<3; ++i){
		edges[i] = *cellData.edge[edgeIdx[i]];
		facet_radii[i] = cell.vertex(facetIdx[i])->weight();
	}

	//calculate distances (intersection corner distances are the respective radii)
	T dists[6] = {
		edges[0].dist, edges[1].dist, facet_radii[0], 
	                   edges[2].dist, facet_radii[1], 
	                                  facet_radii[2]
	};

	//calculate angles
	T angles[6];
	calcAngles4(dists, angles);

	//calculate areas
	T 	angles_i_j[3] = { angles[0], angles[0], angles[1] },
		angles_i_k[3] = { angles[1], angles[3], angles[3] },
		angles_i_l[3] = { angles[2], angles[4], angles[5] },
		areas_i_j[3] = { edges[0].area[0], edges[0].area[1], edges[1].area[1] },
		areas_i_k[3] = { edges[1].area[0], edges[2].area[0], edges[2].area[1] };
	for(unsigned int i=0; i<3; ++i){
		areas[i] = 
			areas_i_j[i] * angle_i_j[i] + 
			areas_i_k[i] * angle_i_k[i] + 
			2.0 * sqr(facetRadii[i]) * (
				angle_i_j[i] + angle_i_k[i] + angle_i_l[i] - M_PI
			);
	}
}

}

#endif

