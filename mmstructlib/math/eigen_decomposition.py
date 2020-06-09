import numpy as np

class EigenModes(object):
    def __init__(self, A):
        self.vec, self.val = np.linalg.eig(A)

        #sort by eigenvalues
        ind = np.argsort(self.val)
        self.vec = self.vec[ind]
        self.val = self.val[:,ind]

        #remove zero values and corresponding vectors
        not_zero_ind = np.logical_not(np.isclose(self.val, 0))
        self.val = self.val[not_zero_ind]
        self.vec = self.vec[:,not_zero_ind]

        #check for negative values
        if np.any(self.val < 0):
            raise ValueError('Matrix has negative eigenvalues')

    @property
    def num_modes(self):
        return self.val.shape[0]

    def mode(self, i):
        return np.outer(self.vec[:,i], self.vec[:,i]) / self.val[i]

    def mode_sum(self, i, j):
        return np.sum([self.mode(k) for k in range(i,j)], axis=0)

    def mode_sum_frac_var(self, frac_var):
        assert 0 <= frac_var <= 1.0
        if not hasattr(self, '_cum_frac_var'):
            inv_val = 1.0 / self.val
            self._cum_frac_var = np.cumsum(inv_val/np.sum(inv_val))
        i = np.searchsorted(self._cum_frac_var, frac_var)
        if self._cum_frac_var[i] != frac_var:
            i += 1
        return i+1, self.mode_sum(0, i+1)

    def all_modes(self):
        return self.mode_sum(0, self.vec.shape[0])
