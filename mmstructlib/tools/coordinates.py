import numpy as np

def atoms_to_atom_mat(atoms):
   return np.vstack([atom.coordinates.flatten() for atom in atoms])

def mean_position(atoms):
   return mean_position_m(atoms_to_atom_mat(atoms))

def mean_position_m(atom_mat):
    return np.mean(atom_mat, axis=0)

def radius_of_gyration(atoms, mass_attr=None):
    return radius_of_gyration(
        atoms_to_atom_mat(atoms), 
        [getattr(a, mass_attr) for a in atoms] if mass_attr else None
    )

def radius_of_gyration_m(atom_mat, masses):
    if not masses:
        masses = np.ones(atom_mat.shape[0])
    assert atom_mat.shape[0] == masses.shape[0]
    mean_pos = mean_position_m(atom_mat)
    return np.sqrt(np.sum( masses * np.sum(np.square(atom_mat - mean_pos), axis=1) ) / np.sum( masses ))

