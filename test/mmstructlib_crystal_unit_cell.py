import unittest
import numpy as np
from mmstructlib.crystal.unit_cell import to_unit_cell, to_cartesian

points = [(1.0,1.0,1.0),(1.0,0.0,2.0)]

class TestCrystalUnitCell(unittest.TestCase):
    def setUp(self):
        pass
    def tearDown(self):
        pass
    def test_simple_cell(self):
        simple_cell = ((1.0,1.0,1.0),np.radians((90.0,90.0,90.0)))
        uc_transform = to_unit_cell(*simple_cell)
        cart_transform = to_cartesian(*simple_cell)
        for point in points:
            t_p = np.dot(uc_transform, np.array(point))
            t_p += np.array([1.0, 1.0, 1.0])
            tt_p = np.dot(cart_transform, t_p)
            self.assertAlmostEqual(tt_p[0], point[0]+1.0)
            self.assertAlmostEqual(tt_p[1], point[1]+1.0)
            self.assertAlmostEqual(tt_p[2], point[2]+1.0)
    def test_weird_cell(self):
        simple_cell = ((2.0,2.0,1.0), np.radians((70.528779365509308,70.528779365509308,90.0)))
        uc_transform = to_unit_cell(*simple_cell)
        cart_transform = to_cartesian(*simple_cell)
        for point in points:
            t_p = np.dot(uc_transform, np.array(point))
            t_p += np.array([1.0, 1.0, 1.0])
            tt_p = np.dot(cart_transform, t_p)
            self.assertAlmostEqual(tt_p[0], point[0]+2.0+1/3.0)
            self.assertAlmostEqual(tt_p[1], point[1]+2.0+1/3.0)
            self.assertAlmostEqual(tt_p[2], point[2]+np.sqrt(7/9.0))

