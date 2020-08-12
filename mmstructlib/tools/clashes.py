from mmstructlib.tools.gridhash import GridHash
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
    gridhash = GridHash(atoms, max_radii*2 - min_clash)

    ret = (
        (clash_val(a1, a2, radii_attr), (a1, a2))
        for a1, a2 in gridhash.all_neighbors_iter()
    )
    return filter(lambda x: x[0] >= min_clash, ret)
