#ifndef BUSV_FIND_VERTEX_INDEX_HPP
#define BUSV_FIND_VERTEX_INDEX_HPP

namespace busv {

template <typename Vertex_handle_t, typename Cell_t>
size_t find_vertex_index(const Cell_t& c, Vertex_handle_t v){
	return 
		c.vertex(0) == v ? 0 : 
			c.vertex(1) == v ? 1 : 
				c.vertex(2) == v ? 2 : 3;
}

}

#endif

