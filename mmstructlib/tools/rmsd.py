import numpy as np
import sys
if sys.version_info[0] >= 3:
    from itertools import zip_longest
else:
    from itertools import izip_longest as zip_longest

def pos_rmsd(atom_lst):
    """
    Calculates the RMSD of a list of atoms.
    """
    coords = np.vstack([a.coordinates.flatten() for a in atom_lst])
    return np.sqrt(np.sum(np.power(coords - np.mean(coords, axis=0),2))/len(atom_lst))

def struct_rmsd(atom_lst1, atom_lst2):
    """
    Calculates the RMSD between two atom lists
    """
    if len(atom_lst1) != len(atom_lst2):
        raise ValueError("Atom arrays not the same length")
    if not atom_lst1:
        raise ValueError("Empty atom lists")
    sqrsum = lambda x: np.dot(x, x)
    return np.sqrt(sum([
        sqrsum(a1.coordinates - a2.coordinates)
            for a1, a2 in zip(atom_lst1, atom_lst2)
    ]) / len(atom_lst1))

def nmr_struct_pos_rmsd(structure, out_attr = 'rmsd'):
    """
    Calculates the C-alpha rmsd of a set of models, usually from a multimodel
    NMR structure.  Requires that each model has exactly the same protein
    components.  Annotates the monomers of the first model with attr.
    """
    positions = [
        [ mon for mon in mod.monomers if mon.is_amino_acid() and mon.atom_by_name('CA') is not None ]
        for mod in structure
    ]
    if not all([len(positions[0]) == len(pos) for pos in positions[1:]]):
        raise ValueError("Models have different number of amino acids")
    positions = list(zip(*positions))

    #verify that models match
    for pos_monomers in positions:
        for mon in pos_monomers[1:]:
            if not pos_monomers[0].id == mon.id:
                raise ValueError("Inconsistent model monomer position {0} {1}".format(pos_monomers[0].id, mon.id))

    #calc and store rmsd
    for pos_monomers in positions:
        setattr(
            pos_monomers[0],
            out_attr,
            pos_rmsd([m.atom_by_name('CA') for m in pos_monomers])
        )

