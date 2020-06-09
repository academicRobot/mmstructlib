#ifndef BUSV_VERTEX_DATA
#define BUSV_VERTEX_DATA

#include <cstddef>
#include <limits>

namespace busv{

template <typename T, typename U=std::size_t>
struct VertexData{
	VertexData() : omega(0), index(std::numeric_limits<U>().max()), edge_count(0) {}
	VertexData(U index, float radius) : omega(0), index(index), edge_count(0), radius(radius) {}
	T omega;
	U index;
	U edge_count;
	float radius; //do not conceviably need higher precision
};

} //namespace busv

#endif

