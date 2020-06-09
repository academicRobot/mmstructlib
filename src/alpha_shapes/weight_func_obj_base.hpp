#ifndef BUSV_WEIGHT_FUNC_OBJ_BASE_HPP
#define BUSV_WEIGHT_FUNC_OBJ_BASE_HPP

#include "cgal_typedefs.hpp"
#include "facet_idxs.hpp"

#include <algorithm>

/* 
 * note on locality calculation:  for facets and cells, locality needs to be
 *    calculated with respect to each vertex.  For example, take facet ABC: if
 *    AB and AC are local, but BC is not, then ABC is local for A, but not B or
 *    C.
 */

namespace busv{

template <
	typename float_t, 
	typename index_t, 
	float_t (*pairwise_func)(float_t, float_t)
>
class weight_func_obj_base{
	public:
		typedef typename cgal_typedefs<float_t, index_t>::Fixed_alpha_shape_3 alpha_shape_t;
		typedef typename alpha_shape_t::Cell Cell;
		typedef typename alpha_shape_t::Facet Facet;
		typedef typename alpha_shape_t::Edge Edge;
		typedef weight_data<float_t, index_t> weight_data_t;
		weight_func_obj_base(const weight_data_t* wdata, index_t local_window) : 
			wdata(wdata), 
			local_window(local_window)
		{}

		void operator()(const Cell& c, float_t weights[4]) const{
			const weight_data_t wd[4] = {
				wdata[c.vertex(0)->info().index],
				wdata[c.vertex(1)->info().index],
				wdata[c.vertex(2)->info().index],
				wdata[c.vertex(3)->info().index]
			};
			const index_t rindex[4] = {
				wd[0].index, wd[1].index, wd[2].index, wd[3].index
			};
			const float_t weight = 
				pairwise_func(
					pairwise_func(wd[0].weight, wd[1].weight),
					pairwise_func(wd[2].weight, wd[3].weight)
				);
			const bool locals[6] = {
				 is_local(rindex[0], rindex[1]), //0        1 2 3
				 is_local(rindex[0], rindex[2]), //1      +------
				 is_local(rindex[0], rindex[3]), //2    0 | 0 1 2
				 is_local(rindex[1], rindex[2]), //3    1 |   3 4
				 is_local(rindex[1], rindex[3]), //4    2 |     5
				 is_local(rindex[2], rindex[3])  //5
			};
			weights[0] = locals[0] && locals[1] && locals[2] ? 1.0 : weight;
			weights[1] = locals[0] && locals[3] && locals[4] ? 1.0 : weight;
			weights[2] = locals[1] && locals[3] && locals[5] ? 1.0 : weight;
			weights[3] = locals[2] && locals[4] && locals[5] ? 1.0 : weight;
		}

		void operator()(const Facet& f, float_t weights[3]) const{
			const Cell& c = *f.first;
			const unsigned char (&facetIdx)[3] = busv::detail::FACET_IDXS[f.second];
			const weight_data_t wd[3] = {
				wdata[c.vertex(facetIdx[0])->info().index],
				wdata[c.vertex(facetIdx[1])->info().index],
				wdata[c.vertex(facetIdx[2])->info().index]
			};
			const index_t rindex[3] = { wd[0].index, wd[1].index, wd[2].index };
			const float_t weight = 
				pairwise_func(
					pairwise_func(wd[0].weight, wd[1].weight),
					wd[2].weight
				);
			const bool locals[3] = {
				 is_local(rindex[0], rindex[1]), //   1 2
				 is_local(rindex[0], rindex[2]), // 0 0 1
				 is_local(rindex[1], rindex[2])  // 1   2
			};
			weights[0] = locals[0] && locals[1] ? 1.0 : weight;
			weights[1] = locals[0] && locals[2] ? 1.0 : weight;
			weights[2] = locals[1] && locals[2] ? 1.0 : weight;
		}

		float_t operator()(const Edge& e) const{
			const Cell& c = *e.first;
			const weight_data_t wd[2] = {
				wdata[c.vertex(e.second)->info().index],
				wdata[c.vertex(e.third)->info().index],
			};
			return is_local(wd[0].index, wd[1].index) 
				? 1.0 : pairwise_func(wd[0].weight, wd[1].weight);
		}

	private:
		bool is_local(index_t i, index_t j) const{
			const bool flag = i > j;
			return ((flag ? i : j) - (flag ? j : i)) <= local_window;
		}

		const weight_data_t* wdata;
		const index_t local_window;
};

}

#endif //BUSV_WEIGHT_FUNC_OBJ_BASE_HPP
