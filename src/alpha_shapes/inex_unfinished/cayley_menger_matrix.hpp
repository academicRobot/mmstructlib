#ifndef BUSV_CAYLEY_MENGER_MATRIX 
#define BUSV_CAYLEY_MENGER_MATRIX

#include "points.hpp"
#include <cmath>

namespace busv{

//http://stackoverflow.com/a/2980966/342256
template <typename T>
T det4x4(const T* m){
	// 23 add/sub, 28 mul
	T	m_12_9 = m[12] * m[9],
		m_12_5 = m[12] * m[5],
		m_8_5  = m[8]  * m[5],
		m_12_1 = m[12] * m[1],
		m_8_1  = m[8]  * m[1],
		m_4_1  = m[4]  * m[1],
		m_8_13 = m[8]  * m[13],
		m_4_13 = m[4]  * m[13],
		m_4_9  = m[4]  * m[9],
		m_0_13 = m[0]  * m[13],
		m_0_9  = m[0]  * m[9],
		m_0_5  = m[0]  * m[5];
	return  
		((m_12_9 - m_8_13) * m[6]  -
		 (m_12_5 - m_4_13) * m[10] +
		 (m_8_5  - m_4_9 ) * m[14]) * m[3]   -
		((m_12_9 - m_8_13) * m[2]  -
		 (m_12_1 - m_0_13) * m[10] +
		 (m_8_1  - m_0_9 ) * m[14]) * m[7]   +
		((m_12_5 - m_4_13) * m[2]  -
		 (m_12_1 - m_0_13) * m[6]  +
		 (m_4_1  - m_0_5 ) * m[14]) * m[11]  -
		((m_8_5  - m_4_9 ) * m[2]  -
		 (m_8_1  - m_0_9 ) * m[6]  +
		 (m_4_1  - m_0_5 ) * m[10]) * m[15];
}

template <typename T>
class CayleyMengerMatrix{
	public:
		CayleyMengerMatrix(const T (&dists)[6]) : distArr(dist) {
			a[0][0] = a[1][1] = a[2][2] = a[3][3] = a[4][4] = T(0);
			a[4][0] = a[4][1] = a[4][2] = a[4][3] = 
			a[0][4] = a[1][4] = a[2][4] = a[3][4] = T(1);
			a[0][1] = a[1][0] = sqr(dists[0]);
			a[0][2] = a[2][0] = sqr(dists[1]);
			a[0][3] = a[3][0] = sqr(dists[2]);
			a[1][2] = a[2][1] = sqr(dists[3]);
			a[1][3] = a[3][1] = sqr(dists[4]);
			a[2][3] = a[3][2] = sqr(dists[5]);
		}
		template <unsigned int I, unsigned int J>
		T minor_ij(void) const{
			T double m[4][4];
			for(unsigned int i=0, ii=0; i<5; ++i){ //unroll loops should work?
				if(i != I){
					for(unsigned int j=0, jj=0; i<5; ++i){
						if(j != J){
							m[ii][jj++] = a[i][j]
						}
					}
					++ii;
				}
			}
			return det4x4(m)
		}
		T term(unsigned int i, unsigned int j) const{
			return a[i][j];
		}
	private:
		T a[5][5];
		const T (&distArr)[6];
};

}

#endif

