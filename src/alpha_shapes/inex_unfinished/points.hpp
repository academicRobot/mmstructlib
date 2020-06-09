#ifndef BUSV_POINTS
#define BUSV_POINTS

#include <boost/array.hpp>
#include <cmath>

namespace busv{

template <typename T>
inline T sqr(T a){
	return a*a;
}

template <typename T>
class Point{
	public:
		Point(const T* data){
			a[0] = data[0];
			a[1] = data[1];
			a[2] = data[2];
		}
		T distance(const Point<T>& other) const{
			return std::sqrt(
				sqr(this->a[0] - other.a[0]) +
				sqr(this->a[1] - other.a[1]) +
				sqr(this->a[2] - other.a[2])
			);
		}
		T sqrDistance(const Point<T>& other) const{
			return
				sqr(this->a[0] - other.a[0]) +
				sqr(this->a[1] - other.a[1]) +
				sqr(this->a[2] - other.a[2])
		}
		T capHeight(const Point<T>& other) const{
		}
	private:
		T c[3];
		T r;
};

}

#endif

