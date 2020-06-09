import numpy as np
from scipy.spatial.distance import pdist, squareform
from mmstructlib.math.eigen_decomposition import EigenModes

def kirchhoff(coord_mat, cutoff, gamma=1.0):
    n, k = coord_mat.shape
    dist_vec = pdist(coord_mat, 'euclidean')
    contact_vec = -gamma * np.array(np.less_equal(dist_vec, cutoff), dtype=np.float)
    mat = squareform(contact_vec)
    mat[np.diag_indices(n)] = -1.0 * np.sum(mat, axis=1)
    return mat

def kirchhoff_pf(coord_mat):
    """
    Yang et al (2009) PNAS 106: 12347-52

    Note: need to multiply inverse distance kirchhoff by -1 to have fluctuation
    signs come out right.  Typo?
    """
    n, k = coord_mat.shape
    vec = -1.0 / np.square(pdist(coord_mat, 'euclidean'))
    mat = squareform(vec)
    mat[np.diag_indices(n)] = -1.0 * np.sum(mat, axis=1)
    return mat

def mean_sq_fluct(coords, cutoff = 7.0, gamma = 1.0):
    mat = kirchhoff(ca_coords, cutoff, gamma)
#    return np.diag(moore_penrose_inverse(mat))
    return np.diag(EigenModes(mat).all_modes())

def mean_sq_fluct_pf(coords):
    mat = kirchhoff_pf(ca_coords)
#    return np.diag(moore_penrose_inverse(mat))
    return np.diag(EigenModes(mat).all_modes())

if __name__ == '__main__':
    import sys
    from mmstructlib.IO import load_cif_from_mirror
    from mmstructlib.tools.rmsd import nmr_struct_pos_rmsd
    from scipy.stats import pearsonr, spearmanr
    mirror = '/var/data/pdb'
    if len(sys.argv) != 3:
        print("usage: {0} pdb_id chain_id".format(sys.argv[0]))
        sys.exit(1)

    struct=load_cif_from_mirror(sys.argv[1], mirror)
    chain=struct[0].polymer_by_pdb_id(sys.argv[2])
    ca_atoms = [a for a in chain.atoms if a.name=="CA"]
    ca_coords = np.vstack([a.coordinates for a in ca_atoms])
    fluct = mean_sq_fluct(ca_coords)
    fluct_pf = mean_sq_fluct_pf(ca_coords)
    if struct.is_xray():
        var_str = 'bfactor'
        var = [a.bfactor for a in ca_atoms]
    elif struct.is_nmr():
        if len(struct) <= 2:
            print("At least three models required for NMR structure")
            sys.exit(1)
        var_str = 'rmsd'
        nmr_struct_pos_rmsd(struct)
        var = [a.monomer.rmsd for a in ca_atoms]
    else:
        print("Unsupported structure type")
        sys.exit(1)

    print("id\tres\t{0}\tfluct\tfluct_pf".format(var_str))
    for a, v, f, pf in zip(ca_atoms, var, fluct, fluct_pf):
        print("{0}\t{1}\t{2:.2f}\t{3:.2f}\t{4:.2f}".format(a.monomer.id, a.monomer.type, v, f, pf))

    print("parametric correlations:")
    print("\tGNM: {0:.3} ({1:.3e})".format(*pearsonr(var, fluct)))
    print("\tpfGNM: {0:.3} ({1:.3e})".format(*pearsonr(var, fluct_pf)))
    print("nonparametric correlations:")
    print("\tGNM: {0:.3} ({1:.3e})".format(*spearmanr(var, fluct)))
    print("\tpfGNM: {0:.3} ({1:.3e})".format(*spearmanr(var, fluct_pf)))


