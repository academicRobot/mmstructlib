import numpy as np
from scipy.spatial.distance import pdist, squareform
from mmstructlib.elastic_network.gnm import moore_penrose_inverse
from mmstructlib.math.eigen_decomposition import EigenModes

from prody.proteins.pdbfile import parsePDBStream
from prody import ANM
from pyStructureTools import PdbFile
def calc_hessian(pdb_id, chain_ids):
    pdb_file = PdbFile(pdb_id)
    struct = parsePDBStream(open(str(pdb_file)), chain=chain_ids, subset='ca', model=None)
    anm = ANM()
    anm.buildHessian(struct, cutoff=15.0)
    return anm.getHessian()

def _hessian(coord_mat, cutoff, power):
    n, k = coord_mat.shape
    nx3 = n*3

    dist_vec = 1.0/np.power(pdist(coord_mat,'euclidean'),power)
    if cutoff: #mask distances greater than cutoff
        dist_vec = np.array(np.greater_equal(dist_vec, 1.0/np.power(cutoff,power)), dtype=np.float) * dist_vec
    dist_mat = squareform(dist_vec)
#    print(cutoff, 1/dist_mat[2,0])

    #ninja broadcasting ftw
    m = np.ones((n, 3, n)) * coord_mat.swapaxes(0,1)
    mt = m.swapaxes(1,2).swapaxes(0,1).reshape(n,nx3)
    mz = np.ones((3, n, nx3)) * mt
    m_j = np.transpose(mz.swapaxes(0,1).reshape(nx3,nx3)).reshape(n,3,n,3).swapaxes(1,2)
    m_i = m_j.swapaxes(0,1) #transpose outer matrix
    m_diff = m_i - m_j
    #m_diff = m_j - m_i
    hessian = np.multiply(m_diff, m_diff.swapaxes(2,3))
    hessian = (hessian.swapaxes(0,2).swapaxes(1,3) * -dist_mat).swapaxes(1,3).swapaxes(0,2)
    hessian[np.diag_indices(n)] = -1.0 * np.sum(hessian, axis=0)

    return hessian

def hessian(coord_mat, cutoff):
    return _hessian(coord_mat, cutoff, 2.0)

def hessian_pf(coord_mat):
    """
    Yang et al (2009) PNAS 106: 12347-52
    Note: the description here is terse, and a little confusing.  They say weight
    each super element by the inverse square of the distance, but the hessian already
    contains this term, so total weighting is by rij^-4, according to that
    description.
    """
    return _hessian(coord_mat, None, 4.0)

def direct_inverse(hessian):
    n = hessian.shape[0]
    nx3 = 3*n
    return np.linalg.inv(hessian.swapaxes(1,2).reshape(nx3,nx3)).reshape(n,3,n,3).swapaxes(1,2)

def pseudo_inverse(hessian):
    n = hessian.shape[0]
    nx3 = 3*n
    return EigenModes(
        hessian.swapaxes(1,2).reshape(nx3,nx3),6
    ).all_modes().reshape(n,3,n,3).swapaxes(1,2)

def super_element_trace(inv_hessian):
    size = inv_hessian.shape[0]
#    print(inv_hessian[0,0])
#    sys.exit(0)
    return [np.trace(inv_hessian[i,i]) for i in range(size)]

def scale_mean1(lst):
    s = sum(lst)
    print(s)
    sys.exit(1)
    return [i/s for i in lst]

def mean_sq_fluct(coords, cutoff = 13.0):
    mat = hessian(ca_coords, cutoff)
#    inv_mat = direct_inverse(mat)
    inv_mat = pseudo_inverse(mat)
#    return scale_mean1(super_element_trace(inv_mat))
    return super_element_trace(inv_mat)

def mean_sq_fluct_pf(coords):
    mat = hessian_pf(ca_coords)
#    inv_mat = direct_inverse(mat)
    inv_mat = pseudo_inverse(mat)
#    return scale_mean1(super_element_trace(inv_mat))
    return super_element_trace(inv_mat)

if __name__ == '__main__':
    import sys
    from mmstructlib.IO import load_cif_from_mirror
    from mmstructlib.tools.rmsd import nmr_struct_pos_rmsd
    from scipy.stats import pearsonr, spearmanr
    mirror = '/var/data/pdb'
    if len(sys.argv) != 3:
        print("usage: {0} pdb_id chain_id".format(sys.argv[0]))
        sys.exit(1)

#    H = calc_hessian(sys.argv[1], sys.argv[2])
#    np.linalg.inv(H)
#    print(H[0:3,0:3])
#    print(H[0:3,6:9])

    struct=load_cif_from_mirror(sys.argv[1], mirror)
    chain=struct[0].polymer_by_pdb_id(sys.argv[2])
    ca_atoms = [a for a in chain.atoms if a.name=="CA"]
    ca_coords = np.vstack([a.coordinates for a in ca_atoms])

#    n = len(ca_atoms)
#    H = hessian(ca_coords, 15.0).swapaxes(1,2).reshape(3*n, 3*n)
#    print(H[0:3,0:3])
#    print(H[0:3,6:9])
#    np.linalg.inv(H)
#    sys.exit(0)

    fluct = mean_sq_fluct(ca_coords)
    fluct_pf = mean_sq_fluct_pf(ca_coords)
    if struct.is_xray():
        var_str = 'bfactor'
        var = [a.bfactor for a in ca_atoms]
    if struct.is_nmr():
        if len(struct) <= 2:
            print("At least three models required for NMR structure")
            sys.exit(1)
        var_str = 'rmsd'
        nmr_struct_pos_rmsd(struct)
        var = [a.monomer.rmsd for a in ca_atoms]

    print("id\tres\t{0}\tfluct\tfluct_pf".format(var_str))
    for a, v, f, pf in zip(ca_atoms, var, fluct, fluct_pf):
        print("{0}\t{1}\t{2:.2f}\t{3:.2f}\t{4:.2f}".format(a.monomer.id, a.monomer.type, v, f, pf))

    print("parametric correlations:")
    print("\tANM: {0:.3} ({1:.3e})".format(*pearsonr(var, fluct)))
    print("\tpfANM: {0:.3} ({1:.3e})".format(*pearsonr(var, fluct_pf)))
    print("nonparametric correlations:")
    print("\tANM: {0:.3} ({1:.3e})".format(*spearmanr(var, fluct)))
    print("\tpfANM: {0:.3} ({1:.3e})".format(*spearmanr(var, fluct_pf)))


