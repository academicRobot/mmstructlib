# cython: c_string_type=unicode, c_string_encoding=ascii

import numpy as np
cimport numpy as np
from libc.stdlib cimport free
from cpython.ref cimport PyObject, Py_INCREF

cdef extern from "nsc.h":
    cdef int _FLAG_DOTS "FLAG_DOTS"
    cdef int _FLAG_VOLUME "FLAG_VOLUME"
    cdef int _FLAG_ATOM_AREA "FLAG_ATOM_AREA"
    cdef int NSC(
        double* coord, double* radii, int num_atoms, int num_dots, int mode_flag,
        double* total_area, double** atom_area, double* total_volume,
        double** surface_dots, int* num_dots
    ) nogil
    cdef int nsc_dclm_weighted (
        double *co, double *radius, int nat, int densit,
        unsigned int* at_res_p, double* res_p,
        unsigned int* at_res_index, unsigned int half_window,
        double *atom_area
    ) nogil

cdef extern from "cassert.h":
    cdef void CASSERT(int predicate, const char* f)

FLAG_DOTS = _FLAG_DOTS
FLAG_VOLUME = _FLAG_VOLUME
FLAG_ATOM_AREA = _FLAG_ATOM_AREA

np.import_array()

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
cdef _MemDeallocator MemDeallocator(double* ptr):
    cdef _MemDeallocator md = _MemDeallocator.__new__(_MemDeallocator)
    md.ptr = ptr
    return md

cdef np.ndarray wrap_pointer(object shape, double* ptr):
    cdef np.npy_intp cshape[2]
    cdef int ndim = len(shape)
    if not 0 < ndim <= 2:
        raise RuntimeError("unsupported number of dimensions")
    for i, v in enumerate(shape):
        cshape[i] = v
    cdef np.ndarray arr = np.PyArray_SimpleNewFromData(ndim, cshape, np.NPY_DOUBLE, ptr)
    np.set_array_base(arr, MemDeallocator(ptr))
    return arr

cpdef object nsc(np.ndarray[np.double_t,ndim=2] coords, np.ndarray[np.double_t,ndim=1] radii, int num_dots, int flags):
    """
    Calculates total surface area, per-atom surface area, surface dots, and/or total
    volume, given numpy arrays of coordinates and radii.  Accuracy increases with
    increasing num_dots, num_dots=1000 gives reliable calculations.  Calculations
    performed depend on flags given, which can be 0 or any bit-wise combination of
    FLAG_ATOM_AREA, FLAG_DOTS, and FLAG_VOLUME.

    Return value is a list: [total_area, areas, dots, total_volume], where
    total_area and total_volume are floats, areas is a numpy array, and dots is
    a 2D numpy array of dot coordinates.  The total_area value is always returned,
    any other may be None, depending on given flags.
    """
    cdef double total_area
    cdef double* areas
    cdef double** areas_ptr = &areas if flags & _FLAG_ATOM_AREA else NULL
    cdef double* points
    cdef double** points_ptr = &points if flags & _FLAG_DOTS else NULL
    cdef int num_points
    cdef int* num_points_ptr = &num_points if flags & _FLAG_DOTS else NULL
    cdef double total_volume
    cdef double* total_volume_ptr = &total_volume if flags & _FLAG_VOLUME else NULL
    cdef int ret_val
    cdef size_t size
    cdef size_t req_dim = 3

    if not (
        (coords.shape[0] == req_dim and coords.flags.f_contiguous) or
        (coords.shape[1] == req_dim and coords.flags.c_contiguous)
    ):
        raise ValueError("NSC: input coordinates the wrong shape, coordinate data needs to be contiguous values (ie: x1, y1, z1, x2, y2, z2, ...)")

    size = coords.size/req_dim

    if not radii.size == size:
        raise ValueError("NSC: coords and radii sizes are inconsistent")

    with nogil:
        ret_val = NSC(<double*>coords.data, <double*>radii.data, size, num_dots, flags, &total_area, areas_ptr, total_volume_ptr, points_ptr, num_points_ptr)

    if ret_val != 0:
        raise RuntimeError("NSC algorithm failed")

    return [
        total_area,
        wrap_pointer([size], areas) if areas_ptr != NULL else None,
        wrap_pointer([num_points,3], points) if points_ptr != NULL else None,
        total_volume if total_volume_ptr != NULL else None
    ]

cpdef object nsc_atoms(object atoms, int num_dots, int flags, double probe_radius = 1.4, const char* radius_attr = "radius"):
    """
    """
    if not atoms:
        raise ValueError("nsc_atoms: atom list is empty")
    cdef np.ndarray[np.double_t,ndim=2] coords = np.vstack([a.coordinates.flatten() for a in atoms])
    cdef np.ndarray[np.double_t,ndim=1] radii = np.array([getattr(a,radius_attr)+probe_radius for a in atoms])
    return nsc(coords, radii, num_dots, flags)

