from mmstructlib.tools.gridhash import GridHash
import numpy as np
from scipy.spatial import distance


def clash_val(atom1, atom2, radii_attr='radius'):
    return getattr(atom1, radii_attr) + getattr(atom2, radii_attr) \
        - distance.euclidean(atom1.coordinates, atom2.coordinates)


def clash_iter(atoms, min_clash=0.4, radii_attr='radius'):
    """
    Returns an iterator over pairs:
        -clash value
        -the pair of clashing atoms
    min_clash.
    """
    max_radii = max([getattr(a, radii_attr) for a in atoms])
    gridhash = GridHash([(a.coordinates, a) for a in atoms], max_radii*2 - min_clash)

    ret = (
        (clash_val(a1, a2, radii_attr), (a1, a2))
        for (c1, a1), (c2, a2) in gridhash.all_neighbors_iter()
    )
    return filter(lambda x: x[0] >= min_clash, ret)

def clash_report(atoms, min_clash, radii_attr='radius'):
    num_atoms = len(atoms)
    max_atom_cv = {a : 0 for a in atoms}
    num_clash = 0
    for cv, (a1, a2) in clash_iter(atoms, min_clash, radii_attr):
        max_atom_cv[a1] = max(cv, max_atom_cv[a1])
        max_atom_cv[a2] = max(cv, max_atom_cv[a2])
        num_clash += 1

    max_atom_cv_v = list(max_atom_cv.values())
    return {
        "clashes_per_atom" : num_clash / num_atoms,
        "median_max_clash_value" : np.median(max_atom_cv_v),
        "p90_max_clash_value": np.percentile(max_atom_cv_v, 90),
        "p95_max_clash_value": np.percentile(max_atom_cv_v, 95),
        "p99_max_clash_value": np.percentile(max_atom_cv_v, 99)
    }

if __name__ == "__main__":
    import sys
    from mmstructlib.IO import load_cif_from_mirror
    from mmstructlib.radii import add_radii
    from mmstructlib.tools.preprocess import strip_hydrogen, strip_water
    if len(sys.argv) != 2:
        exit("usage: {} <struct id>".format(sys.argv[0]))

    struct = load_cif_from_mirror(sys.argv[1])
    strip_water(struct)
    strip_hydrogen(struct)
    add_radii(struct[0])
    res = clash_report(struct[0].atoms, 2.5)
    for field, value in res.items():
        print("{}: {}".format(field, value))