import numpy as np

class Operator(object):
    """
    """
    def __init__(self, translate=[0.0,0.0,0.0], transform=[1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0]):
        transform = np.array(transform, dtype=float)
        assert len(translate) == 3, "AssemblyOperator must have three elements for translate"
        assert transform.size == 9, "AssemblyOperator must have nine elements for translate"
        self.translate = np.array(translate, dtype=float).T
        self.transform = transform.reshape((3,3))

    def apply(self, mat, scratch=None):
        """
        Apply opperation to mat inplace and return result.

        Arguments:
            mat(np.array): nx3 numpy array
            scratch(np.array): nx3 numpy array, temporary used if not provided
        """
        return np.add(
            np.dot(self.transform, mat, scratch),
            self.translate,
            mat
        )

    def transform(self, other):
        """
        Apply a operation to this operation
        """
        self.transform = np.dot(other.transform, self.transform)
        self.translate = np.dot(other.transform, self.translate) + other.translate

