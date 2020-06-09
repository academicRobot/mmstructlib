#ifndef BUSV_CAYLEY_MENGER_MATRIX 
#define BUSV_CAYLEY_MENGER_MATRIX

#include "sqr.hpp"

#include <algorithm>

namespace busv{

template <typename T>
class CayleyMengerMatrix{
	public:
		CayleyMengerMatrix(const T (&dists)[6]) : distArr(dists) {
			for(size_t i=0; i<6; ++i){
				a[i] = sqr(dists[i]);
			}
		}

		T term00(void){ return 0; }
		T term01(void){ return a[0]; }
		T term02(void){ return a[1]; }
		T term03(void){ return a[2]; }
		T term04(void){ return 1; }
		T term11(void){ return 0; }
		T term12(void){ return a[3]; }
		T term13(void){ return a[4]; }
		T term14(void){ return 1; }
		T term22(void){ return 0; }
		T term23(void){ return a[5]; }
		T term24(void){ return 1; }
		T term33(void){ return 0; }
		T term34(void){ return 1; }
		T term44(void){ return 0; }

		T minor00(void) const{
			return sqr(-a[3]+a[4]-a[5])-4.0*a[3]*a[5]; 
		}

		T minor11(void) const{
			return sqr(-a[1]+a[2]-a[5])-4.0*a[1]*a[5]; 
		}

		T minor22(void) const{
			return sqr(-a[0]+a[2]-a[4])-4.0*a[0]*a[4]; 
		}

		T minor33(void) const{
			return sqr(-a[0]+a[1]-a[3])-4.0*a[0]*a[3]; 
		}

		//9 adds, 9 muls. Plus 3 adds, 4 muls for combo.
		T minor04(void) const{ 
			return 
				a[0]*a[5]*(a[3]+a[4]-a[5])
				+a[1]*a[4]*(a[3]-a[4]+a[5])
				+a[2]*a[3]*(-a[3]+a[4]+a[5])
				-2.0*a[3]*a[4]*a[5];
		}

		//Returns {-det(M01), +det(M02), -det(M03), -det(M12), +det(M13), -det(M23)}
		//Note that the sign term of the reduction is included in these minors
		//To get raw minors, multiply each by the appropriate sign: {-, +, -, -, +, -}
		void minors(T (&m)[6]) const{
			T sum = a[0]+a[1]+a[2]+a[3]+a[4]+a[5];

			const unsigned char termIdxs[6][4] = {
				{2, 1, 3, 4},
				{2, 0, 3, 5},
				{1, 0, 4, 5},
				{4, 0, 1, 5},
				{3, 0, 2, 5},
				{3, 1, 2, 4}
			};
			for(size_t i=0; i<6; ++i){
				const unsigned char (&j)[4] = termIdxs[i];
				m[i] = 
					a[5-i] * (sum - 3.0*a[i] - 2.0*a[5-i]) 
					+ (a[j[0]] - a[j[1]]) * (a[j[2]] - a[j[3]]);
//std::cout << "m " << i << " " << a[i] << " " << m[i] << std::endl;
			}
//std::cout << "m00 " << minor00() << std::endl;
		}

	private:
		T a[6];
		const T (&distArr)[6];
};


}

#endif

