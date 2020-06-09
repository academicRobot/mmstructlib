#ifndef CGAL_POINT_TRANSFORM_FUNCTION
#define CGAL_POINT_TRANSFORM_FUNCTION

#include <utility>

namespace busv{

template <typename Gt>
class CGALPointTransformFunction{
	typedef typename Gt::Weighted_point Weighted_point;
	typedef typename Gt::Bare_point Bare_point;
	typedef std::pair<Weighted_point, unsigned int> point_t;
	public:
		typedef point_t result_type;
		CGALPointTransformFunction(const double* points, const double* weights) : 
			points(points), weights(weights)
		{}
		point_t operator()(unsigned int i) const{
			const double* curr = this->points + i*3;
			return 
				std::make_pair(
					Weighted_point(
						Bare_point(curr[0], curr[1], curr[2]), 
						this->weights[i]
					),
					i
				);
		}
	private:
		const double* points;
		const double* weights;
};

}

#endif

