#ifndef BUSV_PROCESS_EDGE
#define BUSV_PROCESS_EDGE

#include "cell_data.hpp"
#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include <cassert>

namespace busv{

typedef <typename T>
inline T sqr(T a){
	return a*a;
}

template <typename T, typename Edge_t>
void processEdge(const Edge_t& e, T (&areas)[2], T& dist){
	unsigned int 
		i = e.second,
		j = e.third;
	assert(i<j);
	dist = std::sqrt(CGAL::squared_distance(
		e.first->vertex(i)->point(),
		e.first->vertex(j)->point()
	));
	T	r1 = e.first->vertex(i)->weight(),
		r2 = e.first->vertex(j)->weight();
	T 	term2 = 0.5 * dist,
		term3 = 0.5 (sqr(r1) - sqr(r2)) / dist;

	areas[0] = 2.0*r1*(r1 - term2 - term3); //note: leaving off pi!
	areas[1] = 2.0*r2*(r2 - term2 + term3);
}

}

#endif

