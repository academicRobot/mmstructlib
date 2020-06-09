import numpy as np

def atoms(As, op):
    scratch = np.empty(3,dtype=np.float)
    for a in As:
        op.apply(a.coordinates, scratch)


def atoms_2(As, op):
    coord_mat = np.vstack(
        [a.coordinates for a in As]
    )
    op.apply(coord_mat)
    for i, a in enumerate(As):
        a.coordinates[:] = coord_mat[...,i]

def atom_container(ac, op):
    atoms(ac.atoms, op)
