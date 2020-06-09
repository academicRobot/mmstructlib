#ifndef BUSV_ALPHA_SHAPE_CONTAINER_HPP
#define BUSV_ALPHA_SHAPE_CONTAINER_HPP

#include "cgal_typedefs.hpp"
#include <iterator>

namespace busv{

template <typename T, typename U>
class AlphaShapeContainer{
	public:
		typedef typename cgal_typedefs<T,U>::Fixed_alpha_shape_3 Fixed_alpha_shape_3;

		AlphaShapeContainer(Fixed_alpha_shape_3* as_ptr, EdgeData<T>* edgeData_ptr) :
			as_ptr(as_ptr), edgeData_ptr(edgeData_ptr), ref_count_ptr(new size_t(1))
		{}

		AlphaShapeContainer(const AlphaShapeContainer& other){
			copy(other);
		}

		AlphaShapeContainer(void) :
			as_ptr(NULL), edgeData_ptr(NULL), ref_count_ptr(NULL)
		{}

		~AlphaShapeContainer(){
			destroy();
		}

		AlphaShapeContainer& operator=(const AlphaShapeContainer& other){
			copy(other);
			return *this;
		}
		
		Fixed_alpha_shape_3& operator*(void){
			return *as_ptr;
		}
		
		const Fixed_alpha_shape_3& operator*(void) const{
			return *as_ptr;
		}

		U get_num_vertices(void) const{
			return std::distance(as_ptr->finite_vertices_begin(), as_ptr->finite_vertices_end());
		}

		U get_max_vertex_index(void) const{
			typedef typename Fixed_alpha_shape_3::Finite_vertices_iterator vert_it_t;
			U max = 0;
			for(
				vert_it_t 
					it = as_ptr->finite_vertices_begin(), 
					eit = as_ptr->finite_vertices_end(); 
				it != eit; 
				++it //, ++i
			){
				U index = it->info().index;
				max = max > it->info().index ? max : index;
			}
			return max;
		}

		void get_omegas(T* omegas) const{
			typedef typename Fixed_alpha_shape_3::Finite_vertices_iterator vert_it_t;
			//size_t i=0;
			for(
				vert_it_t 
					it = as_ptr->finite_vertices_begin(), 
					eit = as_ptr->finite_vertices_end(); 
				it != eit; 
				++it //, ++i
			){
				omegas[it->info().index] = it->info().omega;
			}
		}

		void get_edge_counts(U* edge_counts) const{
			typedef typename Fixed_alpha_shape_3::Finite_vertices_iterator vert_it_t;
			//size_t i=0;
			for(
				vert_it_t 
					it = as_ptr->finite_vertices_begin(), 
					eit = as_ptr->finite_vertices_end(); 
				it != eit; 
				++it //, ++i
			){
				edge_counts[it->info().index] = it->info().edge_count;
			}
		}

	private:
		void copy(const AlphaShapeContainer& other){
			destroy();
			as_ptr = other.as_ptr;
			edgeData_ptr = other.edgeData_ptr;
			ref_count_ptr = other.ref_count_ptr;
			++(*ref_count_ptr);
		}

		void destroy(void){
			if(ref_count_ptr){
				--(*ref_count_ptr);
				if(!(*ref_count_ptr)){
					delete as_ptr;
					delete[] edgeData_ptr;
					delete ref_count_ptr;
				}
			}
		}

		Fixed_alpha_shape_3* as_ptr;
		EdgeData<T>* edgeData_ptr;
		size_t* ref_count_ptr;
};

} //namespace busv

#endif

