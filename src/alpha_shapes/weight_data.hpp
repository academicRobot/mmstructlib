#ifndef BUSV_WEIGHT_DATA_HPP
#define BUSV_WEIGHT_DATA_HPP

namespace busv{

template <typename float_t, typename index_t>
struct weight_data{
	float_t weight;
	index_t index;
//	bool backbone;
	weight_data(void) : weight(0.0), index(0) {}
};

}

#endif //BUSV_WEIGHT_DATA_HPP
