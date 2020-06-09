import unittest
import mmstructlib.sas.nsc as nsc
import numpy as np

def min_dot_offset(dots, positions, radii):
    return [
        min([
            np.abs(np.sqrt(
                sum([x**2 for x in dot.flatten() - pos.flatten()])
            )) - rad 
            for pos, rad in zip(positions, radii)
        ]) 
        for dot in dots
    ]

class AtomMock:
    def __init__(self, pos, radius):
        self.coordinates = np.array(pos)
        self.radius = radius
    def __repr__(self):
        return "Atom({0},{1})".format(str(list(self.coordinates)),self.radius)

class TestNSC(unittest.TestCase):
    "Surface area calucalations"
    def setUp(self):
        pass
    def tearDown(self):
        pass
    def test_simple_total_area(self):
        points = np.array([[0.0, 0.0, 0.0], [3.0, 0.0, 0.0], [0.0, 3.0, 0.0], [0.0, 0.0, 3.0]])
        radii = np.array([1.0, 1.0, 1.0, 1.0])
        result = nsc.nsc(points, radii, 1000, 0)
        self.assertEqual(sum([4*np.pi*r**2 for r in radii]), result[0])
        self.assertEqual(None, result[1])
        self.assertEqual(None, result[2])
        self.assertEqual(None, result[3])
    def test_simple_area(self):
        points = np.array([[0.0, 0.0, 0.0], [3.0, 0.0, 0.0], [0.0, 3.0, 0.0], [0.0, 0.0, 3.0]])
        radii = np.array([1.0, 1.0, 1.0, 1.0])
        result = nsc.nsc(points, radii, 1000, nsc.FLAG_ATOM_AREA)
        self.assertEqual(sum([4*np.pi*r**2 for r in radii]), result[0])
        self.assertEqual([4*np.pi*r**2 for r in radii], list(result[1]))
        self.assertEqual(None, result[2])
        self.assertEqual(None, result[3])
    def test_nested_area(self):
        points = np.array([[0.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0]])
        radii = np.array([1.0, 2.0, 3.0, 4.0])
        result = nsc.nsc(points, radii, 1000, nsc.FLAG_ATOM_AREA)
        self.assertEqual(4*np.pi*4.0**2, result[0])
        self.assertEqual([0.0, 0.0, 0.0, 4*np.pi*4.0**2], list(result[1]))
        self.assertEqual(None, result[2])
        self.assertEqual(None, result[3])
    def test_simple_dots(self):
        points = np.array([[0.0, 0.0, 0.0], [3.0, 0.0, 0.0], [0.0, 3.0, 0.0], [0.0, 0.0, 3.0]])
        radii = np.array([1.0, 1.0, 1.0, 1.0])
        result = nsc.nsc(points, radii, 1000, nsc.FLAG_DOTS)
        self.assertEqual(sum([4*np.pi*r**2 for r in radii]), result[0])
        self.assertEqual(None, result[1])
        self.assertEqual(4008, len(result[2]))
        for diff in min_dot_offset(result[2], points, radii): 
            self.assertAlmostEqual(diff, 0.0)
        self.assertEqual(None, result[3])
    def test_nested_dots(self):
        points = np.array([[0.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0]])
        radii = np.array([1.0, 2.0, 3.0, 4.0])
        result = nsc.nsc(points, radii, 1000, nsc.FLAG_DOTS)
        self.assertEqual(4*np.pi*4.0**2, result[0])
        self.assertEqual(None, result[1])
        self.assertEqual(1002, len(result[2]))
        for diff in min_dot_offset(result[2], points[3:4], radii[3:4]): 
            self.assertAlmostEqual(diff, 0.0)
        self.assertEqual(None, result[3])
    def test_simple_volume(self):
        points = np.array([[0.0, 0.0, 0.0], [3.0, 0.0, 0.0], [0.0, 3.0, 0.0], [0.0, 0.0, 3.0]])
        radii = np.array([1.0, 1.0, 1.0, 1.0])
        result = nsc.nsc(points, radii, 1000, nsc.FLAG_VOLUME)
        self.assertEqual(sum([4*np.pi*r**2 for r in radii]), result[0])
        self.assertEqual(None, result[1])
        self.assertEqual(None, result[2])
        self.assertEqual(sum([(4/3.0)*np.pi*r**3 for r in radii]), result[3])
    def test_nested_volume(self):
        points = np.array([[0.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0]])
        radii = np.array([1.0, 2.0, 3.0, 4.0])
        result = nsc.nsc(points, radii, 1000, nsc.FLAG_VOLUME)
        self.assertEqual(4*np.pi*4.0**2, result[0])
        self.assertEqual(None, result[1])
        self.assertEqual(None, result[2])
        self.assertEqual((4/3.0)*np.pi*4.0**3, result[3])
    def test_simple_atom_area(self):
        points = [[0.0, 0.0, 0.0], [30.0, 0.0, 0.0], [0.0, 30.0, 0.0], [0.0, 0.0, 30.0]]
        radii = [1.0, 1.0, 1.0, 1.0]
        atoms = [AtomMock(p, r) for p, r in zip(points, radii)]
        nsc.area_atoms(atoms, probe_radius=1.0)
        result = [ a.area for a in atoms ]
#        self.assertEqual(sum([4*np.pi*(r+1.0)**2 for r in radii]), result[0])
        self.assertEqual([4*np.pi*(r+1.0)**2 for r in radii], list(result))
    def test_run_all_check(self):
        points = np.array([[0.0, 0.0, 0.0], [3.0, 0.0, 0.0], [0.0, 3.0, 0.0], [0.0, 0.0, 3.0]])
        radii = np.array([1.0, 1.0, 1.0, 1.0])
        result = nsc.nsc(points, radii, 1000, nsc.FLAG_ATOM_AREA | nsc.FLAG_DOTS | nsc.FLAG_VOLUME)
        self.assertTrue(result[0] is not None)
        self.assertTrue(result[1] is not None)
        self.assertTrue(result[2] is not None)
        self.assertTrue(result[3] is not None)
    def test_count_mismatch(self):
        points = np.array([[0.0, 0.0, 0.0], [30.0, 0.0, 0.0], [0.0, 30.0, 0.0], [0.0, 0.0, 30.0]])
        radii = np.array([1.0, 1.0, 1.0])
        self.assertRaises(ValueError, nsc.nsc, points, radii, 1000, nsc.FLAG_ATOM_AREA | nsc.FLAG_DOTS | nsc.FLAG_VOLUME)
    def test_bad_shape(self):
        points = np.array(zip(*[[0.0, 0.0, 0.0], [30.0, 0.0, 0.0], [0.0, 30.0, 0.0], [0.0, 0.0, 30.0]]))
        radii = np.array([1.0, 1.0, 1.0, 1.0])
        self.assertRaises(ValueError, nsc.nsc, points, radii, 1000, nsc.FLAG_ATOM_AREA | nsc.FLAG_DOTS | nsc.FLAG_VOLUME)
            
        
if __name__ == '__main__':
    unittest.main()

