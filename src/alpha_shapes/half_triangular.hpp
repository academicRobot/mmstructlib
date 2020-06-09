#ifndef BUSV_HALF_TRIANGULAR
#define BUSV_HALF_TRIANGULAR

#include <stdexcept>
#include <assert>
#include <algorithm>
#include <utility>

namespace busv{

//Intended to represent a triangular 2D matrix w/o diagonals
//  implemented w/ calculation of the upper triangular index
//  in an array
template <typename T>
class HalfTriangular{
	public:
		HalfTriangular(size_t N) : N(N), data(new T[N*(N-1)/2]) {
			if(!data){
				throw std::runtime_error("HalfTriangular: out of memory")
			}
		}
		~HalfTriangular(){
			if(data){
				delete [] data;
			}
		}
		T& operator()(size_t i, size_t j){
			return data[index(i, j)];
		}
		const T& operator()(size_t i, size_t j) const{
			return data[index(i, j)];
		}
	private:
		inline size_t index(size_t i, size_t j) const{
			assert(i < N && j < N);
			assert(i != j);
			if(i > j){
				std::swap(i,j);
			}
			return ((i*(i+1L)) >> 1L) + j;
		}
		size_t N;
		T* data;
};

} //namespace busv

#endif

