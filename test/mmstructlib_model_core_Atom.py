import unittest
import numpy as np
from mmstructlib.model import Atom
from mmstructlib.util.double_linked_hierarchy import Parent, Child

class MonomerMock(Parent):
    pass

class TestModelAtom(unittest.TestCase):
    def setUp(self):
        #try:
            self.parent = MonomerMock() #mock monomer
            self.atom = Atom("C","UNK",["0.5","2.0","-4.0"],"10.0","1.0")
            self.parent.append(self.atom)
        #except:
        #    self.fail()
    def tearDown(self):
        pass
    def test_delete(self):
        "Deleting atom from parent"
        self.atom.delete()
        self.assertTrue(self.atom not in self.parent)
    def test_child(self):
        "Parent Atom tracking"
        self.assertTrue(self.atom in self.parent)
    def test_parent(self):
        "Atom parent tracking"
        self.assertEqual(self.atom.monomer, self.parent)
    def test_multi_occ(self):
        "Atom is not multi occupancy"
        self.assertFalse(self.atom.is_multi_occ())
    def test_element(self):
        self.assertEqual(self.atom.element, "C")
    def test_name(self):
        self.assertEqual(self.atom.name, "UNK")
    def test_coordinates(self):
        self.assertTrue((self.atom.coordinates == np.array([0.5, 2.0, -4.0])).all())
    def test_bfactor(self):
        self.assertEqual(self.atom.bfactor, 10.0)
    def test_occupancy(self):
        self.assertEqual(self.atom.occupancy, 1.0)

if __name__ == '__main__':
    unittest.main()

