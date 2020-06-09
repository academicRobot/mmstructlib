#ifndef BUSV_CALC_ANGLES
#define BUSV_CALC_ANGLES

#include "cayley_menger_matrix.hpp"

#define _USE_MATH_DEFINES
#include <cmath>


namespace busv{

template <typename T>
inline T invcot(T x){
	return M_PI * 0.5 - std::atan(x);
}

//README: 
//note on indices, input and output indices reference
//edges acording to
//
//     0 1 2 3 
//   0   0 1 2
//   1     3 4
//   2       5
//   3
//
//the output angle is dihedral around the edge i and j, 
//not between faces i and j
template <typename T>
void calcAngles(const T (&dists)[6], T (&angles)[6]){
	CayleyMengerMatrix<T> cmm(dists);

	T minors[6];
	cmm.minors(minors);

	T sqr_volume =
		(
			//term(0,0)=0 so don't bother: cmm.term(0,0) * cmm.template minor_ij<0,0>() +
			cmm.term01() * minors[0] + 
			cmm.term02() * minors[1] + 
			cmm.term03() * minors[2] +
			cmm.term04() * cmm.minor04()
		) / 288.0;
	
	//coplanar tetrahedra will yield a volume of zero (possibly less then zero due to
	//   rounding error).  For all intents and purposes, these tetrahedra yield zero
	//   angles.
	if(sqr_volume <= 0){
		for(unsigned int i = 0; i < 6; ++i){
			angles[i] = 0.0;
		}
	}
	else{
		T volume = std::sqrt(sqr_volume);

		//note: minors come out indexed with respect to two faces, rather than an edge
		//  taking two faces as an edge yields the opposite edge, hence 5-i
		for(unsigned int i = 0; i < 6; ++i){
			//from paper: angles[i] = invcot(sign[i] * volume_term * dists[i] * minors[i]);
			//corrected: angles[i] = invcot(sign[i] * minors[i] / (24.0 * dists[i] * volume));
			//one less division: angles[i] = std::atan(sign[i] * 24.0 * dists[i] * volume / minors[i]);
			//correct inverse cotangent function, now defined 0 to pi
//			angles[i] = invcot(sign[i] * minors[i] / (24.0 * dists[i] * volume));
			//angles[5-i] = invcot(2.0 * dists[5-i] * minors[i] / (3.0*volume));
			angles[i] = invcot(minors[5-i] / (24.0 * dists[i] * volume));
		}
	}
}

}

#endif

