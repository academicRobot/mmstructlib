import unittest
import numpy as np
from mmstructlib.model.xray import UnitCell

class TestModelXrayUnitCell(unittest.TestCase):
    def setUp(self):
        try:
            self.unitcell = UnitCell(["69.146", "69.146", "216.772"], ["90.00", "90.00", "120.00"], "P 61 2 2")
        except:
            self.fail()
    def tearDown(self):
        pass
    def test_dimensions(self):
        self.assertTrue((self.unitcell.dimensions == np.array([69.146, 69.146, 216.772])).all())
    def test_angles(self):
        self.assertTrue((self.unitcell.angles == np.radians(np.array([90.00, 90.00, 120.00]))).all())
    def test_space_group(self):
        self.assertEqual(self.unitcell.space_group, 'P 61 2 2')

if __name__ == '__main__':
    unittest.main()

