import unittest
import numpy as np
from mmstructlib.model import MultiOccAtom
from mmstructlib.util.double_linked_hierarchy import Parent

class MonomerMock(Parent):
    pass

atom_vals = [
    [[0.5,2.0,-4.0],10.0,0.2,"A"],
    [[5.0,-1.0,4.0],1.0,0.7,"B"],  #max occ
    [[2.5,0.0,-2.0],0.1,0.1,"C"]]

class TestModelMultiOccAtom(unittest.TestCase):
    def setUp(self):
        try:
            self.parent = MonomerMock() #mock monomer
            self.atom = MultiOccAtom("C","UNK")
            self.parent.append(self.atom)
            for v in atom_vals:
                self.atom.add_atom(*v)
        except:
            self.fail()
    def tearDown(self):
        pass
    def test_delete(self):
        "Deleting MultiOccAtom from parent"
        self.atom.delete()
        self.assertTrue(self.atom not in self.parent)
    def test_child(self):
        "Parent MultiOccAtom tracking"
        self.assertTrue(self.atom in self.parent)
    def test_parent(self):
        "Atom parent tracking"
        self.assertEqual(self.atom.monomer, self.parent)
    def test_multi_occ(self):
        "MultiOccAtom is not multi occupancy"
        self.assertTrue(self.atom.is_multi_occ())
    def test_element(self):
        self.assertEqual(self.atom.element, "C")
    def test_name(self):
        self.assertEqual(self.atom.name, "UNK")
    def test_coordinates(self):
        self.assertTrue((self.atom.coordinates == np.array(atom_vals[1][0])).all())
    def test_bfactor(self):
        self.assertEqual(self.atom.bfactor, atom_vals[1][1])
    def test_occupancy(self):
        self.assertEqual(self.atom.occupancy, atom_vals[1][2])
    def test_add_atom(self):
        self.atom.add_atom([0.0,0.0,0.0],100.0,1.0,"D")
        self.assertEqual(len(self.atom), 4)
        self.assertTrue((self.atom.coordinates == np.array([0.0,0.0,0.0])).all())
        self.assertEqual(self.atom.bfactor, 100.0)
        self.assertEqual(self.atom.occupancy, 1.0)

if __name__ == '__main__':
    unittest.main()

