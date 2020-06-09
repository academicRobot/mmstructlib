cdef extern from "alpha_shapes/alpha_shape_container.hpp" namespace "busv":
    cdef cppclass AlphaShapeContainer[T, U]:
        AlphaShapeContainer()
        AlphaShapeContainer(const AlphaShapeContainer&)
        AlphaShapeContainer& assign "operator="(const AlphaShapeContainer&)
        U get_num_vertices() const
        U get_max_vertex_index() const
        void get_omegas(T* omegas) const
        void get_edge_counts(U* edge_counts) const

cdef extern from "alpha_shapes/alpha_shape_weighted_area.hpp" namespace "busv":
    cdef void alpha_shape_weighted_area[T, U, V](
        AlphaShapeContainer[T,U]& asc,
        const V& weight_func_obj,
        T* areas
    ) nogil

cdef extern from "alpha_shapes/create_alpha_shape.hpp" namespace "busv":
    AlphaShapeContainer[T, U] create_alpha_shape[T, U](U, T, const T*, const T*)

cdef extern from "alpha_shapes/weight_data.hpp" namespace "busv":
    cdef cppclass weight_data[T, U]:
        T weight
        U index

cdef extern from "alpha_shapes/weight_func_obj_default.hpp" namespace "busv":
    cdef cppclass weight_func_obj_default[T, U]:
        weight_func_obj_defalut()

cdef extern from "alpha_shapes/weight_func_obj_min.hpp" namespace "busv":
    cdef cppclass weight_func_obj_min[T, U]:
        weight_func_obj_min(const weight_data[T, U]* wdata, U local_window)

cdef extern from "alpha_shapes/weight_func_obj_mult.hpp" namespace "busv":
    cdef cppclass weight_func_obj_mult[T, U]:
        weight_func_obj_mult(const weight_data[T, U]* wdata, U local_window)

