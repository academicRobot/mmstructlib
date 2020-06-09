#ifndef BUSV_FACET_IDXS
#define BUSV_FACET_IDXS

namespace busv{

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

/* Not needed, just subract one from index if greater than index of opposite
 * vertex, same thing is true for FACET_IDXS, but direct indexing is not
 * potentially dangerous in that case.  In this case, some mappings are
 * undefined.  Should functionalize and throw error if opposite and mappy are
 * the same.
const unsigned char FACET_IDXS_REV[4][3] = {
	{ , 0, 1, 2}, //0
	{0,  , 1, 2}, //1
	{0, 1,  , 2}, //2
	{0, 1, 2,  }  //3
};
*/
} }

}

#endif

