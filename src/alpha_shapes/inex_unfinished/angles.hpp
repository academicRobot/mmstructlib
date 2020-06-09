#ifndef BUSV_ANGLES
#define BUSV_ANGLES

#include "points.hpp"
#include "cayley_menger_matrix.hpp"
#include <cmath>

namespace busv{

template <typename T>
T invcot(T x){
	//2 * tan(1) - tan(x)
	return std::atan(1.0/x);
}

template <typename T>
void calcMinors4(const CayleyMengerMatrix<T>& cmm, T (&minors)[6]){
	minors[0] = cmm.minor_ij<0,1>();
	minors[1] = cmm.minor_ij<0,2>();
	minors[2] = cmm.minor_ij<0,3>();
	minors[3] = cmm.minor_ij<1,2>();
	minors[4] = cmm.minor_ij<1,3>();
	minors[5] = cmm.minor_ij<2,3>();
}

template <typename T>
void calcAngles4(const T (&dists)[6], T (&angles)[6]){
	CayleyMengerMatrix<T> cmm(dists);
	T minors[6];
	calcMinors4(cmm, minors);

	T volume_term = 2.0 / ( 3.0 * std::sqrt(
		(
			cmm.term(0,0) * cmm.minor_ij<0,0>() - 
			cmm.term(0,1) * minors[0] + 
			cmm.term(0,2) * minors[1] - 
			cmm.term(0,3) * minors[2] +
			cmm.term(0,4) * cmm.minor_ij<0,4>()
		) / 288.0
	));

	// i                1   1   1   2   2   3
	// j                2   3   4   3   4   4
	// sum              3   4   5   5   6   7
	const T sign[6] = {-1,  1, -1, -1,  1, -1};
	for(unsigned int i = 0; i < 6; ++i){
		angles[pos] = invcot(sign[i] * volume_term * dists[i] * minors[i]);
	}
}

}

#endif

