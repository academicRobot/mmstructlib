#ifndef BUSV_CELL_DATA
#define BUSV_CELL_DATA

namespace busv{

template <typename T>
struct EdgeData{
	T area[2]; //note: actually area/pi
	T dist;
};

template <typename T>
struct FacetData{
	T area[3];
};

namespace detail{
//the 255s make valgrind cry
const unsigned char EDGE_LUT[4][4] = {
	{255,  0,  1,  2},
	{  0,255,  3,  4},
	{  1,  3,255,  5},
	{  2,  4,  5,255},
};
}

template <typename T>
class CellData{
	public:
		inline T& edge(const unsigned int (&idx)[2]){
			return edges[detail::EDGE_LUT[idx[0]][idx[1]]];
		}
		inline const T& edge(const unsigned int (&idx)[2]) const{
			return edges[detail::EDGE_LUT[idx[0]][idx[1]]];
		}
		inline T& facet(unsigned int i){
			return facets[i];
		}
		inline const T& facet(unsigned int i) const{
			return facets[i];
		}
	private:
		//indexed by EDGE_LUT
		EdgeData<T>* edges[6];

		//indexed by opposing vertex index
		FacetData<T>* facets[4];
};

}

#endif


