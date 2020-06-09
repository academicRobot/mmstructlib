#ifndef BUSV_WEIGHT_FUNC_OBJ_MIN_HPP
#define BUSV_WEIGHT_FUNC_OBJ_MIN_HPP

#include "weight_func_obj_base.hpp"
#include <algorithm>

namespace busv{

namespace detail {
template <typename float_t>
inline float_t min(float_t a, float_t b){
	return a <= b ? a : b;
}
}

template <typename float_t, typename index_t>
class weight_func_obj_min : 
	public weight_func_obj_base<
		float_t,
		index_t,
		&busv::detail::min<float_t>
	> 
{
	public:
		typedef weight_data<float_t, index_t> weight_data_t;
		weight_func_obj_min(const weight_data_t* wdata, index_t local_window) : 
			weight_func_obj_base<float_t,index_t,&busv::detail::min<float_t> >(wdata, local_window)
		{}
};

}

#endif //BUSV_WEIGHT_FUNC_OBJ_MIN_HPP
