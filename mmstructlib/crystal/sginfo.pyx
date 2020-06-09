from libcpp.string cimport string
from libcpp.vector cimport vector
cdef extern from "sginfo_python/sg_info.hpp":
    int sgToTransforms(
        string spaceGroup,
        vector[double]& mat,
        vector[double]& trans) except +

def sgTransforms(string spaceGroup):
    cdef vector[double] mat
    cdef vector[double] trans
    cdef int num_trans = sgToTransforms(spaceGroup, mat, trans)

    return [
        [
            [mat[j] for j in range(i*9,(i+1)*9)],
            [trans[j] for j in range(i*3,(i+1)*3)]
        ]
        for i in range(num_trans)
    ]
