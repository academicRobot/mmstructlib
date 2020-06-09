import numpy as np
cimport numpy as np
from libc.stdlib cimport malloc, free
from cpython.ref cimport PyObject, Py_INCREF
from cpython.mem cimport PyMem_Malloc, PyMem_Free
#from alpha_shapes cimport create_alpha_shape, weight_data, weight_func_obj_default, weight_func_obj_min, weight_func_obj_mult, alpha_shape_weighted_area
cimport alpha_shape_weighted as asw

#shouldn't need to do this
#np.import_array()

np_float_t = np.float64
np_uint_t = np.uint64
ctypedef double float_t
ctypedef size_t int_t

cdef class AlphaShape:
    """
    Use helper function create_alpha_shape to instantiate
    """
    cdef asw.AlphaShapeContainer[float_t, int_t] asc
    cdef int_t num_input_points
    def __cinit__(self, float_t alpha, np.ndarray[float_t,ndim=2] points, np.ndarray[float_t,ndim=1] radii):
        cdef size_t size
        cdef int req_dim = 3
        self.num_input_points = radii.size

        dim = points.shape
        if not (
            (dim[0] == req_dim and points.flags.f_contiguous) or 
            (dim[1] == req_dim and points.flags.c_contiguous)
        ):
            raise RuntimeError("input coordinates the wrong shape, coordinate data needs to be contiguous values (ie: x1, y1, z1, x2, y2, z2, ...)")

        size = points.size/req_dim

        if self.num_input_points != size:
            raise RuntimeError("coords and radii sizes are inconsistent")
            
        self.asc.assign(
            asw.create_alpha_shape[float_t, int_t](
                self.num_input_points, alpha, 
                <float_t*>points.data, 
                <float_t*>radii.data
            )
        )

    def get_max_vertex_index(self):
        """
        Largest vertex index retained in the alphashape
        """
        return self.asc.get_max_vertex_index()

    def get_num_vertices(self):
        """
        Number of vertices may be less than the number of input atoms, due
        to degenerate atoms
        """
        return self.asc.get_num_vertices()

    def get_omegas(self):
        cdef np.ndarray[float_t,ndim=1] omegas = np.ndarray(shape=(self.num_input_points,), dtype=np_float_t, order='C')
        self.asc.get_omegas(&(omegas[0]))
        return omegas

    def get_edge_counts(self):
        cdef np.ndarray[int_t,ndim=1] edge_counts = np.ndarray(shape=(self.num_input_points,), dtype=np_uint_t, order='C')
        self.asc.get_edge_counts(&(edge_counts[0]))
        return edge_counts

def create_alpha_shape(object atoms, float_t alpha=0.0, float_t probe_radius=1.4, str radius_attr="radius"):
    cdef np.ndarray coords = np.vstack([a.coordinates.flatten() for a in atoms])
    cdef np.ndarray radii = np.array([getattr(a,radius_attr)+probe_radius for a in atoms])
    return AlphaShape(alpha, coords, radii)

#weight data
cdef class WeightData:
    cdef int length
    cdef asw.weight_data[float_t, int_t]* wdata
    def __cinit__(self, int_t length):
        self.length = length
        self.wdata = <asw.weight_data[float_t, int_t]*> PyMem_Malloc(length*sizeof(asw.weight_data[float_t, int_t]))
        if not self.wdata:
            raise MemoryError()

    def __dealloc(self):
        PyMem_Free(self.wdata)

    def set_weights(self, weights):
        cdef int i
        if self.length != len(weights):
            raise RuntimeError("Length mismatch {0}, {1}".format(self.length, len(weights)))
        for i in range(self.length):
            self.wdata[i].weight = weights[i]

    def set_indices(self, indices):
        cdef int i
        if self.length != len(indices):
            raise RuntimeError("Length mismatch {0}, {1}".format(self.length, len(indices)))
        for i in range(self.length):
            self.wdata[i].index = indices[i]

def assign_indices(object molecules, int_t chain_inc=100, str index_attr='as_index'):
    cdef int_t i

    i = 0
    for chain in molecules:
        if hasattr(chain, 'seq_res'):
            for res, index in chain.seq_res:
                if index is not None:
                    setattr(chain[index], index_attr, i)
                i += 1
        else:
            for mon in chain.monomers:
                setattr(mon, index_attr, i)
        i += chain_inc

def create_weight_data(object atoms, str index_attr='as_index', str weight_attr='as_weight'):
    cdef int_t length

    length = len(atoms)
    weight_data = WeightData(length)
    weight_data.set_indices([getattr(a.monomer, index_attr) for a in atoms])
    weight_data.set_weights([getattr(a.monomer, weight_attr) for a in atoms])

    return weight_data

#calculate areas
def alpha_shape_area_weighted(AlphaShape a_s, WeightData w_d, int_t window_size, method=None):
    """
    method = min|mult|default
    """
    cdef int length = a_s.num_input_points
    cdef np.ndarray[float_t,ndim=1] areas = np.ndarray(shape=(length,), dtype=np_float_t, order='C')

    #need to change weight function object -> virtual functions?
    #cdef weight_func_obj_min[float_t, int_t] wfo_min(w_d, window_size)
    #cdef weight_func_obj_mult[float_t, int_t] wfo_mult(w_d, window_size)
    cdef asw.weight_func_obj_default[float_t, int_t] wfo_def

    if length != w_d.length:
        raise ValueError("Length mismatch between alpha shape and weight data; {0}, {1}".format(length, w_d.length))
    if method=="min":
        #alpha_shape_weighted_area(a_s.asc, wfo_min, areas.data)
        asw.alpha_shape_weighted_area[float_t, int_t, asw.weight_func_obj_min[float_t, int_t] ](a_s.asc, asw.weight_func_obj_min[float_t, int_t](w_d.wdata, window_size), <float_t*>areas.data)
    elif method=="mult":
        #alpha_shape_weighted_area(a_s.asc, wfo_mult, areas.data)
        asw.alpha_shape_weighted_area[float_t, int_t, asw.weight_func_obj_mult[float_t, int_t] ](a_s.asc, asw.weight_func_obj_mult[float_t, int_t](w_d.wdata, window_size), <float_t*>areas.data)
    elif method=="default":
        #alpha_shape_weighted_area(a_s.asc, wfo_def, areas.data)
        asw.alpha_shape_weighted_area[float_t, int_t, asw.weight_func_obj_default[float_t, int_t] ](a_s.asc, wfo_def, <float_t*>areas.data)
    else:
        raise ValueError("method must be one of 'min', 'mult', or 'default'")
    return areas

