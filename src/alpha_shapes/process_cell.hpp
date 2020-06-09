#ifndef BUSV_PROCESS_CELL
#define BUSV_PROCESS_CELL

#include "calc_angles.hpp"

namespace busv{

template <typename T, typename Cell_t>
void processCell(const Cell_t& cell, T (&angles)[6]){
	const EdgeDataContainer<T>& edc = cell.info();

	T dists[6] = {
		edc.edgeData(0,1).dist, edc.edgeData(0,2).dist, edc.edgeData(0,3).dist,
		                        edc.edgeData(1,2).dist, edc.edgeData(1,3).dist,
		                                                edc.edgeData(2,3).dist
	};
/*std::cout << "cell dist "
<< dists[0] << " "
<< dists[1] << " "
<< dists[2] << " "
<< dists[3] << " "
<< dists[4] << " "
<< dists[5] << std::endl;*/

	calcAngles(dists, angles);
}

}

#endif

