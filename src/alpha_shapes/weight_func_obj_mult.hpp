#ifndef BUSV_WEIGHT_FUNC_OBJ_MULT_HPP
#define BUSV_WEIGHT_FUNC_OBJ_MULT_HPP

#include "weight_func_obj_base.hpp"
#include <algorithm>

namespace busv{

namespace detail {
template <typename float_t>
float_t mult(float_t a, float_t b){
	return a * b;
}
}

typedef float (*test_func_ptr_t)(float, float);
test_func_ptr_t test_func_ptr = &busv::detail::mult;

template <typename float_t, typename index_t>
class weight_func_obj_mult : 
	public weight_func_obj_base<
		float_t,
		index_t,
		&busv::detail::mult<float_t>
	> 
{
	public:
		typedef weight_data<float_t, index_t> weight_data_t;
		weight_func_obj_mult(const weight_data_t* wdata, index_t local_window) : 
			weight_func_obj_base<float_t,index_t,&busv::detail::mult<float_t> > (wdata, local_window)
		{}
};

}

#endif //BUSV_WEIGHT_FUNC_OBJ_MULT_HPP

