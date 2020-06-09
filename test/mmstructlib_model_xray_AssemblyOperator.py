import unittest
import numpy as np
from mmstructlib.model.xray import AssemblyOperator

class TestModelXrayAssemblyOperator(unittest.TestCase):
    def setUp(self):
        try:
            self.ao = AssemblyOperator(
                '1', 
                'identity operation',
                '1_555',
                'x,y,z',
                ['0.0000000000', '0.0000000000', '0.0000000000'],
                ['1.0000000000', '0.0000000000', '0.0000000000', '0.0000000000', '1.0000000000', '0.0000000000', '0.0000000000', '0.0000000000', '1.0000000000']
            )
        except:
            self.fail()
    def tearDown(self):
        pass
    def test_id(self):
        self.assertEqual(self.ao.id, '1')
    def test_type(self):
        self.assertEqual(self.ao.type, 'identity operation')
    def test_name(self):
        self.assertEqual(self.ao.name, '1_555')
    def test_symmetry(self):
        self.assertEqual(self.ao.symmetry, 'x,y,z')
    def test_dimensions(self):
        self.assertTrue((self.ao.translate == np.array([0.0, 0.0, 0.0])).all())
    def test_angles(self):
        self.assertTrue((self.ao.transform == np.array([[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]])).all())

if __name__ == '__main__':
    unittest.main()

