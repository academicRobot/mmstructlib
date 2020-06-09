#ifndef BUSV_POINT_TRANSFORM_FUNCTION
#define BUSV_POINT_TRANSFORM_FUNCTION

#include "vertex_data.hpp"
#include "sqr.hpp"

#include <utility>

namespace busv{

template <typename Gt, typename T, typename U>
class PointTransformFunction;

template <typename Gt, typename T, typename U, typename V>
class PointTransformFunction<Gt, VertexData<T, V>, U>{
	typedef typename Gt::Weighted_point Weighted_point;
	typedef typename Gt::Bare_point Bare_point;
	typedef std::pair<Weighted_point, VertexData<T, V> > point_t;
	public:
		typedef point_t result_type;
		PointTransformFunction(const U* points, const U* radii) : 
			points(points), radii(radii)
		{}
		point_t operator()(unsigned int i) const{
			const U* curr = this->points + i*3;
			return 
				std::make_pair(
					Weighted_point(
						Bare_point(curr[0], curr[1], curr[2]), 
						sqr(this->radii[i])
					),
					VertexData<T, V>(i, this->radii[i])
				);
		}
	private:
		const U* points;
		const U* radii;
};

template <typename Gt, typename T, typename U>
class PointTransformFunction{
	typedef typename Gt::Weighted_point Weighted_point;
	typedef typename Gt::Bare_point Bare_point;
	typedef std::pair<Weighted_point, T> point_t;
	public:
		typedef point_t result_type;
		PointTransformFunction(const U* points, const U* radii) : 
			points(points), radii(radii)
		{}
		point_t operator()(unsigned int i) const{
			const U* curr = this->points + i*3;
			return 
				std::make_pair(
					Weighted_point(
						Bare_point(curr[0], curr[1], curr[2]), 
						sqr(this->radii[i])
					),
					T(i)
				);
		}
	private:
		const U* points;
		const U* radii;
};

}

#endif

