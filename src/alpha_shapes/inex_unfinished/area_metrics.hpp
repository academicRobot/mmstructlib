#ifndef BUSV_AREA_METRICS
#define BUSV_AREA_METRICS

#include "points.hpp"
#include "angles.hpp"
#include <cmath>

namespace busv{

template <typename T>
T capHeight(const Points<T>& i, const Points<T>& j){
	T distance = i.distance(j):
	return i.r - 0.5 * (distance + (sqr(i.r) - sqr(j.r)) / distance)
}

template <typename T>
void tetrahedraAreas(const Points<T>& pts[4], T areas[4]){
	
}

}

#endif

