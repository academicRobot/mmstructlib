#ifndef BUSV_EDGE_DATA_HPP
#define BUSV_EDGE_DATA_HPP

namespace busv{

template <typename T>
struct EdgeData{
	EdgeData() : dist(0) {
		phi[0] = phi[1] = area[0] = area[1] = 0.0;
	}
	T	dist,
		phi[2],
		area[2];
};

} //namespace busv

#endif

