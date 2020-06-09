import unittest
import numpy as np
from mmstructlib.crystal.lattice import construct_space_group_operators

class TestCrystalLattice(unittest.TestCase):
    def setUp(self):
        pass
    def tearDown(self):
        pass
    def comp_matrix(self, mat1, mat2):
        self.assertTrue((mat1==mat2).all())
    def test_space_group(self):
        ops = construct_space_group_operators(b"P 21 21 21")
        self.assertEqual(len(ops), 4)
        self.comp_matrix(ops[0].transform, np.array([[1.0, 0.0, 0.0],[0.0, 1.0, 0.0],[0.0, 0.0, 1.0]]))
        self.comp_matrix(ops[1].transform, np.array([[-1.0, 0.0, 0.0],[0.0, -1.0, 0.0],[0.0, 0.0, 1.0]]))
        self.comp_matrix(ops[2].transform, np.array([[1.0, 0.0, 0.0],[0.0, -1.0, 0.0],[0.0, 0.0, -1.0]]))
        self.comp_matrix(ops[3].transform, np.array([[-1.0, 0.0, 0.0],[0.0, 1.0, 0.0],[0.0, 0.0, -1.0]]))
        self.comp_matrix(ops[0].translate, np.array([0.0, 0.0, 0.0]))
        self.comp_matrix(ops[1].translate, np.array([0.5, 0.0, 0.5]))
        self.comp_matrix(ops[2].translate, np.array([0.5, 0.5, 0.0]))
        self.comp_matrix(ops[3].translate, np.array([0.0, 0.5, 0.5]))
    def test_bad_space_group(self):
        self.assertRaises(RuntimeError, construct_space_group_operators, b"I AM BAD")
        
