#ifndef BUSV_PROCESS_EDGE
#define BUSV_PROCESS_EDGE

#include "sqr.hpp"

#include <CGAL/squared_distance_3.h>

#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include <cassert>
#include <algorithm>
#include <utility>

namespace busv{

template <typename T, typename Edge_t>
void processEdge(const Edge_t& e, T (&areas)[2], T& dist){
	typedef typename Edge_t::first_type::value_type::Vertex_handle vh_t;
	vh_t v1 = e.first->vertex(e.second),
	     v2 = e.first->vertex(e.third);
	if(v1>v2){
		std::swap(v1,v2);
	}
	dist = std::sqrt(CGAL::squared_distance(
		v1->point(),
		v2->point()
	));
	T	r1 = v1->info().radius,
		r2 = v2->info().radius;
	T 	term2 = 0.5 * dist,
		term3 = 0.5 * (sqr(r1) - sqr(r2)) / dist;

	areas[0] = 2.0*r1*(r1 - term2 - term3); //note: leaving off pi!
	areas[1] = 2.0*r2*(r2 - term2 + term3);
}

}

#endif

