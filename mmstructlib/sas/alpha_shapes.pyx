import numpy as np
cimport numpy as np
from libc.stdlib cimport malloc, free
from cpython.ref cimport PyObject, Py_INCREF

np.import_array()

cdef extern from "alpha_shapes/alpha_shape_area.hpp" namespace "busv":
    cdef void alpha_shape_area[T, U](
        size_t numPoints, T alpha, 
        const T* points, const T* weights, 
        T* areas
    ) nogil

cdef extern from "alpha_shapes/alpha_shapes.hpp" namespace "busv":
    cdef void alpha_shapes[T,U](
        U numPoints, T alpha, 
        const T* points, const T* weights, 
        U* num_edges, U** edges,
        U* num_triangles, U** triangles,
        U* num_tetrahedra, U** tetrahedra
    ) nogil

#Array wrapper approach taken from http://mail.scipy.org/pipermail/numpy-discussion/2011-December/059579.html
cdef class _MemDeallocator:
    """
    Wrap a C-allocated array to use as a base for a numpy array.  Will deallocate
    array when destroyed
    """
    cdef void* ptr
    def __cinit__(self):
        self.ptr = NULL
    def __dealloc__(self):
        if self.ptr != NULL:
            free(self.ptr)

cdef _MemDeallocator MemDeallocator(np.uint32_t* ptr):
    cdef _MemDeallocator md = _MemDeallocator.__new__(_MemDeallocator)
    md.ptr = ptr
    return md

cdef np.ndarray wrap_pointer(object shape, np.uint32_t* ptr):
    cdef np.npy_intp cshape[2]
    cdef int ndim = len(shape)
    if not 0 < ndim <= 2:
        raise RuntimeError("unsupported number of dimensions")
    for i, v in enumerate(shape):
        cshape[i] = v
    cdef np.ndarray arr = np.PyArray_SimpleNewFromData(ndim, cshape, np.NPY_UINT32, ptr)
    np.set_array_base(arr, MemDeallocator(ptr))
    return arr

cpdef np.ndarray[np.double_t,ndim=1] alpha_shape_areas(np.ndarray[np.double_t,ndim=2] coords, np.ndarray[np.double_t,ndim=1] radii):
    """
    Calculates per-atom surface area given numpy arrays of coordinates and radii.  
    """
    cdef size_t size
    cdef int req_dim = 3

    if not (
        (coords.shape[0] == req_dim and coords.flags.f_contiguous) or 
        (coords.shape[1] == req_dim and coords.flags.c_contiguous)
    ):
        raise RuntimeError("NSC: input coordinates the wrong shape, coordinate data needs to be contiguous values (ie: x1, y1, z1, x2, y2, z2, ...)")

    size = coords.size/req_dim

    if not radii.size == size:
        raise RuntimeError("NSC: coords and radii sizes are inconsistent")

    cdef np.ndarray[np.double_t,ndim=1] area = np.empty(size, np.double)

    #with nogil:
    alpha_shape_area[np.double_t,np.uint32_t](size, 0.0, 
            <double*>coords.data, <double*>radii.data, 
            <double*>area.data
        )

    return area

def area_atoms(object atoms, double probe_radius = 1.4, const char* radius_attr = "radius", const char* out_attr="area"):
    """
    """
    if not atoms:
        raise ValueError("atom_surface_area: atom list is empty")
    cdef np.ndarray coords = np.vstack([a.coordinates.flatten() for a in atoms])
    cdef np.ndarray radii = np.array([getattr(a,radius_attr)+probe_radius for a in atoms])
    cdef np.ndarray[np.double_t,ndim=1] areas = alpha_shape_areas(coords, radii)
    for atom, area in zip(atoms, areas):
        setattr(atom, out_attr, area)

cpdef object alpha_shape(np.ndarray[np.double_t,ndim=2] coords, np.ndarray[np.double_t,ndim=1] radii):
    """
    Calculates per-atom surface area given numpy arrays of coordinates and radii.  
    """
    cdef size_t size
    cdef int req_dim = 3
    cdef np.uint32_t numEdges
    cdef np.uint32_t* edges
    cdef np.uint32_t numTriangles
    cdef np.uint32_t* triangles
    cdef np.uint32_t numTetrahedra
    cdef np.uint32_t* tetrahedra

    if not (
        (coords.shape[0] == req_dim and coords.flags.f_contiguous) or 
        (coords.shape[1] == req_dim and coords.flags.c_contiguous)
    ):
        raise RuntimeError("alpha_shape: input coordinates the wrong shape, coordinate data needs to be contiguous values (ie: x1, y1, z1, x2, y2, z2, ...)")

    size = coords.size/req_dim

    if not radii.size == size:
        raise RuntimeError("alpha_shape: coords and radii sizes are inconsistent")

#    with nogil:
    alpha_shapes[np.double_t, np.uint32_t](
            size, 0.0, 
            <double*>coords.data, <double*>radii.data, 
            &numEdges, &edges,
            &numTriangles, &triangles,
            &numTetrahedra, &tetrahedra
        )

    return {
        "edges" : wrap_pointer([numEdges, 2], edges),
        "triangles" : wrap_pointer([numTriangles, 3], triangles),
        "tetrahedra" : wrap_pointer([numTetrahedra, 4], tetrahedra)
    }

def atom_alpha_shape(object atoms, double probe_radius = 1.4, str radius_attr = "radius", out_attr="area"):
    """
    """
    if not atoms:
        raise ValueError("atom_surface_area: atom list is empty")
    cdef np.ndarray coords = np.vstack([a.coordinates.flatten() for a in atoms])
    cdef np.ndarray radii = np.array([getattr(a,radius_attr)+probe_radius for a in atoms])
    shape = alpha_shape(coords, radii)

    #convert index array to nested lists of atoms
    tetrahedra = [ 
        (atoms[i1], atoms[i2], atoms[i3], atoms[i4]) 
        for i1, i2, i3, i4 in shape["tetrahedra"]
    ]
    triangles = [ 
        (atoms[i1], atoms[i2], atoms[i3]) 
        for i1, i2, i3 in shape["triangles"]
    ]
    edges = [ (atoms[i1], atoms[i2]) for i1, i2 in shape["edges"] ]
        
    return {
        "tetrahedra" : tetrahedra,
        "triangles" : triangles,
        "edges" : edges
    }

