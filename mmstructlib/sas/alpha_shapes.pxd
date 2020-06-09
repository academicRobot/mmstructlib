import numpy as np
cimport numpy as np
from libc.stdlib cimport malloc, free
from cpython.ref cimport PyObject, Py_INCREF

#low level functions

#cdef extern from "alpha_shapes/process_cells.hpp" namespace "busv":
#    cdef void process_cells[T, U, V](AlphaShapeContainer[T, U]&, const V&)

#cdef extern from "alpha_shapes/evaluate_facet_area.hpp" namespace "busv":
#    cdef void evaluate_facet_area[T, U, V](AlphaShapeContainer[T, U]&, const V&, T*)

#cdef extern from "alpha_shapes/evaluate_edge_area.hpp" namespace "busv":
#    cdef void evaluate_edge_area[T, U, V](AlphaShapeContainer[T, U]&, const V&, T*)

#cdef extern from "alpha_shapes/evaluate_vertex_area.hpp" namespace "busv":
#    cdef void evaluate_vertex_area[T, U](AlphaShapeContainer[T, U]&, T*)

#high level functions

