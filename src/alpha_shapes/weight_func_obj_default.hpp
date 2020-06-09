#ifndef BUSV_WEIGHT_FUNC_OBJ_DEFAULT_HPP
#define BUSV_WEIGHT_FUNC_OBJ_DEFAULT_HPP

#include "cgal_typedefs.hpp"

namespace busv{

template <typename float_t, typename index_t>
class weight_func_obj_default{
	public:
		typedef typename cgal_typedefs<float_t, index_t>::Fixed_alpha_shape_3 alpha_shape_t;
		typedef typename alpha_shape_t::Cell Cell;
		typedef typename alpha_shape_t::Facet Facet;
		typedef typename alpha_shape_t::Edge Edge;
		void operator()(const Cell& c, float_t weights[4]) const{
			weights[0] = weights[1] = weights[2] = weights[3] = 1.0;
		}
		void operator()(const Facet& f, float_t weights[3]) const{
			weights[0] = weights[1] = weights[2] = 1.0;
		}
		float_t operator()(const Edge& e) const{
			return 1.0;
		}
};

}

#endif //BUSV_WEIGHT_FUNC_DEFAULT_HPP