def area_atoms(object atoms, int num_dots = 1002, double probe_radius = 1.4, const char* radius_attr="radius", out_attr="area"):
    areas = nsc_atoms(atoms, num_dots, _FLAG_ATOM_AREA, probe_radius, radius_attr)[1]
    for at, ar in zip(atoms, areas):
        setattr(at, out_attr, ar)

def total_area_atoms(object atoms, int num_dots = 1002, double probe_radius = 1.4, const char* radius_attr = "radius"):
    return nsc_atoms(atoms, num_dots, 0, probe_radius, radius_attr)[0]

def dots_atoms(object atoms, int num_dots = 1002, double probe_radius = 1.4, const char* radius_attr = "radius"):
    return nsc_atoms(atoms, num_dots, _FLAG_DOTS, probe_radius, radius_attr)[2]

def volume_atoms(object atoms, int num_dots = 1002, double probe_radius = 1.4, const char* radius_attr = "radius"):
    return nsc_atoms(atoms, num_dots, _FLAG_VOLUME, probe_radius, radius_attr)[3]

#ensure that the numpy datatypes match up with the c datatypes
#CASSERT(sizof(

def atoms_area_weighted(
	object model, 
	unsigned int half_window, 
	int num_dots=362, 
	double probe_radius = 1.4, 
	object mon_indices = None,
	const char* radius_attr = "radius", 
	const char* weight_attr = "weight", 
	const char* out_attr = "area"
):
    """
    Calculates per-atom, weighted surface area for atoms in the given mmstructlib model.

    Weights should annotate monomer instances (via weight_attr) and express the fraction
    of monomer unfolded in the model ensemble, where 1 is completely unfolded and 0 is
    completely folded.

    The half_window parametr specifies the local window size (window = half_window*2+1).
    Resdiues with indices within half_window of each other in sequence will ignore
    weight when calculating area (local window always buries surface area).
    
    mon_indices is an optional list of integers that should be the same length as the 
    number of monomers in the model.  By default, this will be set so that monomers in
    the same molecule will have locality set based on there position in the monomer list,
    and monomers in different molecules will never be local.  This argument allows custom
    locality criteria to be set.

    Radii are given per atom object (via radius_attr).

    Output annotates atom objects (via out_attr).
    """
    cdef int ret_val
    cdef size_t size
    cdef unsigned int mol_inc = half_window*2;

    atoms = [a for mol in model.molecules for mon in mol.monomers for a in mon.atoms]
    size = len(atoms)

    cdef np.ndarray[np.double_t,ndim=2] coords = np.array(np.vstack(
        [a.coordinates.flatten() for a in atoms]),
        dtype=np.float64
    )
    cdef np.ndarray[np.double_t,ndim=1] radii = np.array(
        [getattr(a,radius_attr)+probe_radius for a in atoms],
        dtype=np.float64
    )
    cdef np.ndarray[np.uint32_t,ndim=1] atom_res_probs_index = np.array(
        [i for i, mon in enumerate([mon for mol in model.molecules for mon in mol.monomers]) for a in mon],
        dtype=np.uint32
    )
    cdef np.ndarray[np.double_t,ndim=1] res_probs = np.array(
        [getattr(mon,weight_attr) for mol in model.molecules for mon in mol.monomers],
        dtype=np.float64
    )
    index = 0
    indices = []
    if mon_indices is None:
        for mol in model.molecules:
            for mon in mol.monomers:
                indices.extend([index] * len(mon.atoms))
                index += 1
            index += mol_inc
    else:
        for mon, mon_index in zip([mon for mol in model.molecules for mon in mol.monomers], mon_indices):
            indices.extend([mon_index] * len(mon.atoms))

    cdef np.ndarray[np.uint32_t,ndim=1] atom_res_index = np.array(indices, dtype=np.uint32)
    cdef np.ndarray[np.double_t,ndim=1] areas = np.empty(size, dtype=np.float64)

    with nogil:
        ret_val = nsc_dclm_weighted (
            <double*>coords.data, <double*>radii.data, size, num_dots,
            <unsigned int*>atom_res_probs_index.data, <double*>res_probs.data,
            <unsigned int*>atom_res_index.data, half_window,
            <double*>areas.data
        )

    if ret_val != 0:
        raise RuntimeError("NSC weighted algorithm failed")

    for i in range(size):
        setattr(atoms[i], out_attr, areas[i])


