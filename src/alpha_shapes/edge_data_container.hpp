#ifndef BUSV_EDGE_DATA_CONTAINER_HPP
#define BUSV_EDGE_DATA_CONTAINER_HPP

#include "edge_data.hpp"
#include <cstddef>
#include <cassert>

namespace busv{

//namespace { don't really need this anoymizer and VS doesn't like it
namespace detail { 
	unsigned char _ed_index[4][4] = {
		{ 255,   0,   1,   2},
		{   0, 255,   3,   4},
		{   1,   3, 255,   5},
		{   2,   4,   5, 255}
	};
} //}

template <typename T>
class EdgeDataContainer{
	public:
		template <typename Edge_t>
		const EdgeData<T>& edgeData(const Edge_t& e) const{
			return *edgeDataPtr[detail::_ed_index[e.second][e.third]]; 
		}

		template <typename Edge_t>
		EdgeData<T>& edgeData(const Edge_t& e){
			return *edgeDataPtr[detail::_ed_index[e.second][e.third]]; 
		}

		EdgeData<T>& edgeData(size_t i, size_t j){
			return *edgeDataPtr[detail::_ed_index[i][j]]; 
		}

		const EdgeData<T>& edgeData(size_t i, size_t j) const{
			return *edgeDataPtr[detail::_ed_index[i][j]]; 
		}

		template <typename Edge_t>
		void setEdgeData(const Edge_t& e, EdgeData<T>& ed){
			edgeDataPtr[detail::_ed_index[e.second][e.third]] = &ed; 
		}

		void setEdgeData(size_t i, size_t j, EdgeData<T>& ed){
			edgeDataPtr[detail::_ed_index[i][j]] = &ed; 
		}

	private:
		EdgeData<T>* edgeDataPtr[6];
};

} //namespace busv

#endif

